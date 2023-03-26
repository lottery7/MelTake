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

    void on_play_pause_button_clicked();

    void on_mute_button_clicked();

    void on_volume_slider_valueChanged(int value);

private:
    Ui::MainWindow *ui;

    //возможно стоит вынести это в отдельный заголовочный файл, посмотрим по дальнейшей реализации
    QStandardItemModel *m_playlist_model;
    QMediaPlaylist *m_playlist;
    QMediaPlayer *m_player;


    // возможно в другое место перенести пока не знаю ? спросить!
    int last_volume_slider_value=5;
    bool is_muting=false;

    // Это тоже перенести! Это параметер который должен сохраняться в settings
    // Такие значения надо помечать по-особому, к примеру stored_ - сохраненный

    // Или лучше пометить from_settings?
    int stored_volume_value = 60;


};
#endif // MAINWINDOW_H
