#include <taglib/attachedpictureframe.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v2tag.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QPixmap>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QWidget>
#include "QCoreApplication"
#include "include/audio_visualizer.hpp"
#include "include/fwd.hpp"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    set_shortcuts();
    setWindowOpacity(0.95);

    m_tracks_model = new QStandardItemModel(this);
    m_tracks_model->setHorizontalHeaderLabels(
        QStringList() << tr("track_name") << tr("path")
    );
    set_model_to_view(m_tracks_model, ui->playlist_table_view);

    m_playlists_model = new QStandardItemModel(m_playlists.size(), 1, this);
    m_playlists_model->setHorizontalHeaderLabels(
        QStringList() << tr("playlists")
    );
    set_model_to_view(m_playlists_model, ui->playlists_table);

    m_player = new QMediaPlayer(this);
    m_player->setVolume(stored_volume_value);
    ui->volume_slider->setValue(stored_volume_value);

    m_default_playlist = new QMediaPlaylist(m_player);
    m_default_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    m_default_playlist->setObjectName("All tracks");
    add_playlist_to_table(m_default_playlist);
    m_playlists.append(m_default_playlist);
    m_player->setPlaylist(m_default_playlist);

    QPixmap pixmap(":/resources/cover.png");
    ui->album_cover_lable->setPixmap(
        pixmap.scaled(300, 300, Qt::KeepAspectRatio)
    );

    // визуализатор
    m_visualizer = new audio_app::audio_visualizer();
    m_visualizer->set_player(m_player);
    m_visualizer->show();
    m_visualizer->set_fragment_shader_path(
        "C:/Users/HUAWEI/Documents/project/"
        "build-qmake_project-Desktop_Qt_5_15_2_MinGW_64_bit-Debug/shaders/"
        "main.frag"
    );

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("./MusicDatabase.db");
    if (!m_database.open()) {
        qDebug("database is not created!");
        return;
    }

    m_query = new QSqlQuery(m_database);
    m_query->exec("CREATE TABLE MusicDatabase(Path TEXT, Playlist TEXT);");

    m_model = new QSqlTableModel(this, m_database);
    m_model->setTable("MusicDatabase");
    m_model->select();

    m_query->exec("SELECT * FROM MusicDatabase");
    while (m_query->next()) {
        QString track_path = m_query->value(0).toString();
        QString playlist_name = m_query->value(1).toString();
        if (!track_path.isEmpty()) {
            QFile track(track_path);
            bool is_found = false;
            for (int i = 0; i < m_default_playlist->mediaCount(); i++) {
                QMediaContent mediaContent = m_default_playlist->media(i);
                QString path = mediaContent.canonicalUrl().toString().toLower();
                QString small_track_path = track_path.toLower();
                if (small_track_path == path) {
                    is_found = true;
                }
            }
            if (!is_found) {
                add_track_to_playlist(
                    track, track_path, m_default_playlist, true
                );
            }
            if (!playlist_name.isEmpty() && playlist_name != "All tracks") {
                int index = return_playlist_index_if_exists(playlist_name);
                if (index == -1) {
                    create_playlist_with_given_name(playlist_name, true);
                    index = return_playlist_index_if_exists(playlist_name);
                }
                add_track_to_playlist(
                    track, track_path, m_playlists[index], false
                );
            }
        } else if (!playlist_name.isEmpty()) {
            int index = return_playlist_index_if_exists(playlist_name);
            if (index == -1) {
                create_playlist_with_given_name(playlist_name, true);
            }
        }
    }

    if (!m_default_playlist->isEmpty()) {
        change_the_displayed_track_information(0);
    }

    connect(
        ui->playlist_table_view, &QTableView::doubleClicked, m_default_playlist,
        [this](const QModelIndex &index) {
            m_default_playlist->setCurrentIndex(index.row());
        }
    );

    connect(
        m_default_playlist, &QMediaPlaylist::currentIndexChanged,
        ui->album_cover_lable,
        [this](int index) { change_the_displayed_track_information(index); }
    );

    connect(
        m_player, &QMediaPlayer::durationChanged, ui->process_slyder,
        &QSlider::setMaximum
    );
    connect(
        m_player, &QMediaPlayer::positionChanged, ui->process_slyder,
        &QSlider::setValue
    );
    connect(
        m_player, &QMediaPlayer::durationChanged, ui->process_slyder,
        &QSlider::setSingleStep
    );
    ui->process_slyder->setSingleStep(1000);
    connect(
        ui->process_slyder, &QSlider::sliderMoved, m_player,
        &QMediaPlayer::setPosition
    );
    connect(
        ui->add_playlist, &QPushButton::clicked, this,
        &MainWindow::create_playlist
    );
    connect(
        ui->playlists_table, &QTableView::clicked, this,
        [=](const QModelIndex &index) { on_playlists_table_clicked(index); }
    );
}

void MainWindow::set_model_to_view(
    QStandardItemModel *model,
    QTableView *view
) {
    view->setModel(model);
    view->hideColumn(1);
    view->horizontalHeader()->setVisible(false);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    view->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::add_track_to_playlist(
    QFile &track,
    const QString &track_path,
    QMediaPlaylist *playlist,
    bool is_default
) {
    if (track.exists()) {
        QList<QStandardItem *> items;
        TagLib::MPEG::File mpeg_track(track_path.toStdString().c_str());
        std::string track_name(mpeg_track.tag()->title().toCString(true));
        auto artist = mpeg_track.tag()->artist().toCString(true);
        track_name += " - " + std::string(artist);
        items.append(new QStandardItem(track_name.c_str()));
        items.append(new QStandardItem(track_path));
        if (is_default) {
            m_tracks_model->appendRow(items);
        }
        playlist->addMedia(QUrl(track_path));
    }
}

void MainWindow::create_database(
    const QString &filename,
    const QString &creating_parameter,
    const QString &table_name,
    QSqlDatabase &database,
    QSqlQuery *query,
    QSqlTableModel *model
) {
    database.setDatabaseName(filename);
    if (!database.open()) {
        qDebug("database is not created!");
        return;
    }
    query->exec(creating_parameter);
    model->setTable(table_name);
    model->select();
}

MainWindow::~MainWindow() {
    // TODO settings saving

    delete ui;
    delete m_tracks_model;
    delete m_default_playlist;
    delete m_player;
}

void MainWindow::add_track_from_the_playlist_to_database(
    const QString &track_path,
    const QString &playlist_name
) {
    m_query->prepare(
        "INSERT INTO MusicDatabase (path, playlist) VALUES (:path, :playlist);"
    );
    m_query->bindValue(":path", track_path);
    m_query->bindValue(":playlist", playlist_name);
    bool b = m_query->exec();
    if (!b) {
        qDebug() << "Unable to add the track to the database";
    }
}

void MainWindow::on_load_tracks_button_clicked() {
    ui->statusbar->showMessage("load_tracks_button_clicked");

    QModelIndex index = ui->playlists_table->currentIndex();
    int row = index.row();
    if (row == -1) {
        row = 0;
    }

    auto tracks_num = m_playlists[row]->mediaCount();
    QStringList track_paths = QFileDialog::getOpenFileNames(
        this, tr("Open files"), QString(), tr("MPEG Files (*.mp3 *.mpeg *.mpg)")
    );

    for (auto &track_path : track_paths) {
        m_query->prepare(
            "SELECT EXISTS (SELECT * FROM MusicDatabase WHERE Path = "
            ":trackpath)"
        );
        m_query->bindValue(":trackpath", track_path);
        bool ex = m_query->exec();
        m_query->next();
        if (!ex) {
            qDebug() << "Error executing query:" << m_query->lastError().text();
            return;
        }

        qDebug() << "RESULT " << m_query->value(0).toString();
        if (m_query->value(0).toString() == "0") {
            QFile track(track_path);
            add_track_to_playlist(track, track_path, m_default_playlist, true);
            add_track_from_the_playlist_to_database(track_path, "All tracks");
            if (m_playlists[row]->objectName() != "All tracks") {
                add_track_to_playlist(
                    track, track_path, m_playlists[row], false
                );
                add_track_from_the_playlist_to_database(
                    track_path, m_playlists[row]->objectName()
                );
            }
        }
    }
    if (!m_playlists[row]->isEmpty() && tracks_num == 0) {
        change_the_displayed_track_information(0);
    }
}

void MainWindow::on_next_track_button_clicked() {
    if (m_default_playlist->nextIndex() == m_default_playlist->currentIndex()) {
        m_player->setPosition(0);
    } else {
        m_default_playlist->next();
    }
}

void MainWindow::on_previous_track_button_clicked() {
    QString qs = QString::number(m_default_playlist->currentIndex()) + " -> " +
                 QString::number(m_default_playlist->previousIndex());
    ui->statusbar->showMessage(qs);
    if (m_default_playlist->previousIndex() ==
        m_default_playlist->currentIndex()) {
        m_player->setPosition(0);
    } else {
        m_default_playlist->previous();
    }
}

void MainWindow::on_play_pause_button_clicked() {
    ui->statusbar->showMessage("play_pause_button_clicked");
    if (!m_default_playlist->isEmpty() &&
        m_player->state() != QMediaPlayer::PlayingState) {
        m_player->play();
        ui->play_pause_button->setChecked(true);
    } else {
        m_player->pause();
        ui->play_pause_button->setChecked(false);
    }
}

void MainWindow::on_mute_button_clicked() {
    ui->statusbar->showMessage("mute_button_clicked");
    is_muting = true;

    if (m_player->isMuted()) {  // разьъючиваем
        m_player->setMuted(false);
        ui->mute_button->setChecked(false);
        if (last_volume_slider_value == 0) {
            last_volume_slider_value = 20;
        }
        ui->volume_slider->setValue(last_volume_slider_value);
        m_player->setVolume(last_volume_slider_value);
    }

    else {  // замъючиваем
        last_volume_slider_value = m_player->volume();
        ui->volume_slider->setValue(0);
        m_player->setMuted(true);
        ui->mute_button->setChecked(true);
    }

    is_muting = false;
}

void MainWindow::on_volume_slider_valueChanged(int value) {
    if (!is_muting) {
        m_player->setVolume(value);
        if (m_player->isMuted() && value > 0 ||
            !m_player->isMuted() && value == 0) {  // рязмъючиваем
            on_mute_button_clicked();
        }
    }
}

void MainWindow::on_random_mode_button_clicked() {
    if (ui->random_mode_button->isChecked()) {
        m_default_playlist->setPlaybackMode(QMediaPlaylist::Random);
        ui->track_loop_mode_button->setChecked(false);
    } else {
        m_default_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
}

void MainWindow::on_loop_mode_button_clicked() {
    m_default_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    ui->random_mode_button->setChecked(false);
    ui->track_loop_mode_button->setChecked(false);
}

void MainWindow::on_track_loop_mode_button_clicked() {
    if (ui->track_loop_mode_button->isChecked()) {
        m_default_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->random_mode_button->setChecked(false);
    } else {
        m_default_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
}

void MainWindow::delete_track() {
    // TODO
    //           QModelIndex track_index =
    //           ui->playlist_table_view->currentIndex(); QModelIndex
    //           playlist_index = ui->playlists_table->currentIndex();
    //           qDebug()<<QString::number(track_index)<<"
    //           "<<QString::number(playlist_index);

    //           if (track_index.isValid() && playlist_index.isValid()) {
    //               QMediaContent selected_track =
    //               m_playlists[playlist_index]->media(track_index);
    //               selected_track.canonicalUrl().toString(); //delete in
    //               database
    //               m_playlists[playlist_index]->removeMedia(track_index.row());
    //           }
}

void MainWindow::delete_playlist() {
    // TODO
}

void MainWindow::change_the_displayed_track_information(int track_index_in_list
) {
    TagLib::MPEG::File mpeg_track(
        m_tracks_model->data(m_tracks_model->index(track_index_in_list, 1))
            .toString()
            .toStdString()
            .c_str()
    );
    TagLib::ID3v2::Tag *tag = mpeg_track.ID3v2Tag();
    if (tag) {
        TagLib::ID3v2::FrameList frames = tag->frameList("APIC");
        if (!frames.isEmpty()) {
            TagLib::ID3v2::AttachedPictureFrame *cover =
                static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front(
                ));
            if (cover) {
                ui->statusbar->showMessage("have a cover");
                QImage coverImage;
                coverImage.loadFromData(
                    reinterpret_cast<const uchar *>(cover->picture().data()),
                    static_cast<int>(cover->picture().size())
                );
                QPixmap pixmap = QPixmap::fromImage(coverImage);
                ui->album_cover_lable->setPixmap(
                    pixmap.scaled(300, 300, Qt::KeepAspectRatio)
                );
            }
        } else {
            ui->statusbar->showMessage("do not have cover");
            QPixmap pixmap(":/resources/cover.png");
            ui->album_cover_lable->setPixmap(
                pixmap.scaled(300, 300, Qt::KeepAspectRatio)
            );
        }
    }
    ui->track_title_lable->setText(tag->title().toCString(true));
    std::string track_artist_album(mpeg_track.tag()->artist().toCString(true));
    auto album = mpeg_track.tag()->album().toCString(true);
    if (std::string(album) != "") {
        track_artist_album += " - " + std::string(album);
    }
    ui->track_artist_album_lable->setText(track_artist_album.c_str());
}

void MainWindow::connect_new_shortcut_with_function(
    QKeySequence key,
    QShortcut *shortcut,
    const char *slot
) {
    shortcut = new QShortcut(this);
    shortcut->setKey(key);
    QObject::connect(shortcut, SIGNAL(activated()), this, slot);
}

void MainWindow::set_shortcuts() {
    connect_new_shortcut_with_function(
        QKeySequence(Qt::Key_F6), volume_up_shortcut, SLOT(increase_volume())
    );
    connect_new_shortcut_with_function(
        QKeySequence(Qt::Key_F5), volume_down_shortcut, SLOT(decrease_volume())
    );
    connect_new_shortcut_with_function(
        QKeySequence(Qt::Key_F4), volume_off_shortcut,
        SLOT(on_mute_button_clicked())
    );

    connect_new_shortcut_with_function(
        QKeySequence(Qt::Key_Space), pause_play_shortcut,
        SLOT(on_play_pause_button_clicked())
    );
    connect_new_shortcut_with_function(
        QKeySequence(Qt::Key_Right), next_track_shortcut,
        SLOT(on_next_track_button_clicked())
    );
    connect_new_shortcut_with_function(
        QKeySequence(Qt::Key_Left), previous_track_shortcut,
        SLOT(on_previous_track_button_clicked())
    );

    connect_new_shortcut_with_function(
        QKeySequence(Qt::Key_F11), expand_window,
        SLOT(expand_window_fullscreen())
    );
    connect_new_shortcut_with_function(
        QKeySequence(Qt::Key_F12), toggle_app, SLOT(toggle())
    );
}

void MainWindow::increase_volume() {
    on_volume_slider_valueChanged(ui->volume_slider->value() + 5);
    ui->volume_slider->setValue(ui->volume_slider->value() + 5);
}

void MainWindow::decrease_volume() {
    on_volume_slider_valueChanged(ui->volume_slider->value() - 5);
    ui->volume_slider->setValue(ui->volume_slider->value() - 5);
}

void MainWindow::toggle_window(QMainWindow *mainWindow) {
    if (mainWindow->isMinimized() || mainWindow->isMaximized()) {
        mainWindow->showNormal();
    } else {
        mainWindow->showMinimized();
    }
}

void MainWindow::make_fullscreen(QMainWindow *mainWindow) {
    if (mainWindow->isFullScreen()) {
        mainWindow->showNormal();
    } else {
        mainWindow->showFullScreen();
    }
}

void MainWindow::expand_window_fullscreen() {
    make_fullscreen(this);
}

void MainWindow::toggle() {
    toggle_window(this);
}

void MainWindow::add_playlist_to_table(QMediaPlaylist *playlist) {
    QStandardItem *item = new QStandardItem(playlist->objectName());
    m_playlists_model->appendRow(item);
}

void MainWindow::create_playlist() {
    QString playlist_name = QInputDialog::getText(
        this, tr("New Playlist"), tr("Enter playlist name:")
    );
    create_playlist_with_given_name(playlist_name, false);
}

void MainWindow::create_playlist_with_given_name(
    const QString &playlist_name,
    bool is_from_database
) {
    QMediaPlaylist *new_playlist = new QMediaPlaylist(m_player);
    if (!playlist_name.isEmpty()) {
        new_playlist->setObjectName(playlist_name);
    } else {
        new_playlist->setObjectName(
            "Playlist " + QString::number(m_playlists.size())
        );
    }
    m_playlists.append(new_playlist);
    add_playlist_to_table(new_playlist);
    if (!is_from_database) {
        add_track_from_the_playlist_to_database("", new_playlist->objectName());
    }
}

int MainWindow::return_playlist_index_if_exists(const QString &playlist_name) {
    if (playlist_name.isEmpty()) {
        return -1;
    }
    for (auto i = 0; i < m_playlists.size(); i++) {
        if (m_playlists[i]->objectName() == playlist_name) {
            return i;
        }
    }
    return -1;
}

void MainWindow::on_playlists_table_clicked(const QModelIndex &index) {
    int row = index.row();
    qDebug() << QString::number(row) << QString::number(m_playlists.size());
    m_tracks_model->clear();

    qDebug() << m_playlists[row]->objectName();
    QMediaPlaylist *playlist = new QMediaPlaylist(m_playlists[row]);
    m_player->setPlaylist(playlist);

    for (int i = 0; i < m_playlists[row]->mediaCount(); i++) {
        QList<QStandardItem *> items;
        TagLib::MPEG::File mpeg_track(m_playlists[row]
                                          ->media(i)
                                          .canonicalUrl()
                                          .toString()
                                          .toStdString()
                                          .c_str());
        std::string track_name(mpeg_track.tag()->title().toCString(true));
        auto artist = mpeg_track.tag()->artist().toCString(true);
        track_name += " - " + std::string(artist);
        items.append(new QStandardItem(track_name.c_str()));
        m_tracks_model->appendRow(items);
    }
}
