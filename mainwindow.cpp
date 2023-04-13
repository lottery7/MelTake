#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/fwd.hpp"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QWidget>
#include <QFileDialog>
#include <QPixmap>

#include "QCoreApplication"
#include <taglib/tag.h>
#include <taglib/taglib.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>

#include "include/audio_visualizer.hpp"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // делает окно слегка прозрачным
    setWindowOpacity( 0.95 );
    // для красивой реализации нужно будет добавить размытие и увеличение контрастности

    m_playlist_model = new QStandardItemModel(this);
    m_playlist_model->setHorizontalHeaderLabels(QStringList()<<tr("track_name")<<tr("path")); // заменить тупь на трек
    ui->playlist_tabel_view->setModel(m_playlist_model);
    ui->playlist_tabel_view->hideColumn(1);
    ui->playlist_tabel_view->horizontalHeader()->setVisible(false); // спрятать заголовки столбцов
    ui->playlist_tabel_view->setSelectionBehavior(QAbstractItemView::SelectRows);   /// Разрешаем выбирать строки
    ui->playlist_tabel_view->setSelectionMode(QAbstractItemView::SingleSelection);   /// Разрешаем выбирать только один трек
    ui->playlist_tabel_view->setEditTriggers(QAbstractItemView::NoEditTriggers);   /// Отключаем редактирование
    ui->playlist_tabel_view->horizontalHeader()->setStretchLastSection(true);

    m_player = new QMediaPlayer(this);
    m_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);
    m_player->setVolume(stored_volume_value);
    ui->volume_slider->setValue(stored_volume_value);
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);

    // визуализатор
    m_visualizer = new audio_app::audio_visualizer;
    m_visualizer->set_media_player(m_player);
    m_visualizer->resize(800, 320);
    m_visualizer->show();

    // установка дефолтной обложки
    QPixmap pixmap(":/resources/cover.png");
    ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));

    if(QFile::exists(TRACK_DATA)){
        qDebug()<<"file exists";
    }
    else{
        QFile file("track_data.txt");
    }

    QFile track_data_file(TRACK_DATA);
    if(track_data_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream data_stream(&track_data_file);
        QString track_path;
        data_stream.seek(0);
        while(!data_stream.atEnd()){
            track_path=data_stream.readLine();
            if(track_path.left(8)=="volume: "){
                track_path=track_path.mid(8);
                ui->volume_slider->setValue(track_path  .toInt());
                continue;
            }
            track_path = track_path.mid(1,track_path.size()-2);
            QFile track(track_path);
            if(track.exists()){
                QList<QStandardItem *> items;
                TagLib::MPEG::File mpeg_track (track_path.toStdString().c_str());
                std::string track_name (mpeg_track.tag()->title().toCString(true));
                auto artist = mpeg_track.tag()->artist().toCString(true);
                track_name+=" - "+std::string(artist);
                items.append(new QStandardItem(track_name.c_str()));
                items.append(new QStandardItem(track_path));
                m_playlist_model->appendRow(items);
                m_playlist->addMedia(QUrl(track_path));
            }
        }

        // Глянуть!
        if (!m_playlist->isEmpty()){
            TagLib::MPEG::File mpeg_track (m_playlist_model->data(m_playlist_model->index(0, 1)).toString().toStdString().c_str());
            TagLib::ID3v2::Tag* tag = mpeg_track.ID3v2Tag();
            if (tag) {
                TagLib::ID3v2::FrameList frames = tag->frameList("APIC");
                if (!frames.isEmpty()) {
                    TagLib::ID3v2::AttachedPictureFrame* cover = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
                    if (cover){
                        QImage coverImage;
                        coverImage.loadFromData(reinterpret_cast<const uchar*>(cover->picture().data()), static_cast<int>(cover->picture().size()));
                        QPixmap pixmap = QPixmap::fromImage(coverImage);
                        ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));}
                    else{
                        QPixmap pixmap(":/resources/cover.png");
                        ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));
                    }
                }
            }
            ui->track_title_lable->setText(tag->title().toCString(true));
            std::string track_artist_album (mpeg_track.tag()->artist().toCString(true));
            auto album = mpeg_track.tag()->album().toCString(true);
            if (std::string(album)!=""){
                track_artist_album+=" - "+std::string(album);
            }
            ui->track_artist_album_lable->setText(track_artist_album.c_str());
        }

    }

            // при дабл-клике по треку устанавливаем его в плейлисте
    connect(ui->playlist_tabel_view, &QTableView::doubleClicked, m_playlist, [this](const QModelIndex &index){
        m_playlist->setCurrentIndex(index.row());
    });


            // установить название и обложку выбранного трека (вынесу потом это в отдельную функцию, а то в 3-ех местах повторяется такой кусище)
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, ui->album_cover_lable, [this](int index){
        TagLib::MPEG::File mpeg_track (m_playlist_model->data(m_playlist_model->index(index, 1)).toString().toStdString().c_str());
        TagLib::ID3v2::Tag* tag = mpeg_track.ID3v2Tag();
        if (tag) {
            TagLib::ID3v2::FrameList frames = tag->frameList("APIC");
            if (!frames.isEmpty()) {
                TagLib::ID3v2::AttachedPictureFrame* cover = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
                if (cover){
                QImage coverImage;
                coverImage.loadFromData(reinterpret_cast<const uchar*>(cover->picture().data()), static_cast<int>(cover->picture().size()));
                QPixmap pixmap = QPixmap::fromImage(coverImage);
                ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));}
                else{
                QPixmap pixmap(":/resources/cover.png");
                ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));
                }
            }
        }
        ui->track_title_lable->setText(tag->title().toCString(true));
        std::string track_artist_album (mpeg_track.tag()->artist().toCString(true));
        auto album = mpeg_track.tag()->album().toCString(true);
        if (std::string(album)!=""){
            track_artist_album+=" - "+std::string(album);
        }
        ui->track_artist_album_lable->setText(track_artist_album.c_str());
    });

            // изменение progress_bar при смене трека
    connect(m_player,&QMediaPlayer::durationChanged, ui->track_progress_bar,[&] (qint64 duration){
        ui->track_progress_bar->setMaximum(duration);
    });

            // изменение progress_bar при проигрывание
    connect(m_player,&QMediaPlayer::positionChanged, ui->track_progress_bar,[&] (qint64 position){
        ui->track_progress_bar->setValue(position);
    });
}

MainWindow::~MainWindow()
{
    // запись обновленных настроек в файл с датой
    QFile track_data(TRACK_DATA);
    QStringList track_paths;
    QTextStream data_stream(&track_data);
    if(track_data.open(QIODevice::ReadWrite | QIODevice::Text)){
        while(!data_stream.atEnd()){
            QString track = data_stream.readLine();
            if(track.mid(0,8)!="volume: "){
                track_paths<<track;
            }
        }
        QString s = "volume: "+QString::number(ui->volume_slider->value());
        track_paths.prepend(s);
    }
    track_data.resize(0);
    for(int i = 0;i<=track_paths.size()-1;i++){
        data_stream<<track_paths.at(i)<<"\n";
    }

    delete ui;
    delete m_playlist_model;
    delete m_playlist;
    delete m_player;
}

void MainWindow::on_load_tracks_button_clicked()
{
    auto tracks_num = m_playlist->mediaCount();
    ui->statusbar->showMessage("load_tracks_button_clicked");
    QStringList track_paths = QFileDialog::getOpenFileNames(this,tr("Open files"),QString(),tr("MPEG Files (*.mp3 *.mpeg *.mpg)"));
    QFile track_data_file(TRACK_DATA);
    if(track_data_file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){ /// Мне не нравиться эта строка, она не дает загрузить треки, если что-то не так датой

        for (auto &track_path: track_paths){
            QTextStream data_stream(&track_data_file);

            data_stream.setAutoDetectUnicode(true);
            data_stream.setCodec("UTF-8");

            QString track_info='"'+track_path+'"';
            bool is_found = false;
            data_stream.seek(0);
            while(!data_stream.atEnd()){
                QString line = data_stream.readLine();
                if(line==track_info){
                    is_found=true;
                    break;
                }
            }
            if(!is_found){
                data_stream<<track_info<<"\n";
                QList<QStandardItem *> items;
                TagLib::MPEG::File mpeg_track (track_path.toStdString().c_str());
                std::string track_name (mpeg_track.tag()->title().toCString(true));
                auto artist = mpeg_track.tag()->artist().toCString(true);
                track_name+=" - "+std::string(artist);
                items.append(new QStandardItem(track_name.c_str()));
                items.append(new QStandardItem(track_path));
                m_playlist_model->appendRow(items);
                m_playlist->addMedia(QUrl(track_path));             
            }
        }
    }


    if (!m_playlist->isEmpty() && tracks_num==0){
        TagLib::MPEG::File mpeg_track (m_playlist_model->data(m_playlist_model->index(0, 1)).toString().toStdString().c_str());
        TagLib::ID3v2::Tag* tag = mpeg_track.ID3v2Tag();
        if (tag) {
            TagLib::ID3v2::FrameList frames = tag->frameList("APIC");
            if (!frames.isEmpty()) {
                TagLib::ID3v2::AttachedPictureFrame* cover = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(frames.front());
                if (cover){
                    QImage coverImage;
                    coverImage.loadFromData(reinterpret_cast<const uchar*>(cover->picture().data()), static_cast<int>(cover->picture().size()));
                    QPixmap pixmap = QPixmap::fromImage(coverImage);
                    ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));}
                else{
                    QPixmap pixmap(":/resources/cover.png");
                    ui->album_cover_lable->setPixmap(pixmap.scaled(300,300,Qt::KeepAspectRatio));
                }
            }
        }
        ui->track_title_lable->setText(tag->title().toCString(true));
        std::string track_artist_album (mpeg_track.tag()->artist().toCString(true));
        auto album = mpeg_track.tag()->album().toCString(true);
        if (std::string(album)!=""){
            track_artist_album+=" - "+std::string(album);
        }
        ui->track_artist_album_lable->setText(track_artist_album.c_str());
    }

}


void MainWindow::on_next_track_button_clicked()
{
    QString qs =QString::number(m_playlist->currentIndex())+" -> "+ QString::number(m_playlist->nextIndex());
    ui->statusbar->showMessage(qs);

    if (m_playlist->nextIndex()==m_playlist->currentIndex()){
        m_player->setPosition(0);
    }
    else{
        m_playlist->next();
    }
}


void MainWindow::on_previous_track_button_clicked()
{
    QString qs =QString::number(m_playlist->currentIndex())+" -> "+QString::number(m_playlist->previousIndex());
    ui->statusbar->showMessage(qs);
    if (m_playlist->previousIndex()==m_playlist->currentIndex()){
        m_player->setPosition(0);
    }
    else{
        m_playlist->previous();
    }
}


void MainWindow::on_play_pause_button_clicked()
{
    ui->statusbar->showMessage("play_pause_button_clicked");
    if (!m_playlist->isEmpty() && m_player->state()!=QMediaPlayer::PlayingState) {
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
    ui->statusbar->showMessage("mute_button_clicked");
    is_muting=true;

    if (m_player->isMuted()){ // разьъючиваем
        m_player->setMuted(false);
        ui->mute_button->setChecked(false);
        if (last_volume_slider_value==0){
            last_volume_slider_value=20;
        }
        ui->volume_slider->setValue(last_volume_slider_value);
        m_player->setVolume(last_volume_slider_value);
    }

    else { // замъючиваем
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
        if (m_player->isMuted() && value>0 || !m_player->isMuted() && value==0){ // рязмъючиваем
            on_mute_button_clicked();
        }
    }
}

void MainWindow::on_random_mode_button_clicked()
{
    m_playlist->setPlaybackMode(QMediaPlaylist::Random);
    ui->loop_mode_button->setChecked(false);
    ui->track_loop_mode_button->setChecked(false);
}


void MainWindow::on_loop_mode_button_clicked()
{
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);
    ui->random_mode_button->setChecked(false);
    ui->track_loop_mode_button->setChecked(false);
}


void MainWindow::on_track_loop_mode_button_clicked()
{
    m_playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    ui->random_mode_button->setChecked(false);
    ui->loop_mode_button->setChecked(false);
}

void MainWindow::delete_track_from_datafile(QString track_path_to_delete){
    QFile track_data_file(TRACK_DATA);
    QString left_track_data;
    if(track_data_file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){
        QTextStream data_stream(&track_data_file);
        data_stream.seek(0);
        while(!data_stream.atEnd()){
            QString line = data_stream.readLine();
            if(!(line.mid(1,line.size()-2)==track_path_to_delete)){
                left_track_data.append(line+"\n");
            }
        }
        track_data_file.resize(0);
        data_stream<<left_track_data;
        track_data_file.close();
    }
}
