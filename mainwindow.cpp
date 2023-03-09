#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Нужен метод получения обложки с текущего трека,
    // если текущий трек не выбран или у него нет обложки, то передается обложка по умолчанию

//    QPixmap current_cover (""PATH"");
//    int current_cover_width = ui->ui_current_cover->width();
//    int current_cover_height = ui->ui_current_cover->height();
//    ui->ui_current_cover->setPixmap(current_cover.scaled(current_cover_width,current_cover_height, Qt::KeepAspectRatio));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_ui_start_stop_button_clicked()
{
    ui->statusbar->showMessage("start_stop_button_clicked");
}


void MainWindow::on_ui_next_track_button_clicked()
{
    ui->statusbar->showMessage("next_track_button_clicked");
}


void MainWindow::on_ui_previou_track_button_clicked()
{
    ui->statusbar->showMessage("previou_track_button_clicked");
}
