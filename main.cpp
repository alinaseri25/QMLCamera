#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QImageReader>

#include <../../QtLibraries/QMLFastImageView/fastimageitem.h>
#include <backend.h>

int main(int argc, char *argv[])
{
    QImageReader::setAllocationLimit(1024);
    QGuiApplication app(argc, argv);

    qmlRegisterType<FastImageItem>("FastImage",1,0,"FastImageItem");
    Backend *myBackend = new Backend(&app);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("myBackend",myBackend);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("QMLCamera", "Main");

    return app.exec();
}
