#pragma once

#include <QMainWindow>
#include <include/SerialPort.hpp>

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
    void on_pbConnectSerial_clicked();

private slots:
    void on_cbxComPorts_activated(int index);

private:
    Ui::MainWindow *ui;
    SerialPort* m_spPort;
};
