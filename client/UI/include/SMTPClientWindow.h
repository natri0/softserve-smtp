#ifndef SMTPCLIENTWINDOW_H
#define SMTPCLIENTWINDOW_H

#include <QMainWindow>
#include "Structures.h"

class QLineEdit;
class QTextEdit;
class QPlainTextEdit;
class QPushButton;
class QListWidget;
class QSplitter;
class SmtpController;

class SmtpClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    SmtpClientWindow(QWidget *parent = nullptr);
    ~SmtpClientWindow();

private slots:
    void onSend();
    void onAddAttachment();
    void onRemoveAttachment();
    void onConfigureServer();

    void updateLog(const QString &message);
    void onSendSuccess();
    void onSendFailed(const QString &error);

private:
    void setupUi();
    void createToolBar();
    QWidget* createComposerWidget();
    QWidget* createLogPanel();

private:
    SmtpSettings m_Settings;
    SmtpController *m_Controller;

    QLineEdit *fromLineEdit;
    QLineEdit *toLineEdit;
    //QLineEdit *ccLineEdit;
    QLineEdit *subjectLineEdit;
    QTextEdit *bodyTextEdit;

    QListWidget *attachmentsListWidget;
    QPushButton *addAttachmentButton;
    QPushButton *removeAttachmentButton;

    QPushButton *sendButton;

    QPlainTextEdit *logTextEdit;
};

#endif // SMTPCLIENTWINDOW_H
