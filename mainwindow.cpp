#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "include/fwd.hpp"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QWidget>
#include <QFileDialog>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // делает окно слегка прозрачным
    setWindowOpacity( 0.95 );
    // для красивой реализации нужно будет добавить размытие

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

    if(QFile::exists(TRACK_DATA)){
        qDebug()<<"file exists";
    }
    else{
        QFile file("track_data.txt");
    }


    /// Получение дефолтного плейлиста из уже знакомых треков из прошлого открытия приложения
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
                 ui->volume_slider->setValue(track_path.toInt());
                 continue;
             }
             track_path = track_path.mid(1,track_path.size()-2);
             QFile track(track_path);
             if(track.exists()){
                 QList<QStandardItem *> items;
                 items.append(new QStandardItem(QDir(track_path).dirName()));
                 items.append(new QStandardItem(track_path));
                 m_playlist_model->appendRow(items);
                 m_playlist->addMedia(QUrl(track_path));
             }
         }
     }

    // при дабл-клике по треку устанавливаем его в плейлисте
    connect(ui->playlist_tabel_view, &QTableView::doubleClicked, m_playlist, [this](const QModelIndex &index){
        m_playlist->setCurrentIndex(index.row());
    });

    // установить название выбранного трека
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, ui->current_track_label, [this](int index){
        ui->current_track_label->setText(m_playlist_model->data(m_playlist_model->index(index, 0)).toString());
    });

    // изменение progress_bar при смене трека
    connect(m_player,&QMediaPlayer::durationChanged, ui->track_progress_bar,[&] (qint64 duration){
        ui->track_progress_bar->setMaximum(duration);
    });

    // изменение progress_bar при проигрывание
    connect(m_player,&QMediaPlayer::positionChanged, ui->track_progress_bar,[&] (qint64 position){
        ui->track_progress_bar->setValue(position);
    });

//    connect(ui->track_progress_bar,&Q)

}

MainWindow::~MainWindow()
{

    ///запись обновленных настроек в файл с датой
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
    ui->statusbar->showMessage("load_tracks_button_clicked");
    QStringList files = QFileDialog::getOpenFileNames(this,tr("Open files"),QString(),tr("Audio Files (*.mp3)"));
    QFile track_data_file(TRACK_DATA);
    if(track_data_file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){
        for (auto &u:files){
            QTextStream data_stream(&track_data_file);

            data_stream.setAutoDetectUnicode(true);
            data_stream.setCodec("UTF-8");

            QString track_info='"'+u+'"';
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
                items.append(new QStandardItem(QDir(u).dirName()));
                items.append(new QStandardItem(u));
                m_playlist_model->appendRow(items);
                m_playlist->addMedia(QUrl(u));
            }
        }
    }
}


void MainWindow::on_next_trak_button_clicked()
{
    ui->statusbar->showMessage("next_track_button_clicked");
    if (m_playlist->nextIndex()==m_playlist->currentIndex()){
        m_player->setPosition(0);
    }
    else{
        m_playlist->next();
    }
}


void MainWindow::on_previous_track_button_clicked()
{
    ui->statusbar->showMessage("previou_track_button_clicked");
    if (m_playlist->previousIndex()==m_playlist->currentIndex()){
        m_player->setPosition(0);
    }
    else{
        m_playlist->previous();
    }
}


void MainWindow::on_play_pause_button_clicked()
{
//    ui->play_pause_button->setIcon(QIcon(":/icons/..."));
//    ui->play_pause_button->setIconSize(QSize(65, 65));
    ui->statusbar->showMessage("play_pause_button_clicked");


    // проверка на то, что музыка есть!
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
//    last_volume_slider_value=m_player->volume();
//    ui->statusbar->showMessage(QString::number(last_volume_slider_value));
    is_muting=true;

    if (m_player->isMuted()){ // разьъючиваем
        m_player->setMuted(false);
        ui->mute_button->setChecked(false);

        // Стоит ли разрешать проигрывать трек на 0 громкости?
        if (last_volume_slider_value==0){
            last_volume_slider_value=10;
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
    ui->statusbar->showMessage("volume_slider_valueChanged");
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

void delete_track_from_datafile(QString track_path_to_delete){
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

