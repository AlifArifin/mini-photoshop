QT = core gui
QT += charts
CONFIG+=c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    binary.cpp \
    bitplane.cpp \
    dialoghistogram.cpp \
    formcontrast.cpp \
    formgrayslicing.cpp \
    formtransformationlog.cpp \
    formtranslation.cpp \
    grayscale.cpp \
    image.cpp \
    image_format.cpp \
    image_type.cpp \
    imagepreview.cpp \
    inputdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    monochrome.cpp \
    tabimage.cpp \
    tabpage.cpp \
    truecolor.cpp \
    utils.cpp

FORMS += \
    bitplane.ui \
    dialoghistogram.ui \
    formcontrast.ui \
    formgrayslicing.ui \
    formtransformationlog.ui \
    formtranslation.ui \
    imagepreview.ui \
    inputdialog.ui \
    mainwindow.ui \
    tabpage.ui

HEADERS += \
    binary.h \
    bitplane.h \
    bmp_header.h \
    convolution.h \
    dialoghistogram.h \
    edge_detection.h \
    formcontrast.h \
    formgrayslicing.h \
    formtransformationlog.h \
    formtranslation.h \
    geometry.h \
    grayscale.h \
    histogram.h \
    image.h \
    image_format.h \
    image_type.h \
    imagepreview.h \
    inputdialog.h \
    mainwindow.h \
    mapping.h \
    monochrome.h \
    operation.h \
    padding.h \
    pbm_state.h \
    pgm_state.h \
    ppm_state.h \
    resolution.h \
    rgba.h \
    tabimage.h \
    tabpage.h \
    transformation.h \
    truecolor.h \
    utils.h
