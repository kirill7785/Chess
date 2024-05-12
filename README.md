# ascii-шахматы
![alt_text](https://github.com/kirill7785/Chess/blob/main/pic/cheh2.jpg)  
«Искусство жизни больше напоминает искусство борьбы…» Марк Аврелий.  
Принцип оптимальности Р. Беллмана -  «Оптимальная политика обладает тем свойством, что, каковы бы ни были начальное состояние и принятое начальное решение, последующие решения должны составлять оптимальную политику относительно состояния, возникшего в результате первоначального решения.»  

![alt_text](https://raw.githubusercontent.com/kirill7785/Chess/main/pic/новые%20фигуры.bmp)

# Шахматная доска
Физические шахматы без движка. Фигуры перемещаются мышью. Начинают белые. Щелчок левой кнопки мыши по фигуре подсвечиват клетки доски куда фигура может пойти. Повторный щелчок левой кнопкой мыши по подсвеченному полю приводит к ходу ранее выбранной фигуры на это поле.  Если на поле куда переместилась наша фигура до этого стояла вражеская фигура, то эта вражеская фигура удаляется из списка фигур. Есть возврат произвольно допустимого числа ходов назад - по одному полуходу назад Edit ->Undo (или стрелка влево). Листание вперед произвольно допустимое число раз Edit->Next (или стрелка вправо).  Возврат очередного хода назад также возвращает назад и сьеденную на этом ходу вражескую фигуру. Программа хранит историю полуходов всей партии. Если мы вернулись на m полуходов назад, а потом пошли путем щелчка левой кнопкой мыши сначала по фигуре, потом по полю куда надо пойти этой фигурой, то после этого хода история в суффиксе файла логгирования будет формироваться заново и суффикс файла логгирования будет утерян путем перезаписи. Т.е. после этого нового хода нельзя будет просмотреть с помощью стрелки вправо вперед предысторию до этого хода если мы до этого двигались назад с пощью стрелки влево. История в суффиксе файла логгирования будет формироваться заново.   Можно просто двигать фигуры на доске. Неправильный ход не по правилам автоматически отменяется. Если после щелчка левой кнопкой мыши по фигуре следующим щелчком левой кнопки мыши щелкнуть по другой фигуре которая сможет пойти, то хода не последует, а произойдет смена активной фигуры которая собирается ходить с соответствующей подсветкой новых квадратов на доске. Работает интеллектуальная подсветка хода. Предусмотрены рокировка и взятие на проходе. Рокировка задаётся ходом короля на две клетки, а ладья перемещается автоматом в нужное место мгновенно. Играют двое людей или один человек сам с собой. Это графический интерфейс для будущего шахматного движка. Всего 6000 строк кода на Паскале в среде Lazarus. По окончани партии программа объяляет мат или пат в Form1.caption. Запись партии  ведётся и после того как партия сыграна ходы можно посмотреть и проанализировать - используются стрелки влево и вправо. Можно вернутся назад к определённому ходу и продолжить партию с этого места. В программе есть сохранение состояния включая файл логгирования в файл File->Save  и считывания из файла текущего состояния включая файл логгирования File->Open. Можно сохранять в файл на диск и просматривать читая с диска  в программе целые шахматные партии. Смотри пример партии мат Легаля в папке с exe файлом. 08.07.2023 Фигуры сделаны на выбор двух видов. Пример одних реализованных фигур показан на картинке доски выше. 
![alt_text](https://github.com/kirill7785/Chess/blob/main/pic/Алёхин%20и%20Капабланка.jpg)
(Алехин и Капабланка Москва 1913.)
![alt_text](https://github.com/kirill7785/Chess/blob/main/pic/Ход%20чёрных%20мат%20в%204%20хода.png)
(Шахматная задача (четырёхходовка). Чёрные начинают и выигрывают.)
Добавлен редактор начальной расстановки фигур на шахматной доске. При запуске программы установлена начальная позиция белых и чёрных войск. Чтобы создать пользовательскую расстановку фигур нужно сбросить настройки к пустой доске без фигур Edit->Set empty board. Далее нажимая мышью на один из 64 свободных квадратов на доске необходимо выбрать цвет и фигуру устанавливаемую в этот квадрат, а также ответить на вопрос ставим ли мы фигуру на доску или завершаем расстановку начальной позиции фигур. При ответе - ставим фигуру на доску (continue) пользователь щелкает по пустым квадратам мышью и выбирает что на них будет посталено (цвет, фигура). Никакой проверки пользовательского ввода на данный момент не реализовано. Если пользователь ошибся при вводе фигуры, то можно установить пустую фигуру empty figure, это приведёт к удалению фигуры из списка фигур на доске. По завершении расстановки фигур в начальной позиции пользователю будет задан вопрос чей ход: черных или белых и делали ли белые и черные рокировку. На этом работа редактора начальной позиции завершается. Есть сохранение и чтение из файла установленной начальной позиции. Это позволяет записывать шахматные задачи, записывать решения шахматных задач, обдумывать решение шахматных задач в редакторе без движка. 14 июля 2023 появилась возможность увеличения размеров доски по нажатии стрелки вверх на клавиатуре. Уменьшение - стрелка вниз на клавиатуре. Доску с фигурами можно увеличить во весь экран монитора. Я надеюсь это снизит нагрузку на глаза пользователей нашей шахматной программы. Также я добавил возможность просмотра повёрнутой доски.
![alt_text](https://github.com/kirill7785/Chess/blob/main/pic/Приложение%20Шахматная%20доска%20собираемое%20в%20RAD%20Studio%2010.4.1.png)
Приложение шахматная доска перенесено из Lazarus в RAD Studio 10.4.1 с полным сохранением функциональности.
![alt_text](https://github.com/kirill7785/Chess/blob/main/pic/chess_tree.jpg)
# chess_cpp - алгоритм минимакс
(алгоритм минимакс) на языке С++. Программа играет за черных. chess.exe консольная программа на С++. Пользователь вводит ходы за белых в стиле e2e4. Компьютер отвечает за чёрных. Консольное приложение на С++ для визуализации использует консольную графику. Для правильного отображения консольной графики нужно установить Линукс терминал Gygwin64 Therminal и запустить исполняемый файл chess.exe в cygwin терминале.
Я начал писать движок летом 2022 года по выходным. Вдохновился вот этими материалами  https://yandex.ru/video/preview/15975598581602463921
https://yandex.ru/video/preview/9677257828908314356
https://yandex.ru/video/preview/14890789634999508129
Спасибо автору этих видео. Они вдохновили меня начать писать шахматы. Только я выбрал язык C++ для движка и Pascal для интерфейса. Что касается самих шахмат то я очень слабый любитель. 
# Алгоритм (ветвей и границ)
Все современные шахматные программы пользуются вариантами вот этого алгоритма:  

int Поиск(альфа, бета, глубина)  
{  
if (глубина <= 0) return -ФорсированныйВариант(-бета, -альфа);  
if (МалоВероятноПозицияЛучшеАльфы(глубина, альфа)) return альфа;  
if (ОченьВероятноПозицияЛучшеБеты(глубина, бета)) return бета;  
if (ОценкаПозицииИзвестна(глубина)) return оценка;  
ГенерацияВсехХодов();  
СортируемХоды();  
for (i = 0; i < количество_ходов; ++i) {  
ДелаемХод(i);  
оценка = -Поиск(-бета, -альфа, НоваяГлубина(i, глубина));  
ВозвращаемХод(i);  
if (оценка > альфа) {  
if (оценка >= бета) return оценка;  
альфа = оценка;  
}  
}  
if (количество_ходов == 0) {  
if (НамШах()) return глубина - МАТ;  
else return 0; // пат  
}  
return альфа;  
}  

int ФорсированныйВариант(альфа, бета)  
{  
оценка = СтатическаяОценкаПозиции();  
if (оценка >= бета) return оценка;  
if (оценка > альфа) альфа = оценка;  
ГенерацияВзятий(); // если очень хочется, то можно добавить и шахи  
СортируемХоды();  
for (i = 0; i < количество_ходов; ++i) {  
if (ОченьВероятноХодНеУлучшаетПозициюДоАльфы(i, альфа)) continue; // отсечение бесполезных ходов  
ДелаемХод(i);  
оценка = -ФорсированныйВариант(-бета, -альфа);  
ВозвращаемХод(i);  
if (оценка > альфа) {  
if (оценка > бета) return оценка;  
альфа = оценка;  
}  
}  
return альфа;  
}  
  
Все основные отличия шахматных прог друг от друга в следующих функциях:


СтатическаяОценкаПозиции()

Обычно состоит из:

в начале игры:

материал
безопасность и атаки на королей
пешечная структура
проходные пешки
централизация фигур
мобильность фигур

к концу игры немного меняется:

безопасность и атаки на королей - не учитывается
централизация короля
становятся более значимыми проходные пешки




МалоВероятноПозицияЛучшеАльфы(глубина)  
ОченьВероятноПозицияЛучшеБеты(глубина)  
ОценкаПозицииИзвестна(глубина)  

Эти три функции помогают существенно сокращать перебор.
У простейших прог этих функций вовсе нет.
В простых прогах тут используются хэш-таблица, в которой храняться оценки уже просчитанных позиций. Если в переборе позиция встретиться второй раз (за счет перестановки ходов), то можно пропустить повторный перебор.
В функции ОченьВероятноПозицияЛучшеБеты часто применяется эвристика пустого хода. Коротко можно объяснить так: если мы пропустим ход, а соперник не сможет нас опустить (ниже беты), то, видимо, у нас очень хорошая позиция. Эту эвристику нельзя применять, если есть вероятность цугцванга.
Функцию МалоВероятноПозицияЛучшеАльфы - используют очень редко, если вообще используют :-)




СортируемХоды()

Очень важно как можно лучше сортировать ходы.
Прога с оптимальной сортировкой ходов будет считать варианты в два раза длиннее, чем прога с наихудшей сортировкой.




НоваяГлубина(i, глубина)

Эта функция определяет насколько интересен текущий ход.
В простейших прогах все ходы считаются равноинтересными и функция возвращает: глубина - 1.
В простых шахи считаются более интересными и у них единичка не вычитается.
В более продвинутых определяются и другие типы ходов заслуживающих более длительного анализа (размены, ходы усиливающие атаку на короля, ходы угрожающие выигрышем материала).
Некоторые ходы могут быть признаны и вовсе малоинтересными (например, жертва ферзя, без видимых причин). Тут можно из глубины вычитать больше единицы.




Обычно проги при контроле времени несколько секунд на ход успевают расчитывать на глубину 8 - 10 полуходов (в середине партии).
Этого вполне достаточно чтобы находить маты.

![alt_text](https://github.com/kirill7785/Chess/blob/main/pic/Ускоренный%20дракон.jpg)
# debut_book.txt
Дебютная книга - это список из 70 шахматных позиций и ответов на них за чёрных. Если чёрные находят позицию на доске в дебютной книге они отвечают по дебютному справочнику. Если одной позиции соответствует несколько ответов за чёрных то выбирается случайный ответ из списка предложенных. Дебютная книга используется для того чтобы черные делали первые ходы более разнообразно. Если не будет дебютной книги то можно запомнить последовательность ходов и воспроизводить ее из раза в раз компьютер будет отвечать детерминированно. Дебютная книга вносит приятный элемент случайности в ходы чёрных. Дебютная книга очень маленькая и была создана всего за один день без подготовки. 3 декабря 2023 дебютная книга пересмотрена и расширена до 240 уникальных позиций. Изменён формат дебютной книги, он стал более понятен человеку. Новая дебютная книга на 240 позиций пока не подключена к программе т.к. нужно еще написать скрипт переводящий ее в числовой формат для машины. Новая книга появится в версии программы e2e4 1.03 на сайте FreeSoft. Пока доступна версия дебютной книги из 70 позиций в программе e2e4 1.02.
# ascii-chess 1967
 Движок подключен к интерфейсу. Ссылка на исполняемый файл project1.exe в папке https://github.com/kirill7785/Chess/tree/main/ascii-chess%201967. Исполняемый файл project1.exe нужно запускать. При этом в одной папке с файлом project1.exe должна лежать chess.dll и debut_book.txt.  Движок реализует:    
 1. два полухода без форсированного варианта;  
 2.  четыре полухода без форсированного варианта;  
 3.    два полухода c форсированным вариантом;  
 4. шесть полуходов с форсированным вариантом;  
 5.  четыре полухода c форсированным вариантом.  
     Первый вариант пропускает детский мат, но сопротивляется. Программу приятно обыгрывать совсем слабому любителю шахмат. Шесть полуходов считаются наиболее долго, поэтому будьте осторожны потянет ли ваш процессор в разумное время 6 полуходов. На xeon 2630v4 2.2GHz
 ответный ход компьютера 6 полуходов с форсированным вариантом занимает несколько секунд, т.е. не мгновенно. Изборочные продления форсированного варианта на глубине 6 очень сильно сократили время работы alpha-beta алгоритма за счёт отсечений по сравнению со случаем alpha-beta без продлений, которая работала от 4 до 11мин на ход. Исходник Шахматная доска в настоящий момент заморожен на случай если кто то захочет свой движок к нему прикрутить, а разработка после присоединения моего движка продолжена в папке ascii-chess 1967.
 # отличие версии ascii-chess 1968 от версии 1967
 1. Улучшенные ascii символы фигур.  
 2. Перечень съеденных фигур отображается справа во время партии.  
 3. 14 июля 2023 появилась возможность увеличения размеров доски по нажатии стрелки вверх на клавиатуре. Уменьшение - стрелка вниз на клавиатуре. Доску с фигурами можно увеличить во весь экран монитора. Я надеюсь это снизит нагрузку на глаза пользователей нашей шахматной программы.
 # отличие версии ascii-chess 1969 от предыдущих версий (Актуальная версия на 02.01.2024).
 1. Добавлен режим просмотра сыгранной партии.
 2. Возможна игра с часами.
 3. Доску можно поворачивать на 180 градусов.
 4. Добавлен режим помошника когда ЭВМ подсказывает лучший ход пользователю который играет за белых.
 5. Если на вашей ЭВМ фигуры стоят неровно есть возможность подстроить центровку фигур по клеткам доски.
 6. Добавлен редактор расстановки начальной позиции на доске. Предусмотрены различные форы.
 7. Добавлен режим бот против бота. В этом режиме компьютер играет сам с собой и это позволяет прогнозировать развитие партии вперёд на несколько ходов, что можно использовать при  решении шахматных задач.
 8. Добавлено чтение/запись FEN позиций (анализатор FEN строки).
 9. Добавлена библиотека шахматных головоломок: мат в один, два, три и четыре хода. Всего порядка 2500 головоломок добавлено.
 # ascii-chess 1970 (FreeSoft.ru e2e4 1.03) (Актуальная версия на 12.05.2024).
 1. Добавлено 391к головоломок.
 2. Доступна дебютная база в 240 позиций.
 3. Опционально реализовано детектирование мата в статической оценке позиции.
 4. Исправлены некоторые баги, подправлен графический интерфейс пользователя.
 5. Добавлена запись сыгранной партиии в формате pgn.
    Надеюсь программа отработае штатно и вы получите удовольствие от использования программы.  
 # parallel chess engine 
 По материалам статьи https://github.com/hennimohammed/parallel_chess_engine я распараллелил alpha-beta алгоритм м получил ускорение работы движка на 6 полуходов за счёт задействования четырёх потоков центрального процессора. Результаты ускорения приведены в таблице - время одного полухода:  
 один поток 9с; 19с; 30с; 94с;   
 четыре потока 3с; 7с; 13с; 36с.  
 Это ускорение только за счёт распараллеливания alpha-beta алгоритма.  
# Критика моего шахматного движка (моей шахматной программы)
6 полуходов считаются от 2 до 11с. На 8 полуходов уходит от 2 до 5минут. Программа получилась медленная. Я реализовал в программе эвристику нулевого хода и эвристику убийцы (один лучший ход) это ускорило программу. Без эвристики нулевого хода и эвристики убийцы 6 полуходов считалось также долго как сейчас 8 полуходов т.е. за 2мин. С учетом эвристики нулевого хода и эвристики убийцы 6 полуходами можно пользоваться т.к. 11с не проблема. 5 минут для восьми полуходов напротив нужно уже тратить часа два, три на партию что невсегда возможно. (для запуска используется 4 ядерный процессор 2.2ГГц). Профилирование показывает что код получился медленным. На глубине 6 полуходов 13,6% уходит на стат оценку позиции на доске, 13% на генерацию перемещений, 12% на проверку есть ли Шах, 7,4% на стат цену пешки, 6% на генерацию легальных ходов ферзя, 6% на вычисление легальной мобильности фигур, 5% на коническую оценку, 4,9% на генерацию легальных ходов коня, 4% на генерацию легальных ходов слона, 2% на генерацию легальных ходов Короля, 1,5% на вычисление форсированных продлений (собственное время, полное же время на продления 42% так как они часто вызываются), и т.д. Критика в том что многое из этого нужно сделать принципиально быстрее используя специальные техники. Например стат оценку доски можно быстро вычислять не меняя большую часть, а меняя только то что изменилось от одной сходившей фигуры. Часто позиции отличаются всего одним ходом. Но это требует переделывания и возможно усложнения создания программмного кода. Я написал по простому, с первого раза и получилось медленно.  
 # Известные проблемы
 1. В версиях Шахматная доска, ascii-chess 1967, ascii-chess 1968 исправлен user Serega315 bug report:
Kf3 любой за черных, пешка f может перепрыгнуть коня. 
Программа тестируется. Явных багов не обнаружено.
# Системные требования
Программа работает под ОС Windows x64 на персональном компьютере.
# Используемое ПО
движок собран в Visual Studio C++ Comunity 2022. Также использовался c++ компилятор clang++. Графический интерфейс в Lazarus 3.2. free Pascal 3.2.2. Графический интерфейс приложения Шахматная доска собран Delphi RAD Studio 10.4.1 под windows 32bit.


