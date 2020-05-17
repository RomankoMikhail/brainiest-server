#include "ongoinggame.h"

#include "models/answer.h"
#include "models/gamehascipher.h"
#include "models/gamehasquestion.h"
#include "models/question.h"
#include "models/questionhasanswer.h"
#include "models/useranswered.h"

/*
bool OngoingGame::create(QList<int> userIds, int authorId)
{
    auto themes = Question::getThemes();

    if (themes.size() < 16)
        return false;

    std::random_shuffle(themes.begin(), themes.end());

    // Get themes
    QString round1Theme = themes.at(0);
    QVector<QString> round2Themes;
    QVector<QString> round3Themes;

    for (int i = 1; i < 13; i++)
    {
        round2Themes.push_back(themes.at(i));
    }

    for (int i = 13; i < 16; i++)
    {
        round3Themes.push_back(themes.at(i));
    }

    // Round 1

    auto round1Questions = Question::getByTheme(round1Theme);
    std::random_shuffle(themes.begin(), themes.end());

    for(int i = 0; i < std::min(round1Questions.size(), 18); i++)
    {
        RoundQuestion rq;
        rq.mQuestionId = round1Questions.at(i);
        mRound1.push_back(rq);
    }

    // Round 2

    for(auto theme : round2Themes)
    {
        auto questions = Question::getByTheme(theme);
        std::random_shuffle(questions.begin(), questions.end());

        mRound2[theme] = QVector<int>();

        for(int i = 0; i < std::min(questions.size(), 6); i++)
        {
            mRound2[theme].push_back(questions[i]);
        }
    }

    // Round 3

    return true;
}*/

bool OngoingGame::answerRound1(const int &userId, const int &userAnswerId)
{
    // Check if user CAN answer
}

bool OngoingGame::selectRound2(const int &userId, const QString &theme)
{
    // Check if user CAN select
}

bool OngoingGame::answerRound2(const int &userId, const QString &userAnswer)
{
    // Check if user CAN answer
}

bool OngoingGame::selectRound3(const int &userId, const QString &theme)
{
    // Check if user CAN select
}

bool OngoingGame::answerRound3(const int &userId, const QString &userAnswer)
{
    // Check if user CAN answer
}

bool OngoingGame::load(int gameId)
{
    auto questions = GameHasQuestion::getQuestionIds(gameId);
    auto ciphers   = GameHasCipher::getCiphersIds(gameId);

    for (auto questionId : questions)
    {
        GameHasQuestion ghq = GameHasQuestion::getById(gameId, questionId);
        Question q          = Question::getById(questionId);

        switch (ghq.round())
        {
        case 1:
        default:
            mRound1.push_back(QuestionData(q.id(), false));
            break;
        case 2:
            mRound2.push_back(QuestionData(q.id(), false));
            break;
        case 3:
            mRound3.push_back(QuestionData(q.id(), false));
            break;
        }
    }

    return true;
}



QJsonObject OngoingGame::currentStatus()
{
    QJsonObject object;
}

QString OngoingGame::encrypted(const QString &mEncryptedWord)
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

#include "jarowinkler.h"

int OngoingGame::nearestAnswer(const int &questionId, const QString &userAnswer)
{
    using AnswerProbability = std::pair<int, double>;

    auto sortingFunction = [](const std::pair<int, double> &a,
                              const std::pair<int, double> &b) {
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
        auto answer = Answer::getById(answerId);
        double metric =
            jaroWinkler(answer.answer().toUpper(), userAnswer.toUpper());

        mAnswerProbability.append(AnswerProbability(answer.id(), metric));
    }

    // Sort by best probability
    std::sort(mAnswerProbability.begin(), mAnswerProbability.end(),
              sortingFunction);

    auto bestAnswer = mAnswerProbability.at(0);

    if (bestAnswer.second >= 0.8)
        return bestAnswer.first;

    return 0;
}

bool OngoingGame::createAnswer(const int &userId, const int &questionId,
                               const int &answerId)
{
    auto answers = QuestionHasAnswer::getAnswerIds(questionId);

    if (answers.isEmpty())
        return false;

    if (!answers.contains(answerId))
        return false;

    auto ua = UserAnswered::create(userId, mGameId, questionId, answerId);

    return ua.isValid();
}

void OngoingGame::generateOrder()
{

}
