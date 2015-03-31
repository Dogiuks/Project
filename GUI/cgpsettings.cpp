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
}
