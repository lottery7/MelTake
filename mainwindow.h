#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/mpegfile.h>

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

    void on_next_track_button_clicked();

    void on_previous_track_button_clicked();

    void on_play_pause_button_clicked();

    void on_mute_button_clicked();

    void on_volume_slider_valueChanged(int value);

    void on_random_mode_button_clicked();

    void on_loop_mode_button_clicked();

    void on_track_loop_mode_button_clicked();

    void delete_track_from_datafile(QString track_path_to_delete);

private:
    Ui::MainWindow *ui;

    ///            QMap<QString, TagLib::MPEG::File*> path_to_mpeg;        -     НА ПОТОМ!!!

    QMediaPlayer *m_player;    // пригрыватель

    QMediaPlaylist *m_playlist;    // структура с обектами, которые воспроизводятся в проигрыватели

    QStandardItemModel *m_playlist_model;   // структура необходимая для отображения треков в таблице треков
    
    int last_volume_slider_value=5;
    bool is_muting=false;
    int stored_volume_value = 60;
};
#endif // MAINWINDOW_H
