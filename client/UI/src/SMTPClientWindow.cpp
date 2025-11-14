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

    m_Settings.server = "smtp.example.com";
    m_Settings.username = "user";
    m_Settings.password = "pass";
    m_Settings.port = 587;
    m_Settings.logLevel = "DEBUG";

    m_lastFromAddress = m_Settings.username;

    updateLog("Ready. Please configure server settings before sending.");
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
    QSplitter *hSplitter = new QSplitter(Qt::Horizontal);

    navigationListWidget = new QListWidget;
    navigationListWidget->addItem(new QListWidgetItem(QIcon::fromTheme("mail-send"), "Sent"));
    navigationListWidget->addItem(new QListWidgetItem(QIcon::fromTheme("mail-receive"), "Inbox (nyi)"));    navigationListWidget->setMaximumWidth(150);
    hSplitter->addWidget(navigationListWidget);

    mainContentStack = new QStackedWidget;

    sentItemsListWidget = new QListWidget;
    mainContentStack->addWidget(sentItemsListWidget);

    QLabel* inboxPlaceholder = new QLabel("Inbox functionality is not implemented.");
    inboxPlaceholder->setAlignment(Qt::AlignCenter);
    mainContentStack->addWidget(inboxPlaceholder);

    hSplitter->addWidget(mainContentStack);
    hSplitter->setStretchFactor(1, 1);

    connect(navigationListWidget, &QListWidget::currentRowChanged,
            this, &SmtpClientWindow::onNavigationChanged);

    navigationListWidget->setCurrentRow(0);

    return hSplitter;
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
    mainContentStack->setCurrentIndex(index);
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
    loggerCombo->setCurrentIndex(m_Settings.securityType);
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

    // ** Add the sent email to our list **
    if (!m_pendingEmail.subject.isEmpty() || !m_pendingEmail.to.isEmpty())
    {
        // Add to our internal data list
        m_sentEmails.prepend(m_pendingEmail);

        // Create a display string for the list widget
        QString to = m_pendingEmail.to.join(", ");
        QString subject = m_pendingEmail.subject.isEmpty() ? "(No Subject)" : m_pendingEmail.subject;
        QString itemText = QString("To: %1  |  %2").arg(to, subject);

        // Add to the top of the visible list
        QListWidgetItem *item = new QListWidgetItem(itemText);
        item->setToolTip(m_pendingEmail.body); // Show body on hover
        sentItemsListWidget->insertItem(0, item);
    }

    m_pendingEmail = Email(); // Clear the pending email
}

void SmtpClientWindow::onSendFailed(const QString &error)
{
    updateLog(tr("Failed to send message! Reason: %1").arg(error));
    statusBar()->showMessage("Failed to send message!", 5000);
    sendButton->setEnabled(true);
    m_pendingEmail = Email();
    QMessageBox::critical(this, "Send Error",  tr("Could not send the email. Reason: %1").arg(error));
}
