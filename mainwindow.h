#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

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
    void pushButton1_clicked(int istemp);
    void pushButton2_clicked();
    void pushButton3_clicked();
    void on_read();
    void stop_connection();

private:
    Ui::MainWindow *ui;
    QProcess *p;
};
#endif // MAINWINDOW_H
