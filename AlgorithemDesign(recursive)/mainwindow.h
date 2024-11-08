#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "const.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setWork();
    void READ_LOW();
    void COMPARE_HIGH();
    void CALMID();
    void FINDMID();
    void CMPTARGET();
    void UpdateHigh();
    void UpdateLow();
    void CALL();
    void SUCCESS();
    void RETURN();
    void STOP();

private slots:
    void on_btn_input_clicked();

    void on_btn_step_clicked();

    void on_btn_initTape_clicked();

    void on_btn_runAll_clicked();

    void on_comboBox_activated(int index);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
