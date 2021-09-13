QT += quick quickcontrols2

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


# ====================================== Debug ======================================
linux-g++ | linux-g++-64 | linux-g++-32 {
CONFIG += c++17 console
LIBS += -lstdc++fs -lpthread;
}

INCLUDEPATH += $$PWD/BinOM/src/
HEADERS += \
    BinOM/src/binom/includes/file_storage/file_memory_manager.h \
    BinOM/src/binom/includes/file_storage/file_node_visitor.h \
    BinOM/src/binom/includes/file_storage/file_storage.h \
    BinOM/src/binom/includes/file_storage/fmm_containers.h \
    BinOM/src/binom/includes/utils/file.h \
    BinOM/src/binom/includes/utils/node_visitor_base.h \
    BinOM/src/binom/includes/utils/var_mem_mngr.h \
    BinOM/src/binom/includes/binom.h \
    BinOM/src/binom/includes/variables/variable.h \
    BinOM/src/binom/includes/variables/primitive.h \
    BinOM/src/binom/includes/variables/buffer_array.h \
    BinOM/src/binom/includes/variables/array.h \
    BinOM/src/binom/includes/variables/object.h \
    BinOM/src/binom/includes/variables/value.h \
    BinOM/src/binom/includes/utils/node_visitor.h \
    BinOM/src/binom/includes/utils/ctypes.h \
    BinOM/src/binom/includes/utils/types.h \
    BinOM/src/binom/includes/utils/mem.h \
    BinOM/src/binom/includes/utils/exception.h \
    BinOM/src/binom/includes/utils/rwguard.h \
    BinOM/src/binom/includes/utils/path.h \
    BinOM/src/binom/includes/utils/query.h \
    BinOM/src/binom/includes/utils/byte_array.h \
    BinOM/src/binom/includes/file_storage/file_structs.h \

SOURCES += \
    BinOM/src/binom/sources/utils/TestExpressionFrame.cpp \
    BinOM/src/binom/sources/file_storage/file_node_visitor.cpp \
    BinOM/src/binom/sources/binom.cpp \
    BinOM/src/binom/sources/variables/variable.cpp \
    BinOM/src/binom/sources/variables/primitive.cpp \
    BinOM/src/binom/sources/variables/buffer_array.cpp \
    BinOM/src/binom/sources/variables/array.cpp \
    BinOM/src/binom/sources/variables/object.cpp \
    BinOM/src/binom/sources/variables/value.cpp \
    BinOM/src/binom/sources/utils/node_visitor.cpp\
    BinOM/src/binom/sources/utils/byte_array.cpp \
    BinOM/src/binom/sources/utils/mem.cpp \
    BinOM/src/binom/sources/utils/path.cpp \
    BinOM/src/binom/sources/utils/query.cpp \
    BinOM/src/binom/sources/file_storage/file_memory_manager.cpp

# ====================================== Debug ======================================




SOURCES += \
        main.cpp \
        qbinom.cpp \
        qbinommodel.cpp

RESOURCES += qml.qrc
#INCLUDEPATH += $$PWD/include/
#LIBS += -L$$PWD/lib/ -lbinom -lstdc++fs

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    qtquickcontrols2.conf

HEADERS += \
    qbinom.h \
    qbinommodel.h
