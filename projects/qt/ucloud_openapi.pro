TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/local/ssl/include
INCLUDEPATH += ../../src

SOURCES += ../../example/ucloud.cpp
SOURCES += ../../src/util.cpp
SOURCES += ../../src/HttpRequest.cpp
SOURCES += ../../src/TokenRequest.cpp
SOURCES += ../../src/AccessRequest.cpp
SOURCES += ../../src/BaseRequest.cpp
SOURCES += ../../src/UcloudOpenApi.cpp

HEADERS += ../../src/util.h
HEADERS += ../../src/HttpRequest.h
HEADERS += ../../src/TokenRequest.h
HEADERS += ../../src/AccessRequest.h
HEADERS += ../../src/BaseRequest.h
HEADERS += ../../src/UcloudOpenApi.h

LIBS += -L/usr/lib -lcurl
LIBS += -L/usr/local/ssl/lib -lssl -lcrypto
