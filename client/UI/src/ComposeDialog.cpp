#include "ComposeDialog.h"
#include <QDialogButtonBox>
#include <QtWidgets>


ComposeDialog::ComposeDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    setMinimumSize(600, 500);
    setWindowTitle("Compose Email");
}

void ComposeDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(createComposerWidget());

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    QPushButton *sendButton = buttonBox->addButton(tr("Send"), QDialogButtonBox::AcceptRole);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &ComposeDialog::validateAndAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

QWidget* ComposeDialog::createComposerWidget()
{
    QWidget *composerContainer = new QWidget;
    QVBoxLayout *composerLayout = new QVBoxLayout(composerContainer);

    QFormLayout *headerLayout = new QFormLayout;
    fromLineEdit = new QLineEdit;
    fromLineEdit->setPlaceholderText("your_email@example.com");
    toLineEdit = new QLineEdit;
    toLineEdit->setPlaceholderText("recipient@example.com; another@example.com");
    subjectLineEdit = new QLineEdit;

    headerLayout->addRow("From:", fromLineEdit);
    headerLayout->addRow("To:", toLineEdit);
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

    connect(addAttachmentButton, &QPushButton::clicked, this, &ComposeDialog::onAddAttachment);
    connect(removeAttachmentButton, &QPushButton::clicked, this, &ComposeDialog::onRemoveAttachment);

    composerLayout->addLayout(headerLayout);
    composerLayout->addWidget(bodyTextEdit, 1);
    composerLayout->addWidget(attachmentsGroup);

    return composerContainer;
}

Email ComposeDialog::getEmail() const
{
    Email email;
    email.from = fromLineEdit->text();
    email.to = toLineEdit->text().split(';', Qt::SkipEmptyParts);
    email.subject = subjectLineEdit->text();
    email.body = bodyTextEdit->toPlainText();

    for(int i = 0; i < attachmentsListWidget->count(); ++i) {
        email.attachmentPaths.append(attachmentsListWidget->item(i)->text());
    }
    return email;
}

void ComposeDialog::validateAndAccept()
{
    QStringList errors;
    if (fromLineEdit->text().trimmed().isEmpty()) {
        errors.append("'From' field cannot be empty.");
    }
    if (toLineEdit->text().trimmed().isEmpty()) {
        errors.append("'To' field cannot be empty.");
    }
    if (bodyTextEdit->toPlainText().trimmed().isEmpty()) {
        errors.append("The email body cannot be empty.");
    }

    if (!errors.isEmpty()) {
        QMessageBox::warning(this, "Invalid Input", errors.join("\n"));
        return;
    }

    QDialog::accept();
}

void ComposeDialog::setFrom(const QString &from)
{
    fromLineEdit->setText(from);
}

void ComposeDialog::onAddAttachment()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this, "Select files to attach");
    if (!filePaths.isEmpty()) {
        attachmentsListWidget->addItems(filePaths);
    }
}

void ComposeDialog::onRemoveAttachment()
{
    qDeleteAll(attachmentsListWidget->selectedItems());
}
