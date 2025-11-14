#ifndef SMTPCLIENTWINDOW_H
#define SMTPCLIENTWINDOW_H

#include <QMainWindow>
#include <QList>
#include "Structures.h"

class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QListWidget;
class QSplitter;
class QStackedWidget;
class SmtpController;

class SmtpClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    SmtpClientWindow(QWidget *parent = nullptr);
    ~SmtpClientWindow();

private slots:
    void onCompose();
    void onNavigationChanged(int index);
    void onConfigureServer();

    void updateLog(const QString &message);
    void onSendSuccess();
    void onSendFailed(const QString &error);

private:
    void setupUi();
    void createToolBar();
    QWidget* createMainLayout();
    QWidget* createLogPanel();

private:
    SmtpSettings m_Settings;
    SmtpController *m_Controller;

    QListWidget *navigationListWidget;
    QStackedWidget *mainContentStack;
    QListWidget *sentItemsListWidget;

    QPushButton *sendButton;
    QPlainTextEdit *logTextEdit;

    Email m_pendingEmail;
    QList<Email> m_sentEmails;
    QString m_lastFromAddress;
};

#endif // SMTPCLIENTWINDOW_H
