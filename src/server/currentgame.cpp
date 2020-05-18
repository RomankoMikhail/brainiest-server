#include "currentgame.h"

#include "models/answer.h"
#include "models/gamehascipher.h"
#include "models/gamehasquestion.h"
#include "models/player.h"
#include "models/question.h"
#include "models/questionhasanswer.h"
#include "models/useranswered.h"

void CurrentGame::checkAnswer(const int &userId, const int &questionId, const int &answerId)
{
    auto questionHasAnswer = QuestionHasAnswer::getById(questionId, answerId);

    if (questionHasAnswer.isValid() && questionHasAnswer.correct())
    {
        mUsersScore[userId]++;
    }
}

QVector<int> CurrentGame::getTopUsers(int top)
{
    QVector<int>                 result;
    QVector<std::pair<int, int>> usersScore;

    for (auto it = mUsersScore.begin(); it != mUsersScore.end(); ++it)
    {
        if (mOrder.contains(it.key()))
            usersScore.append(std::pair<int, int>(it.key(), it.value()));
    }

    std::sort(usersScore.begin(), usersScore.end(),
              [](const std::pair<int, int> &a, const std::pair<int, int> &b) -> bool {
                  return a.second >= b.second;
              });

    for (int i = 0; i < top; i++)
        result.append(usersScore.at(i).first);

    return result;
}

bool CurrentGame::processAnswerRound1(const int &userId, const int &questionId,
                                      const int &userAnswerId)
{
    auto userAnswer = UserAnswered::getById(userId, mGameId, questionId);
    if (userAnswer.isValid())
        return false;

    auto isAnswerAdded = createAnswer(userId, questionId, userAnswerId);

    if (!isAnswerAdded)
        return false;

    checkAnswer(userId, questionId, userAnswerId);

    return true;
}

bool CurrentGame::processSelectRound2(const int &userId, const QString &theme)
{
    if (mOrder.current() != userId)
        return false;

    if (!mRound2Theme.isNull())
        return false;

    if (!roundThemes(mRound2).contains(theme))
        return false;

    mRound2Theme = theme;

    return true;
}

bool CurrentGame::processAnswerRound2(const int &userId, const QString &userAnswer)
{
    if (mOrder.current() != userId)
        return false;

    auto questionId = firstUnanswered(mRound2, mRound2Theme);

    if (questionId == 0)
        return false;

    int nearestAnswerId = nearestAnswer(questionId, userAnswer);

    auto userAnswered = UserAnswered::create(userId, mGameId, questionId, nearestAnswerId);

    if (!userAnswered.isValid())
        return false;

    checkAnswer(userId, questionId, nearestAnswerId);

    return true;
}

bool CurrentGame::processSelectRound3(const int &userId, const int &index)
{
    if (mOrder.current() != userId)
        return false;

    if (index >= mRound3.size())
        return false;

    if (!UserAnswered::getUsersIds(mGameId, mRound3.at(mRound3Index)).isEmpty())
        return false;

    mRound3Index = index;

    return true;
}

bool CurrentGame::processAnswerRound3(const int &userId, const QString &userAnswer)
{
    if (mOrder.current() != userId)
        return false;

    auto questionId = firstUnanswered(mRound3);

    if (questionId == 0)
        return false;

    int nearestAnswerId = nearestAnswer(questionId, userAnswer);

    auto userAnswered = UserAnswered::create(userId, mGameId, questionId, nearestAnswerId);

    if (!userAnswered.isValid())
        return false;

    checkAnswer(userId, questionId, nearestAnswerId);

    return true;
}

void CurrentGame::next(const CurrentGame::States &newState, std::function<void(void)> function)
{
    mPreviousState = mCurrentState;
    mCurrentState  = newState;

    if (function != nullptr)
        function();

    notify();
}

void CurrentGame::prev()
{
    mCurrentState = mPreviousState;
}

void CurrentGame::setTime(int seconds)
{
    mUntil = QDateTime::fromSecsSinceEpoch(QDateTime::currentSecsSinceEpoch() + seconds);
}

QJsonObject CurrentGame::status()
{
    QJsonObject object;

    object["state"] = mCurrentState;
    object["time"]  = QDateTime::currentSecsSinceEpoch() - mUntil.toSecsSinceEpoch();

    QJsonArray usersLeft;

    for (auto id : mOrder.get())
    {
        usersLeft.append(id);
    }

    object["users"] = usersLeft;

    switch (mCurrentState)
    {
    case Round1:
        object["questionId"] = mRound1[mRound1Index];
        break;

    case Round2:
        object["currentUserId"] = mOrder.current();
        object["questionId"]    = firstUnanswered(mRound2, mRound2Theme);
        object["theme"]         = mRound2Theme;
        break;

    case Round2Select:
        object["currentUserId"] = mOrder.current();
        object["themes"]        = QJsonArray::fromStringList(unasweredThemes(mRound2));
        break;

    case Round3Select:
        object["currentUserId"] = mOrder.current();
        // object["items"];
        break;

    case Round3:
        object["currentUserId"] = mOrder.current();
        object["questionId"]    = mRound3[mRound3Index];
        object["index"]         = mRound3Index;

        break;
    case Decrypt:
        object["decrypt"] = encrypted(mWordToDecrypt);
        break;

    default:
        break;
    }
}

bool CurrentGame::load(int gameId)
{
    auto questions = GameHasQuestion::getQuestionIds(gameId);
    auto ciphers   = GameHasCipher::getCiphersIds(gameId);

    if(questions.isEmpty() || ciphers.isEmpty())
        return false;

    for (auto questionId : questions)
    {
        GameHasQuestion ghq = GameHasQuestion::getById(gameId, questionId);
        Question        q   = Question::getById(questionId);

        switch (ghq.round())
        {
        case 1:
        default:
            mRound1.push_back(q.id());
            break;
        case 2:
            mRound2.push_back(q.id());
            break;
        case 3:
            mRound3.push_back(q.id());
            break;
        }
    }

    if(mRound1.isEmpty() || mRound2.isEmpty() || mRound3.isEmpty())
        return false;

    mGameId = gameId;

    auto userIds = Player::getUserIds(gameId);

    if(userIds.isEmpty())
        return false;

    mOrder.clear();
    mUsersScore.clear();

    for (auto userId : userIds)
    {
        mOrder.append(userId);
        mUsersScore[userId] = 0;
    }

    return true;
}

bool CurrentGame::decrypt(const int &userId, const QString &userAnswer)
{
}

bool CurrentGame::answerRound1(const int &userId, const int &userAnswerId)
{
    auto result = processAnswerRound1(userId, mRound1Index, userAnswerId);

    if (result == false)
        return false;

    notify();
    update();
    return true;
}

bool CurrentGame::answerRound2(const int &userId, const QString &userAnswer)
{
    auto result = processAnswerRound2(userId, userAnswer);

    if (result == false)
        return false;

    notify();
    update();
    return true;
}

bool CurrentGame::answerRound3(const int &userId, const QString &userAnswer)
{
    auto result = processAnswerRound3(userId, userAnswer);

    if (result == false)
        return false;

    next(Round3Select);
    update();
    return true;
}

bool CurrentGame::selectRound2(const int &userId, const QString &theme)
{
    auto result = processSelectRound2(userId, theme);

    if (result == false)
        return false;

    setTime(60);
    next(Round2);
    update();
    return true;
}

bool CurrentGame::selectRound3(const int &userId, const int &index)
{
    auto result = processSelectRound3(userId, index);

    if (result == false)
        return false;

    setTime(5);
    next(Round3);
    update();
    return true;
}

void CurrentGame::update()
{
    switch (mCurrentState)
    {
    case Prepare:
        if (QDateTime::currentDateTime() >= mUntil)
        {
            next(Round1, [this]() {
                mRound1Index = 0;
                setTime(5);
            });
        }
        break;

    case Decrypt:
        switch (mPreviousState)
        {
        case Round1:
            if (QDateTime::currentDateTime() >= mUntil || everyoneDecrypted())
            {
                next(Round2Select, [this]() { mRound2Theme = QString(); });
                // Set new order
            }
            break;

        case Round2:
            if (QDateTime::currentDateTime() >= mUntil)
            {
                next(Round3Select, [this]() { mRound3Index = 0; });
                // Set new order
            }
            break;

        default:
            break;
        }
        break;

    case Round1:
        if (QDateTime::currentDateTime() >= mUntil || everyoneAnswered(mRound1[mRound1Index]))
        {
            mRound1Index++;
            if (mRound1Index < mRound1.size())
            {
                notify();
                setTime(5);
            }
            else
            {
                auto usersIds = getTopUsers(6);

                mOrder.clear();

                for (auto id : usersIds)
                    mOrder.append(id);

                setTime(60);
                next(Decrypt);
            }
        }
        break;

    case Round2:
        if (QDateTime::currentDateTime() >= mUntil || false)
        {
            int questionId;
            while ((questionId = firstUnanswered(mRound2, mRound2Theme)) != 0)
            {
                UserAnswered::create(mOrder.current(), mGameId, questionId, 0);
            }
            mOrder.advance();
            next(Round2Select);
        }
        break;

    case Round3:
        if (QDateTime::currentDateTime() >= mUntil || false)
        {
            UserAnswered::create(mOrder.current(), mGameId, mRound3[mRound3Index], 0);

            mOrder.advance();
            next(Round3Select);
        }
        break;

    default:
        break;
    }
}

#include "jarowinkler.h"

int CurrentGame::nearestAnswer(const int &questionId, const QString &userAnswer)
{
    using AnswerProbability = std::pair<int, double>;

    auto sortingFunction = [](const std::pair<int, double> &a, const std::pair<int, double> &b) {
        return a.second >= b.second;
    };

    // Find answers
    auto answers = QuestionHasAnswer::getAnswerIds(questionId);

    if (answers.isEmpty())
        return 0;

    QVector<AnswerProbability> mAnswerProbability;

    for (auto answerId : answers)
    {
        // Calculate metric for every answer
        auto   answer = Answer::getById(answerId);
        double metric = jaroWinkler(answer.answer().toUpper(), userAnswer.toUpper());

        mAnswerProbability.append(AnswerProbability(answer.id(), metric));
    }

    // Sort by best probability
    std::sort(mAnswerProbability.begin(), mAnswerProbability.end(), sortingFunction);

    auto bestAnswer = mAnswerProbability.at(0);

    if (bestAnswer.second >= 0.8)
        return bestAnswer.first;

    return 0;
}

bool CurrentGame::createAnswer(const int &userId, const int &questionId, const int &answerId)
{
    auto answers = QuestionHasAnswer::getAnswerIds(questionId);

    if (answers.isEmpty())
        return false;

    if (!answers.contains(answerId))
        return false;

    auto ua = UserAnswered::create(userId, mGameId, questionId, answerId);

    return ua.isValid();
}

bool CurrentGame::everyoneAnswered(const int &questionId)
{
    auto userIds = UserAnswered::getUsersIds(mGameId, questionId);

    for (auto id : mOrder.get())
    {
        if (!userIds.contains(id))
            return false;
    }
    return true;
}

bool CurrentGame::everyoneDecrypted()
{
    for (auto id : mOrder.get())
    {
        if (!mUsersDecrypted.keys().contains(id))
            return false;
    }
    return true;
}

#include <webserver.hpp>

extern WebServer *webServer;

void CurrentGame::notify()
{
    WebSocketFrame frame;

    QJsonDocument d;
    QJsonObject   o;
    o["id"]      = mGameId;
    o["updated"] = 1;
    d.setObject(o);

    QString t = QString(d.toJson(QJsonDocument::Compact));
    frame.setOpcode(WebSocketFrame::OpcodeText);
    frame.setData(t);
    frame.setIsFinalFrame(true);
    webServer->sendAllWebSockets(frame);
}

int CurrentGame::gameId() const
{
    return mGameId;
}

void CurrentGame::setGameId(int gameId)
{
    mGameId = gameId;
}

QStringList CurrentGame::unasweredThemes(const QVector<int> &questionIds)
{
    QStringList themes;

    for (const auto &questionId : questionIds)
    {
        auto question = Question::getById(questionId);

        if (!themes.contains(question.theme()))
        {
            if (UserAnswered::getUsersIds(mGameId, questionId).isEmpty())
                themes.append(question.theme());
        }
    }

    return themes;
}

QStringList CurrentGame::roundThemes(const QVector<int> &questionIds)
{
    QStringList themes;

    for (const auto &questionId : questionIds)
    {
        auto question = Question::getById(questionId);

        if (!themes.contains(question.theme()))
            themes.append(question.theme());
    }

    return themes;
}

int CurrentGame::firstUnanswered(const QVector<int> &questionIds, const QString &theme)
{
    for (const auto &questionId : questionIds)
    {
        auto question = Question::getById(questionId);

        if (!question.isValid())
            continue;

        if (!theme.isNull() && question.theme() != theme)
            continue;

        if (!UserAnswered::getUsersIds(mGameId, questionId).isEmpty())
            return questionId;
    }

    return 0;
}

CurrentGame::States CurrentGame::currentState() const
{
    return mCurrentState;
}

QString CurrentGame::encrypted(const QString &mEncryptedWord)
{
    QString result;
    for (auto letter : mEncryptedWord)
    {
        if (letter == "А" || letter == "Б" || letter == "В")
        {
            result += "1";
        }
        if (letter == "Г" || letter == "Д" || letter == "Е" || letter == "Ё")
        {
            result += "2";
        }
        if (letter == "Ж" || letter == "З" || letter == "И" || letter == "Й")
        {
            result += "3";
        }
        if (letter == "К" || letter == "Л" || letter == "М")
        {
            result += "4";
        }
        if (letter == "Н" || letter == "О" || letter == "П")
        {
            result += "5";
        }
        if (letter == "Р" || letter == "С" || letter == "Т")
        {
            result += "6";
        }
        if (letter == "У" || letter == "Ф" || letter == "Х")
        {
            result += "7";
        }
        if (letter == "Ц" || letter == "Ч" || letter == "Ш")
        {
            result += "8";
        }
        if (letter == "Щ" || letter == "Ы" || letter == "Ь")
        {
            result += "9";
        }
        if (letter == "Э" || letter == "Ю" || letter == "Я")
        {
            result += "0";
        }
    }

    return result;
}
