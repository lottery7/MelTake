/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *main_layout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *load_tracks_button;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *main_trackl_layout;
    QLabel *album_cover_lable;
    QVBoxLayout *verticalLayout_3;
    QLabel *track_title_lable;
    QLabel *track_artist_album_lable;
    QHBoxLayout *track_control_panel_layout;
    QGridLayout *play_panel;
    QPushButton *previous_track_button;
    QSpacerItem *horizontalSpacer;
    QPushButton *play_pause_button;
    QPushButton *next_track_button;
    QGridLayout *gridLayout;
    QPushButton *random_mode_button;
    QPushButton *track_loop_mode_button;
    QPushButton *loop_mode_button;
    QPushButton *mute_button;
    QSlider *volume_slider;
    QSpacerItem *horizontalSpacer_4;
    QProgressBar *track_progress_bar;
    QTableView *playlist_tabel_view;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1200, 800);
        QPalette palette;
        QBrush brush(QColor(180, 200, 193, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Window, brush);
        QBrush brush1(QColor(240, 240, 240, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush);
        MainWindow->setPalette(palette);
        main_layout = new QWidget(MainWindow);
        main_layout->setObjectName(QString::fromUtf8("main_layout"));
        verticalLayout_2 = new QVBoxLayout(main_layout);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout_2->setContentsMargins(10, 10, 10, 10);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        load_tracks_button = new QPushButton(main_layout);
        load_tracks_button->setObjectName(QString::fromUtf8("load_tracks_button"));

        horizontalLayout_3->addWidget(load_tracks_button);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_3);

        main_trackl_layout = new QHBoxLayout();
        main_trackl_layout->setSpacing(10);
        main_trackl_layout->setObjectName(QString::fromUtf8("main_trackl_layout"));
        main_trackl_layout->setSizeConstraint(QLayout::SetDefaultConstraint);
        album_cover_lable = new QLabel(main_layout);
        album_cover_lable->setObjectName(QString::fromUtf8("album_cover_lable"));
        album_cover_lable->setEnabled(true);
        album_cover_lable->setScaledContents(true);

        main_trackl_layout->addWidget(album_cover_lable);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetMinimumSize);
        track_title_lable = new QLabel(main_layout);
        track_title_lable->setObjectName(QString::fromUtf8("track_title_lable"));
        QFont font;
        font.setPointSize(11);
        font.setBold(true);
        track_title_lable->setFont(font);

        verticalLayout_3->addWidget(track_title_lable);

        track_artist_album_lable = new QLabel(main_layout);
        track_artist_album_lable->setObjectName(QString::fromUtf8("track_artist_album_lable"));

        verticalLayout_3->addWidget(track_artist_album_lable);

        track_control_panel_layout = new QHBoxLayout();
        track_control_panel_layout->setSpacing(10);
        track_control_panel_layout->setObjectName(QString::fromUtf8("track_control_panel_layout"));
        track_control_panel_layout->setSizeConstraint(QLayout::SetMinimumSize);
        play_panel = new QGridLayout();
        play_panel->setObjectName(QString::fromUtf8("play_panel"));
        play_panel->setSizeConstraint(QLayout::SetMinimumSize);
        play_panel->setVerticalSpacing(0);
        previous_track_button = new QPushButton(main_layout);
        previous_track_button->setObjectName(QString::fromUtf8("previous_track_button"));

        play_panel->addWidget(previous_track_button, 1, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        play_panel->addItem(horizontalSpacer, 1, 1, 1, 1);

        play_pause_button = new QPushButton(main_layout);
        play_pause_button->setObjectName(QString::fromUtf8("play_pause_button"));
        play_pause_button->setCheckable(true);

        play_panel->addWidget(play_pause_button, 0, 1, 1, 1);

        next_track_button = new QPushButton(main_layout);
        next_track_button->setObjectName(QString::fromUtf8("next_track_button"));

        play_panel->addWidget(next_track_button, 1, 2, 1, 1);


        track_control_panel_layout->addLayout(play_panel);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
        random_mode_button = new QPushButton(main_layout);
        random_mode_button->setObjectName(QString::fromUtf8("random_mode_button"));
        random_mode_button->setCheckable(true);

        gridLayout->addWidget(random_mode_button, 0, 0, 1, 1);

        track_loop_mode_button = new QPushButton(main_layout);
        track_loop_mode_button->setObjectName(QString::fromUtf8("track_loop_mode_button"));
        track_loop_mode_button->setCheckable(true);

        gridLayout->addWidget(track_loop_mode_button, 1, 0, 1, 1);

        loop_mode_button = new QPushButton(main_layout);
        loop_mode_button->setObjectName(QString::fromUtf8("loop_mode_button"));
        loop_mode_button->setCheckable(true);
        loop_mode_button->setChecked(true);

        gridLayout->addWidget(loop_mode_button, 2, 0, 1, 1);

        mute_button = new QPushButton(main_layout);
        mute_button->setObjectName(QString::fromUtf8("mute_button"));
        mute_button->setCheckable(true);

        gridLayout->addWidget(mute_button, 3, 0, 1, 1);

        volume_slider = new QSlider(main_layout);
        volume_slider->setObjectName(QString::fromUtf8("volume_slider"));
        volume_slider->setValue(60);
        volume_slider->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(volume_slider, 3, 1, 1, 1);


        track_control_panel_layout->addLayout(gridLayout);


        verticalLayout_3->addLayout(track_control_panel_layout);


        main_trackl_layout->addLayout(verticalLayout_3);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        main_trackl_layout->addItem(horizontalSpacer_4);


        verticalLayout_2->addLayout(main_trackl_layout);

        track_progress_bar = new QProgressBar(main_layout);
        track_progress_bar->setObjectName(QString::fromUtf8("track_progress_bar"));
        track_progress_bar->setEnabled(true);
        track_progress_bar->setValue(0);
        track_progress_bar->setTextVisible(false);

        verticalLayout_2->addWidget(track_progress_bar);

        playlist_tabel_view = new QTableView(main_layout);
        playlist_tabel_view->setObjectName(QString::fromUtf8("playlist_tabel_view"));

        verticalLayout_2->addWidget(playlist_tabel_view);

        MainWindow->setCentralWidget(main_layout);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        load_tracks_button->setText(QCoreApplication::translate("MainWindow", "load_tracks", nullptr));
        album_cover_lable->setText(QCoreApplication::translate("MainWindow", "Standart_app_cover", nullptr));
        track_title_lable->setText(QCoreApplication::translate("MainWindow", "Track_Title", nullptr));
        track_artist_album_lable->setText(QCoreApplication::translate("MainWindow", "Track_ Artist - Track_Album", nullptr));
        previous_track_button->setText(QCoreApplication::translate("MainWindow", "previous_track", nullptr));
        play_pause_button->setText(QCoreApplication::translate("MainWindow", "play_pause", nullptr));
        next_track_button->setText(QCoreApplication::translate("MainWindow", "next_track", nullptr));
        random_mode_button->setText(QCoreApplication::translate("MainWindow", "Random", nullptr));
        track_loop_mode_button->setText(QCoreApplication::translate("MainWindow", "Track Loop", nullptr));
        loop_mode_button->setText(QCoreApplication::translate("MainWindow", "Loop", nullptr));
        mute_button->setText(QCoreApplication::translate("MainWindow", "mute", nullptr));
        track_progress_bar->setFormat(QCoreApplication::translate("MainWindow", "%p%", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
