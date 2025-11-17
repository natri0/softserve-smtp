#ifndef SMTPCLIENTWINDOW_H
#define SMTPCLIENTWINDOW_H

#include <QMainWindow>
#include <QList>
#include "Structures.h"

class QLineEdit;
class QTextEdit;
class QPlainTextEdit;
class QPushButton;
class QListWidget;
class QSplitter;
class QStackedWidget;
class SmtpController;
class QListWidgetItem;

class SmtpClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    SmtpClientWindow(QWidget *parent = nullptr);
    ~SmtpClientWindow();

signals:
    void statusUpdated(const QString& message);

private slots:
    void onCompose();
    void onNavigationChanged(int index);
    void onConfigureServer();

    void updateLog(const QString &message);
    void onSendSuccess();
    void onSendFailed(const QString &error);

    void onSentItemClicked(QListWidgetItem *item);
    void onInboxItemClicked(QListWidgetItem *item);

private:
    void setupUi();
    void createToolBar();
    QWidget* createMainLayout();
    QWidget* createLogPanel();

    QWidget* createEmailDetailWidget();
    void displayEmail(const Email& email);
private:
    SmtpSettings m_Settings;
    SmtpController *m_Controller;

    QListWidget *navigationListWidget;
    QStackedWidget *listStack;
    QListWidget *sentItemsListWidget;
    QListWidget *inboxListWidget;

    QPushButton *sendButton;
    QPlainTextEdit *logTextEdit;

    Email m_pendingEmail;
    QList<Email> m_sentEmails;
    QString m_lastFromAddress;

    QList<Email> m_receivedEmails;

    void addEmailToSentList(const Email& email);
    void addEmailToInboxList(const Email& email);
    void addMockSentEmails();
    void addMockInboxEmails();

    QWidget* emailDetailWidget;
    QLineEdit* detailFromLineEdit;
    QLineEdit* detailToLineEdit;
    QLineEdit* detailSubjectLineEdit;
    QTextEdit* detailBodyTextEdit;
};

#endif // SMTPCLIENTWINDOW_H
