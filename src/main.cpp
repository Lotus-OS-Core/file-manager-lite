#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "mainwindow.h"

void loadStyleSheet(QApplication& app, bool darkMode) {
    QString styleFile = darkMode ? ":/qss/dark.qss" : ":/qss/light.qss";
    QFile file(styleFile);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        app.setStyleSheet(stream.readAll());
        file.close();
    } else {
        qWarning() << "Failed to load stylesheet:" << styleFile;
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("Lotus-DIR");
    app.setApplicationDisplayName("Lotus-DIR");
    app.setOrganizationName("Lotus-DIR");
    app.setOrganizationDomain("lotus-dir.local");
    app.setWindowIcon(QIcon(":/icons/app.png"));
    
    // Load default style (light)
    loadStyleSheet(app, false);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
