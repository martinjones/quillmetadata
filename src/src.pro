##########
# the project file in src, it is not created automatically/
##########

TEMPLATE = lib
TARGET = quillmetadata
# Please do not remove this INCLUDEPATH in any case
#INCLUDEPATH += . $$[QT_INSTALL_HEADERS]/quillimagefilter
DEPENDPATH += .

DEFINES     +=
MOC_DIR = .moc

CONFIG += release

#CONFIG += quillimagefilter

LIBS += -lexif -lexempi
# Generate pkg-config support by default
# Note that we HAVE TO also create prl config as QMake implementation
# mixes both of them together.
CONFIG += create_pc create_prl no_install_prl

#QMAKE_PKGCONFIG_REQUIRES = quillimagefilter QtGui
QMAKE_PKGCONFIG_INCDIR = $$[QT_INSTALL_HEADERS]/$$TARGET
QMAKE_PKGCONFIG_LIBDIR = $$[QT_INSTALL_LIBS]

QMAKE_CXXFLAGS += -Werror
# this is for adding coverage information while doing qmake as "qmake COV_OPTION=on"
# message is shown when 'make' is executed
for(OPTION,$$list($$lower($$COV_OPTION))){
    isEqual(OPTION, on){
        message("TEST COVERAGE IS ENABLED")
        QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs -fno-elide-constructors
        LIBS += -lgcov
    }
}

# --- input

HEADERS += quillmetadata.h \

SOURCES += quillmetadata.cpp \

INSTALL_HEADERS = QuillMetadata \
                  quillmetadata.h \

# --- install
headers.files = $$INSTALL_HEADERS
headers.path = $$[QT_INSTALL_HEADERS]/$$TARGET
target.path = $$[QT_INSTALL_LIBS]
pkgconfig.files = quill.pc
pkgconfig.path = $$[QT_INSTALL_LIBS]/pkgconfig
prf.files = quill.prf
prf.path = $$[QMAKE_MKSPECS]/features
INSTALLS += target headers pkgconfig prf


QMAKE_CLEAN += *.gcov *.gcno *.log *.moc_* *.gcda

