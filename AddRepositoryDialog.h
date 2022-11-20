#ifndef ADDREPOSITORYDIALOG_H
#define ADDREPOSITORYDIALOG_H

#include <QDialog>
#include "ui_AddRepositoryDialog.h"

class AddRepositoryDialog : public QDialog, public Ui::AddRepositoryDialog
{
    Q_OBJECT
public:
    AddRepositoryDialog(QWidget *parent = 0, QString const &dir = QString());
    QString path() const;

private slots:
    void on_browseBtn_clicked();
    void on_lineEdit_textChanged(QString const &arg1);

private:
    void validate();
};

#endif // ADDREPOSITORYDIALOG_H