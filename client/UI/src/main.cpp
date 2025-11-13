#include "SmtpClientWindow.h"
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  // NOW set the plugin paths (after app is created)
  QString appDir = QCoreApplication::applicationDirPath();
  QCoreApplication::addLibraryPath(appDir + "/platforms");
  QCoreApplication::addLibraryPath(appDir);

  // Debug: verify paths
  qDebug() << "App directory:" << appDir;
  qDebug() << "Library paths:" << QCoreApplication::libraryPaths();

    QFile file(":/styles/style.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        a.setStyleSheet(stream.readAll());
        file.close();
    }

    SmtpClientWindow w;
    w.show();
    return a.exec();
}
