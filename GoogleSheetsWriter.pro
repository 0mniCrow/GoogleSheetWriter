#-------------------------------------------------
#
# Project created by QtCreator 2025-04-24T13:46:28
#
#-------------------------------------------------

QT       += core gui network networkauth

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GoogleSheetsWriter
TEMPLATE = app


SOURCES += main.cpp\
    filemanager.cpp \
        googlesheetsmodifier.cpp \
    httpscommunicator.cpp \
    jsonparser.cpp \
    googlesheetmodel.cpp

HEADERS  += googlesheetsmodifier.h \
    filemanager.h \
    httpscommunicator.h \
    jsonparser.h \
    googlesheetmodel.h

FORMS    += googlesheetsmodifier.ui
