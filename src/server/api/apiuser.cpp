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
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    object["username"]   = user.username();
    object["name"]       = user.name();
    object["surname"]    = user.surname();
    object["patronymic"] = user.patronymic();

    response.setData(formResponse(object), "application/json");
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
    {
        response.setData(formError(RegistrationFailed), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onUserUpdate(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(name);
    REQUIRE_STRING(surname);

    REQUIRE_TOKEN();

    User user = User::getById(Singleton::tokens().value(token));

    QString patronymic = request.arguments().value("patronymic");

    if (!user.isValid())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    user.setName(name);
    user.setSurname(surname);
    user.setPatronymic(patronymic);

    if (!user.update())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
    return;
}

void onUserChangePassword(const HttpRequest &request, HttpResponse &response)
{
    REQUIRE_STRING(password);
    REQUIRE_STRING(newPassword);

    REQUIRE_TOKEN();

    User user = User::getById(Singleton::tokens().value(token));

    if (!user.isValid())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    if (user.password() != password)
    {
        response.setData(formError(AuthFailed), "application/json");
        return;
    }

    user.setPassword(newPassword);
    user.update();

    if (!user.update())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
    return;
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

    response.setData(formResponse(object), "application/json");
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
    {
        response.setData(formError(AuthFailed), "application/json");
        return;
    }

    response.setData(formResponse(object), "application/json");
}
