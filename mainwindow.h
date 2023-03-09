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
    void on_ui_start_stop_button_clicked();

    void on_ui_next_track_button_clicked();

    void on_ui_previou_track_button_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
