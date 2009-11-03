HEADERS = Configs.h \
    openvpn.h \
    preferences.h \
    openvpnqlistitem.h \
    appfunc.h \
    wiz_startpage.h \
    wiz_generalpage.h \
    wiz_remotepage.h \
    wiz_endpage.h \
    wiz_vpnwizard.h \
    wiz_certpage.h \
    wiz_advpage.h \
    stduserauth.h \
    pkcs_auth.h \
    appinfo.h \
    configexport.h \
    importconfig.h \
    deleteconfig.h \
    tapdriver.h \
    single_application.h \
    proxysettings.h \
    vpnlog.h \
    editconfig.h \
    tapinfo.h
SOURCES = main.cpp \
    Configs.cpp \
    openvpn.cpp \
    preferences.cpp \
    openvpnqlistitem.cpp \
    appfunc.cpp \
    wiz_startpage.cpp \
    wiz_generalpage.cpp \
    wiz_remotepage.cpp \
    wiz_endpage.cpp \
    wiz_vpnwizard.cpp \
    wiz_certpage.cpp \
    wiz_advpage.cpp \
    stduserauth.cpp \
    pkcs_auth.cpp \
    appinfo.cpp \
    configexport.cpp \
    importconfig.cpp \
    deleteconfig.cpp \
    tapdriver.cpp \
    single_application.cpp \
    proxysettings.cpp \
    vpnlog.cpp \
    editconfig.cpp \
    tapinfo.cpp
RESOURCES = systray.qrc
QT += xml \
    svg

# install
target.path = $$[QT_INSTALL_EXAMPLES]/desktop/systray
sources.files = $$SOURCES \
    $$HEADERS \
    $$RESOURCES \
    $$FORMS \
    systray.pro \
    resources \
    images
sources.path = $$[QT_INSTALL_EXAMPLES]/desktop/systray
INSTALLS += target \
    sources
wince* { 
    CONFIG(debug, release|debug):addPlugins.sources = $$QT_BUILD_TREE/plugins/imageformats/qsvgd4.dll
    CONFIG(release, release|debug):addPlugins.sources = $$QT_BUILD_TREE/plugins/imageformats/qsvg4.dll
    addPlugins.path = imageformats
    DEPLOYMENT += addPlugins
}
FORMS += preferences.ui \
    wiz_startpage.ui \
    wiz_generalpage.ui \
    wiz_remotepage.ui \
    wiz_endpage.ui \
    wiz_certpage.ui \
    wiz_advpage.ui \
    stduserauth.ui \
    pkcs_auth.ui \
    appinfo.ui \
    configexport.ui \
    importconfig.ui \
    deleteconfig.ui \
    proxysettings.ui \
    vpnlog.ui \
    editconfig.ui \
    tapinfo.ui
LIBS += -L"../../../../mingw/lib"\
        -liphlpapi
RC_FILE = myapp.rc


