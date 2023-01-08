# Chess
![alt_text](https://raw.githubusercontent.com/kirill7785/Chess/main/pic/шахматная%20доска.bmp)

# Шахматная доска
Физические шахматы без движка. Фигуры перемещаются мышью. Сьеденная фигура автоматом сьедается. Есть возврат одного хода назад (всего одного, но в любой момент игры после того как очередной ход сделан).  Можно просто двигать фигуры на доске. Работает интеллектуальная подсветка хода. Предусмотрены рокировка и взятие на проходе. 
Играют двое людей или человек сам с собой. Это графический интерфейс для будущего шахматного движка. Всего 5386 строк кода на Паскале в среде Lazarus. По окончани партии программа объяляет мат или пат в Form1.caption.
# chess_cpp
Шахматный движок (алгоритм минимакс) на языке С++. Программа играет за черных. chess.exe консольная программа на С++. Пользователь вводит ходы за белых в стиле e2e4. Компьютер отвечает за чёрных. Консольное приложение на С++ для визуализации использует консольную графику. Для правильного отображения консольной графики нужно установить Линукс терминал Gygwin64 Therminal и запустить исполняемый файл chess.exe в cygwin терминале.
Я начал писать движок летом 2022 года по выходным. Вдохновился вот этими материалами  https://yandex.ru/video/preview/15975598581602463921
https://yandex.ru/video/preview/9677257828908314356
https://yandex.ru/video/preview/14890789634999508129
Спасибо автору этих видео. Они вдохновили меня начать писать шахматы. Только я выбрал язык C++ для движка и Pascal для интерфейса. Что касается самих шахмат то я очень слабый любитель. Любовь к шахматам мне привил мой дед который играл со мной в шахматы в школьные годы. Без него бы ничего не было.
# debut_book.txt
Дебютная книга из 70 шахматных позиций и ответов на них за чёрных. Дебютная книга используется для того чтобы черные делали первые ходы более разнообразно. Если не будет дебютной книги то можно запомнить последоватеьность ходов и воспроизводить ее из раза в раз компьютер будет отвечать детерминированно. Дебютная книга вносит приятный элемен случайности в ходы чёрных. Дебютная книга очень маленькая и была создана всего за один день без подготовки.
# Проблемы:
 В программе нет сохранения состояния в файл и считывания из файла текущего состояния. Записи партии не ведётся и после того как партия сыграна ходы посмотреть нельзя.
 Движок не подключен к интерфейсу.
# Системные требования
Программа работает под ОС Windows.


