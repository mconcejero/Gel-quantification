#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnopen_clicked();

    void on_btnclear_clicked();

    void on_btncsv_clicked();

    void on_btnclear_2_clicked();

    void on_btnsave_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
