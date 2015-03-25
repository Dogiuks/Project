#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cgp_warehouse.h"
#include "cgp.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cgpButton_clicked()
{
    run_EA();

}


void MainWindow::on_actionOpen_triggered()
{
    create_empty_input();
/*************************delete*************************************/
    define_warehouse("F:/IP/Project/GUI/warehouse.txt", "F:/IP/Project/GUI/warehouse_grid.txt");
    read_list("F:/IP/Project/GUI/files.txt");
/*************************delete****************************************/
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
            if (warehouse_grid[i*warehouse_grid_x+j])
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
        ui->listWidget->addItem(files[i].name);
    }
    ui->listWidget->setCurrentRow(0);
}


void MainWindow::on_buttonHome_clicked()
{
    ui->listWidget->setCurrentRow(0);
    updateWarehouseWiew();
}

QColor MainWindow::getGradient( int atPoint, int max)
{
    int red;
    int green;
    int blue;
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
    else if(num>0)
    {
        red = 6-num;
        green = 0;
        blue = 255;
    }
    else
    {
        red = 0;
        green = 0;
        blue = 0;
    }
    return QColor(red, green, blue, 255);
}

void MainWindow::updateWarehouseWiew(void)
{
    int count = 0;
    product_pointer *selected_output;
    selected_output = (product_pointer *)output_array[ui->listWidget->currentRow()];

    for(int i=0; i<warehouse_grid_y;i++)
    {
        for(int j=0; j<warehouse_grid_x; j++)
        {
            if (warehouse_grid[i*warehouse_grid_x+j])
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
    if(ui->listWidget->currentRow()<num_files-1)
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
