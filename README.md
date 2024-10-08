# MelTake
> десктопное приложение для проигрывания и визуализации музыки

# Презентация
https://drive.google.com/drive/folders/1uR5TMsWEFcuoaOUGfCsIEOnIVgv4k9Vq

**Возможности пользователя:** 
- Использование базовых возможностей музыкального плеера (play, stop, повторение трека/плейлиста, следующий/предыдущий трек, изменение громкости)
- Встроенная визуализация музыки
- Создание собственных плейлистов
- Использование горячих клавиш для базовых функций

![ghost_icon](https://github.com/B-E-D-A/audio-app-MelTake/assets/112130616/96c7d71a-d39c-4777-8bf6-d56edbb25ed7)

# Инструкция по сборке

1) Установить Qt 5.15.2 вместе с модулем Qt Multimedia
1) Скачать и скомпилировать библиотеки taglib 1.13 и fftw 3.3.10
1) В qmake_project.pro файле указать пути до файлов библиотек:
	- `TAGLIB_LIB_PATH` - путь до папки с .dll.a файлами библиотеки
	- `FFTW_LIB_PATH` - путь до папки с .dll файлами библиотеки
	- `TAGLIB_INCLUDE_PATH`, `FFTW_INCLUDE_PATH` - пути до папок с заголовочными файлами соответствующих библиотек
1) В `PATH` добавить пути до .dll файлов библиотек, либо скопировать их в папку, гле будет лежать скомпилированная программа
1) Компилируем проект либо в Qt Creator, либо в командной строке:
	```
	qmake
	make
	```
# Вид приложения
![audio_app_qHq0L1kvkk](https://github.com/B-E-D-A/audio-app-MelTake/assets/52651781/0ba50155-c0d1-4b62-9f70-683440b77398)
 ![image](https://github.com/B-E-D-A/audio-app-MelTake/assets/112130616/d827d023-f691-4508-8539-83600a85d687) 

 
