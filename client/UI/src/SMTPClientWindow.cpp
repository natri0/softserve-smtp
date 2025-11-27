#include "SMTPClientWindow.h"
#include "SMTPController.h"
#include "ComposeDialog.h"

#include <QtWidgets>

SmtpClientWindow::SmtpClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_Controller(new SmtpController(this))
{
    setupUi();

    connect(m_Controller, &SmtpController::statusUpdated, this, &SmtpClientWindow::updateLog);
    connect(m_Controller, &SmtpController::sendSuccess, this, &SmtpClientWindow::onSendSuccess);
    connect(m_Controller, &SmtpController::sendFailed, this, &SmtpClientWindow::onSendFailed);

    m_Settings.server = "127.0.0.1";
    m_Settings.username = "user@smtp.test";
    m_Settings.password = "pass";
    m_Settings.port = 12345;
    m_Settings.logLevel = "DEBUG";
    m_Settings.securityType = 1;

    m_lastFromAddress = m_Settings.username;

    m_Controller->init(m_Settings);

    addMockSentEmails();
    addMockInboxEmails();

    updateLog("Ready. Please configure server settings before sending.");

    if (!m_receivedEmails.isEmpty()) {
        displayEmail(m_receivedEmails.first());
        inboxListWidget->setCurrentRow(0);
    }
}

SmtpClientWindow::~SmtpClientWindow()
{
}

void SmtpClientWindow::setupUi()
{
    setWindowTitle("Qt SMTP Client");
    setMinimumSize(800, 700);

    createToolBar();

    QWidget *mainLayout = createMainLayout();
    QWidget *logPanel = createLogPanel();

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(mainLayout);
    mainSplitter->addWidget(logPanel);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 1);

    setCentralWidget(mainSplitter);
}

void SmtpClientWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar("Main");
    sendButton = new QPushButton(QIcon::fromTheme("mail-send"), " Send");
    sendButton->setObjectName("PrimaryButton");
    sendButton->setShortcut(QKeySequence::Save);
    connect(sendButton, &QPushButton::clicked, this, &SmtpClientWindow::onCompose);

    QPushButton *settingsButton = new QPushButton(QIcon::fromTheme("configure"), " Settings");
    connect(settingsButton, &QPushButton::clicked, this, &SmtpClientWindow::onConfigureServer);

    toolBar->addWidget(sendButton);
    toolBar->addWidget(settingsButton);
}


QWidget* SmtpClientWindow::createMainLayout()
{
    QWidget *mainWidget = new QWidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    navigationListWidget = new QListWidget;
    navigationListWidget->addItem(new QListWidgetItem(QIcon::fromTheme("mail-send"), "Sent"));
    navigationListWidget->addItem(new QListWidgetItem(QIcon::fromTheme("mail-receive"), "Inbox"));
    navigationListWidget->setMaximumWidth(150);
    mainLayout->addWidget(navigationListWidget);

    listStack = new QStackedWidget;

    sentItemsListWidget = new QListWidget;
    listStack->addWidget(sentItemsListWidget);

    inboxListWidget = new QListWidget;
    listStack->addWidget(inboxListWidget);

    emailDetailWidget = createEmailDetailWidget();

    QSplitter *contentSplitter = new QSplitter(Qt::Horizontal);
    contentSplitter->addWidget(listStack);
    contentSplitter->addWidget(emailDetailWidget);
    contentSplitter->setStretchFactor(0, 1);
    contentSplitter->setStretchFactor(1, 2);

    mainLayout->addWidget(contentSplitter, 1);

    connect(navigationListWidget, &QListWidget::currentRowChanged,
            this, &SmtpClientWindow::onNavigationChanged);

    connect(sentItemsListWidget, &QListWidget::itemClicked,
            this, &SmtpClientWindow::onSentItemClicked);

    connect(inboxListWidget, &QListWidget::itemClicked,
            this, &SmtpClientWindow::onInboxItemClicked);

    navigationListWidget->setCurrentRow(1);

    return mainWidget;
}

QWidget* SmtpClientWindow::createLogPanel()
{
    QGroupBox* logGroup = new QGroupBox("Communication Log");
    logTextEdit = new QPlainTextEdit;
    logTextEdit->setReadOnly(true);
    logTextEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    QVBoxLayout* layout = new QVBoxLayout(logGroup);
    layout->addWidget(logTextEdit);
    return logGroup;
}

void SmtpClientWindow::onNavigationChanged(int index)
{
    listStack->setCurrentIndex(index);
    displayEmail(Email());
}

void SmtpClientWindow::onCompose()
{
    ComposeDialog dialog(this);
    dialog.setFrom(m_lastFromAddress);

    if (dialog.exec() == QDialog::Accepted)
    {
        Email email = dialog.getEmail();

        m_pendingEmail = email;
        m_lastFromAddress = email.from;

        sendButton->setEnabled(false); // Disable "Send" button while sending
        statusBar()->showMessage("Sending...");
        m_Controller->sendEmail(email, m_Settings);
    }
}


void SmtpClientWindow::onConfigureServer()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Server Configuration");

    QFormLayout form(&dialog);

    QLineEdit *serverEdit = new QLineEdit(m_Settings.server);
    form.addRow("Server:", serverEdit);

    QSpinBox *portSpin = new QSpinBox();
    portSpin->setRange(1, 65535);
    portSpin->setValue(m_Settings.port);
    form.addRow("Port:", portSpin);

    QComboBox *securityCombo = new QComboBox();
    securityCombo->addItems({"None", "SSL/TLS"});
    securityCombo->setCurrentIndex(m_Settings.securityType);
    form.addRow("Security:", securityCombo);

    QComboBox *loggerCombo = new QComboBox();
    loggerCombo->addItems({"NONE", "PROD", "DEBUG", "TRACE"});
    int logIndex = loggerCombo->findText(m_Settings.logLevel);
    loggerCombo->setCurrentIndex(logIndex > -1 ? logIndex : 2);
    form.addRow("Logger level:", loggerCombo);

    QLineEdit *userEdit = new QLineEdit(m_Settings.username);
    form.addRow( "Username:", userEdit);

    QLineEdit *passEdit = new QLineEdit(m_Settings.password);
    passEdit->setEchoMode(QLineEdit::Password);
    form.addRow( "Password:", passEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        m_Settings.server = serverEdit->text();
        m_Settings.port = portSpin->value();
        m_Settings.securityType = securityCombo->currentIndex();
        m_Settings.username = userEdit->text();
        m_Settings.password = passEdit->text();
        m_Settings.logLevel = loggerCombo->currentText();

        m_Controller->updateSettings(m_Settings);
        m_lastFromAddress = m_Settings.username;
        updateLog("Server configuration updated.");
    }
}

void SmtpClientWindow::updateLog(const QString &message)
{
    logTextEdit->appendPlainText(QTime::currentTime().toString("hh:mm:ss") + "> " + message);
}

void SmtpClientWindow::onSendSuccess()
{
    updateLog("Message sent successfully");
    statusBar()->showMessage("Message sent successfully!", 3000);
    sendButton->setEnabled(true);

    addEmailToSentList(m_pendingEmail);

    m_pendingEmail = Email();
}

void SmtpClientWindow::onSendFailed(const QString &error)
{
    updateLog(tr("Failed to send message! Reason: %1").arg(error));
    statusBar()->showMessage("Failed to send message!", 5000);
    sendButton->setEnabled(true);
    m_pendingEmail = Email();
    QMessageBox::critical(this, "Send Error",  tr("Could not send the email. Reason: %1").arg(error));
}

void SmtpClientWindow::onSentItemClicked(QListWidgetItem *item)
{
    int row = sentItemsListWidget->row(item);
    if (row >= 0 && row < m_sentEmails.count()) {
        displayEmail(m_sentEmails.at(row));
    }
}

void SmtpClientWindow::onInboxItemClicked(QListWidgetItem *item)
{
    int row = inboxListWidget->row(item);
    if (row >= 0 && row < m_receivedEmails.count()) {
        displayEmail(m_receivedEmails.at(row));
    }
}

QWidget* SmtpClientWindow::createEmailDetailWidget()
{
    QFrame *detailFrame = new QFrame;
    detailFrame->setFrameShape(QFrame::StyledPanel);

    QVBoxLayout *mainLayout = new QVBoxLayout(detailFrame);

    QFormLayout *headerLayout = new QFormLayout;
    headerLayout->setContentsMargins(5, 5, 5, 5);

    detailFromLineEdit = new QLineEdit;
    detailFromLineEdit->setReadOnly(true);
    detailFromLineEdit->setFrame(false);

    detailToLineEdit = new QLineEdit;
    detailToLineEdit->setReadOnly(true);
    detailToLineEdit->setFrame(false);

    detailSubjectLineEdit = new QLineEdit;
    detailSubjectLineEdit->setReadOnly(true);
    detailSubjectLineEdit->setFrame(false);

    QFont subjectFont = detailSubjectLineEdit->font();
    subjectFont.setBold(true);
    detailSubjectLineEdit->setFont(subjectFont);

    headerLayout->addRow("From:", detailFromLineEdit);
    headerLayout->addRow("To:", detailToLineEdit);
    headerLayout->addRow("Subject:", detailSubjectLineEdit);

    detailBodyTextEdit = new QTextEdit;
    detailBodyTextEdit->setReadOnly(true);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(detailBodyTextEdit, 1);

    return detailFrame;
}

void SmtpClientWindow::displayEmail(const Email& email)
{
    detailFromLineEdit->setText(email.from);
    detailToLineEdit->setText(email.to.join(", "));
    detailSubjectLineEdit->setText(email.subject);
    detailBodyTextEdit->setPlainText(email.body);
}

void SmtpClientWindow::addEmailToSentList(const Email& email)
{
    m_sentEmails.prepend(email);

    QString to = email.to.join(", ");
    QString subject = email.subject;
    QString itemText = QString("To: %1  |  %2").arg(to, subject);

    QListWidgetItem *item = new QListWidgetItem(itemText);
    sentItemsListWidget->insertItem(0, item);
}

void SmtpClientWindow::addEmailToInboxList(const Email& email)
{
    m_receivedEmails.prepend(email);

    QString from = email.from;
    QString subject = email.subject;
    QString itemText = QString("From: %1  |  %2").arg(from, subject);

    QListWidgetItem *item = new QListWidgetItem(itemText);
    inboxListWidget->insertItem(0, item);
}

void SmtpClientWindow::addMockSentEmails()
{
    Email email1;
    email1.from = m_lastFromAddress;
    email1.to = {"test@tester.com"};
    email1.subject = "Mock Sent Email 1";
    email1.body = "Mock email1 set.1";
    addEmailToSentList(email1);

    Email email2;
    email2.from = m_lastFromAddress;
    email2.to = {"greattest@tester.com", "notgreattest@tester.com"};
    email2.subject = "Mock Sent Email 2";
    email2.body = "Mock email1 set.2";
    addEmailToSentList(email2);
}

void SmtpClientWindow::addMockInboxEmails()
{
    Email email1;
    email1.from = "support@tester.io";
    email1.to = {m_lastFromAddress};
    email1.subject = "Mock Received Email 1";
    email1.body = "Mock email received 1";
    addEmailToInboxList(email1);

    Email email2;
    email2.from = "support@example.com";
    email2.to = {m_lastFromAddress};
    email2.subject = "Mock Received Email 2";
    email2.body = "Mock email received 2";
    addEmailToInboxList(email2);

    Email email3;
    email3.from = "noreply@github.com";
    email3.to = {m_lastFromAddress};
    email3.subject = "Mock Received Email 3";
    email3.body = "Mock email received 3";
    addEmailToInboxList(email3);
}
