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

private:
    Ui::CGPSettings *ui;
};

#endif // CGPSETTINGS_H
