#include "AddBoardDialog.h"
#include "ui_AddBoardDialog.h"

AddBoardDialog::AddBoardDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddBoardDialog)
{
    ui->setupUi(this);
}

AddBoardDialog::~AddBoardDialog()
{
    delete ui;
}
