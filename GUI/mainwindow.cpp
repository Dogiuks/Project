#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cgp_warehouse.h"
#include "cgp.h"
#include "cgpsettings.h"
#include "opendialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    make_legend();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cgpButton_clicked()
{
    ui->statusbar->showMessage("CGP running!", 0);
    run_EA();
    ui->statusbar->showMessage("CGP finished with fitness:"+QString::number(get_best_fitness()), 0);
}


void MainWindow::on_actionOpen_triggered()
{
    int warehouse_grid_y, warehouse_grid_x;
    int num_files;
    set_first_run();
    create_empty_input();
    define_warehouse("F:/IP/Project/GUI/warehouse.txt", "F:/IP/Project/GUI/warehouse_grid.txt");
    read_list("F:/IP/Project/GUI/files.txt");
    num_files = get_num_files();
    warehouse_grid_x = get_warehouse_grid_x();
    warehouse_grid_y = get_warehouse_grid_y();
    ui->warehouseWiew->setColumnCount(warehouse_grid_x);
    ui->warehouseWiew->setRowCount(warehouse_grid_y);
    for(int i=0; i<warehouse_grid_x;i++)
    {
        ui->warehouseWiew->setColumnWidth(i, 25);
    }
    for(int i=0; i<warehouse_grid_y;i++)
    {
        ui->warehouseWiew->setRowHeight(i, 25);
    }
    for(int i=0; i<warehouse_grid_y;i++)
    {
        for(int j=0; j<warehouse_grid_x; j++)
        {
            ui->warehouseWiew->setItem(i, j, new QTableWidgetItem());
            if (get_warehouse_grid(j, i))
                ui->warehouseWiew->item(i,j)->setBackgroundColor(Qt::black);
            else
                ui->warehouseWiew->item(i,j)->setBackgroundColor(Qt::white);
        }
    }

    ui->cgpButton->setEnabled(true);
    ui->buttonHome->setEnabled(true);
    ui->buttonBack->setEnabled(true);
    ui->buttonNext->setEnabled(true);
    ui->listWidget->clear();
    for(int i = 0; i<num_files; i++)
    {
        ui->listWidget->addItem(get_file_name(i));
    }
    ui->listWidget->setCurrentRow(0);
}


void MainWindow::on_buttonHome_clicked()
{
    ui->listWidget->setCurrentRow(0);
    updateWarehouseWiew();
    //ui->warehouseWiew->item(0,0)->setText(QString::number(MU));
}

QColor MainWindow::getGradient( int atPoint, int max)
{
    int red;
    int green;
    int blue;

    if(atPoint == 0)
    {
        return QColor(0, 0, 0, 255);
    }
    int num = (74*atPoint/max);

    if(num>60)
    {
        red = 255;
        green = 0;
        blue = 224+(num-75)*16;
    }
    else if(num>35)
    {
        red = 255;
        green = 10*(61-num);
        blue = 0;
    }
    else if(num>28)
    {
            red = 253-38*(35-num);
            green = 255;
            blue = 0;
    }
    else if(num>21)
    {
            red = 0;
            green = 255;
            blue = 16+38*(28-num);
    }
    else if(num>6)
    {
            red = 0;
            green = 228-16*(21-num);
            blue = 255;
    }
    else
    {
        red = 6-num;
        green = 0;
        blue = 255;
    }
    return QColor(red, green, blue, 255);
}

void MainWindow::updateWarehouseWiew(void)
{
    int count = 0;
    int warehouse_grid_y, warehouse_grid_x;
    product_pointer *selected_output;

    selected_output = (product_pointer *)return_selected_output(ui->listWidget->currentRow());

    warehouse_grid_x = get_warehouse_grid_x();
    warehouse_grid_y = get_warehouse_grid_y();

    for(int i=0; i<warehouse_grid_y;i++)
    {
        for(int j=0; j<warehouse_grid_x; j++)
        {
            if (get_warehouse_grid(j, i))
            {
                ui->warehouseWiew->item(i,j)->setBackgroundColor(getGradient(selected_output[count]->demand, 100));
                ui->warehouseWiew->item(i,j)->setText(QString::number(selected_output[count]->code));
                count++;
            }
        }
    }
}

void MainWindow::on_buttonNext_clicked()
{
    int num_files;
    num_files = get_num_files()-1;
    if(ui->listWidget->currentRow()<num_files)
    {
        ui->listWidget->setCurrentRow(ui->listWidget->currentRow()+1);
        updateWarehouseWiew();
    }
}

void MainWindow::on_buttonBack_clicked()
{
    if(ui->listWidget->currentRow()>0)
    {
        ui->listWidget->setCurrentRow(ui->listWidget->currentRow()-1);
        updateWarehouseWiew();
    }
}

void MainWindow::on_actionCGP_settings_triggered()
{
    CGPSettings settings;
    settings.setModal(true);
    settings.exec();
}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    updateWarehouseWiew();
}

void MainWindow::make_legend(void)
{
    for(int i=0;i<34;i++)
    {
        ui->legend->addItem(QString::number(i*3+1));
        ui->legend->item(i)->setBackgroundColor(getGradient(i*3+1, 100));
        ui->legend->item(i)->setTextColor(Qt::black);
    }

}
