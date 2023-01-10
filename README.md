# ascii-шахматы
![alt_text](https://github.com/kirill7785/Chess/blob/main/pic/cheh2.jpg)
«Искусство жизни больше напоминает искусство борьбы…» Марк Аврелий
Принцип оптимальности Беллмана -  «Оптимальная политика обладает тем свойством, что, каковы бы ни были начальное состояние и принятое начальное решение, последующие решения должны составлять оптимальную политику относительно состояния, возникшего в результате первоначального решения.»

![alt_text](https://raw.githubusercontent.com/kirill7785/Chess/main/pic/шахматная%20доска.bmp)

# Шахматная доска
Физические шахматы без движка. Фигуры перемещаются мышью. Начинают белые. Щелчок левой кнопки мыши по фигуре подсвечиват клетки доски куда фигура может пойти. Повторный щелчок левой кнопкой мыши по подсвеченному полю приводит к ходу ранее выбранной фигуры на это поле.  Если на поле куда переместилась наша фигура до этого стояла вражеская фигура, то эта вражеская фигура удаляется из списка фигур. Есть возврат произвольно допустимого числа ходов назад - по одному полуходу назад Edit ->Undo (или стрелка влево). Листание вперед произвольно допустимое число раз Edit->Next (или стрелка вправо).  Возврат очередного хода назад также возвращает назад и сьеденную на этом ходу вражескую фигуру. Программа хранит историю полуходов всей партии. Если мы вернулись на m полуходов назад, а потом пошли путем щелчка левой кнопкой мыши сначала по фигуре, потом по полю куда надо пойти этой фигурой, то после этого хода история в суффиксе файла логгирования будет формироваться заново и суффикс файла логгирования будет утерян путем перезаписи. Т.е. после этого нового хода нельзя будет просмотреть с помощью стрелки вправо вперед предысторию до этого хода если мы до этого двигались назад с пощью стрелки влево. История в суффиксе файла логгирования будет формироваться заново.   Можно просто двигать фигуры на доске. Неправильный ход не по правилам автоматически отменяется. Если после щелчка левой кнопкой мыши по фигуре следующим щелчком левой кнопки мыши щелкнуть по другой фигуре которая сможет пойти, то хода не последует, а произойдет смена активной фигуры которая собирается ходить с соответствующей подсветкой новых квадратов на доске. Работает интеллектуальная подсветка хода. Предусмотрены рокировка и взятие на проходе. Рокировка задаётся ходом короля на две клетки, а ладья перемещается автоматом в нужное место мгновенно. Играют двое людей или один человек сам с собой. Это графический интерфейс для будущего шахматного движка. Всего 6000 строк кода на Паскале в среде Lazarus. По окончани партии программа объяляет мат или пат в Form1.caption. Запись партии  ведётся и после того как партия сыграна ходы можно посмотреть и проанализировать - используются стрелки влево и вправо. Можно вернутся назад к определённому ходу и продолжить партию с этого места. В программе есть сохранение состояния включая файл логгирования в файл File->Save  и считывания из файла текущего состояния включая файл логгирования File->Open. Можно сохранять в файл на диск и просматрвать читая с диска  в программе целые шахматные партии. Смотри пример партии мат Легаля в папке с exe файлом.
# chess_cpp - алгоритм минимакс
(алгоритм минимакс) на языке С++. Программа играет за черных. chess.exe консольная программа на С++. Пользователь вводит ходы за белых в стиле e2e4. Компьютер отвечает за чёрных. Консольное приложение на С++ для визуализации использует консольную графику. Для правильного отображения консольной графики нужно установить Линукс терминал Gygwin64 Therminal и запустить исполняемый файл chess.exe в cygwin терминале.
Я начал писать движок летом 2022 года по выходным. Вдохновился вот этими материалами  https://yandex.ru/video/preview/15975598581602463921
https://yandex.ru/video/preview/9677257828908314356
https://yandex.ru/video/preview/14890789634999508129
Спасибо автору этих видео. Они вдохновили меня начать писать шахматы. Только я выбрал язык C++ для движка и Pascal для интерфейса. Что касается самих шахмат то я очень слабый любитель. Любовь к шахматам мне привил мой дед который играл со мной в шахматы в школьные годы. Без него бы ничего не было.
# debut_book.txt
Дебютная книга из 70 шахматных позиций и ответов на них за чёрных. Дебютная книга используется для того чтобы черные делали первые ходы более разнообразно. Если не будет дебютной книги то можно запомнить последоватеьность ходов и воспроизводить ее из раза в раз компьютер будет отвечать детерминированно. Дебютная книга вносит приятный элемен случайности в ходы чёрных. Дебютная книга очень маленькая и была создана всего за один день без подготовки.
# Проблемы:
 Движок не подключен к интерфейсу.
# Системные требования
Программа работает под ОС Windows.


