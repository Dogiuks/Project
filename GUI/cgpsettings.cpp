#include "cgpsettings.h"
#include "ui_cgpsettings.h"
#include "cgp.h"
#include "cgp_warehouse.h"

CGPSettings::CGPSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGPSettings)
{
    ui->setupUi(this);
    ui->elytism_spinBox->setValue(get_elitism());
    ui->levels_back_spinBox->setValue(get_levels_back());
    ui->mutation_rate_spinBox->setValue(get_mutation_rate());
    ui->num_col_spinBox->setValue(get_num_col());
    ui->num_generations_spinBox->setValue(get_num_generations());
    ui->num_row_spinBox->setValue(get_num_row());
    ui->Penalty_spinBox->setValue(get_penalty());
    ui->preserve_parents_checkBox->setChecked(get_preserve_parents());
    ui->population_size_spinBox->setValue(get_population_size());
    ui->roulete_spinBox->setValue(get_roulete());
    ui->tournament_spinBox->setValue(get_tournament());
    ui->t_size_spinBox->setValue(get_t_size());
    ui->t_size_spinBox->setEnabled(ui->tournament_spinBox->value());
}

CGPSettings::~CGPSettings()
{
    delete ui;
}

void CGPSettings::on_buttonBox_accepted()
{
    set_elitism(ui->elytism_spinBox->value());
    set_levels_back(ui->levels_back_spinBox->value());
    set_mutation_rate(ui->mutation_rate_spinBox->value());
    set_num_row(ui->num_row_spinBox->value());
    set_num_col(ui->num_col_spinBox->value());
    set_num_generations(ui->num_generations_spinBox->value());
    set_penalty(ui->Penalty_spinBox->value());
    set_population_size(ui->population_size_spinBox->value());
    set_preserve_parents(ui->preserve_parents_checkBox->isChecked());
    set_roulete(ui->roulete_spinBox->value());
    set_tournament(ui->tournament_spinBox->value());
    set_t_size(ui->t_size_spinBox->value());
    this->close();
}

void CGPSettings::on_buttonBox_rejected()
{
    this->close();
}

void CGPSettings::on_tournament_spinBox_valueChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->t_size_spinBox->setDisabled(true);
    }
    else
    {
        ui->t_size_spinBox->setEnabled(true);
    }
}

void CGPSettings::on_elytism_spinBox_valueChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->preserve_parents_checkBox->setDisabled(true);
    }
    else
    {
        ui->preserve_parents_checkBox->setEnabled(true);
    }
}
