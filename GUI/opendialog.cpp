#include "opendialog.h"
#include "ui_opendialog.h"
#include "cgp_warehouse.h"
#include "mainwindow.h"

OpenDialog::OpenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenDialog)
{
    ui->setupUi(this);
}

OpenDialog::~OpenDialog()
{
    delete ui;
}

void OpenDialog::on_pushButton_clicked()
{
    char* str = const_cast<char*>(ui->lineEdit->text().toLatin1().constData());
    int status;
    //strcpy(str, ui->lineEdit->text().toStdString().data());
    if(get_num_files()>ui->spinBox->value())
    {
        status = add_file(str, get_num_files()+1, ui->checkBox->isChecked());
    }
    else
    {
        status = add_file(str, get_num_files()+1, ui->checkBox->isChecked());
    }
    if (status)
        this->close();
}

void OpenDialog::on_pushButton_2_clicked()
{
    ui->lineEdit->clear();
    this->close();
}
