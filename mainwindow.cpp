#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QWidget>
#include <QFileDialog>
#include <QPixmap>
#include <QDesktopWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QCoreApplication>
#include <QInputDialog>

#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/audio_visualizer.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("MelTake");

    set_shortcuts();

    m_current_playlist_model = new QStandardItemModel(this);
    set_current_playlist_view();

    m_playlists_model = new QStandardItemModel(this);
    m_playlists_model->setHorizontalHeaderLabels(QStringList()<<tr("playlists"));
    ui->playlists_table_view->setModel(m_playlists_model);
    ui->playlists_table_view->hideColumn(1);
    ui->playlists_table_view->horizontalHeader()->setVisible(false);
    ui->playlists_table_view->verticalHeader()->setVisible(false);
    ui->playlists_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlists_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->playlists_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->playlists_table_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    m_player = new QMediaPlayer(this);
    m_current_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_current_playlist);
    m_player->setVolume(stored_volume_value);
    ui->volume_slider->setValue(stored_volume_value);
    m_current_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    m_player->setNotifyInterval(25);
    ui->track_time_lable->setText("00:00 / 00:00");

    m_current_playlist->setObjectName("All tracks");
    m_playlists.append(m_current_playlist);
    add_playlist_to_table(m_current_playlist);

    display_nodata_information();

    m_visualizer = new audio_app::audio_visualizer();
    m_visualizer->set_player(m_player);
    m_visualizer->show();


    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("./MusicDatabase.db");
    if(!m_database.open()){
        qDebug("database is not created!");
        return;
    }

    m_query = new QSqlQuery(m_database);
    m_query->exec("CREATE TABLE MusicDatabase(Path TEXT, Playlist TEXT);");

    m_model = new QSqlTableModel(this, m_database);
    m_model->setTable("MusicDatabase");
    m_model->select();

    m_query->exec("SELECT * FROM MusicDatabase");
    while(m_query->next()){
        QString track_path = m_query->value(0).toString();
        QString playlist_name = m_query->value(1).toString();
        if (!track_path.isEmpty()) {
            QFile track(track_path);
            bool is_found = false;
            for (int i = 0; i < m_current_playlist->mediaCount(); i++) {
                QMediaContent mediaContent = m_current_playlist->media(i);
                QString path = mediaContent.canonicalUrl().toString().toLower();
                QString small_track_path = track_path.toLower();
                if (small_track_path == path) {
                    is_found = true;
                }
            }
            if (!is_found) {
                add_track_to_playlist(
                        track, track_path, m_current_playlist, false
                );
            }
            if (!playlist_name.isEmpty() && playlist_name != "All tracks") {
                int index = return_playlist_index_if_exists(playlist_name);
                if (index == -1) {
                    create_playlist_with_given_name(playlist_name, true);
                    index = return_playlist_index_if_exists(playlist_name);
                }
                add_track_to_playlist(
                        track, track_path, m_playlists[index], true
                );
            }
        } else if (!playlist_name.isEmpty()) {
            int index = return_playlist_index_if_exists(playlist_name);
            if (index == -1) {
                create_playlist_with_given_name(playlist_name, true);
            }
        }
    }

    if (!m_current_playlist->isEmpty()){
        change_the_displayed_track_information(0);
        m_current_playlist->setCurrentIndex(0);
    }

    connect(ui->tracks_table_view, &QTableView::doubleClicked, m_current_playlist, [this](const QModelIndex &index){
        m_current_playlist->setCurrentIndex(index.row());
    });

    connect(m_current_playlist, &QMediaPlaylist::currentIndexChanged, ui->album_cover_lable, [this](int index){
        if (m_current_playlist->currentIndex()!=-1)
        {
            change_the_displayed_track_information(index);
        }
    });

    connect(ui->playlists_table_view, &QTableView::clicked, ui->current_playlist_lable, [this](const QModelIndex &index){
            QString str = m_playlists_model->data(m_playlists_model->index(index.row(), 0)).toString();
            ui->current_playlist_lable->setText("● "+str+" ●");
        });

    connect(m_player, &QMediaPlayer::durationChanged, ui->process_slyder, &QSlider::setMaximum);

    connect(m_player, &QMediaPlayer::positionChanged, ui->process_slyder, &QSlider::setValue);

    connect(ui->process_slyder, &QSlider::sliderMoved, m_player, &QMediaPlayer::setPosition);

    connect(m_player, &QMediaPlayer::positionChanged, ui->track_time_lable, [this](){
        QString time = get_str_time_from_seconds(m_player->position()/1000)+'/'+current_track_duracrion;
        ui->track_time_lable->setText(time);
    });

    connect(
            ui->add_playlist_button, &QPushButton::clicked, this,
            &MainWindow::create_playlist
    );

    connect(
            ui->playlists_table_view, &QTableView::clicked, this,
            [=](const QModelIndex &index) { on_playlists_table_clicked(index); }
    );

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_current_playlist_model;
    delete m_current_playlist;
    delete m_player;
}


void MainWindow::set_current_playlist_view() {
    m_current_playlist_model->setHorizontalHeaderLabels(QStringList()<<tr("track_name")<<tr("path")<<tr("time")<<tr("action"));
    ui->tracks_table_view->setModel(m_current_playlist_model);
    ui->tracks_table_view->hideColumn(1);
    ui->tracks_table_view->horizontalHeader()->setVisible(false);
    ui->tracks_table_view->verticalHeader()->setVisible(false);
    ui->tracks_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tracks_table_view->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tracks_table_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tracks_table_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tracks_table_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tracks_table_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tracks_table_view->setColumnWidth(1, 50);
    ui->tracks_table_view->setColumnWidth(2, 50);
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
        add_track_and_playlist_to_database("", new_playlist->objectName());
    }
}

void MainWindow::create_delete_button(int i){
    QPushButton* button = new QPushButton();
    button->setFixedSize(50, 50);
    button->setIcon(QIcon(":/resources/buttons/cross.png"));
    button->setStyleSheet("background-color: rgba(255, 255, 255, 0); padding-top: 10px;");

    connect(button, &QPushButton::clicked, [&] () {
        int index = ui->tracks_table_view->currentIndex().row();
        QString path = ui->tracks_table_view->model()->data(ui->tracks_table_view->currentIndex(), Qt::UserRole).toString();
        delete_track(index);
        delete_track_from_database(path, m_player->playlist()->objectName());

    });
    ui->tracks_table_view->setIndexWidget(m_current_playlist_model->index(i,3), button);

}

void MainWindow::form_track_item(const QString &track_path, QList<QStandardItem *> &items){
    TagLib::MPEG::File mpeg_track (track_path.toStdString().c_str());
    std::string track_name (mpeg_track.tag()->title().toCString(true));
    auto artist = mpeg_track.tag()->artist().toCString(true);
    track_name+=" - "+std::string(artist);
    track_name=' '+track_name;
    current_track_duracrion = get_str_time_from_seconds(mpeg_track.audioProperties()->lengthInSeconds());
    items.append(new QStandardItem(track_name.c_str()));
    items.append(new QStandardItem(track_path));
    items.append(new QStandardItem(current_track_duracrion));
    items.append(new QStandardItem());
}


void MainWindow::add_track_to_playlist(QFile &track, const QString &track_path, QMediaPlaylist* playlist, bool is_from_database){
    if(track.exists()){
        QList<QStandardItem *> items;
        form_track_item(track_path, items);
        if(!is_from_database){
            m_current_playlist_model->appendRow(items);
        }
        create_delete_button(playlist->mediaCount());
        playlist->addMedia(QUrl(track_path));
    }
}

void MainWindow::add_track_and_playlist_to_database(
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


void MainWindow::on_load_tracks_button_clicked()
{
    QModelIndex index = ui->playlists_table_view->currentIndex();
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
        if (m_query->value(0).toString() == "0") {
            QFile track(track_path);
            add_track_to_playlist(track, track_path, m_current_playlist, false);
            add_track_and_playlist_to_database(track_path, "All tracks");
            if (m_playlists[row]->objectName() != "All tracks") {
                add_track_to_playlist(
                        track, track_path, m_playlists[row], true
                );
                add_track_and_playlist_to_database(
                        track_path, m_playlists[row]->objectName()
                );
            }
        }
    }

    if (!m_current_playlist->isEmpty() && tracks_num==0){
        change_the_displayed_track_information(0);
        m_current_playlist->setCurrentIndex(0);
    }
}


void MainWindow::delete_track(int index)
{
    QModelIndex i = m_current_playlist_model->index(m_current_playlist->currentIndex(), 1);
    QString str1 = m_current_playlist_model->data(i).toString();

    m_current_playlist_model->removeRows(index,1);
    m_current_playlist->removeMedia(index);
    if (m_current_playlist->mediaCount()==0){
        display_nodata_information();
        return;
    }

    i = m_current_playlist_model->index(m_current_playlist->currentIndex(), 1);
    QString str2 = m_current_playlist_model->data(i).toString();

    if (str1!=str2){
        change_the_displayed_track_information(m_current_playlist->currentIndex());
    }
}

void MainWindow::delete_track_from_database(const QString &track_path, QString playlist_name){
    qDebug() <<playlist_name;
    if(playlist_name==""){
        playlist_name = "All tracks";
    }
    if(playlist_name!="All tracks"){
        m_query->prepare("DELETE FROM MusicDatabase WHERE Path = :path AND Playlist = :playlist_name");
        m_query->bindValue(":path", track_path);
        m_query->bindValue(":playlist", playlist_name);
        bool b = m_query->exec();
        if (!b) {
            qDebug() << "Unable to delete the track from the database";
        }
    }
    else{
        m_query->prepare("DELETE FROM MusicDatabase WHERE Path = :path");
        m_query->bindValue(":path", track_path);
        bool b = m_query->exec();
        if (!b) {
            qDebug() << "Unable to delete the track from the database";
        }
    }
}

void MainWindow::on_next_track_button_clicked()
{
    if (m_current_playlist->nextIndex()==m_current_playlist->currentIndex()){
        m_player->setPosition(0);
    }
    else{
        m_current_playlist->next();
    }
}

void MainWindow::on_previous_track_button_clicked()
{
    if (m_current_playlist->previousIndex()==m_current_playlist->currentIndex()){
        m_player->setPosition(0);
    }
    else{
        m_current_playlist->previous();
    }
}

void MainWindow::on_play_pause_button_clicked()
{
    if (!m_current_playlist->isEmpty() && m_player->state()!=QMediaPlayer::PlayingState) {
        m_player->play();
        ui->play_pause_button->setChecked(true);
    }
    else {
        m_player->pause();
        ui->play_pause_button->setChecked(false);
    }
}

void MainWindow::on_mute_button_clicked()
{
    is_muting=true;

    if (m_player->isMuted()){
        m_player->setMuted(false);
        ui->mute_button->setChecked(false);
        if (last_volume_slider_value==0){
            last_volume_slider_value=20;
        }
        ui->volume_slider->setValue(last_volume_slider_value);
        m_player->setVolume(last_volume_slider_value);
    }

    else {
        last_volume_slider_value=m_player->volume();
        ui->volume_slider->setValue(0);
        m_player->setMuted(true);
        ui->mute_button->setChecked(true);
    }

    is_muting=false;
}

void MainWindow::on_volume_slider_valueChanged(int value)
{
    if (!is_muting){
        m_player->setVolume(value);
        if ((m_player->isMuted() && value>0) || (!m_player->isMuted() && value==0)){
            on_mute_button_clicked();
        }
    }
}

void MainWindow::on_random_mode_button_clicked()
{
    if (ui->random_mode_button->isChecked()){
        m_current_playlist->setPlaybackMode(QMediaPlaylist::Random);
        ui->track_loop_mode_button->setChecked(false);
    }
    else{
        m_current_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
}

void MainWindow::on_track_loop_mode_button_clicked()
{
    if (ui->track_loop_mode_button->isChecked()){
        m_current_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
        ui->random_mode_button->setChecked(false);
    }
    else {
        m_current_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
}


void MainWindow::on_playlists_table_clicked(const QModelIndex &index) {
    int row = index.row();
    m_current_playlist_model->clear();
    set_current_playlist_view();

    QMediaPlaylist *playlist = new QMediaPlaylist(m_playlists[row]);
    m_player->setPlaylist(playlist);

    for (int j = 0; j < m_playlists[row]->mediaCount(); j++) {
        QList<QStandardItem *> items;
        form_track_item(m_playlists[row]
                        ->media(j)
                        .canonicalUrl()
                        .toString(), items);
        m_current_playlist_model->appendRow(items);
        create_delete_button(j);
    }
}

void MainWindow::change_the_displayed_track_information (int track_index){

    TagLib::MPEG::File mpeg_track (m_current_playlist_model->data(m_current_playlist_model->index(track_index, 1)).toString().toStdString().c_str());
    current_track_duracrion = get_str_time_from_seconds(mpeg_track.audioProperties()->lengthInSeconds());

    TagLib::ID3v2::Tag* tag = mpeg_track.ID3v2Tag();

    if (tag)
    {
        TagLib::ID3v2::FrameList frames = tag->frameList("APIC");
        if (!frames.isEmpty()) {
            TagLib::ID3v2::AttachedPictureFrame* cover = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
            if (cover)
            {
                QImage coverImage;
                coverImage.loadFromData(reinterpret_cast<const uchar*>(cover->picture().data()), static_cast<int>(cover->picture().size()));
                QPixmap pixmap = QPixmap::fromImage(coverImage);
                ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));
            }
        }
        else
        {
            QPixmap pixmap(":/resources/cover.png");
            ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));
        }
        if (tag->title().isEmpty()){
            ui->track_title_lable->setText("No Title");
        }
        else{
            ui->track_title_lable->setText (tag->title().toCString(true));
        }
        if (tag->artist().isEmpty()){
            ui->track_artist_album_lable->setText("No Artist");
        }
        else{
            std::string track_artist_album (tag->artist().toCString(true));
            auto album = tag -> album().toCString(true);
            if (std::string (album) != ""){
                track_artist_album += " - " + std::string (album);
                ui->track_artist_album_lable->setText (track_artist_album.c_str());
            }
        }
        return;
    }
    display_nodata_information();
}

void MainWindow::display_nodata_information (){
    ui->track_title_lable->setText("No Title");
    ui->track_artist_album_lable->setText("No Artist");
    QPixmap pixmap(":/resources/cover.png");
    ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));
}

QString MainWindow::get_str_time_from_seconds(int seconds){
    QString str;
    seconds+=3;
    int hours = seconds / 3600;
    seconds %= 3600;
    int minutes = seconds / 60;
    seconds %= 60;
    if (hours){
        str+=QString::number(hours)+":";
    }
    str+=QString::number(minutes)+":";

    if (seconds<10){
        str+='0';
    }
    str+=QString::number(seconds);
    return str;
}

void MainWindow::connect_new_shortcut_with_function(QKeySequence key, QShortcut* shortcut, const char* slot) {
    shortcut = new QShortcut(this);
    shortcut->setKey(key);
    QObject::connect(shortcut, SIGNAL(activated()), this, slot);
}

void MainWindow::set_shortcuts(){

    connect_new_shortcut_with_function(QKeySequence(Qt::Key_F6), volume_up_shortcut, SLOT(increase_volume()));
    connect_new_shortcut_with_function(QKeySequence(Qt::Key_F5), volume_down_shortcut, SLOT(decrease_volume()));
    connect_new_shortcut_with_function(QKeySequence(Qt::Key_F4), volume_off_shortcut, SLOT(on_mute_button_clicked()));

    connect_new_shortcut_with_function(QKeySequence(Qt::Key_Space), pause_play_shortcut, SLOT(on_play_pause_button_clicked()));
    connect_new_shortcut_with_function(QKeySequence(Qt::Key_Right), next_track_shortcut, SLOT(on_next_track_button_clicked()));
    connect_new_shortcut_with_function(QKeySequence(Qt::Key_Left), previous_track_shortcut, SLOT(on_previous_track_button_clicked()));

    connect_new_shortcut_with_function(QKeySequence(Qt::Key_F11), expand_window, SLOT(expand_window_fullscreen()));
    connect_new_shortcut_with_function(QKeySequence(Qt::Key_F12), toggle_app, SLOT(toggle()));
}

void MainWindow::increase_volume(){
    on_volume_slider_valueChanged(ui->volume_slider->value()+5);
    ui->volume_slider->setValue(ui->volume_slider->value()+5);

}

void MainWindow::decrease_volume(){
    on_volume_slider_valueChanged(ui->volume_slider->value()-5);
    ui->volume_slider->setValue(ui->volume_slider->value()-5);
}

void MainWindow::toggle_window(QMainWindow* mainWindow) {
    if (mainWindow->isMinimized() || mainWindow->isMaximized()) {
        mainWindow->showNormal();
    } else {
        mainWindow->showMinimized();
    }
}

void MainWindow::make_fullscreen(QMainWindow* mainWindow) {
    if (mainWindow->isFullScreen()) {
        mainWindow->showNormal();
    } else {
        mainWindow->showFullScreen();
    }
}

void MainWindow::expand_window_fullscreen(){
    make_fullscreen(this);
}

void MainWindow::toggle(){
    toggle_window(this);
}
