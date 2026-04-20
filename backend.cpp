#include "backend.h"

static Backend* g_mainWindowInstance = nullptr;

#ifdef ANDROID
extern "C"
    JNIEXPORT void JNICALL
        Java_org_verya_QMLCamera_TestBridge_nativeOnPermissionResult
    (JNIEnv *env, jclass /*clazz*/, jstring msg)
{
    if (!g_mainWindowInstance)
        return;

    // تبدیل jstring به QString در همان thread JNI
    QString jsonStr = QJniObject(msg).toString();

    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    QJsonObject root = doc.object();
    QJsonArray perms = root["results"].toArray();

    for (const QJsonValue &v : perms) {
        QJsonObject o = v.toObject();
        QString permission = o["permission"].toString();
        bool granted = o["granted"].toBool();
        qDebug() << permission << (granted ? "✅" : "❌");
    }

    // QMetaObject::invokeMethod(g_mainWindowInstance, [=]() {
    //     QString str = (qMsg == "start") ? "Play" : "Pause";

    //     qDebug() << "str:" << str << "--msg:" << qMsg;

    //     g_mainWindowInstance->onPlayPause(str);
    // }, Qt::QueuedConnection);
}
#endif

Backend::Backend(QObject *parent)
    : QObject{parent}
{
    g_mainWindowInstance = this;

#ifdef ANDROID
    QStringList permissions = {"android.permission.CAMERA"};
    askForPermission(permissions,12);

    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid())
        return;

    QJniObject::callStaticMethod<void>(
        "org/verya/QMLCamera/MainActivity",
        "manageScreenAndWakeLock",
        "(Landroid/content/Context;ZZ)V",
        context.object(),
        (jboolean)true,  // screenAlwaysOn
        (jboolean)true    // wakeLock
        );

    QJniObject::callStaticMethod<void>(
        "org/verya/QMLCamera/MainActivity",
        "setDimTimeoutFromQt",
        "(J)V",
        0
        );
#endif
}

Backend::~Backend()
{
    emit stopCamera();
    worker.quit();

    if (!worker.wait(3000)) {
        qWarning() << "Worker did not stop, forcing terminate";
        worker.terminate();
        worker.wait();
    }
}

void Backend::onQmlLoaded(QVideoSink *sink)
{
    videoSink = sink;
    connect(this,&Backend::cameraListRequest,&cam,&Camera::camerListRequest);
    connect(this,qOverload<int,bool>(&Backend::cameraSelected),&cam,qOverload<int,bool>(&Camera::cameraSelected));
    connect(this,qOverload<const QString&,bool>(&Backend::cameraSelected),&cam,qOverload<const QString&,bool>(&Camera::cameraSelected));
    connect(this,&Backend::stopCamera,&cam,&Camera::stopCamera);
    connect(this,&Backend::setVideoSink,&cam,&Camera::setVideoSink);
    connect(this,&Backend::startCamera,&cam,&Camera::startCamera);
    connect(&cam,&Camera::cameraListResponse,this,&Backend::onCameraListResponse);
    connect(&cam,&Camera::newFrameRecieved,this,&Backend::onNewFrameRecieved);
    connect(&cam,&Camera::reportFrameRate,this,&Backend::onReportFrameRate);
    connect(&cam,&Camera::cameraStatusChanged,this,&Backend::onCameraStatusChanged);

    cam.moveToThread(&worker);
    worker.start(QThread::HighPriority);

    emit setVideoSink(videoSink);
}

void Backend::onCamerListRequest()
{
    emit cameraListRequest();
}

void Backend::onCameraUrlSelected(QString url)
{
    emit cameraSelected(url);
}

void Backend::onCameraSelected(int camera)
{
    emit cameraSelected(camera);
}

void Backend::onCameraListResponse(const QVariantList &_cameras)
{
    emit cameraListResponse(_cameras);
}

void Backend::onNewFrameRecieved(CameraFrame *_frame)
{
    emit newImage(_frame->height,_frame->width);
}

void Backend::onReportFrameRate(int _FPS)
{
    emit reportFrameRate(_FPS);
}


void Backend::onCameraStatusChanged(CameraState _state, const QString &_description)
{
    if(_state == CameraState::Starting)
    {
        emit startCamera();
    }
}



void Backend::askForPermission(const QStringList &permissions, int requestCode)
{
#ifdef ANDROID
    QJniEnvironment env;
    jobjectArray jPerms = env->NewObjectArray(permissions.size(),
                                              env->FindClass("java/lang/String"),
                                              nullptr);

    for (int i = 0; i < permissions.size(); ++i)
        env->SetObjectArrayElement(jPerms, i,
                                   QJniObject::fromString(permissions[i]).object<jstring>());

    QJniObject::callStaticMethod<void>(
        "org/verya/QMLCamera/MainActivity",
        "requestAppPermissions",
        "([Ljava/lang/String;I)V",
        jPerms,
        requestCode
        );
#endif
}

