/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *ui_current_track_panel;
    QLabel *ui_current_cover;
    QLabel *ui_current_track;
    QHBoxLayout *Control_Panel;
    QPushButton *ui_previous_track_button;
    QPushButton *ui_start_stop_button;
    QPushButton *ui_next_track_button;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(395, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(20, 190, 341, 201));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        ui_current_track_panel = new QHBoxLayout();
        ui_current_track_panel->setObjectName("ui_current_track_panel");
        ui_current_cover = new QLabel(verticalLayoutWidget);
        ui_current_cover->setObjectName("ui_current_cover");

        ui_current_track_panel->addWidget(ui_current_cover);

        ui_current_track = new QLabel(verticalLayoutWidget);
        ui_current_track->setObjectName("ui_current_track");
        ui_current_track->setWordWrap(true);

        ui_current_track_panel->addWidget(ui_current_track);


        verticalLayout->addLayout(ui_current_track_panel);

        Control_Panel = new QHBoxLayout();
        Control_Panel->setObjectName("Control_Panel");
        ui_previous_track_button = new QPushButton(verticalLayoutWidget);
        ui_previous_track_button->setObjectName("ui_previous_track_button");

        Control_Panel->addWidget(ui_previous_track_button);

        ui_start_stop_button = new QPushButton(verticalLayoutWidget);
        ui_start_stop_button->setObjectName("ui_start_stop_button");

        Control_Panel->addWidget(ui_start_stop_button);

        ui_next_track_button = new QPushButton(verticalLayoutWidget);
        ui_next_track_button->setObjectName("ui_next_track_button");

        Control_Panel->addWidget(ui_next_track_button);


        verticalLayout->addLayout(Control_Panel);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        ui_current_cover->setText(QCoreApplication::translate("MainWindow", "Track_Img", nullptr));
        ui_current_track->setText(QCoreApplication::translate("MainWindow", "Track_Name", nullptr));
        ui_previous_track_button->setText(QCoreApplication::translate("MainWindow", "Last_Track_Button", nullptr));
        ui_start_stop_button->setText(QCoreApplication::translate("MainWindow", "Start-Stop-Button", nullptr));
        ui_next_track_button->setText(QCoreApplication::translate("MainWindow", "Next_Track_Button", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
