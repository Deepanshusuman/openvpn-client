
#include <QtCore>
#include <QtGui>

#include "preferences.h"
#include "single_application.h"

// Settings Singleton
#include "settings.h"
#include "message.h"
#include <debug.h>
#include <appfunc.h>

#include <QtSql>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(systray);

    // Die Anwenung ist eine Single Application,
    // wenn eine Instanz schon offen ist,
    // die neue geschlossen und von der bereits
    // vorhandenen das Statusfenster ge�ffnet
    SingleApplication app(argc, argv, "1x4z37");

    if (app.isRunning()){
        app.sendMessage("Someone is out there");
        return 0;
    }
    // Config aus Textdatei auslesen
    // Commandline Parameter haben eine h�here Prio als


    // Plugins laden
    app.addLibraryPath(app.applicationDirPath() + QLatin1String("/plugins"));

    // RC 4: Einstellungen werden aus einer config gelesen
    //
    Settings::getInstance();
    Settings::getInstance()->refresh();
    Debug::enableDebugging(false);
    // Commandline Parameter �bergeben
    for (int x = 0; x < argc; x++) {
        if (0 == strcmp(argv[x], "-noService")) {
            // Es soll kein Dienst verwendet werden
            Settings::getInstance()->setIsRunAsService(false);
        } else if (0 == strcmp(argv[x], "-reconnect")) {
            // Reconnect beim Trennen
            Settings::getInstance()->setIsAutoReconnect(true);
        } else if (0 == strcmp(argv[x], "-noBallon")) {
            // Keine Ballon Messages im Systray anzeigen
            Settings::getInstance()->setIsShowNoBallonMessage(true);
        } else if (0 == strcmp(argv[x], "-noPopUp")) {
            // Kein Start Popup
            Settings::getInstance()->setPopUpDialog(false);
        } else if (0 == strcmp(argv[x], "-noSplash")) {
            // Kein Start Splashscreen
            Settings::getInstance()->setShowSplashScreen(false);
        } else if (0 == strcmp(argv[x], "-port") && x + 1 < argc) {
            // Der POrt f�r den Client
            Settings::getInstance()->setListenPort(static_cast<quint16>(QString(argv[x + 1]).toInt()));
        } else if (0 == strcmp(argv[x], "-servicePort") && x + 1 < argc) {
            // Der Port f�r den Service
            Settings::getInstance()->setServerPort(static_cast<quint16>(QString(argv[x + 1]).toInt()));
        } else if (0 == strcmp(argv[x], "-useEnglish")) {
            // Client st in Englisch
            Settings::getInstance()->setIsLanguageGerman(false);
        } else if (0 == strcmp(argv[x], "-portable")) {
            // CLient installiert TAP
            Settings::getInstance()->setIsPortableClient(true);
        } else if (0 == strcmp(argv[x], "-disableSaveData")) {
            // CLient darf user daten nicht speichern
            Settings::getInstance()->setDisableSaveData(true);
        } else if (0 == strcmp(argv[x], "-manage")) {
            // Benutzer kann Einstellungen �ndern
            Settings::getInstance()->setIsManageClient(true);
        } else if (0 == strcmp(argv[x], "-debug")) {
            // Benutzer kann Einstellungen �ndern
            Debug::enableDebugging(true);
        }
    }

    // Portabel bauen
    //Settings::getInstance()->setIsManageClient(true);
    Settings::getInstance()->setIsPortableClient(true);
    // Wenn Portable ist, immer mit lokalen VPN verbinden
    if (Settings::getInstance()->getIsPortableClient()) {
        Settings::getInstance()->setIsRunAsService(false);
    }

    Debug::setDebugPath(AppFunc::getAppSavePath());
    Debug::setDebugLevel(DebugLevel::All);
    Debug::enableMSecs(true);

    // Die �bersetzungsdatei laden
    QString trans_file ("SpSSLVpn_eng.qm");
    QString trans_dialog_file ("qt_eng.qm");
    if (Settings::getInstance()->getIsLanguageGerman()) {
        trans_file = QLatin1String("SpSSLVpn_ger.qm");
        trans_dialog_file = QLatin1String("qt_de.qm");
    }

    QTranslator translator;
    translator.load(trans_dialog_file, QApplication::applicationDirPath());
    translator.load(trans_file, QApplication::applicationDirPath());
    app.installTranslator(&translator);

    // Pr�fen, ob der Systray vorhanden ist,
    // dies muss bei !Windows nicht immer gegeben sein
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        Message::error(QObject::tr("No systray found on this system."), QObject::tr("Securepoint SSL VPN"));
        return 1;
    }

    // Die Anwendung soll nicht geschlossen werden, weil das Hauptfenster geschlossen wird
    // Appende ist nicht mit close sondern mit exit zu machen.
    QApplication::setQuitOnLastWindowClosed(false);

    // Splashscreen �ffnen
    QPixmap pixmap(":/images/startscreen.png");
    // Wenn im Anwendungsverzeichnis eine splash.png ist, diese als Startbildschirm laden
    QString splashPath(QCoreApplication::applicationDirPath() + QLatin1String("/splash.png"));
    //
    if (QFile::exists(splashPath)) {
        // Datei laden
        QPixmap tempSplash (splashPath);
        // Nun die Bild gr��e �berpr�fen es muss 480x320 sein
        if (tempSplash.height() == 320 && tempSplash.width() == 480) {
            // Alles Ok, das Bild setzen
            pixmap = tempSplash;
        }
    }
    QSplashScreen splash(pixmap);
    if (Settings::getInstance()->showSplashScreen()) {
        splash.show();
        app.processEvents();
        // Splash nach 3 Sekunden schlie�en
        QTimer::singleShot(3000, &splash, SLOT(close()));
    }

    // Main Window
    // Die Verbindung soll �ber den Dienst hergestellt werden
    // Es muss der Daemon gestartet werden, der versucht eine Verbindung zum
    // Dienst herzustellen. Bei einem Fehler erscheint eine Ballonmeldung
    if (Settings::getInstance()->getIsRunAsSevice()) {
        if (!Preferences::instance()->startDaemon()) {
            Message::error(QObject::tr("Can't connect to the ssl vpn service. Application is shuting down!"));
            return 1;
        }
    }

    // Konfigurationen suchen
    Preferences::instance()->searchStartConfigDir();
    QObject::connect(&app, SIGNAL(messageAvailable(QString)), &app, SLOT(receiveMessage(QString)));

    // Anwendung ohne Dialog starten
    return app.exec();
}
