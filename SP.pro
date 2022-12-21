TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    TestApp.c \
    reconf/loadplnm.c \
    reconf/reconfiguration.c \
    reconf/strpolinom.c
