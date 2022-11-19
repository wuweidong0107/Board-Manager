#include <QFileDialog>
#include <QDebug>
#include "AddRepositoryDialog.h"

AddRepositoryDialog::AddRepositoryDialog(QWidget *parent, QString const &dir)
{
    setupUi(this);
    //connect(browseBtn, &QPushButton::clicked, this, &AddRepositoryDialog::on_browseBtn_clicked);
}

void AddRepositoryDialog::on_browseBtn_clicked()
{
    QString dir = lineEdit->text();
    if (dir.isEmpty()) {
        dir = "~";
    }
    qDebug()<<__func__<<"1";
    dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"), dir);
    if (!dir.isEmpty()) {
        lineEdit->setText(dir);
    }
    qDebug()<<__func__<<"2";    
}

void AddRepositoryDialog::on_lineEdit_textChanged(const QString &arg1)
{
    
}
