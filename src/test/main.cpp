#include <QtCore>
#include <QtHttpServer>

QVariant simple(const qint32 id)
{
    return QString("User %1 detail").arg(id).toUtf8();
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QHttpServer httpServer;
    httpServer.route("/", [](qint32 id) { return QString("Hello world %1").arg(id); });

    //httpServer.route("/user/<arg>/detail", std::function(simple);

    // httpServer.route
    httpServer.listen(QHostAddress::Any, 81);
    return app.exec();
}
