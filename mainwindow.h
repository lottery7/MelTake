#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QShortcut>
#include <QMessageBox>

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/mpegfile.h>

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableView>
#include <QDebug>
#include<QSqlTableModel>

#include "include/audio_visualizer.hpp"

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

    void change_the_displayed_track_information (int track_index_in_list);

    void delete_track_from_datafile(QString track_path_to_delete);

//    void add_database(QString filename, QString creating_parameter, QString table_parameter, QSqlDatabase database, QSqlQuery query, QSqlTableModel model);

    void add_track_to_playlist(QFile& track, QString &track_path);

    void set_shortcuts();

    void connect_new_shortcut_with_function(QKeySequence key, QShortcut* shortcut, const char* slot);

    void increase_volume();

    void decrease_volume();

    void toggle_window(QMainWindow* mainWindow);

    void toggle();

    void make_fullscreen(QMainWindow* mainWindow);

    void expand_window_fullscreen();


private:
    Ui::MainWindow *ui;

    QShortcut *volume_up_shortcut;
    QShortcut *volume_down_shortcut;
    QShortcut *volume_off_shortcut;
    QShortcut *pause_play_shortcut;
    QShortcut *next_track_shortcut;
    QShortcut *previous_track_shortcut;
    QShortcut *expand_window;
    QShortcut *toggle_app;

    ///            QMap<QString, TagLib::MPEG::File*> path_to_mpeg;        -     НА ПОТОМ!!!

    QMediaPlayer *m_player;    // пригрыватель

    QMediaPlaylist *m_playlist;    // структура с обектами, которые воспроизводятся в проигрыватели

    QStandardItemModel *m_playlist_model;   // структура необходимая для отображения треков в таблице треков

    audio_app::audio_visualizer *m_visualizer;

    int last_volume_slider_value=5;
    bool is_muting=false;
    int stored_volume_value = 60;

    QSqlDatabase m_database;
    QSqlQuery *m_query;
    QSqlTableModel *m_model;

    /*QSqlDatabase m_settings;
    QSqlQuery *m_settings_query;
    QSqlTableModel *m_settings_model;*/

};
#endif // MAINWINDOW_H
