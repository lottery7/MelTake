> # include
### track_structure.hpp
Конструкторы, операторы и методы для структуры трека. Трек хранится по имени и по файловому пути.
* сейчас только конструкторы, операторы= и объявление методов `play_track` + `stop_track` + `edit_track`
### playlist_structure.hpp
Конструкторы, операторы и методы для структуры плейлиста
* сейчас только конструкторы, операторы=, методы `add_new_track_to_playlist`, `add_old_track_to_playlist`, `delete_track_from_playlist`
### settings_structure.hpp
Конструкторы, операторы и методы для структуры настроек
* конструкторы, операторы= и дефолтный вариант настроек(светлая тема), методы `set_theme`, `set_visualisation` (вроде большего и не надо)
### fwd.hpp
В данный момент это удобный для включения в другие файлы hpp-ник со всеми структурами, также здесь прописана структура music_base:
Методы `beginning_setup`, `assert_or_create_trackdata_file`, `upload_info_from_trackdata_file`, `find_playlist`, `app_view` и немножко ошибок, которые могут кинуться в процессе программы
> # src
### track_functions.cpp
Реализация всех функций трека - `play_track`, `stop_track`, `back_track`, `forward_track`,  `repeat_track`, `mix_track`, `edit_track`

`play_track` - трек начинает играть

`stop_track` - трек остановлен

`back_track` - предыдущий трек в установленном порядке треков

`forward_track` - следующий трек в установленном порядке треков

`repeat_track`- повтор трека

`mix_track` - перемешать треки в плейлисте, в котором находится трек, играющий сейчас

`edit_track` - изменить метаданные трека (название, исполнителя, жанр)

* сейчас только базовые функции `play` + `stop` + `edit_track`
### playlist_functions.cpp
Реализация всех функций плейлиста - `play_playlist`, `stop_playlist`, `repeat_playlist`, `mix_playlist`, `edit_playlist`, `create_playlist`, `delete_playlist`, `add_new_track_to_playlist`, `add_old_track_to_playlist`, `delete_track_from_playlist`, `edit_playlist_name`, `edit_playlist_cover`

`play_playlist` - проигрывание плейлиста по порядку добавленных треков

`stop_playlist` - плейлист остановлен

`repeat_playlist`- повтор проигрывания плейлиста, когда дошли до конца списка треков в нем

`mix_playlist` - играть весь плейлист в перемешанном порядке

`edit_playlist`- изменить данные плейлиста (название, обложку, состав) (пользователь будет видеть это как список функций, которые он может выбрать - изменить название, изменить обложку, добавить треки, удалить треки)
- `add_new_track_to_playlist` - пользователь может добавить 1 трек
- `add_old_track_to_playlist` - используется при запуске программы для добавления в систему треки, загруженные ранее
- `delete_track_from_playlist` - пользователь может удалить 1 трек
- `edit_playlist_name` - изменить название плейлиста
- `edit_playlist_cover` - поменять картинку плелиста на новую, загруженную с устройства

`create_playlist` - создать новый плейлист (с пустым названием и без обложки)

`delete_playlist` - удалить какой-то плейлист и все данные в нем (на компьютере, конечно же, не удаляется)

* сейчас написаны `add_new_track_to_playlist`, `add_old_track_to_playlist`, `delete_track_from_playlist`
### settings_functions.cpp
Реализация всех функций настроек - `set_theme`, `set_visualisation`

`set_theme` - изменить тему (светлая/темная)

`set_visualisation` - выбрать вариант визуализации (базовую или посложнее)

* все методы реализованы

### main.cpp
Необходимые действия для запуска программы - закачка ранее загруженных треков в соответствующие плейлисты (`user_music_base.beginning_setup`, `user_music_base.upload_info_from_trackdata_file`) + просмотр, правильно ли все загрузилось (`app_view`)
