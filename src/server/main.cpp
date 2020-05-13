#include "actions.hpp"
#include "singleton.hpp"
#include "webserver.hpp"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeDatabase>

bool isPathInDirectory(const QString &filePath, const QString &directoryPath)
{
    QString absouluteFilePath;
    QString absouluteDirectoryPath;

    absouluteFilePath      = QFileInfo(filePath).absolutePath();
    absouluteDirectoryPath = QDir(directoryPath).absolutePath();

    QString relativePath = QDir(absouluteDirectoryPath).relativeFilePath(absouluteFilePath);

    // Проверяем, есть ли выход за пределы католога
    if (relativePath.contains(".."))
        return false;

    // Проверяем, выходит ли католог на другой диск
    QFileInfoList rootDrivesPaths = QDir::drives();

    for (const auto &fileInfo : rootDrivesPaths)
    {
        if (relativePath.contains(fileInfo.path()))
            return false;
    }

    return true;
}

#include "api/apianswer.h"
#include "api/apiciphers.h"
#include "api/apiquestion.h"
#include "api/apiuser.h"

WebSocketFrame onEchoServer(SocketContext & /*context*/, const WebSocketFrame &frame)
{
    return frame;
}

static QString webrootDirectory = "webroot";

void onFileSystemAccess(const HttpRequest &request, HttpResponse &response)
{
    if (request.path().size() == 0)
    {
        response.setStatusCode(HttpResponse::CodeBadRequest);
        response.write("<html></head><head><body>");
        response.write("<h1>Resource not found</h1>");
        response.write(
            QString("<p>Requested resource \"" + request.path() + "\" not found</p>").toUtf8());
        response.write("</body></html>");
        return;
    }

    QString accessUri = request.path();

    if (accessUri.endsWith('/'))
        accessUri += "index.html";

    bool isNotRestrictedPath = isPathInDirectory(webrootDirectory + accessUri, webrootDirectory);

    if (!isNotRestrictedPath)
    {
        response.setStatusCode(HttpResponse::CodeForbidden);
        response.write("<html></head><head><body>");
        response.write("<h1>Access denied</h1>");
        response.write(
            QString("<p>You lack required permissions for resource \"" + request.path() + "\"</p>")
                .toUtf8());
        response.write("</body></html>");
    }
    else
    {
        QString path = webrootDirectory + accessUri;

        QFileInfo fileInfo(path);
        if (fileInfo.exists() && fileInfo.isFile())
        {
            QFile file(fileInfo.filePath());

            file.open(QIODevice::ReadOnly);

            if (file.isOpen())
            {
                response.setStatusCode(HttpResponse::CodeOk);
                response.setData(file.readAll());
                response.addHeader("content-type",
                                   Singleton::mimeDatabase().mimeTypeForFile(path).name());
            }
            else
            {
                response.setStatusCode(HttpResponse::CodeNotFound);
                response.write("<html></head><head><body>");
                response.write("<h1>Resource not found</h1>");
                response.write(
                    QString("<p>Requested resource \"" + request.path() + "\" not found</p>")
                        .toUtf8());
                response.write("</body></html>");
            }
        }
        else
        {
            response.setStatusCode(HttpResponse::CodeNotFound);
            response.write("<html></head><head><body>");
            response.write("<h1>Resource not found</h1>");
            response.write(
                QString("<p>Requested resource \"" + request.path() + "\" not found</p>").toUtf8());
            response.write("</body></html>");
        }
    }
}



int main(int argc, char *argv[])
{
    //controller.add("/api/user/login", new ApiUserLogin);

    const quint16 defaultServerPort = 8080;
    const int defaultClients        = 50;
    const int defaultTimeout        = 5;

    QCoreApplication app(argc, argv);

    QSettings configuration("config.ini", QSettings::IniFormat);

    if (configuration.status() != QSettings::NoError)
    {
        qCritical() << "Can't read settings from config.ini";
        return -1;
    }

    // configuration.

    quint16 port          = configuration.value("port", defaultServerPort).toInt();
    QString address       = configuration.value("listen", "127.0.0.1").toString();
    webrootDirectory      = configuration.value("webroot", "webroot").toString();
    QString databaseName  = configuration.value("database", "storage.db3").toString();
    int maxClient         = configuration.value("clients", defaultClients).toInt();
    int connectionTimeout = configuration.value("timeout", defaultTimeout).toInt();

    QSqlError error = Singleton::database().init(databaseName);
    if (error.isValid())
    {
        qCritical() << "Can't initialize database";
        return -1;
    }

    WebServer server(maxClient, connectionTimeout);

    server.registerHttpRoute("^\\/((?!api)).*$", onFileSystemAccess);

    server.registerHttpRoute("/api/user/login", onUserLogin);
    server.registerHttpRoute("/api/user/register", onUserRegister);
    server.registerHttpRoute("/api/user/list", onUserList);
    server.registerHttpRoute("/api/user/info", onUserInfo);
    server.registerHttpRoute("/api/user/update", onUserUpdate);
    server.registerHttpRoute("/api/user/changePassword", onUserChangePassword);

    server.registerHttpRoute("/api/question/info", onQuestionInfo);
    server.registerHttpRoute("/api/question/update", onQuestionUpdate);
    server.registerHttpRoute("/api/question/add", onQuestionAdd);
    server.registerHttpRoute("/api/question/list", onQuestionList);
    server.registerHttpRoute("/api/question/listAnswers", onQuestionListAnswers);
    server.registerHttpRoute("/api/question/addAnswer", onQuestionAddAnswer);
    server.registerHttpRoute("/api/question/removeAnswer", onQuestionRemoveAnswer);
    server.registerHttpRoute("/api/question/updateAnswer", onQuestionUpdateAnswer);
    server.registerHttpRoute("/api/question/themes", onQuestionThemes);

    server.registerHttpRoute("/api/ciphers/info", onCiphersInfo);
    server.registerHttpRoute("/api/ciphers/update", onCiphersUpdate);
    server.registerHttpRoute("/api/ciphers/add", onCiphersAdd);
    server.registerHttpRoute("/api/ciphers/list", onCiphersList);

    server.registerHttpRoute("/api/answer/info", onAnswerInfo);
    server.registerHttpRoute("/api/answer/update", onAnswerUpdate);
    server.registerHttpRoute("/api/answer/add", onAnswerAdd);
    server.registerHttpRoute("/api/answer/list", onAnswerList);
    server.registerHttpRoute("/api/answer/listQuestions", onAnswerListQuestions);

    server.registerWebSocketRoute("/api/echo", onEchoServer);

    if (server.listen(QHostAddress(address), port))
    {
        qDebug() << "Server listening on" << address << ":" << QString::number(port);
    }
    else
    {
        return -1;
    }

    return QCoreApplication::exec();
}
