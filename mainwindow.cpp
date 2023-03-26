#include "mainwindow.h"
#include "./ui_mainwindow.h"
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

    m_playlist_model = new QStandardItemModel(this);
    m_playlist_model->setHorizontalHeaderLabels(QStringList()<<tr("track_name")<<tr("path")); // заменить тупь на трек
    ui->playlist_tabel_view->setModel(m_playlist_model);
    ui->playlist_tabel_view->hideColumn(1);
    ui->playlist_tabel_view->horizontalHeader()->setVisible(false); // спрятот заголовки столбцов
    ui->playlist_tabel_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->playlist_tabel_view->setSelectionMode(QAbstractItemView::SingleSelection);
//        ui->playlist_tabel_view->setEditTriggers(QAbstractItemView::NoEditTriggers);   /// Отключаем редактирование
    ui->playlist_tabel_view->horizontalHeader()->setStretchLastSection(true);

    m_player = new QMediaPlayer(this);
    m_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);
    m_player->setVolume(stored_volume_value);
    ui->volume_slider->setValue(stored_volume_value);

    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);  // Устанавливаем циклический режим проигрывания плейлиста

    // При даблклике по треку в таблице устанавливаем трек в плейлисте
    connect(ui->playlist_tabel_view, &QTableView::doubleClicked, [this](const QModelIndex &index){
        m_playlist->setCurrentIndex(index.row());
    });


    // установить название выбранного трека
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, [this](int index){
        ui->current_track_label->setText(m_playlist_model->data(m_playlist_model->index(index, 0)).toString());
    });

}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_playlist_model;
    delete m_playlist;
    delete m_player;
}


void MainWindow::on_load_tracks_button_clicked()
{
    ui->statusbar->showMessage("load_tracks_button_clicked");
    QStringList files = QFileDialog::getOpenFileNames(this,tr("Open files"),QString(),tr("Audio Files (*.mp3)"));

    //// Передать полученные пaths

    // Далее устанавливаем данные по именам и пути к файлам
    // в плейлист и таблицу отображающую плейлист
    // Добавить проверку на то, что трек уже находится в листе
    foreach (QString filePath, files) {
        QList<QStandardItem *> items;
        items.append(new QStandardItem(QDir(filePath).dirName()));
        items.append(new QStandardItem(filePath));
        m_playlist_model->appendRow(items);
        m_playlist->addMedia(QUrl(filePath));
    }




}

void MainWindow::on_next_trak_button_clicked()
{
    ui->statusbar->showMessage("next_track_button_clicked");
//    m_player.
}


void MainWindow::on_previous_track_button_clicked()
{
    ui->statusbar->showMessage("previou_track_button_clicked");
}


void MainWindow::on_play_pause_button_clicked()
{
//    ui->play_pause_button->setIcon(QIcon(":/icons/..."));
//    ui->play_pause_button->setIconSize(QSize(65, 65));
    ui->statusbar->showMessage("play_pause_button_clicked");

    // проверка на то, что музыка есть!
    if (m_player->state()!=QMediaPlayer::PlayingState) {
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

