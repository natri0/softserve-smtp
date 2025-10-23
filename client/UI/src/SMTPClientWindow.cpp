#include "SMTPClientWindow.h"
#include "SMTPController.h"

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

    QWidget *composerWidget = createComposerWidget();
    QWidget *logPanel = createLogPanel();

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(composerWidget);
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
    connect(sendButton, &QPushButton::clicked, this, &SmtpClientWindow::onSend);

    QPushButton *settingsButton = new QPushButton(QIcon::fromTheme("configure"), " Settings");
    connect(settingsButton, &QPushButton::clicked, this, &SmtpClientWindow::onConfigureServer);

    toolBar->addWidget(sendButton);
    toolBar->addWidget(settingsButton);
}


QWidget* SmtpClientWindow::createComposerWidget()
{
    QWidget *composerContainer = new QWidget;
    QVBoxLayout *composerLayout = new QVBoxLayout(composerContainer);

    QFormLayout *headerLayout = new QFormLayout;
    fromLineEdit = new QLineEdit;
    fromLineEdit->setPlaceholderText("your_email@example.com");
    toLineEdit = new QLineEdit;
    toLineEdit->setPlaceholderText("recipient@example.com; another@example.com");
    //ccLineEdit = new QLineEdit;
    subjectLineEdit = new QLineEdit;

    headerLayout->addRow("From:", fromLineEdit);
    headerLayout->addRow("To:", toLineEdit);
    //headerLayout->addRow("Cc:", ccLineEdit);
    headerLayout->addRow("Subject:", subjectLineEdit);

    bodyTextEdit = new QTextEdit;

    QGroupBox *attachmentsGroup = new QGroupBox("Attachments");
    QVBoxLayout *attachmentsLayout = new QVBoxLayout(attachmentsGroup);
    attachmentsListWidget = new QListWidget;
    attachmentsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    QHBoxLayout *attachmentButtonsLayout = new QHBoxLayout;
    addAttachmentButton = new QPushButton("Add...");
    removeAttachmentButton = new QPushButton("Remove");
    attachmentButtonsLayout->addStretch();
    attachmentButtonsLayout->addWidget(addAttachmentButton);
    attachmentButtonsLayout->addWidget(removeAttachmentButton);

    attachmentsLayout->addWidget(attachmentsListWidget);
    attachmentsLayout->addLayout(attachmentButtonsLayout);

    connect(addAttachmentButton, &QPushButton::clicked, this, &SmtpClientWindow::onAddAttachment);
    connect(removeAttachmentButton, &QPushButton::clicked, this, &SmtpClientWindow::onRemoveAttachment);

    composerLayout->addLayout(headerLayout);
    composerLayout->addWidget(bodyTextEdit, 1);
    composerLayout->addWidget(attachmentsGroup);

    return composerContainer;
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


void SmtpClientWindow::onSend()
{
    Email email;
    email.from = fromLineEdit->text();
    email.to = toLineEdit->text().split(';', Qt::SkipEmptyParts);
    //email.cc = ccLineEdit->text().split(';', Qt::SkipEmptyParts);
    email.subject = subjectLineEdit->text();
    email.body = bodyTextEdit->toPlainText();

    for(int i = 0; i < attachmentsListWidget->count(); ++i) {
        email.attachmentPaths.append(attachmentsListWidget->item(i)->text());
    }

    sendButton->setEnabled(false);
    statusBar()->showMessage("Sending...");
    m_Controller->sendEmail(email, m_Settings);
}

void SmtpClientWindow::onAddAttachment()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this, "Select files to attach");
    if (!filePaths.isEmpty()) {
        attachmentsListWidget->addItems(filePaths);
    }
}

void SmtpClientWindow::onRemoveAttachment()
{
    qDeleteAll(attachmentsListWidget->selectedItems());
}

void SmtpClientWindow::onConfigureServer()
{
    QDialog dialog(this);
    dialog.setWindowTitle( "Server Configuration");

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
        updateLog("Server configuration updated.");
    }
}

void SmtpClientWindow::updateLog(const QString &message)
{
    logTextEdit->appendPlainText(QTime::currentTime().toString("hh:mm:ss") + "> " + message);
}

void SmtpClientWindow::onSendSuccess()
{
    updateLog( "Message sent successfully");
    statusBar()->showMessage("Message sent successfully!", 5000);
    sendButton->setEnabled(true);
    QMessageBox::information(this, "Success",  "The email was sent successfully.");
}

void SmtpClientWindow::onSendFailed(const QString &error)
{
    updateLog(tr("Failed to send message! Reason: %1").arg(error));
    statusBar()->showMessage("Failed to send message!", 5000);
    sendButton->setEnabled(true);
    QMessageBox::critical(this, "Send Error",  tr("Could not send the email. Reason: %1").arg(error));
}
