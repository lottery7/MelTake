#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>

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
    void on_load_tracks_button_clicked();

    void on_next_trak_button_clicked();

    void on_previous_track_button_clicked();

    void on_start_stop_button_clicked();

private:
    Ui::MainWindow *ui;

    //возможно стоит вынести это в отдельный заголовочный файл, посмотрим по дальнейшей реализации
    QStandardItemModel *m_playlist_model;
    QMediaPlaylist *m_playlist;
    QMediaPlayer *m_player;

};
#endif // MAINWINDOW_H
