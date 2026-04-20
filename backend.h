#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QThread>
#include <QDebug>
#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QCoreApplication>
#endif
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <../../QtLibraries/camera/camera.h>

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);
    ~Backend(void);

    QVideoSink *videoSink;

public slots:
    void onQmlLoaded(QVideoSink *sink);
    void onCamerListRequest(void);
    void onCameraSelected(int camera);
    void onCameraUrlSelected(QString url);

private:
    QThread worker;
    Camera cam;

    void askForPermission(const QStringList &permissions, int requestCode);

private slots:
    void onCameraListResponse(const QVariantList &_cameras);
    void onNewFrameRecieved(CameraFrame *_frame);
    void onReportFrameRate(int _FPS);
    void onCameraStatusChanged(CameraState _state,const QString &_description);

signals:
    void cameraListRequest(void);
    void cameraSelected(int _camera, bool autoReconnect = false);
    void cameraSelected(const QString &url, bool autoReconnect = false);
    void stopCamera(void);
    void start(void);
    void setVideoSink(QVideoSink *_sink = nullptr);

    //signals for QML
    void newImage(int h, int w, QImage *_lastFrame = nullptr);
    void cameraListResponse(const QVariantList &_cameras);
    void reportFrameRate(int _FPS);
    void startCamera(void);
};

#endif // BACKEND_H
