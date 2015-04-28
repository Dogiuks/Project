#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QColor getGradient( int atPoint, int max);
    void updateWarehouseWiew(void);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void on_cgpButton_clicked();

    void on_actionOpen_triggered();

    void on_buttonHome_clicked();

    void on_buttonNext_clicked();

    void on_buttonBack_clicked();

    void on_actionCGP_settings_triggered();

    void on_listWidget_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
