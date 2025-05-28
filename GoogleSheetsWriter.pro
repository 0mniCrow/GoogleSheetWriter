#-------------------------------------------------
#
# Project created by QtCreator 2025-04-24T13:46:28
#
#-------------------------------------------------

QT       += core gui network networkauth xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GoogleSheetsWriter
TEMPLATE = app


SOURCES += main.cpp\
    filemanager.cpp \
        googlesheetsmodifier.cpp \
    httpscommunicator.cpp \
    jsonparser.cpp \
    googlesheetmodel.cpp \
    xmlparser.cpp

HEADERS  += googlesheetsmodifier.h \
    filemanager.h \
    httpscommunicator.h \
    jsonparser.h \
    googlesheetmodel.h \
    xmlparser.h

FORMS    += googlesheetsmodifier.ui
