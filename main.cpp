#include "mainwindow.h"
#include <fwd.hpp>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Первоначально объявляем текущий плейлист и трек (спросить - почему при разкомменчивание программа crushed)
        audio_app::playlist current_playlist;
        audio_app::track current_track;
        MainWindow w;
        // Первоначально паказывается отображение по умолчанию

    // Далее проверяем - знает ли плеер хотя бы об одной музыкальной композиции
        //   Нужно создать что-то постоянное (не удаляется после закрытия),
        //   где можно хранить все изветные плееру треки
        if (true){
            // если приложение не знает ни об одном музыкальном файле,
            // то предложить пользователю указать пользователю путь до папки/файла) и трека
        }
        else {
            // если знает, то
            // 1) открыть плейлист, прослушиваемый до закрытия приложения
            // 2) создать defaul_playlist
        }

    // далее присваеваем значение  переменной current_track

    w.show();
    return a.exec();
}