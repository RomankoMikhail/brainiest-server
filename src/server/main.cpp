#include "actions.hpp"
#include "singleton.hpp"
#include "user.hpp"
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
    QString absouluteFilePath, absouluteDirectoryPath;

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

WebSocketFrame onActionServer(SocketContext &context, const WebSocketFrame &frame)
{
    QJsonDocument requestDocument, responseDocument;

    requestDocument = QJsonDocument::fromJson(frame.data());

    if (!requestDocument.isObject() || !requestDocument.object().contains("cmd"))
    {
        responseDocument.setObject(Action::formResponseFromCode(Action::ReturnCodeError));
    }
    else
    {
        auto response = Singleton::action().exec(requestDocument.object()["cmd"].toString(),
                                                 requestDocument.object());

        responseDocument.setObject(response);
    }

    WebSocketFrame responseFrame;
    responseFrame.setOpcode(WebSocketFrame::OpcodeText);
    responseFrame.setData(responseDocument.toJson());
    responseFrame.setIsFinalFrame(true);
    return responseFrame;
}

WebSocketFrame onEchoServer(SocketContext &context, const WebSocketFrame &frame)
{
    return frame;
}

static QString webrootDirectory = "webroot";


void onFileSystemAccess(const HttpRequest &request, HttpResponse &response)
{
    QMimeDatabase mimeDatabase;

    for(const auto &cookie : request.cookies())
    {
        qDebug() << cookie.name() << cookie.value();
    }

    Cookie myCookie;
    myCookie.setName("test");
    myCookie.setValue("Hello there");
    myCookie.setMaxAge(60);
    //myCookie.setD("/");

    response.addCookie(myCookie);

    if (request.path().size() == 0)
    {
        response.setStatusCode(HttpResponse::CodeBadRequest);
        response.write("<html></head><head><body>");
        response.write("<h1>Resource not found</h1>");
        response.write(QString("<p>Requested resource \"" + request.path() + "\" not found</p>").toUtf8());
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
        response.write(QString("<p>You lack required permissions for resource \"" + request.path() + "\"</p>").toUtf8());
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
                response.addHeader("content-type", mimeDatabase.mimeTypeForFile(path).name());
            }
            else
            {
                response.setStatusCode(HttpResponse::CodeNotFound);
                response.write("<html></head><head><body>");
                response.write("<h1>Resource not found</h1>");
                response.write(QString("<p>Requested resource \"" + request.path() + "\" not found</p>").toUtf8());
                response.write("</body></html>");
            }
        }
        else
        {
            response.setStatusCode(HttpResponse::CodeNotFound);
            response.write("<html></head><head><body>");
            response.write("<h1>Resource not found</h1>");
            response.write(QString("<p>Requested resource \"" + request.path() + "\" not found</p>").toUtf8());
            response.write("</body></html>");
        }
    }
}

int main(int argc, char *argv[])
{
    QMimeDatabase mimeDatabase;

    QString databaseName = "storage.db3";
    QString address = "127.0.0.1";
    int port        = 8080;
    qint64 maxClient = 50;
    qint64 connectionTimeout = 5;

    QCoreApplication a(argc, argv);

    QSettings configuration("config.ini", QSettings::IniFormat);

    if(configuration.status() != QSettings::NoError)
    {
        qCritical() << "Can't read settings from config.ini";
        return -1;
    }

    //configuration.

    port = configuration.value("port", 8080).toInt();
    address = configuration.value("listen", "127.0.0.1").toString();
    webrootDirectory = configuration.value("webroot", "webroot").toString();
    databaseName = configuration.value("database", "storage.db3").toString();
    maxClient = configuration.value("clients", 50).toInt();
    connectionTimeout = configuration.value("timeout", 5).toInt();


    QSqlError error = Singleton::database().init(databaseName);
    if (error.isValid())
    {
        qCritical() << "Can't initialize database";
        return -1;
    }

    WebServer server(maxClient, connectionTimeout);

    server.registerHttpRoute("^\\/((?!api)).*$", onFileSystemAccess);
    server.registerWebSocketRoute("/api/echo", onEchoServer);
    server.registerWebSocketRoute("/api/action", onActionServer);
    server.registerWebSocketRoute("/", onActionServer);

    registerActions();

    if (server.listen(QHostAddress(address), port))
    {
        qDebug() << "Server listening on" << address << ":" << QString::number(port);
    }
    else
    {
        return -1;
    }

    return a.exec();
}
