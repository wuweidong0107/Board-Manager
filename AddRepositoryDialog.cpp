#include "Git.h"
#include <QFileDialog>
#include <QDebug>
#include "AddRepositoryDialog.h"

AddRepositoryDialog::AddRepositoryDialog(QWidget *parent, QString const &dir)
    : QDialog(parent)
{
    setupUi(this);
    lineEdit->setText(dir);
}

void AddRepositoryDialog::on_browseBtn_clicked()
{
    QString dir = lineEdit->text();
    if (dir.isEmpty()) {
        dir = "~";
    }
    dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), dir);
    if (!dir.isEmpty()) {
        lineEdit->setText(dir);
    }
}

void AddRepositoryDialog::on_lineEdit_textChanged(const QString &arg1)
{
    validate();
}

void AddRepositoryDialog::validate()
{
    QString path = lineEdit->text();
    QString text;
    if (!Git::isValidRepo(path)) {
        text = tr("Invalid Package.");
    } else {
        text = tr("");
    }
    label_warning->setText(text);
    label_warning->setStyleSheet("color:red");
}
