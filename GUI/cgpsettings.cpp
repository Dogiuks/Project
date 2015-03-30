#include "cgpsettings.h"
#include "ui_cgpsettings.h"

CGPSettings::CGPSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGPSettings)
{
    ui->setupUi(this);
}

CGPSettings::~CGPSettings()
{
    delete ui;
}

void CGPSettings::on_buttonBox_accepted()
{

}
