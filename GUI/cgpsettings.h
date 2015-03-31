#ifndef CGPSETTINGS_H
#define CGPSETTINGS_H

#include <QDialog>

namespace Ui {
class CGPSettings;
}

class CGPSettings : public QDialog
{
    Q_OBJECT

public:
    explicit CGPSettings(QWidget *parent = 0);
    ~CGPSettings();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_tournament_spinBox_valueChanged(int arg1);

    void on_elytism_spinBox_valueChanged(int arg1);

private:
    Ui::CGPSettings *ui;
};

#endif // CGPSETTINGS_H
