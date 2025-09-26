QT += widgets core gui
CONFIG += c++17
TEMPLATE = app
TARGET = EmploymentHelper

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    flowlayout.cpp \
    platformhelper.cpp

HEADERS += \
    mainwindow.h \
    flowlayout.h \
    types.h \
    platformhelper.h

# 资源（如果后续添加图标等，可启用）
# RESOURCES += resources.qrc

# 高 DPI 支持
DEFINES += QT_DEPRECATED_WARNINGS

# macOS 特定设置
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0
    ICON = app.icns
}

# Windows: 启用控制台输出（调试用，如需隐藏控制台可去掉下面一行）
# CONFIG += console
# release: CONFIG -= console

# 其他文件（不会编译，仅展示 / 便于在 Qt Creator 中可见）
OTHER_FILES += \
    resume_data.json