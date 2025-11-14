#ifndef COMPOSEDIALOG_H
#define COMPOSEDIALOG_H

#include <QDialog>
#include "Structures.h"

class QLineEdit;
class QTextEdit;
class QListWidget;
class QPushButton;
class QDialogButtonBox;

class ComposeDialog : public QDialog
{
    Q_OBJECT

public:
    ComposeDialog(QWidget *parent = nullptr);

    // Call this to get the completed email
    Email getEmail() const;

    void setFrom(const QString &from);
private slots:
    void onAddAttachment();
    void onRemoveAttachment();
    void validateAndAccept();

private:
    void setupUi();
    QWidget* createComposerWidget();

private:
    QLineEdit *fromLineEdit;
    QLineEdit *toLineEdit;
    QLineEdit *subjectLineEdit;
    QTextEdit *bodyTextEdit;

    QListWidget *attachmentsListWidget;
    QPushButton *addAttachmentButton;
    QPushButton *removeAttachmentButton;

    QDialogButtonBox *buttonBox;
};

#endif // COMPOSEDIALOG_H
