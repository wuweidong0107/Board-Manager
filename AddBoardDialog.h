#ifndef ADDBOARDDIALOG_H
#define ADDBOARDDIALOG_H

#include <QDialog>

namespace Ui {
class AddBoardDialog;
}

class AddBoardDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddBoardDialog(QWidget *parent = nullptr);
    ~AddBoardDialog();
    
private:
    Ui::AddBoardDialog *ui;
};

#endif // ADDBOARDDIALOG_H
