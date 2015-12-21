CONFIG += c++11

INCLUDEPATH += ../include ../src
LIBS += -L ../src -lyaml-cpp -Wl,-rpath=../src

QT -= core gui
