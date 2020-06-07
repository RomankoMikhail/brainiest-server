#include "apiuser.h"
#include "../api.h"
#include "../models/user.h"
#include "../singleton.hpp"
#include "token.h"

#include <QJsonArray>

void onUserInfo(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_INT(id);

    REQUIRE_TOKEN();

    QJsonObject object;

    User user = User::getById(id);

    if (!user.isValid())
        SEND_ERROR(GeneralError);

    object["username"]   = user.username();
    object["name"]       = user.name();
    object["surname"]    = user.surname();
    object["patronymic"] = user.patronymic();

    SEND_RESPONSE(object);
}

void onUserRegister(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(username);
    REQUIRE_STRING(password);
    REQUIRE_STRING(name);
    REQUIRE_STRING(surname);

    QString patronymic = request.arguments().value("patronymic");

    User newUser = User::create(name, surname, patronymic, username, password);

    if (!newUser.isValid())
        SEND_ERROR(RegistrationFailed);

    SEND_RESPONSE();
}

void onUserUpdate(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(name);
    REQUIRE_STRING(surname);

    REQUIRE_TOKEN();

    User user = User::getById(Singleton::tokens().value(token));

    QString patronymic = request.arguments().value("patronymic");

    if (!user.isValid())
        SEND_ERROR(GeneralError);

    user.setName(name);
    user.setSurname(surname);
    user.setPatronymic(patronymic);

    if (!user.update())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
    return;
}

void onUserGet(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    QJsonObject object;
    User user = User::getById(Singleton::tokens().value(token));

    object["id"]         = user.id();
    object["name"]       = user.name();
    object["surname"]    = user.surname();
    object["patronymic"] = user.patronymic();
    object["username"]   = user.username();

    SEND_RESPONSE(object);
}

void onUserChangePassword(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(password);
    REQUIRE_STRING(newPassword);

    REQUIRE_TOKEN();

    User user = User::getById(Singleton::tokens().value(token));

    if (!user.isValid())
        SEND_ERROR(GeneralError);

    if (user.password() != password)
        SEND_ERROR(AuthFailed);

    user.setPassword(newPassword);
    user.update();

    if (!user.update())
        SEND_ERROR(GeneralError);

    SEND_RESPONSE();
    return;
}

void onTokenCheck(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    SEND_RESPONSE();
}

void onUserList(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_TOKEN();

    QJsonObject object;

    QList<int> ids = User::getIds();

    QJsonArray userArray;

    for (const auto &id : ids)
    {
        QJsonObject userObject;
        User user = User::getById(id);

        userObject["id"]         = user.id();
        userObject["name"]       = user.name();
        userObject["surname"]    = user.surname();
        userObject["patronymic"] = user.patronymic();
        userObject["username"]   = user.username();

        userArray.append(userObject);
    }
    object["items"] = userArray;

    SEND_RESPONSE(object);
}

void onUserLogin(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(username);
    REQUIRE_STRING(password);

    QJsonObject object;

    if (User::getByUsername(username).password() == password &&
        !password.isNull())
    {
        QString token = Token::generate();
        Singleton::tokens().insert(token, User::getByUsername(username).id());

        object["token"] = token;
    }
    else
        SEND_ERROR(AuthFailed);

    SEND_RESPONSE(object);
}
