#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QShortcut>
#include <QMessageBox>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableView>
#include <QDebug>
#include <QSqlTableModel>

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/mpegfile.h>

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

    void on_track_loop_mode_button_clicked();

    void change_the_displayed_track_information (int track_index);

    void display_nodata_information ();

    QString get_str_time_from_seconds(int seconds);

    void add_track_to_playlist(QFile& track, const QString &track_path, QMediaPlaylist *playlist, bool is_from_database);

    void delete_track(int index);

    void add_playlist_to_table(QMediaPlaylist *playlist);

    void on_playlists_table_clicked(const QModelIndex &index);

    int return_playlist_index_if_exists(const QString &playlist_name);

    void add_track_from_the_playlist_to_database(
            const QString &track_path,
            const QString &playlist_name
    );

    void set_current_playlist_view();

    void create_playlist();

    void create_playlist_with_given_name(const QString & playlist_name, bool is_from_database);

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

    QMediaPlayer *m_player;

    QMediaPlaylist *m_current_playlist;

    QStandardItemModel *m_current_playlist_model;

    QVector<QMediaPlaylist *> m_playlists;

    QStandardItemModel *m_playlists_model;

    audio_app::audio_visualizer *m_visualizer;

    int last_volume_slider_value=5;
    bool is_muting=false;
    int stored_volume_value = 60;
    QString current_track_duracrion;

    QSqlDatabase m_database;
    QSqlQuery *m_query;
    QSqlTableModel *m_model;

    QShortcut *volume_up_shortcut;
    QShortcut *volume_down_shortcut;
    QShortcut *volume_off_shortcut;
    QShortcut *pause_play_shortcut;
    QShortcut *next_track_shortcut;
    QShortcut *previous_track_shortcut;
    QShortcut *expand_window;
    QShortcut *toggle_app;

};
#endif // MAINWINDOW_H
