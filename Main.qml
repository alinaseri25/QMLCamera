import QtQuick
import QtQuick.Controls
import QtMultimedia

import "theme"
import "components"
import "pages"
import FastImage 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Theme { id: appTheme }

    Toast {
        id: reportToast
        themeManager: appTheme
    }

    signal qmlLoaded(VideoSink sink)
    signal camerListRequest()
    signal cameraSelected(int cameraId)

    /* ========= HEADER ========= */
    WalkieHeader {
        id: header
        theme: appTheme
        width: parent.width
        anchors.top: parent.top

        onSettingsClicked: settingsDialog.open()
    }

    /* ========= MAIN ========= */
    Item {
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        Text{
            id: frameRate
            text: ""
            width: 200
            height: 100
            anchors.top: parent.top
            anchors.left: parent.left
            color: "#00FF00"
            z: 2
        }

        Text{
            id: frameSize
            text: ""
            width: 200
            height: 100
            anchors.top: parent.top
            anchors.right: parent.right
            color: "#00FF00"
            z: 2
        }

        // FastImageItem{
        //     id: imageDisplay
        //     anchors.fill: parent
        //     z: 1
        // }

        VideoOutput
        {
            id: imageDisplay
            anchors.fill: parent
            fillMode: VideoOutput.Stretch
            z: 1
        }
    }

    /* ========= SETTINGS DIALOG ========= */
    Dialog {
        id: settingsDialog
        modal: true

        // ✅ دقیقاً وسط صفحه
        anchors.centerIn: Overlay.overlay

        // ✅ اندازه منطقی دیالوگ
        width: 300
        height: 400

        // ✅ حذف padding پیش‌فرض Dialog
        padding: 0

        // ✅ background واقعی Dialog
        background: Rectangle {
            color: appTheme.surface
            radius: appTheme.radius.md
            border.color: appTheme.border
            border.width: 1
        }

        // ✅ content دقیقاً هم‌اندازه Dialog
        contentItem: SettingsPage {
            anchors.fill: parent
            theme: appTheme

            onCancelClicked: {
                settingsDialog.close()
            }
            onApplyClicked: {
                settingsDialog.close()
            }
        }
    }

    Component.onCompleted: {
        qmlLoaded.connect(myBackend.onQmlLoaded)
        camerListRequest.connect(myBackend.onCamerListRequest)
        cameraSelected.connect(myBackend.onCameraSelected)

        qmlLoaded(imageDisplay.videoSink)
        camerListRequest()
        cameraSelected(0)
    }

    Connections{
        target: myBackend

        function onNewImage(h,w,_img){
            //imageDisplay.setImage(_img)
            frameSize.text = "Width: " + w + " Height: " + h
        }

        function onReportFrameRate(_FPS){
            frameRate.text = "Camera FPS : " + _FPS
        }

        function onDoorResult(success, message){
            reportToast.showMessage(success,message);
        }
    }
}
