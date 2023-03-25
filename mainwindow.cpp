#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardItemModel>
#include <QWidget>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
        
    // Настройка таблицы плейлиста
        m_playlist_model = new QStandardItemModel(this);
        ui->playlist_tabel_view->setModel(m_playlist_model);    // Устанавливаем модель данных в TableView
        // Устанавливаем заголовки таблицы

        m_playlist_model->setHorizontalHeaderLabels(QStringList()  << tr("track")
                                                                << tr("path"));
        ui->playlist_tabel_view->hideColumn(1);    // Скрываем колонку, в которой хранится путь к файлу
        ui->playlist_tabel_view->verticalHeader()->setVisible(false);                  // Скрываем нумерацию строк
        ui->playlist_tabel_view->setSelectionBehavior(QAbstractItemView::SelectRows);  // Включаем выделение строк
        ui->playlist_tabel_view->setSelectionMode(QAbstractItemView::SingleSelection); // Разрешаем выделять только одну строку
        ui->playlist_tabel_view->setEditTriggers(QAbstractItemView::NoEditTriggers);   // Отключаем редактирование
        // Включаем подгонку размера последней видимой колонки к ширине TableView
        ui->playlist_tabel_view->horizontalHeader()->setStretchLastSection(true);

        m_player = new QMediaPlayer(this);          // Инициализируем плеер
        m_playlist = new QMediaPlaylist(m_player);  // Инициализируем плейлист
        m_player->setPlaylist(m_playlist);          // Устанавливаем плейлист в плеер
        m_player->setVolume(70);                    // Устанавливаем громкость воспроизведения треков
        m_playlist->setPlaybackMode(QMediaPlaylist::Loop);  // Устанавливаем циклический режим проигрывания плейлиста

        // При даблклике по треку в таблице устанавливаем трек в плейлисте
           connect(ui->playlist_tabel_view, &QTableView::doubleClicked, [this](const QModelIndex &index){
               m_playlist->setCurrentIndex(index.row());
           });

           // при изменении индекса текущего трека в плейлисте, устанавливаем название файла в специальном лейбле
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
    // С помощью диалога выбора файлов делаем множественный выбор mp3 файлов
       QStringList files = QFileDialog::getOpenFileNames(this,
                                                         tr("Open files"),
                                                         QString(),
                                                         tr("Audio Files (*.mp3)"));

       // Далее устанавливаем данные по именам и пути к файлам
       // в плейлист и таблицу отображающую плейлист
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
}


void MainWindow::on_previous_track_button_clicked()
{
    ui->statusbar->showMessage("previou_track_button_clicked");
}


void MainWindow::on_start_stop_button_clicked()
{
    try{
    if (ui->start_stop_button->isChecked()){m_player->play();}
    else {m_player->stop();}
    ui->statusbar->showMessage("start_stop_button_clicked");
    }
    catch (std::exception &ex){
        ui->statusbar->showMessage(ex.what());
    }

}

