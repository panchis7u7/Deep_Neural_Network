#include "ui/ui_MainWindow.h"
#include <include/MainWindow.hpp>
#include <include/SerialPort.hpp>
#include <include/SerialPort.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList qSerialPortList;
    for(const auto& serialPort : SerialPort::getAvailablePorts())
        qSerialPortList << QString::fromStdString(serialPort);
    ui->cbxComPorts->addItems(qSerialPortList);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cbxComPorts_activated(int index)
{
    (void)index;
}


void MainWindow::on_pbConnectSerial_clicked()
{
    QString selectedItem = ui->cbxComPorts->currentText();
    if(selectedItem != "") {
        SerialPort port(selectedItem.toStdString());
        port.connect();
    }
}

