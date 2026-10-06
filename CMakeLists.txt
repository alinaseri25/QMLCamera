cmake_minimum_required(VERSION 3.16)

# ✅ خواندن VERSION از فایل
file(READ "${CMAKE_SOURCE_DIR}/version.txt" VERSION_STRING)
string(STRIP "${VERSION_STRING}" VERSION_STRING)

# ✅ تجزیه version به اجزای MAJOR.MINOR.PATCH
string(REPLACE "." ";" VERSION_LIST ${VERSION_STRING})
list(GET VERSION_LIST 0 VERSION_MAJOR)
list(GET VERSION_LIST 1 VERSION_MINOR)
list(GET VERSION_LIST 2 VERSION_PATCH)

# ✅ محاسبه versionCode (مثلاً: 1.3.0 → 10300)
math(EXPR VERSION_CODE "${VERSION_MAJOR} * 10000 + ${VERSION_MINOR} * 100 + ${VERSION_PATCH}")

project(QMLCamera VERSION ${VERSION_STRING} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Multimedia)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(QMLCamera
    main.cpp
)

qt_add_qml_module(QMLCamera
    URI QMLCamera
    QML_FILES
        Main.qml
        components/CBottomBar.qml
        components/CButton.qml
        components/CCard.qml
        components/CTextField.qml
        components/WalkieHeader.qml
        components/Toast.qml
        theme/Theme.qml
        pages/SettingsPage.qml

    SOURCES
        backend.h backend.cpp
        ../../QtLibraries/camera/camera.h ../../QtLibraries/camera/camera.cpp
        ../../QtLibraries/QMLFastImageView/fastimageitem.h ../../QtLibraries/QMLFastImageView/fastimageitem.cpp
        ../../QtLibraries/dahua/dahuadoorcontroller.h ../../QtLibraries/dahua/dahuadoorcontroller.cpp
)

qt6_add_resources(QMLCamera "qmlCamera"
    PREFIX "/"
    FILES
        version.txt
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(QMLCamera PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QMLCamera
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(ANDROID)
    set_target_properties(QMLCamera PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR
            ${CMAKE_CURRENT_SOURCE_DIR}/android
            QT_ANDROID_VERSION_NAME "${VERSION_STRING}"
            QT_ANDROID_VERSION_CODE "${VERSION_CODE}"
    )
endif()

target_link_libraries(QMLCamera
    PRIVATE
        Qt6::Quick
        Qt6::Multimedia
)

include(GNUInstallDirs)
install(TARGETS QMLCamera
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
