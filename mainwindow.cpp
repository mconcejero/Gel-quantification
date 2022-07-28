#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

#include "proceso.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_btnopen_clicked()
{
    if(ui->legel->text()=="")
      {
        QString fich=QFileDialog::getOpenFileName(this,"Abrir gel","","Ficheros de imagen (*.jpg *.png)");
        if (fich!="")
          ui->btnopen->setText("Analizar gel");
        ui->legel->setText(fich);
      }
    else
      {
        ui->btnsave->setEnabled(false);
        QString f = ui->legel->text();
        QMessageBox msgbox;
        if (comienza(f.toUtf8().constData())==false)
          {
            msgbox.setText("Error: El gel no se analizó correctamente");
          }
        else
          {
            msgbox.setText("OK: El gel se analizó correctamente");
            ui->btnsave->setEnabled(true);
          }
        msgbox.exec();
      }
}

void MainWindow::on_btnclear_clicked()
{
    ui->legel->setText("");
    ui->btnopen->setText("Buscar gel");
    ui->btnsave->setEnabled(false);
}

void MainWindow::on_btncsv_clicked()
{
    QString fich=QFileDialog::getSaveFileName(this,"Guardar resultado","","Fichero de datos (*.csv)");
    if(fich!="")
      {
        if (!fich.contains(".csv"))
        {
          fich +=".csv";
        }
      }
    ui->lecsv->setText(fich);
}

void MainWindow::on_btnclear_2_clicked()
{
    ui->lecsv->setText("");
}

void MainWindow::on_btnsave_clicked()
{
    if (ui->lecsv->text()=="")
      {
        QMessageBox msgbox;
        msgbox.setText("Error: No ha especificado un fichero donde guardar el resultado");
        msgbox.exec();
      }
    else
      {
        QString f = ui->lecsv->text();
        guarda(f.toUtf8().constData());
      }
}
