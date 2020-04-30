#-------------------------------------------------
#
# Project created by QtCreator 2019-05-09T15:24:49
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Simple_LableMe
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        simplelableme.cpp \
    logger_class.cpp

HEADERS += \
        simplelableme.h \
    logger_class.h

FORMS += \
        simplelableme.ui
INCLUDEPATH += D:/opencv_3_4_1/include \
                D:/opencv_3_4_1/include/opencv \
                D:/opencv_3_4_1/include/opencv2

INCLUDEPATH +=D:/Anaconda3/include

#LIBS +=D:/Anaconda3/libs/python37.lib
CONFIG(release, debug|release){
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_imgproc341
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_highgui341
 LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_imgcodecs341
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_video341
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_videoio341
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_core341
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_ccalib341
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_calib3d341
}
else
{
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_imgproc341d
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_highgui341d
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_imgcodecs341d
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_video341d
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_videoio341d
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_core341d
LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_ccalib341d

LIBS += -L$$PWD/../../../opencv_3_4_1/lib/ -lopencv_calib3d341d

}

UI_DIR=./UI

TRANSLATIONS    += simpel_LabelMe_zh.ts