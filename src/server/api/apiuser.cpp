#include "../models/user.h"
#include "../singleton.hpp"
#include "../api.h"
#include "token.h"
#include "apiuser.h"

#include <QJsonArray>

void onUserInfo(const HttpRequest &request, HttpResponse &response)
{
    QJsonObject object;

    if (!request.arguments().contains("id"))
    {
        response.setData(formError(MissingParameter), "application/json");
        return;
    }

    int id = request.arguments().value("id").toInt();

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
    QString username   = request.arguments().value("username");
    QString password   = request.arguments().value("password");
    QString name       = request.arguments().value("name");
    QString surname    = request.arguments().value("surname");
    QString patronymic = request.arguments().value("patronymic");

    User newUser = User::create(name, surname, patronymic, username, password);

    if (!newUser.isValid())
    {
        response.setData(formError(GeneralError), "application/json");
        return;
    }

    response.setData(formResponse(), "application/json");
}

void onUserUpdate(const HttpRequest &request, HttpResponse &response)
{
    if (!request.arguments().contains("name") || !request.arguments().contains("surname"))
    {
        response.setData(formError(MissingParameter), "application/json");
        return;
    }

    if (!request.arguments().contains("token"))
    {
        response.setData(formError(TokenRequired), "application/json");
        return;
    }

    QString token = request.arguments().value("token");

    if (!Singleton::tokens().contains(token))
    {
        response.setData(formError(InvalidToken), "application/json");
        return;
    }

    User user = User::getById(Singleton::tokens().value(token));

    QString name       = request.arguments().value("name");
    QString surname    = request.arguments().value("surname");
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
    if (!request.arguments().contains("password") || !request.arguments().contains("newPassword"))
    {
        response.setData(formError(MissingParameter), "application/json");
        return;
    }

    if (!request.arguments().contains("token"))
    {
        response.setData(formError(TokenRequired), "application/json");
        return;
    }

    QString token       = request.arguments().value("token");
    QString password    = request.arguments().value("password");
    QString newPassword = request.arguments().value("newPassword");

    if (!Singleton::tokens().contains(token))
    {
        response.setData(formError(InvalidToken), "application/json");
        return;
    }

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
    QJsonObject object;

    if (!request.arguments().contains("token"))
    {
        response.setData(formError(TokenRequired), "application/json");
        return;
    }

    QString token = request.arguments().value("token");

    if (!Singleton::tokens().contains(token))
    {
        response.setData(formError(InvalidToken), "application/json");
        return;
    }

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
    QJsonObject object;

    QString username = request.arguments().value("username");
    QString password = request.arguments().value("password");

    if (!request.arguments().contains("username") || !request.arguments().contains("password"))
    {
        response.setData(formError(MissingParameter), "application/json");
        return;
    }

    if (User::getByUsername(username).password() == password && !password.isNull())
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
