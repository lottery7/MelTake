#include "mainwindow.h"
#include "include/fwd.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    
    MainWindow window;
    window.show();
    
    
    

//            audio_app::playlist current_playlist;
//            audio_app::track current_track;


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


//    w.show();


    return a.exec();
}
