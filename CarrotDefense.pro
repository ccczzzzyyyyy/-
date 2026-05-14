QT       += core gui widgets

CONFIG   += c++17

RESOURCES += resources.qrc

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    gamescene.cpp \
    gameview.cpp \
    map.cpp \
    tower.cpp \
    enemy.cpp \
    projectile.cpp \
    wavecontroller.cpp \
    gamemanager.cpp \
    towerpanel.cpp \
    towereffect.cpp

HEADERS += \
    mainwindow.h \
    gamescene.h \
    gameview.h \
    map.h \
    tower.h \
    enemy.h \
    projectile.h \
    wavecontroller.h \
    gamemanager.h \
    towerpanel.h \
    towereffect.h

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
