﻿// chess.cpp : Алгоритм шахматной игры.
// Компьютер играет за чёрных.
// Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
// (6-7).08.2022 Код пешек. Код короля. Пешка делает просто ходы. Взятия на проход нету. В ферзи пешка превращаться не умеет. 
//               Доска печатает себя в коноль. Никакого минимакса еще нет.
// (13-14).08.2022 (выходные 1500 строк в ядре.). Написан код фигур - слон, ладья, ферзь и конь.  Написан алгоритм минимакса. 
// Разыгрываются окончания партий. Нету рокировки, нету взятия на проход,
//                        список ходов в случае шаха не доработан.
// 15.08.2022 (понедельник) установил unity.
// 16.08.2022 (вторник) графическая часть шахматной игры с помощью видео на yuotube написана. Реализует  drag and drop.
// 17.08.2022 (среда) Прикрутил свой шахматрый движок на dll к графической части шахмат. Впервые расставлена начальная позиция фигур.
//            Первый показ Василевскому и Костину.
// 19.08.2022  Можно делать только те ходы фигурами чтобы король не был под шахом. Реализовано 19.08.2022.
// 23.08.2022 Сделаны списки фигур, доска не перебирается в поисках фигуры. Сделана сортировка ходов для alpha-beta алгоритма.
//            После того как сделал поддержку списков фигур и сортировку ходов програма стала значительно медленней играть.
//            Еле хватает на 4 полухода. Возможно в замедлении также сыграло роль что правила игры (шахи, мат пат взятие на проходе
//             стало прописано корректно и это добавило операторов if в программу серьезно добавило.) Хотя надо признать что играть программа стала 
//             интресно. 
// 25.08.2022 Реализовано всё кроме рокировки. Чёрный король думает порядка одной минуты на глубину 4 полухода.
//            В программе прописаны ситуации мат и пат. 25.08.2022 в 17.03 прописал рокировку. Все правила шахматной игры учтены в ядре солвера. 
//            Сделал передачу wList и pList по ссылке что должно ускорить программу.
// 29.08.2022  Интерфейс в юнити падает ссылается на повреждение кучи в dll. Ядро обыгрывает chessTitan из Висты на уровне новичёк (2 из 10). 
// 30.08.2022 Полностью избавился от утечек памяти. По ощущениям программа стала еще быстрее. depth 4 считает за 0.2с.
// 03.09.2022 На depth 2 программа обыгрывает chess Titans уровень 2 (новичёк). Тестирование прошло положительно. Программа не падает.
//            6915 строк кода с нуля.
// 3-4.12.2022 Простейшая дебютная книга на 70 позиций на доске.
// 8.12.2022   Рокировку нельзя делать из под шаха.
// 10.12.2022  Новый консольный визуализатор шахматной доски.
// 11.12.2022  Заменил KING_CENTER_PRIORITY_NUMBER_FIGURES==19 на коническую оценку из Chessprogramming wiki.
// 14.01.2023  Распараллелил алгоритм minimax по OpenMP на два xeon 2630v4. 9431 строка.
//             Применение на первых двух полуходах быстрого поиска на глубину 4 полухода для правильной сортировки ходов ускоряет alpha-beta с 11мин до 3мин 16с.
//            Применение форсированного варианта сильно ускорило alpha-beta алгоритм на глубине 6полуходов.
// 22.01.2023 нашёл статью в интернете https://github.com/hennimohammed/parallel_chess_engine и распараллелил alpha-beta алгоритм.
//            Получил ускорение alpha beta алгоритма:
//            однопоточный время на полуход : 9; 19; 30; 94.
//            четырёх поточный время на полуход : 3; 7; 13; 36.
//            10093 строки кода.
// 11.06.2023 Сообщено об ошибке Первый ход Kf3 любой ход черных и ошибка в том что пешка со стартовой позиции может перепрыгнуть коня (фигуру). Проверено в генераторе ходов для пешки что в движке данной ошибки нет.
// 17.07.2023 (воскресенье 2*CPU xeon 2699v4) null move. Добавлена эвристика нулевого хода.
//            Было 9, 12, 17 параллельный форсированный 6 полуходов. Стало 4, 6, 7 параллельный форсированный 6 полуходов null move. 47, 69, 81 однопоточный форсированный 6 полуходов.
// 21.07.2023 Доработал эвристику нулевого хода: 1. эвристика нулевого хода вызывается с нулевым окном (дополнительные 7% скорости). 2. Эвристика нулевого хода не вызывается при шахе.
//            3. Эвристика нулевого хода вызывается только один раз в каждой ветке ходов. Эвристика нулевого хода реализована только для функции alpha_beta_do1. 10508 строк кода. Подтверждено что метод
//            нулевого хода повысил быстродействие алгоритма на 6 полуходов с форсированным вариантом.
// 23.07.2023 Добавил эвристику убийцы (служба одного лучшего хода на первом месте в списке ходов).
// 10.08.2023 Добавил продления в случае если есть только один ответный ход (не более двух при ply<6),
//            полностью удалил продления в случае выгодных взятий так как программа стреиться кушать фигуры и может попасть в ловушку см. мат Легаля,
//            Добавил продления если promotion пешка превращается в ферзи,
//            Добавил продления если расстояние между ферзём и Королём опасно сокращается (не более 4 раз).
//            Время существенно замедлилось. Добавлен режим быстрая vs интересная (более долгая) игра.
// 23.09.2023 добавлено Futility pruning и Razoring. Добавлен поиск стремления для внешнего вызова минимакс. 14727 строк кода.
// (29-31).12.2023 Реанимировал шахматы на Unity c# v.0.48 && v0.49 на сайте FreeSoft. Исправил ошибку при снятии чёрной фигуры с доски, добавил ход назад, ход вперед, начальную расстановку фигур при нажатии клавиши N на клавиатуре.


#include <iostream>
#include <time.h>
#include <random>
#include <string>
#include <omp.h>


#pragma once

bool Futility_pruning = false;
bool Razoring = false;

using namespace std;

// Если не определено то обычный код с cout
#define CODE_FOR_DLL 1

#ifdef CODE_FOR_DLL

#define EXPORT __declspec(dllexport)

#endif

#define MOBILITY_USER_SETTING true // Ручная (не константная через VALUE) мобильность для Ладьи, Ферзя, Слона и Коня на основе легальных ходов.

// Порог конической оценки. Если коническая оценка меньше POSICIONING_GAME_POROG то это начало партии мобильность ферзя не учитывается,
// а мобильность короля отрицательна.
const float POSICIONING_GAME_POROG = 53.0f;
bool flag_POSICIONING_GAME = false; // false - атакующий стиль, ферзя в бой.

const float static_calibrate = 3.53333f; // На это нужно разделить статическую оценку на это число чтобы получить то что у Стокфиша в начальной позиции - 0.3 для e4.

// Здесь проблемы с реализацией, наверно надо отказаться от этой идеи.
const bool bBOT_INTERESTING = true; // Ослабление игры ЭВМ в 7.5 процентах случаев. Чето не ослабляет или вообще полная лажа получается.
int probability_barier = 0; // Вероятность допуска ошибки ЭВМ в процентах. Мы просто переключаемся на слабого Тьюринга.

bool breduction_white = true; // понижение глубины до единицы за белых

bool bQueekGame = true; // Быстрая игра или дополнительные интересные продления.
int iKingSecurity = 0; // 0 - Никаких продлений в связанных с безопасностью короля не производится.

const int number_cores = 4; // 4 потока

// До трёх ферзей на доске. Много ферзей на пустой доске и количество возможных ходов может превысить 120. 10.08.2023.
const int POOL_SIZE = 250; // 120 100 размер хранилища под количество ходов.
const __int8 POOL_SIZEL = 16;// количество фигур одного цвета.

// 6 - для минимакса.
// 8 - для alpha-beta.
// 2 - Уровень новичёк в chess Titans.
__int8 ilevel_Game = 0; // Уровень игры 0, 1, 2.
__int8 THINKING_DEPTH = 2; //10;  4 глубина анализа позиции в полуходах.
const __int8 None = -1;// несуществующий ход.
const __int8 ENDSHPIL_NUMBER_FIGURES = 8; // Малофигурное окончание менее 8 фигур
const float KING_CENTER_PRIORITY_WEIGHT = 139.0f; // Больше этого порога эндшпиль и Король идет в атаку
const __int8 KING_CENTER_PRIORITY_NUMBER_FIGURES = 19; // В принципе это нормальный порог был.
const float ENDSHPIL_INCREASE_DEPTH_POROUG = 200.0f; // Существенно увеличиваем глубину анализа в глубоком эндшпиле до 12.

int number_chess_positions = 0;


enum class Color { EMPTY = 0 , BLACK = 1, WHITE = 2};

enum class ChessPiece {EMPTY=0, W_KING = 1, W_QUEEN = 2, W_ROOK = 3, W_BISHOP = 4,  W_KNIGHT = 5,   W_PAWN=  6,
                                B_KING = 7, B_QUEEN = 8, B_ROOK = 9, B_BISHOP = 10, B_KNIGHT = 11, B_PAWN = 12};



void set_color(__int8 color)
{
#ifndef CODE_FOR_DLL
    
    if (color == 46) {
        // Команда изменения цвета фона в консоли.
        // Красный
        //printf("\033[0;0m\033[0;41m");//46 - Бирюзовый
        printf("\033[0;0m\033[48;2;0;128;128m"); // Teal
    }
    if (color == 0) {
        printf("\033[0;0m\033[0;40m");
    }
    else if (color == 1) {
        // Зелёный
        //printf("\033[0;0m\033[0;42m");
        printf("\033[0;0m\033[48;2;128;128;128m"); // Gray
    }
    
    
#endif
}

// Куда фигура может ходить.
typedef struct TMOVES {
    __int8 x;
    __int8 y; 

} MOVES_NODE;

MOVES_NODE init() {
    MOVES_NODE res;

    res.x = None;
    res.y = None;

    return res;
}

MOVES_NODE init(__int8 move_init) {
    MOVES_NODE res;

    res.x = move_init;
    res.y = move_init;

    return res;
}

MOVES_NODE init(__int8 x, __int8 y) {
    MOVES_NODE res;

    res.x = x;
    res.y = y;

    return res;
}

typedef struct TLIST_NODE_MOVES {
    MOVES_NODE xy_from; // откуда
    MOVES_NODE xy_to; // куда
    float rate; // Оценка хода.
} LIST_NODE_MOVES;

const int max_h = 100;
LIST_NODE_MOVES killer_euristick[number_cores][max_h];
int number_chess_positions_arr[number_cores];

// Предыдущий ход в партии - это нужно для взятия на проходе.
typedef struct TPREVIOS_MOVE {
    MOVES_NODE xy_from; // откуда
    MOVES_NODE xy_to; // куда
    ChessPiece figure; // какой фигурой был сделан предыдущий ход
} PREVIOS_MOVE;

// Возможные ходы конкретной фигуры.
// У ферзя не более 4*7ходов т.е. типа __int8 хватает с головой.
class MOVES {
public:
    int n = 0; // Количество элементов в массиве возможных шагов.
    MOVES_NODE moves[POOL_SIZE];

    MOVES() {
        n = 0;

        // Инициализация массива. Можно бе нее, т.к. логика кода её не использует, но предупреждение стат анализатора возникает.
        // Если сделать инициализацию то 60% времени альпфа бета алгоритм будет просто тратить на эту бесполезную инициализаци, особенно теперь кода POOL_SIZE увеличен до 120.
        //MOVES_NODE tmp;
        //tmp.x = -1;
        //tmp.y = -1;
        
        //for (int i = 0; i < POOL_SIZE; ++i) {
            //moves[i] = tmp;
        //}
    }

    ~MOVES() {        
        n = 0;
    }

    MOVES(const MOVES& other)
    {
        n = other.n;

        /// You can do this:
        for(int i = 0; i < n; ++i)
           moves[i] = other.moves[i];
        /// or simply call the copy assign operator:
       // operator=(other);
    }

    MOVES& operator=(const MOVES& other)
    {
        // This is only for matrix with the same dimensions.
        // Delete and allocate new array if the dimensions are different. 
        // This is up to the OP if he can have differently sized matrices or not...
        // Also assert will "disappear" if NDEBUG (ie. in release) is defined.
        
        
        n = other.n; 

            for (int i = 0; i < n; ++i)
                moves[i] = other.moves[i];
        
       
        return *this;
    }

    // Добавляет ход (x,y)
    void append(__int8 x, __int8 y)
    {
        // Вообще без каких либо проверок, предполагается что всё корректно.
        ++n;
        //if (n > POOL_SIZE) {
       //     std::cout << " Програмная авария переполнение массива ходов.\n";
       //     system("Pause");
       // }

        moves[n - 1].x = x;
        moves[n - 1].y = y;
    }
    /*void append(__int8 x, __int8 y) {
        if ((x >= 0) && (x <= 7) && (y >= 0) && (y <= 7) && (n <= POOL_SIZE - 1)) {
            ++n;
            if (n - 1 > POOL_SIZE - 1) {
#ifndef CODE_FOR_DLL
                std::cout << "error! moves limit <POOL_SIZE\n";
                system("PAUSE");
#endif
            }
            if ((x >= 0) && (x <= 7)) {
                moves[n - 1].x = x;
            }
            else {
#ifndef CODE_FOR_DLL
                std::cout << "x out of bound " << x << std::endl;
                system("PAUSE");
#endif
            }
            if ((y >= 0) && (y <= 7)) {
                moves[n - 1].y = y;
            }
            else {
#ifndef CODE_FOR_DLL
                std::cout << "y out of bound " << y << std::endl;
                system("PAUSE");
#endif
            }
        }
        else {
            std::cout << " append error!!! error! moves limit <POOL_SIZE\n";
            system("PAUSE");
        }
    }
    */

    // удаляет ход (x,y)
    /*void delete_move(__int8 x, __int8 y) {
        __int8 n1 = 0; // Количество элементов в массиве возможных шагов.
        MOVES_NODE moves1[POOL_SIZE];

        for (__int8 i = 0; i < n; ++i) {
            if ((moves[i].x == x) && (moves[i].y == y)) {
                continue;
            }
            else {
                n1++;
                moves1[n1 - 1] = moves[i];
            }
        }
        n = 0;
        for (__int8 i = 0; i < n1; ++i) {
            n++;
            moves[n - 1] = moves1[i];
        }
       // n1 = 0;
    }*/
    void delete_move(__int8 x, __int8 y) {

        // Экономичный вариант без реалокации памяти 26.08.2022.
        // Работает за одно линейное сканирование массива.

        int j = 0;
        for (int i = 0; i < n; ++i) {
            if ((moves[i].x == x) && (moves[i].y == y)) {
                continue;
            }
            else {
                j++;
                moves[j - 1] = moves[i];
            }
        }
        n = j;
    }

};

// Используется только для списков фигур. Всего фигур при любом раскладе не более 16.
// Каждая фигура в каждый момент времени может занимать только одно поле надоске 8*8.
// Этот тип данных используется для списка белых wList и чёрных bList фигур.
class MOVESL {
public:
    __int8 n = 0; // Количество элементов в массиве возможных шагов.
    MOVES_NODE moves[POOL_SIZEL];

    MOVESL() {
        n = 0;

        // Инициализация массива. Можно бе нее, т.к. логика кода её не использует, но предупреждение стат анализатора возникает.
        MOVES_NODE tmp;
        tmp.x = -1;
        tmp.y = -1;

        for (int i = 0; i < POOL_SIZEL; ++i) {
            moves[i] = tmp;
        }
    }

    ~MOVESL() {
       n = 0;
    }

    MOVESL(const MOVESL& other)
    {        
        n = other.n;

        /// You can do this:
        for (__int8 i = 0; i < n; ++i)
            moves[i] = other.moves[i];
        /// or simply call the copy assign operator:
       // operator=(other);
    }

    /*void realloc_MOVESL() {
        if (moves != nullptr) {

            // n  Количество элементов в массиве возможных шагов.
            MOVES_NODE* moves1 = new MOVES_NODE[n];

            for (__int8 i = 0; i < n; ++i) {
                moves1[i] = moves[i];
            }

            delete[] moves;
            moves = new MOVES_NODE[n]; // 

            for (__int8 i = 0; i < n; ++i) {
                moves[i] = moves1[i];
            }
            delete[] moves1;
        }
    }
    */

    MOVESL& operator=(const MOVESL& other)
    {
        // This is only for matrix with the same dimensions.
        // Delete and allocate new array if the dimensions are different. 
        // This is up to the OP if he can have differently sized matrices or not...
        // Also assert will "disappear" if NDEBUG (ie. in release) is defined.


        n = other.n;

        for (__int8 i = 0; i < n; ++i)
            moves[i] = other.moves[i];


        return *this;
    }

    // Добавляет фигуру с позицией (x,y) в список фигур.
   /* void append(__int8 x, __int8 y) {
        if ((x >= 0) && (x <= 7) && (y >= 0) && (y <= 7) && (n-1 < POOL_SIZEL)) {
            ++n;
            if (n - 1 > POOL_SIZEL- 1) {
#ifndef CODE_FOR_DLL
                printf("error 1 ! moves limit <POOL_SIZEL = %d %d\n", (int)(POOL_SIZEL), (int)(n-1));
                system("PAUSE");
#endif
            }
            if ((x >= 0) && (x <= 7)) {
                moves[n - 1].x = x;
            }
            else {
#ifndef CODE_FOR_DLL
                std::cout << "x out of bound " << x << std::endl;
                system("PAUSE");
#endif
            }
            if ((y >= 0) && (y <= 7)) {
                moves[n - 1].y = y;
            }
            else {
#ifndef CODE_FOR_DLL
                std::cout << "y out of bound " << y << std::endl;
                system("PAUSE");
#endif
            }
        }
        else {
            printf("x= %d y= %d\n",x,y);
            printf("error 2 ! moves limit <POOL_SIZEL = %d %d\n", POOL_SIZEL, n-1);
            system("PAUSE");
        }
    }
    */
    void append(__int8 x, __int8 y)
    {
        // Вообще без каких либо проверок, предполагается что всё корректно.
        ++n;
        moves[n - 1].x = x;
        moves[n - 1].y = y;
    }
    // удаляет фигуру с позицией (x,y) из списка фигур.
    void delete_move(__int8 x, __int8 y) {
        
        // Экономичный вариант без реалокации памяти 26.08.2022.
        // Работает за одно линейное сканирование массива.

        __int8 j = 0;
        for (__int8 i = 0; i < n; ++i) {
            if ((moves[i].x == x) && (moves[i].y == y)) {
                continue;
            }
            else {
                j++;
                moves[j - 1] = moves[i];
            }
        }
        n = j;
    }
    
    /*void delete_move(__int8 x, __int8 y) {
        __int8 n1 = 0; // Количество элементов в массиве возможных шагов.
        MOVES_NODE moves1[POOL_SIZE];

        for (__int8 i = 0; i < n; ++i) {
            if ((moves[i].x == x) && (moves[i].y == y)) {
                continue;
            }
            else {
                n1++;
                moves1[n1 - 1] = moves[i];
            }
        }
        n = 0;
        for (__int8 i = 0; i < n1; ++i) {
            n++;
            moves[n - 1] = moves1[i];
        }
        //n1 = 0;
    }*/

};

// Все возможные ходы для данной доски.
class LIST_MOVES {
public:
    int n = 0; // Количество ходов.
    LIST_NODE_MOVES moves[POOL_SIZE];
 

    LIST_MOVES() {
        n = 0;

        // Инициализация массива. Можно бе нее, т.к. логика кода её не использует, но предупреждение стат анализатора возникает.
        LIST_NODE_MOVES tmp;
        tmp.rate = 0.0f;
        tmp.xy_from.x = -1;
        tmp.xy_from.y = -1;
        tmp.xy_to.x = -1;
        tmp.xy_to.y = -1;

        for (int i = 0; i < POOL_SIZE; ++i) {
            moves[i] = tmp;
        }

    }

    ~LIST_MOVES() {       
        n = 0;
    }

    LIST_MOVES(const LIST_MOVES& other)       
    {       
        n = other.n;

        /// You can do this:
        for (int i = 0; i < n; ++i)
            moves[i] = other.moves[i];
        /// or simply call the copy assign operator:
       // operator=(other);
    }

    LIST_MOVES& operator=(const LIST_MOVES& other)
    {
        // This is only for matrix with the same dimensions.
        // Delete and allocate new array if the dimensions are different. 
        // This is up to the OP if he can have differently sized matrices or not...
        // Also assert will "disappear" if NDEBUG (ie. in release) is defined.


        n = other.n;
        // POOL_SIZE_loc = other.POOL_SIZE_loc;

        for (int i = 0; i < n; ++i)
            moves[i] = other.moves[i];


        return *this;
    }

};


Color get_color1(ChessPiece board_loc[8][8], __int8 x, __int8 y) {
    if (board_loc[y][x] == ChessPiece::EMPTY) {
        return Color::EMPTY;
    }
    else if ((board_loc[y][x] == ChessPiece::W_PAWN) || (board_loc[y][x] == ChessPiece::W_KING) ||
        (board_loc[y][x] == ChessPiece::W_ROOK) || (board_loc[y][x] == ChessPiece::W_BISHOP) ||
        (board_loc[y][x] == ChessPiece::W_QUEEN) || (board_loc[y][x] == ChessPiece::W_KNIGHT))
    {
        return Color::WHITE;
    }
    else return Color::BLACK;
}

class ChessMan {
public:
    Color color;
    ChessMan();
};

ChessMan::ChessMan() : color(Color::EMPTY)
{

}

// Поля которыми чёрные фигуры атакуют белых фигур
void generate_attack_BLACK(bool attack_BLACK[8][8], ChessPiece board[8][8], MOVESL& wList, MOVESL& bList,
    PREVIOS_MOVE previos_moves,
    bool WHITE_O_O, bool WHITE_O_O_O,
    bool BLACK_O_O, bool BLACK_O_O_O);

// Поля которыми белые фигуры атакуют черных фигур
void generate_attack_WHITE(bool attack_WHITE[8][8], ChessPiece board[8][8], MOVESL& wList, MOVESL& bList,
    PREVIOS_MOVE previos_moves,
    bool WHITE_O_O, bool WHITE_O_O_O,
    bool BLACK_O_O, bool BLACK_O_O_O);

// Шахматная доска.
class Board {
public:
    ChessPiece board[8][8];

    // Предыдущий ход в партии - это нужно для взятия на проходе.
    PREVIOS_MOVE previos_moves; // Глобальный доступ к предыдущему сделанному ходу. Значение None если первый ход в партии.

    // Список белых фигур.
    MOVESL wList;
    // Список чёрных фигур.
    MOVESL bList;

    // Начинаем с начала партии.
    bool WHITE_O_O, WHITE_O_O_O; // Флаг рокировок.false можно, true нельзя.
    bool BLACK_O_O, BLACK_O_O_O; // Флаг рокировок.false можно, true нельзя.

    Board();// Конструктор.

    ~Board() {
        wList.~MOVESL();
        bList.~MOVESL();
    }

    // Копирует доску.
    void copy(ChessPiece board2[8][8], MOVESL &wList2, MOVESL &bList2, PREVIOS_MOVE &previos_moves2, bool WHITE_O_O2, bool WHITE_O_O_O2, bool BLACK_O_O2, bool BLACK_O_O_O2) {
        for (__int8 y_1 = 0; y_1 < 8; ++y_1) {
            for (__int8 x_1 = 0; x_1 < 8; ++x_1) {
                board[y_1][x_1] = board2[y_1][x_1];
            }
        }

        wList.n = wList2.n;
        for (__int8 k_1 = 0; k_1 < wList.n; ++k_1) {
            wList.moves[k_1] = wList2.moves[k_1];
        }
        bList.n = bList2.n;
        for (__int8 k_1 = 0; k_1 < bList.n; ++k_1) {
            bList.moves[k_1] = bList2.moves[k_1];
        }

        // Флаг рокировок.false можно, true нельзя.
        WHITE_O_O = WHITE_O_O2;
        WHITE_O_O_O = WHITE_O_O_O2;
        BLACK_O_O = BLACK_O_O2;
        BLACK_O_O_O = BLACK_O_O_O2;

        // Предыдущий ход на этой старой  доске.
        previos_moves.xy_from = previos_moves2.xy_from;
        previos_moves.xy_to = previos_moves2.xy_to;
        previos_moves.figure = previos_moves2.figure;
    }

    // инициализирует пустой доской.
    void init() {

        WHITE_O_O = false;
        WHITE_O_O_O = false;

        BLACK_O_O = false;
        BLACK_O_O_O = false;

        for (__int8 i = 0; i < 8; ++i) {
            for (__int8 j = 0; j < 8; ++j) {
                board[i][j] = ChessPiece::EMPTY;
            }
        }

        // Предыдущий ход
        previos_moves.figure = ChessPiece::EMPTY;
        previos_moves.xy_from.x = None;
        previos_moves.xy_from.y = None;
        previos_moves.xy_to.x = None;
        previos_moves.xy_to.y = None;


        wList.n = 0;
        bList.n = 0;

    }

    void init2() {

        // Если мы анализируем ход в середине партии то информация о 
        // рокировке должна быть передана перед обдумыванием хода.

        // инициализируем списки фигур
        for (__int8 i = 0; i < 8; ++i) {
            for (__int8 j = 0; j < 8; ++j) {
                if ((board[i][j] == ChessPiece::W_KING)||(board[i][j] == ChessPiece::W_QUEEN)||
                    (board[i][j] == ChessPiece::W_ROOK) || (board[i][j] == ChessPiece::W_BISHOP) || 
                    (board[i][j] == ChessPiece::W_KNIGHT) || (board[i][j] == ChessPiece::W_PAWN)) {
                    wList.append(j, i);
                }
                if ((board[i][j] == ChessPiece::B_KING) || (board[i][j] == ChessPiece::B_QUEEN) ||
                    (board[i][j] == ChessPiece::B_ROOK) || (board[i][j] == ChessPiece::B_BISHOP) ||
                    (board[i][j] == ChessPiece::B_KNIGHT) || (board[i][j] == ChessPiece::B_PAWN)) {
                    bList.append(j, i);
                }
            }
        }

        //POOL_SIZEL = (wList.n > bList.n ? wList.n : bList.n);

        // При игре на пешечном окончании фигур мало и важно экономить память ЭВМ.
       // wList.realloc_MOVESL();
       // bList.realloc_MOVESL();

        
    }

    float rate(Color color);

    void print()
    {
       
#ifndef CODE_FOR_DLL

            // Два цвета для ячеек шахматной доски.
            __int8 color[2] = { 1, 46 };//46 - Бирюзовый

            set_color(0);

            if (0) {
                // Альтернативное изображение фигур UNICODE символами шахматных фигур.

                printf(" abcdefgh\n");
                // Печать шахматной доски.
                __int8 k = 0;
                for (__int8 i = 0; i < 8; ++i) {
                    printf("%d", 9 - (i + 1));
                    for (__int8 j = 0; j < 8; ++j) {

                        set_color(color[k]);
                        switch (board[i][j]) {
                        case ChessPiece::EMPTY: printf("."); break;
                        case ChessPiece::W_PAWN: printf("♙");  break;
                        case ChessPiece::W_KING: printf("♔");  break;
                        case ChessPiece::W_ROOK: printf("♖"); break;
                        case ChessPiece::W_BISHOP: printf("♗"); break;
                        case ChessPiece::W_QUEEN: printf("♕"); break;
                        case ChessPiece::W_KNIGHT: printf("♘"); break;
                        case ChessPiece::B_PAWN: printf("♟");  break;
                        case ChessPiece::B_KING: printf("♚");  break;
                        case ChessPiece::B_ROOK: printf("♜"); break;
                        case ChessPiece::B_BISHOP: printf("♝"); break;
                        case ChessPiece::B_QUEEN: printf("♛"); break;
                        case ChessPiece::B_KNIGHT: printf("♞"); break;
                        }
                        /*switch (board[i][j]) {
                        case ChessPiece::EMPTY: std::cout << L'.'; break;
                        case ChessPiece::W_PAWN: std::cout << L'♙';  break;
                        case ChessPiece::W_KING: std::cout << L'♔';  break;
                        case ChessPiece::W_ROOK: std::cout << L'♖'; break;
                        case ChessPiece::W_BISHOP: std::cout << L'♗'; break;
                        case ChessPiece::W_QUEEN: std::cout << L'♕'; break;
                        case ChessPiece::W_KNIGHT: std::cout << L'♘'; break;
                        case ChessPiece::B_PAWN: std::cout << L'♟';  break;
                        case ChessPiece::B_KING: std::cout << L'♚';  break;
                        case ChessPiece::B_ROOK: std::cout << L'♜'; break;
                        case ChessPiece::B_BISHOP: std::cout << L'♝'; break;
                        case ChessPiece::B_QUEEN: std::cout << L'♛'; break;
                        case ChessPiece::B_KNIGHT: std::cout << L'♞'; break;
                        }*/

                        k = 1 - k;
                    }
                    set_color(0);
                    printf("\n");

                    k = 1 - k;
                   
                }
                printf(" abcdefgh\n");

                set_color(0);
            }
            else {

                printf("   a    b    c    d    e    f    g    h ");
                printf("\t\t   h    g    f    e    d    c    b    a \n");

                // Печать шахматной доски.
                __int8 k = 0;
                for (__int8 i = 0; i < 8; ++i) {



                    for (int itop = 0; itop < 2; ++itop) {
                        set_color(0);
                        if (itop == 0) {
                            printf("%d", 9 - (i + 1));
                        }
                        else {
                            printf(" ");
                        }

                        for (__int8 j = 0; j < 8; ++j) {

                            set_color(color[k]);
                            // Покажем фигуры на доске.
                            if (0) {
                                /*switch (board[i][j]) {
                                case ChessPiece::EMPTY: std::cout << L'.'; break;
                                case ChessPiece::W_PAWN: std::cout << L'♙';  break;
                                case ChessPiece::W_KING: std::cout << L'♔';  break;
                                case ChessPiece::W_ROOK: std::cout << L'♖'; break;
                                case ChessPiece::W_BISHOP: std::cout << L'♗'; break;
                                case ChessPiece::W_QUEEN: std::cout << L'♕'; break;
                                case ChessPiece::W_KNIGHT: std::cout << L'♘'; break;
                                case ChessPiece::B_PAWN: std::cout << L'♟';  break;
                                case ChessPiece::B_KING: std::cout << L'♚';  break;
                                case ChessPiece::B_ROOK: std::cout << L'♜'; break;
                                case ChessPiece::B_BISHOP: std::cout << L'♝'; break;
                                case ChessPiece::B_QUEEN: std::cout << L'♛'; break;
                                case ChessPiece::B_KNIGHT: std::cout << L'♞'; break;
                                }*/
                                /*switch (board[i][j]) {
                                case ChessPiece::EMPTY: printf("."); break;
                                case ChessPiece::W_PAWN: printf("♙");  break;
                                case ChessPiece::W_KING: printf("♔");  break;
                                case ChessPiece::W_ROOK: printf("♖"); break;
                                case ChessPiece::W_BISHOP: printf("♗"); break;
                                case ChessPiece::W_QUEEN: printf("♕"); break;
                                case ChessPiece::W_KNIGHT: printf("♘"); break;
                                case ChessPiece::B_PAWN: printf("♟");  break;
                                case ChessPiece::B_KING: printf("♚");  break;
                                case ChessPiece::B_ROOK: printf("♜"); break;
                                case ChessPiece::B_BISHOP: printf("♝"); break;
                                case ChessPiece::B_QUEEN: printf("♛"); break;
                                case ChessPiece::B_KNIGHT: printf("♞"); break;
                                }
                                std::cout << printf(" ");
                               */
                            }
                            else if (0) {
                                switch (board[i][j]) {
                                case ChessPiece::EMPTY:  printf(". "); break;
                                case ChessPiece::W_PAWN:  printf("wp");  break;
                                case ChessPiece::W_KING:   printf("wk");  break;
                                case ChessPiece::W_ROOK:  printf("wr"); break;
                                case ChessPiece::W_BISHOP:   printf("wb"); break;
                                case ChessPiece::W_QUEEN:   printf("wf"); break;
                                case ChessPiece::W_KNIGHT:   printf("wg"); break;
                                case ChessPiece::B_PAWN:   printf("bp");  break;
                                case ChessPiece::B_KING:   printf("bk");  break;
                                case ChessPiece::B_ROOK:   printf("br"); break;
                                case ChessPiece::B_BISHOP:   printf("bb"); break;
                                case ChessPiece::B_QUEEN:   printf("bf"); break;
                                case ChessPiece::B_KNIGHT:   printf("bg"); break;
                                }

                            }
                            else {


                                if (itop == 1) {
                                    switch (board[i][j]) {
                                    case ChessPiece::EMPTY:  printf("     "); break;
                                    case ChessPiece::W_PAWN:  printf(" (_) ");  break;
                                    case ChessPiece::W_KING:   printf(" (_) ");  break;
                                    case ChessPiece::W_ROOK:  printf(" (_) "); break;
                                    case ChessPiece::W_BISHOP:   printf(" (_) "); break;
                                    case ChessPiece::W_QUEEN:   printf(" (_) "); break;
                                    case ChessPiece::W_KNIGHT:   printf(" (_) "); break;
                                    case ChessPiece::B_PAWN:   printf("\033[30m (_) \033[0m");  break;
                                    case ChessPiece::B_KING:   printf("\033[30m (_) \033[0m");  break;
                                    case ChessPiece::B_ROOK:   printf("\033[30m (_) \033[0m"); break;
                                    case ChessPiece::B_BISHOP:   printf("\033[30m (_) \033[0m"); break;
                                    case ChessPiece::B_QUEEN:   printf("\033[30m (_) \033[0m"); break;
                                    case ChessPiece::B_KNIGHT:   printf("\033[30m (_) \033[0m"); break;
                                    }

                                }
                                else {
                                    switch (board[i][j]) {
                                    case ChessPiece::EMPTY:  printf("     "); break;
                                    case ChessPiece::W_PAWN:  printf(" _o_ ");  break;
                                    case ChessPiece::W_KING:   printf(" \\+/ ");  break;
                                    case ChessPiece::W_ROOK:  printf(" [\"] "); break;
                                    case ChessPiece::W_BISHOP:   printf(" (\\) "); break;
                                    case ChessPiece::W_QUEEN:   printf(" \\^/ "); break;
                                    case ChessPiece::W_KNIGHT:   printf(" {o\\ "); break;
                                    case ChessPiece::B_PAWN:   printf("\033[30m _o_ \033[0m");  break;
                                    case ChessPiece::B_KING:   printf("\033[30m \\+/ \033[0m");  break;
                                    case ChessPiece::B_ROOK:   printf("\033[30m [\"] \033[0m"); break;
                                    case ChessPiece::B_BISHOP:   printf("\033[30m (\\) \033[0m"); break;
                                    case ChessPiece::B_QUEEN:   printf("\033[30m \\^/ \033[0m"); break;
                                    case ChessPiece::B_KNIGHT:   printf("\033[30m {o\\ \033[0m"); break;
                                    }
                                }

                            }



                            //std::cout  << L' ';
                            k = 1 - k;
                        }

                        set_color(0);
                        if (itop == 0) {
                            printf("%d", 9 - (i + 1));
                        }
                        else {
                            printf(" ");
                        }
                        //printf("\n\0");


                   
                        set_color(0);
                        if (itop == 0) {
                            printf("\t\t%d", (i + 1));
                        }
                        else {
                            printf("\t\t ");
                        }

                        for (__int8 j = 0; j < 8; ++j) {

                            set_color(color[k]);
                            // Покажем фигуры на доске.
                            if (0) {
                                /*switch (board[i][j]) {
                                case ChessPiece::EMPTY: std::cout << L'.'; break;
                                case ChessPiece::W_PAWN: std::cout << L'♙';  break;
                                case ChessPiece::W_KING: std::cout << L'♔';  break;
                                case ChessPiece::W_ROOK: std::cout << L'♖'; break;
                                case ChessPiece::W_BISHOP: std::cout << L'♗'; break;
                                case ChessPiece::W_QUEEN: std::cout << L'♕'; break;
                                case ChessPiece::W_KNIGHT: std::cout << L'♘'; break;
                                case ChessPiece::B_PAWN: std::cout << L'♟';  break;
                                case ChessPiece::B_KING: std::cout << L'♚';  break;
                                case ChessPiece::B_ROOK: std::cout << L'♜'; break;
                                case ChessPiece::B_BISHOP: std::cout << L'♝'; break;
                                case ChessPiece::B_QUEEN: std::cout << L'♛'; break;
                                case ChessPiece::B_KNIGHT: std::cout << L'♞'; break;
                                }*/
                                /*switch (board[i][j]) {
                                case ChessPiece::EMPTY: printf("."); break;
                                case ChessPiece::W_PAWN: printf("♙");  break;
                                case ChessPiece::W_KING: printf("♔");  break;
                                case ChessPiece::W_ROOK: printf("♖"); break;
                                case ChessPiece::W_BISHOP: printf("♗"); break;
                                case ChessPiece::W_QUEEN: printf("♕"); break;
                                case ChessPiece::W_KNIGHT: printf("♘"); break;
                                case ChessPiece::B_PAWN: printf("♟");  break;
                                case ChessPiece::B_KING: printf("♚");  break;
                                case ChessPiece::B_ROOK: printf("♜"); break;
                                case ChessPiece::B_BISHOP: printf("♝"); break;
                                case ChessPiece::B_QUEEN: printf("♛"); break;
                                case ChessPiece::B_KNIGHT: printf("♞"); break;
                                }
                                std::cout << printf(" ");
                               */
                            }
                            else if (0) {
                                switch (board[7-i][7-j]) {
                                case ChessPiece::EMPTY:  printf(". "); break;
                                case ChessPiece::W_PAWN:  printf("wp");  break;
                                case ChessPiece::W_KING:   printf("wk");  break;
                                case ChessPiece::W_ROOK:  printf("wr"); break;
                                case ChessPiece::W_BISHOP:   printf("wb"); break;
                                case ChessPiece::W_QUEEN:   printf("wf"); break;
                                case ChessPiece::W_KNIGHT:   printf("wg"); break;
                                case ChessPiece::B_PAWN:   printf("bp");  break;
                                case ChessPiece::B_KING:   printf("bk");  break;
                                case ChessPiece::B_ROOK:   printf("br"); break;
                                case ChessPiece::B_BISHOP:   printf("bb"); break;
                                case ChessPiece::B_QUEEN:   printf("bf"); break;
                                case ChessPiece::B_KNIGHT:   printf("bg"); break;
                                }

                            }
                            else {


                                if (itop == 1) {
                                    switch (board[7-i][7-j]) {
                                    case ChessPiece::EMPTY:  printf("     "); break;
                                    case ChessPiece::W_PAWN:  printf(" (_) ");  break;
                                    case ChessPiece::W_KING:   printf(" (_) ");  break;
                                    case ChessPiece::W_ROOK:  printf(" (_) "); break;
                                    case ChessPiece::W_BISHOP:   printf(" (_) "); break;
                                    case ChessPiece::W_QUEEN:   printf(" (_) "); break;
                                    case ChessPiece::W_KNIGHT:   printf(" (_) "); break;
                                    case ChessPiece::B_PAWN:   printf("\033[30m (_) \033[0m");  break;
                                    case ChessPiece::B_KING:   printf("\033[30m (_) \033[0m");  break;
                                    case ChessPiece::B_ROOK:   printf("\033[30m (_) \033[0m"); break;
                                    case ChessPiece::B_BISHOP:   printf("\033[30m (_) \033[0m"); break;
                                    case ChessPiece::B_QUEEN:   printf("\033[30m (_) \033[0m"); break;
                                    case ChessPiece::B_KNIGHT:   printf("\033[30m (_) \033[0m"); break;
                                    }

                                }
                                else {
                                    switch (board[7-i][7-j]) {
                                    case ChessPiece::EMPTY:  printf("     "); break;
                                    case ChessPiece::W_PAWN:  printf(" _o_ ");  break;
                                    case ChessPiece::W_KING:   printf(" \\+/ ");  break;
                                    case ChessPiece::W_ROOK:  printf(" [\"] "); break;
                                    case ChessPiece::W_BISHOP:   printf(" (\\) "); break;
                                    case ChessPiece::W_QUEEN:   printf(" \\^/ "); break;
                                    case ChessPiece::W_KNIGHT:   printf(" {o\\ "); break;
                                    case ChessPiece::B_PAWN:   printf("\033[30m _o_ \033[0m");  break;
                                    case ChessPiece::B_KING:   printf("\033[30m \\+/ \033[0m");  break;
                                    case ChessPiece::B_ROOK:   printf("\033[30m [\"] \033[0m"); break;
                                    case ChessPiece::B_BISHOP:   printf("\033[30m (\\) \033[0m"); break;
                                    case ChessPiece::B_QUEEN:   printf("\033[30m \\^/ \033[0m"); break;
                                    case ChessPiece::B_KNIGHT:   printf("\033[30m {o\\ \033[0m"); break;
                                    }
                                }

                            }



                            //std::cout  << L' ';
                            k = 1 - k;
                        }

                        set_color(0);
                        if (itop == 0) {
                            printf("%d",  (i + 1));
                        }
                        else {
                            printf(" ");
                        }
                        printf("\n\0");


                    }
                    k = 1 - k;
                    //set_color(0);
                    //std::cout << std::endl;

                }
                set_color(0);
                printf("   a    b    c    d    e    f    g    h ");
                printf("\t\t   h    g    f    e    d    c    b    a \n");
            }
#endif
        
        
    }

    Color get_color(ChessPiece board_loc[8][8], __int8 x, __int8 y) {
        if (board_loc[y][x] == ChessPiece::EMPTY) {
            return Color::EMPTY;
        }
        else if ((board_loc[y][x] == ChessPiece::W_PAWN) || (board_loc[y][x] == ChessPiece::W_KING) ||
            (board_loc[y][x] == ChessPiece::W_ROOK) || (board_loc[y][x] == ChessPiece::W_BISHOP) ||
            (board_loc[y][x] == ChessPiece::W_QUEEN)|| (board_loc[y][x] == ChessPiece::W_KNIGHT))
        {
            return Color::WHITE;
        }
        else return Color::BLACK;
    }

    MOVES get_moves(__int8 x, __int8 y);
    MOVES get_moves_white(__int8 x, __int8 y);
    MOVES get_moves_black(__int8 x, __int8 y);

    void move(MOVES_NODE xy_from, MOVES_NODE xy_to)
    {

        if ((xy_to.x >= 0) && (xy_to.x <= 7) && (xy_to.y >= 0) && (xy_to.y <= 7)) {
            if ((xy_from.x >= 0) && (xy_from.x <= 7) && (xy_from.y >= 0) && (xy_from.y <= 7)) {

                // По правилам игры мы не можем сами встать под шах при рокировке.
               
               


                // Проверка на рокировку.
                // Поля где происходит рокировка не под атакой чёрных. Это важное правило. 08.12.2022
                // Король не должен быть под шахом.
                // В случае длинной рокировки Ладья может перепрыгнуть через битое поле b1.
                if ((board[xy_from.y][xy_from.x] == ChessPiece::W_KING) && (xy_from.x == 4) && (xy_to.x == 6) && (!WHITE_O_O) /* && (!WHITE_O_O_O)*/ &&
                    (xy_from.y == 7) && (board[7][5] == ChessPiece::EMPTY) && (board[7][6] == ChessPiece::EMPTY)&&
                    (board[7][7] == ChessPiece::W_ROOK)) {

                    // Рокировка в короткую сторону.
                    bool attack_BLACK[8][8];
                    for (__int8 j = 0; j <= 7; ++j) {
                        for (__int8 i = 0; i <= 7; ++i) {
                            attack_BLACK[j][i] = false; // Инициализация все поля свободны.
                        }
                    }

                    // Поля которыми чёрные фигуры атакуют белых фигур
                    generate_attack_BLACK(attack_BLACK, board, wList, bList, previos_moves,
                        WHITE_O_O, WHITE_O_O_O,
                        BLACK_O_O, BLACK_O_O_O);


                    if ((!attack_BLACK[7][5]) && (!attack_BLACK[7][6]) && (!attack_BLACK[xy_from.y][xy_from.x])) {

                        // ход Короля на две клетки вправо
                         // from
                        wList.delete_move(xy_from.x, xy_from.y);

                        /*for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == xy_from.x) && (wList.moves[i].y == xy_from.y)) {
                                wList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }
                        for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == xy_from.x) && (bList.moves[i].y == xy_from.y)) {
                                bList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }*/

                        wList.append(6, 7);
                        board[7][6] = board[7][4];
                        board[7][4] = ChessPiece::EMPTY;

                        // Завершающий ход ладьи так как рокировка это двойной ход.
                         // from
                        wList.delete_move(7, 7);

                        /*for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == 7) && (wList.moves[i].y == 7)) {
                                wList.delete_move(7, 7);
                                break;
                            }
                        }
                        for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == 7) && (bList.moves[i].y == 7)) {
                                bList.delete_move(7, 7);
                                break;
                            }
                        }*/

                        wList.append(5, 7);
                        board[7][5] = board[7][7];
                        board[7][7] = ChessPiece::EMPTY;

                        WHITE_O_O = true;
                        WHITE_O_O_O = true;

                    }
                }
                else if ((board[xy_from.y][xy_from.x] == ChessPiece::W_KING) && (xy_from.x == 4) && (xy_to.x == 2) /* && (!WHITE_O_O)*/ && (!WHITE_O_O_O) &&
                    (xy_from.y == 7) && (board[7][1] == ChessPiece::EMPTY) && (board[7][2] == ChessPiece::EMPTY)
                    && (board[7][3] == ChessPiece::EMPTY) &&
                    (board[7][0] == ChessPiece::W_ROOK) ) 
                {

                    bool attack_BLACK[8][8];
                    for (__int8 j = 0; j <= 7; ++j) {
                        for (__int8 i = 0; i <= 7; ++i) {
                            attack_BLACK[j][i] = false; // Инициализация все поля свободны.
                        }
                    }

                    // Поля которыми чёрные фигуры атакуют белых фигур
                    generate_attack_BLACK(attack_BLACK, board, wList, bList, previos_moves,
                        WHITE_O_O, WHITE_O_O_O,
                        BLACK_O_O, BLACK_O_O_O);

                    if (/* && (!attack_BLACK[7][1]) &&*/ (!attack_BLACK[7][2]) && (!attack_BLACK[7][3]) && (!attack_BLACK[xy_from.y][xy_from.x])) {


                        // Рокировка в длинную сторону.

                        // ход Короля на две клетки влево
                         // from
                        wList.delete_move(xy_from.x, xy_from.y);

                        /*for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == xy_from.x) && (wList.moves[i].y == xy_from.y)) {
                                wList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }
                        for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == xy_from.x) && (bList.moves[i].y == xy_from.y)) {
                                bList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }*/

                        wList.append(2, 7);
                        board[7][2] = board[7][4];
                        board[7][4] = ChessPiece::EMPTY;

                        // Завершающий ход ладьи так как рокировка это двойной ход.
                         // from
                        wList.delete_move(0, 7);

                        /*for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == 0) && (wList.moves[i].y == 7)) {
                                wList.delete_move(0, 7);
                                break;
                            }
                        }
                        for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == 0) && (bList.moves[i].y == 7)) {
                                bList.delete_move(0, 7);
                                break;
                            }
                        }*/

                        wList.append(3, 7);
                        board[7][3] = board[7][0];
                        board[7][0] = ChessPiece::EMPTY;

                        WHITE_O_O = true;
                        WHITE_O_O_O = true;
                    }
                }
                else  if ((board[xy_from.y][xy_from.x] == ChessPiece::B_KING) && (xy_from.x == 4) && (xy_to.x == 6) && (!BLACK_O_O) /* && (!BLACK_O_O_O)*/ &&
                    (xy_from.y == 0) && (board[0][5] == ChessPiece::EMPTY) && (board[0][6] == ChessPiece::EMPTY) &&
                    (board[0][7] == ChessPiece::B_ROOK) ) {

                    // Рокировка чёрных в короткую сторону.


                    bool attack_WHITE[8][8];
                    for (__int8 j = 0; j <= 7; ++j) {
                        for (__int8 i = 0; i <= 7; ++i) {
                            attack_WHITE[j][i] = false; // Инициализация все поля свободны.
                        }
                    }

                    // Поля которыми белые фигуры атакуют черных фигур
                    generate_attack_WHITE(attack_WHITE, board, wList, bList, previos_moves,
                        WHITE_O_O, WHITE_O_O_O,
                        BLACK_O_O, BLACK_O_O_O);

                    if ((!attack_WHITE[0][5]) && (!attack_WHITE[0][6]) && (!attack_WHITE[xy_from.y][xy_from.x])) {

                        // ход Короля на две клетки вправо
                         // from
                        bList.delete_move(xy_from.x, xy_from.y);

                        /*for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == xy_from.x) && (wList.moves[i].y == xy_from.y)) {
                                wList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }
                        for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == xy_from.x) && (bList.moves[i].y == xy_from.y)) {
                                bList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }*/

                        bList.append(6, 0);
                        board[0][6] = board[0][4];
                        board[0][4] = ChessPiece::EMPTY;

                        // Завершающий ход ладьи так как рокировка это двойной ход.
                         // from
                        bList.delete_move(7, 0);

                        /*for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == 7) && (wList.moves[i].y == 0)) {
                                wList.delete_move(7, 0);
                                break;
                            }
                        }
                        for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == 7) && (bList.moves[i].y == 0)) {
                                bList.delete_move(7, 7);
                                break;
                            }
                        }*/

                        bList.append(5, 0);
                        board[0][5] = board[0][7];
                        board[0][7] = ChessPiece::EMPTY;

                        BLACK_O_O = true;
                        BLACK_O_O_O = true;

                    }
                }
                else if ((board[xy_from.y][xy_from.x] == ChessPiece::B_KING) && (xy_from.x == 4) && (xy_to.x == 2) && /*(!BLACK_O_O) &&*/ (!BLACK_O_O_O) &&
                    (xy_from.y == 0) && (board[0][1] == ChessPiece::EMPTY) && (board[0][2] == ChessPiece::EMPTY)
                    && (board[0][3] == ChessPiece::EMPTY) &&
                    (board[0][0] == ChessPiece::B_ROOK)) {

                    // Рокировка чёрных в длинную сторону.

                    bool attack_WHITE[8][8];
                    for (__int8 j = 0; j <= 7; ++j) {
                        for (__int8 i = 0; i <= 7; ++i) {
                            attack_WHITE[j][i] = false; // Инициализация все поля свободны.
                        }
                    }

                    // Поля которыми белые фигуры атакуют черных фигур
                    generate_attack_WHITE(attack_WHITE, board, wList, bList, previos_moves,
                        WHITE_O_O, WHITE_O_O_O,
                        BLACK_O_O, BLACK_O_O_O);

                    if (/* && (!attack_WHITE[0][1]) &&*/ (!attack_WHITE[0][2]) && (!attack_WHITE[0][3]) && (!attack_WHITE[xy_from.y][xy_from.x])) {

                        // ход Короля на две клетки влево
                         // from
                        bList.delete_move(xy_from.x, xy_from.y);

                        /*for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == xy_from.x) && (wList.moves[i].y == xy_from.y)) {
                                wList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }
                        for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == xy_from.x) && (bList.moves[i].y == xy_from.y)) {
                                bList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }*/

                        bList.append(2, 0);
                        board[0][2] = board[0][4];
                        board[0][4] = ChessPiece::EMPTY;

                        // Завершающий ход ладьи так как рокировка это двойной ход.
                         // from
                        bList.delete_move(0, 0);

                        /*for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == 0) && (wList.moves[i].y == 0)) {
                                wList.delete_move(0, 0);
                                break;
                            }
                        }
                        for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == 0) && (bList.moves[i].y == 0)) {
                                bList.delete_move(0, 0);
                                break;
                            }
                        }*/

                        bList.append(3, 0);
                        board[0][3] = board[0][0];
                        board[0][0] = ChessPiece::EMPTY;

                        BLACK_O_O = true;
                        BLACK_O_O_O = true;

                    }
                }
                else
                {

                    // Обычный ход не рокировка.
                    if ((xy_from.x == 7) && (xy_from.y == 7) && (!WHITE_O_O) && (board[7][7] == ChessPiece::W_ROOK) 
                        && (xy_to.x != xy_from.x) && (xy_to.y != xy_from.y))
                    {
                        WHITE_O_O = true; // Правая белая ладья походила, рокировку в правую сторону делать нельзя.
                    }
                    if ((xy_from.x == 0) && (xy_from.y == 7) && (!WHITE_O_O_O) && (board[7][0] == ChessPiece::W_ROOK)
                        && (xy_to.x != xy_from.x) && (xy_to.y != xy_from.y))
                    {
                        WHITE_O_O_O = true; // Левая белая ладья походила, рокировку в левую сторону делать нельзя.
                    }

                    if ((xy_from.x == 7) && (xy_from.y == 0) && (!BLACK_O_O_O) && (board[0][7] == ChessPiece::B_ROOK)
                        && (xy_to.x != xy_from.x) && (xy_to.y != xy_from.y))
                    {
                        BLACK_O_O = true; // Правая чёрная ладья походила, рокировку в правую сторону делать нельзя.
                    }
                    if ((xy_from.x == 0) && (xy_from.y == 0) && (!BLACK_O_O_O) && (board[0][0] == ChessPiece::B_ROOK)
                        && (xy_to.x != xy_from.x) && (xy_to.y != xy_from.y))
                    {
                        BLACK_O_O_O = true; // Левая чёрная ладья походила, рокировку в левую сторону делать нельзя.
                    }

                    if ((board[xy_from.y][xy_from.x] == ChessPiece::B_KING) && (xy_to.x != xy_from.x) && (xy_to.y != xy_from.y)) {
                        // Король уже ходил, рокировку делать нельзя.
                        BLACK_O_O = true;
                        BLACK_O_O_O = true;
                    }

                    if ((board[xy_from.y][xy_from.x] == ChessPiece::W_KING) && (xy_to.x != xy_from.x) && (xy_to.y != xy_from.y)) {
                        // Король уже ходил, рокировку делать нельзя.
                        WHITE_O_O = true;
                        WHITE_O_O_O = true;
                    }   
                      


                    if (get_color1(board, xy_from.x, xy_from.y) == Color::WHITE) {
                        // from
                        wList.delete_move(xy_from.x, xy_from.y);
                        bList.delete_move(xy_to.x, xy_to.y);

                        /*for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == xy_from.x) && (wList.moves[i].y == xy_from.y)) {
                                wList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }

                        for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == xy_to.x) && (bList.moves[i].y == xy_to.y)) {
                                bList.delete_move(xy_to.x, xy_to.y);
                                break;
                            }
                        }*/

                        wList.append(xy_to.x, xy_to.y);
                    }
                    if (get_color1(board, xy_from.x, xy_from.y) == Color::BLACK) {

                        bList.delete_move(xy_from.x, xy_from.y);
                        wList.delete_move(xy_to.x, xy_to.y);

                        /*for (__int8 i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == xy_from.x) && (bList.moves[i].y == xy_from.y)) {
                                bList.delete_move(xy_from.x, xy_from.y);
                                break;
                            }
                        }

                        // Удаление фигуры из списка фигур на доске
                        for (__int8 i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == xy_to.x) && (wList.moves[i].y == xy_to.y)) {
                                wList.delete_move(xy_to.x, xy_to.y);
                                break;
                            }
                        }*/

                        bList.append(xy_to.x, xy_to.y);
                    }

                    board[xy_to.y][xy_to.x] = board[xy_from.y][xy_from.x];
                    board[xy_from.y][xy_from.x] = ChessPiece::EMPTY;

                    if ((board[xy_to.y][xy_to.x] == ChessPiece::W_PAWN) && (xy_from.x != xy_to.x) &&
                        (this->previos_moves.figure == ChessPiece::B_PAWN) &&
                        (this->previos_moves.xy_from.y == 1) && (this->previos_moves.xy_to.y == 3)&&
                        (xy_from.y == 3) && (xy_to.y == 2) && (xy_to.x == this->previos_moves.xy_to.x) &&
                        (this->previos_moves.xy_from.x == this->previos_moves.xy_to.x)) {
                        // Взятие чёрной пешки на проходе.

                        board[xy_from.y][xy_to.x] = ChessPiece::EMPTY;

                        // Удаление чёрной фигуры из списка фигур на доске
                        bList.delete_move(xy_to.x, xy_from.y);

                        /*for (int i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == xy_to.x) && (wList.moves[i].y == xy_from.y)) {
                                wList.delete_move(xy_to.x, xy_from.y);
                                break;
                            }
                        }

                        for (int i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == xy_to.x) && (bList.moves[i].y == xy_from.y)) {
                                bList.delete_move(xy_to.x, xy_from.y);
                                break;
                            }
                        }*/
                    }

                    if ((board[xy_to.y][xy_to.x] == ChessPiece::B_PAWN) && (xy_from.x != xy_to.x) &&
                        (this->previos_moves.figure == ChessPiece::W_PAWN) &&
                        (this->previos_moves.xy_from.y == 6) && (this->previos_moves.xy_to.y == 4)&&
                        (xy_from.y==4)&&(xy_to.y==5)&&(xy_to.x== this->previos_moves.xy_to.x)&&
                        (this->previos_moves.xy_from.x== this->previos_moves.xy_to.x)) 
                    {
                        // Взятие белой пешки на проходе.

                        board[xy_from.y][xy_to.x] = ChessPiece::EMPTY;

                        // Удаление фигуры из списка белых фигур на доске
                        wList.delete_move(xy_to.x, xy_from.y);

                        /*for (int i = 0; i < wList.n; ++i) {
                            if ((wList.moves[i].x == xy_to.x) && (wList.moves[i].y == xy_from.y)) {
                                wList.delete_move(xy_to.x, xy_from.y);
                                break;
                            }
                        }
                        for (int i = 0; i < bList.n; ++i) {
                            if ((bList.moves[i].x == xy_to.x) && (bList.moves[i].y == xy_from.y)) {
                                bList.delete_move(xy_to.x, xy_from.y);
                                break;
                            }
                        }*/
                    }

                    // Превращаем проходную пешку в ферзи в случае если она дошла до конца доски

                    // Особый случай - чёрная пешка прошла в ферзи.
                    if ((board[xy_to.y][xy_to.x] == ChessPiece::B_PAWN) && (xy_to.y == 7)) {
                        board[xy_to.y][xy_to.x] = ChessPiece::B_QUEEN;
                    }

                    // Особый случай - белая пешка прошла в ферзи.
                    if ((board[xy_to.y][xy_to.x] == ChessPiece::W_PAWN) && (xy_to.y == 0)) {
                        board[xy_to.y][xy_to.x] = ChessPiece::W_QUEEN;
                    }
                }
                }
            else {
                //printf("move unknown xy_from %d\n", debug);
                //system("pause");
            }
            
        }
        else {
            //printf("xy_from x=%d y=%d\n", xy_from.x, xy_from.y);
            //printf("move unknown xy_to  %d\n", debug);
            //system("pause");
        }
    }

    
};

Board::Board()
{
    init();
}

// Пустая клетка
class Empty  {

public:
    Color color;
    Empty();
    Empty(Color icol) {
        color = Color::EMPTY;
    }
    ChessPiece get() {
        return (ChessPiece::EMPTY);
    }


};

Empty::Empty() : color(Color::EMPTY)
{

}

void append_chess_move(Color color, ChessPiece new_board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, MOVES& mgl, MOVES_NODE xy_to, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O);

// Eval отвечает также за мобильность фигуры, материальная цена атаки.
float bishopEvalWhite[8][8] = {
    -2.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -2.0f ,
        -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.5f, 0.0f, -1.0f,
        -1.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, -1.0f,
        -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,
        -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
        -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, -1.0f,
        -2.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -2.0f
};

float bishopEvalBlack[8][8] = {

    -2.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -2.0f,
    -1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, -1.0f,
    -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
     -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f,
     -1.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, -1.0f,
     -1.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.5f, 0.0f, -1.0f,
      -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
      -2.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -2.0f         
};

// Слон
class Bishop {
public:
    Color color;
    float VALUE = 30; // стоимость слона. 33.3

    Bishop();

    Bishop(Color icol) {
        color = icol;
    }

    float get_value() {
        return VALUE;
    }

    ChessPiece get() {
        return (color == Color::WHITE ? ChessPiece::W_BISHOP : ChessPiece::B_BISHOP);
    }

    MOVES get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O);

    float rate(ChessPiece board[8][8], __int8 x, __int8 y) {

#ifdef MOBILITY_USER_SETTING
        return VALUE;
#else
        return VALUE + (color == Color::WHITE ? bishopEvalWhite[y][x] : bishopEvalBlack[y][x]);;
#endif
    }

};

Bishop::Bishop() : color(Color::EMPTY)
{

}



// Перечень возможных ходов слона.
// 18.08.2022 Можно делать только те ходы слоном чтобы король не был под шахом. Реализовано 18,08,2022
MOVES Bishop::get_moves(ChessPiece board[8][8], 
    MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves,
    __int8 x, __int8 y, bool bterminal,
    bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O ) {



    MOVES m;// (14);
    m.n = 0;

    

    // Диагональ 45 градусов..
    for (__int8 j_step = -1; j_step <= 1; j_step += 2) {
        __int8 i_1 = x + j_step;
        __int8 j_1 = y + j_step;
        while ((i_1 >= 0) && (i_1 <= 7) && (j_1 >= 0) && (j_1 <= 7)) {
            Color current_clr = get_color1(board, i_1, j_1);
            if (current_clr == color)
                break;
            if (bterminal) {
                m.append(i_1, j_1);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_1;
                xy_to.y = j_1;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

               

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];


                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }
            if (current_clr != Color::EMPTY)
                break;
            i_1 += j_step; // переходим к следующему ходу.
            j_1 += j_step;
        }
    }

    __int8 j_step = 1;

    __int8 i_1 = x + j_step;
    __int8 j_1 = y - j_step;
    while ((i_1 >= 0) && (i_1 <= 7) && (j_1 >= 0) && (j_1 <= 7)) {
        Color current_clr = get_color1(board, i_1, j_1);
        if (current_clr == color)
            break;
        if (bterminal) {
            m.append(i_1, j_1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = i_1;
            xy_to.y = j_1;

            Board new_board;

            
            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);
           

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }
        if (current_clr != Color::EMPTY)
            break;
        i_1 += j_step; // переходим к следующему ходу.
        j_1 -= j_step;
    }

    i_1 = x - j_step;
    j_1 = y + j_step;
    while ((i_1 >= 0) && (i_1 <= 7) && (j_1 >= 0) && (j_1 <= 7)) {
        Color current_clr = get_color1(board, i_1, j_1);
        if (current_clr == color)
            break;
        if (bterminal) {
            m.append(i_1, j_1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = i_1;
            xy_to.y = j_1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }
        if (current_clr != Color::EMPTY)
            break;
        i_1 -= j_step; // переходим к следующему ходу.
        j_1 += j_step;
    }

    return m;
}

// Eval отвечает также за мобильность фигуры, материальная цена атаки.
float rookEvalWhite[8][8] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f ,
        0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f,
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
        -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
        0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f
};

float rookEvalBlack[8][8] = {

      0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
     -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
     -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
     -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
     -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
     -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.5f,
     0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f,
     0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f        
};

// Ладья
class Rook {
public:
    Color color;
    float VALUE = 50; // стоимость ладьи. 56.3

    Rook();

    Rook(Color icol) {
        color = icol;
    }

    float get_value() {
        return VALUE;
    }

    ChessPiece get() {
        return (color == Color::WHITE ? ChessPiece::W_ROOK : ChessPiece::B_ROOK);
    }

    MOVES get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O);

    float rate(ChessPiece board[8][8], __int8 x, __int8 y) {
#ifdef MOBILITY_USER_SETTING
        return VALUE;
#else
        return VALUE + (color == Color::WHITE ? rookEvalWhite[y][x] : rookEvalBlack[y][x]);
#endif
    }
};

Rook::Rook() : color(Color::EMPTY)
{

}



// Перечень возможных ходов ладьи.
// 18.08.2022 Можно делать только те ходы ладьёй чтобы король не был под шахом. Реализовано 18,08,2022
MOVES Rook::get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O) {

    MOVES m;// (14);
    m.n = 0;
    
    // горизонталь.
    for (__int8 j_step = -1; j_step <= 1; j_step += 2) 
    {
        __int8 i_step = x + j_step;
        while ((i_step >= 0) && (i_step <= 7)) {
            Color current_clr = get_color1(board, i_step, y);
            if (current_clr == color)
                break;
            if (bterminal) {
                m.append(i_step, y);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = y;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }
            if (current_clr != Color::EMPTY)
                break;
            i_step += j_step; // переходим к следующему ходу.
        }
    }

    // вертикаль.
    for (__int8 j_step = -1; j_step <= 1; j_step += 2) {
        __int8 i_step = y + j_step;
        while ((i_step >= 0) && (i_step <= 7)) {
            Color current_clr = get_color1(board, x, i_step);
            if (current_clr == color)
                break;
            if (bterminal) {
                m.append(x, i_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = x;
                xy_to.y = i_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }

            if (current_clr != Color::EMPTY)
                break;
            i_step += j_step; // переходим к следующему ходу.
        }
    }
    return m;
}


// Eval отвечает также за мобильность фигуры, материальная цена атаки.
float evalQueen[8][8] = {
       -2.0f, -1.0f, -1.0f, -0.5f, -0.5f, -1.0f, -1.0f, -2.0f,
        -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, -1.0f,
        -0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, -0.5f,
        0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, -0.5f,
        -1.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f, -1.0f,
        -1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        -2.0f, -1.0f, -1.0f, -0.5f, -0.5f, -1.0f, -1.0f, -2.0f
};

// Ферзь
class Queen {
public:
    Color color;
    float VALUE = 90.0f; // стоимость ферзя. 95

    Queen();

    Queen(Color icol) {
        color = icol;
    }

    float get_value() {
        return VALUE;
    }

    ChessPiece get() {
        return (color == Color::WHITE ? ChessPiece::W_QUEEN : ChessPiece::B_QUEEN);
    }

    MOVES get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O);

    float rate(ChessPiece board[8][8], __int8 x, __int8 y) {
#ifdef MOBILITY_USER_SETTING
        return VALUE;
#else
        return VALUE + evalQueen[y][x];
#endif
    }
};

Queen::Queen() : color(Color::EMPTY)
{

}

// 18.08.2022 Можно делать только те ходы ферзём чтобы король не был под шахом. Реализовано 18,08,2022
MOVES  Queen::get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O)
{

    MOVES m;
    m.n = 0;
    
    // От Ладьи.

    // горизонталь.
    for (__int8 j_step = -1; j_step <= 1; j_step += 2) {
        __int8 i_step = x + j_step;
        while ((i_step >= 0) && (i_step <= 7)) {
            Color current_clr = get_color1(board, i_step, y);
            if (current_clr == color)
                break;
            if (bterminal) {
                m.append(i_step, y);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = y;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }
            if (current_clr != Color::EMPTY)
                break;
            i_step += j_step; // переходим к следующему ходу.
        }
    }

    // вертикаль.
    for (__int8 j_step = -1; j_step <= 1; j_step += 2) {
        __int8 i_step = y + j_step;
        while ((i_step >= 0) && (i_step <= 7)) {
            Color current_clr = get_color1(board, x, i_step);
            if (current_clr == color)
                break;
            if (bterminal) {
                m.append(x, i_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = x;
                xy_to.y = i_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }
            if (current_clr != Color::EMPTY)
                break;
            i_step += j_step; // переходим к следующему ходу.
        }
    }

    // От слона

    // Диагональ 45 градусов..
    for (__int8 j_step = -1; j_step <= 1; j_step += 2) {
        __int8 i_1 = x + j_step;
        __int8 j_1 = y + j_step;
        while ((i_1 >= 0) && (i_1 <= 7) && (j_1 >= 0) && (j_1 <= 7)) {
            Color current_clr = get_color1(board, i_1, j_1);
            if (current_clr == color)
                break;
            if (bterminal) {
                m.append(i_1, j_1);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_1;
                xy_to.y = j_1;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }
            if (current_clr != Color::EMPTY)
                break;
            i_1 += j_step; // переходим к следующему ходу.
            j_1 += j_step;
        }
    }

    __int8 j_step = 1;

    __int8 i_1 = x + j_step;
    __int8 j_1 = y - j_step;
    while ((i_1 >= 0) && (i_1 <= 7) && (j_1 >= 0) && (j_1 <= 7)) {
        Color current_clr = get_color1(board, i_1, j_1);
        if (current_clr == color)
            break;
        if (bterminal) {
            m.append(i_1, j_1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = i_1;
            xy_to.y = j_1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }
        if (current_clr != Color::EMPTY)
            break;
        i_1 += j_step; // переходим к следующему ходу.
        j_1 -= j_step;
    }

    i_1 = x - j_step;
    j_1 = y + j_step;
    while ((i_1 >= 0) && (i_1 <= 7) && (j_1 >= 0) && (j_1 <= 7)) {
        Color current_clr = get_color1(board, i_1, j_1);
        if (current_clr == color)
            break;
        if (bterminal) {
            m.append(i_1, j_1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = i_1;
            xy_to.y = j_1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }
        if (current_clr != Color::EMPTY)
            break;
        i_1 -= j_step; // переходим к следующему ходу.
        j_1 += j_step;
    }


    return m;
}

// Eval отвечает также за мобильность фигуры, материальная цена атаки.
// конь.
float knightEval[8][8] = {

    -5.0f, -4.0f, -3.0f, -3.0f, -3.0f, -3.0f, -4.0f, -5.0f,
    -4.0f, -2.0f, 0.0f, 0.0f, 0.0f, 0.0f, -2.0f, -4.0f,
    -3.0f, 0.0f, 1.0f, 1.5f, 1.5f, 1.0f, 0.0f, -3.0f,
    -3.0f, 0.5f, 1.5f, 2.0f, 2.0f, 1.5f, 0.5f, -3.0f,
    -3.0f, 0.0f, 1.5f, 2.0f, 2.0f, 1.5f, 0.0f, -3.0f,
    -3.0f, 0.5f, 1.0f, 1.5f, 1.5f, 1.0f, 0.5f, -3.0f,
    -4.0f, -2.0f, 0.0f, 0.5f, 0.5f, 0.0f, -2.0f, -4.0f,
    -5.0f, -4.0f, -3.0f, -3.0f, -3.0f, -3.0f, -4.0f, -5.0f
};


// Конь
class Knight {
public:
    Color color;
    float VALUE = 30.0f; // стоимость коня. 30.5

    Knight();

    Knight(Color icol) {
        color = icol;
    }

    float get_value() {
        return VALUE;
    }

    ChessPiece get() {
        return (color == Color::WHITE ? ChessPiece::W_KNIGHT : ChessPiece::B_KNIGHT);
    }

    MOVES get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O);

    MOVES get_moves_terminal(ChessPiece board[8][8], MOVESL& wList, MOVESL& bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O);


    float rate(ChessPiece board[8][8], __int8 x, __int8 y) {
#ifdef MOBILITY_USER_SETTING
        return VALUE;
#else
        return VALUE + knightEval[y][x];
#endif
    }
};

Knight::Knight() : color(Color::EMPTY)
{

}


// Ходы коня.
// 18.08.2022 В полной версии можно делать только те ходы конём чтобы король не был под шахом. Реализовано 18,08,2022
// Это краткая версия только для реализации проверки шахов.
MOVES Knight::get_moves_terminal(ChessPiece board[8][8], MOVESL& wList, MOVESL& bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O)
{



    MOVES m;// (8);
    m.n = 0;



    __int8 i_step = x + 1;
    __int8 j_step = y + 2;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {            
                m.append(i_step, j_step);
        }
    }

    i_step = x - 1;
    j_step = y + 2;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {
            m.append(i_step, j_step);
        }
    }

    i_step = x - 1;
    j_step = y - 2;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {
            m.append(i_step, j_step);
        }
    }

    i_step = x + 1;
    j_step = y - 2;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {
                m.append(i_step, j_step);
        }
    }


    i_step = x + 2;
    j_step = y - 1;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {
                m.append(i_step, j_step);
        }
    }

    i_step = x + 2;
    j_step = y + 1;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {
            // Своя фигура
        }
        else {
                m.append(i_step, j_step);
        }
    }

    i_step = x - 2;
    j_step = y - 1;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {
                m.append(i_step, j_step);
        }
    }

    i_step = x - 2;
    j_step = y + 1;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {
                m.append(i_step, j_step);
        }
    }

    return m;

}


// Ходы коня.
// 18.08.2022 Можно делать только те ходы конём чтобы король не был под шахом. Реализовано 18,08,2022
MOVES Knight::get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O)
{



    MOVES m;// (8);
    m.n = 0;

    

    __int8 i_step = x + 1;
    __int8 j_step = y + 2;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {
            if (bterminal) {
                m.append(i_step, j_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = j_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

              
                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }


        }
    }

    i_step = x - 1;
    j_step = y + 2;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {

            if (bterminal) {
                m.append(i_step, j_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = j_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);                

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }

        }
    }

    i_step = x - 1;
    j_step = y - 2;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {
            if (bterminal) {
                m.append(i_step, j_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = j_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);
               

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }

        }
    }

    i_step = x + 1;
    j_step = y - 2;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {

            if (bterminal) {
                m.append(i_step, j_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = j_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }

        }
    }


    i_step = x + 2;
    j_step = y - 1;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {

            if (bterminal) {
                m.append(i_step, j_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = j_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }

        }
    }

    i_step = x + 2;
    j_step = y + 1;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {
            // Своя фигура
        }
        else {

            if (bterminal) {
                m.append(i_step, j_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = j_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }

        }
    }

    i_step = x - 2;
    j_step = y - 1;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {

            if (bterminal) {
                m.append(i_step, j_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = j_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }

        }
    }

    i_step = x - 2;
    j_step = y + 1;

    if ((i_step >= 0) && (i_step <= 7) && (j_step >= 0) && (j_step <= 7)) {
        Color current_clr = get_color1(board, i_step, j_step);
        if (current_clr == color) {

        }
        else {

            if (bterminal) {
                m.append(i_step, j_step);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = i_step;
                xy_to.y = j_step;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }

        }
    }

    return m;

}



float pawnEvalWhite[8][8] =
{
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f ,
    5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f,
    1.0f, 1.0f, 2.0f, 3.0f, 3.0f, 2.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 1.0f, 2.5f, 2.5f, 1.0f, 0.5f, 0.5f,
    0.0f, 0.0f, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -1.0f, -0.5f, 0.5f,
    0.5f, 1.0f, 1.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.5f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
};

float pawnEvalBlack[8][8] =
{

    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 1.0f, 1.0f, -2.0f, -2.0f, 1.0f, 1.0f, 0.5f,
    0.5f, -0.5f, -1.0f, 0.0f, 0.0f, -1.0f, -0.5f, 0.5f,
    0.0f, 0.0f, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 1.0f, 2.5f, 2.5f, 1.0f, 0.5f, 0.5f,
    1.0f, 1.0f, 2.0f, 3.0f, 3.0f, 2.0f, 1.0f, 1.0f,
     5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f,
     0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f     
};

// Пешка
class Pawn {

public:
    Color color;
    float VALUE = 10.0f; // стоимость пешки.

    Pawn();

    Pawn(Color icol) {
        color = icol;
    }

    float get_value() {
        return VALUE;
    }

    ChessPiece get() {
        return (color == Color::WHITE ? ChessPiece::W_PAWN : ChessPiece::B_PAWN);
    }

    MOVES get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O, bool ball);

    // Прибавляем по баллу к стоимости пешки при приближении её к краю доски.
    float rate(ChessPiece board[8][8], __int8 x, __int8 y) {
        //return VALUE + 1 * (color==Color::BLACK ? y-1 : 6-y );

        bool b_passedPawn = true; // проходная пешка.
        if (color == Color::BLACK)
        {           
            for (int j = y + 1; (b_passedPawn&&(j < 8)); ++j) {
                for (int i = x - 1; i <= x+1; ++i) {
                    if ((i >= 0) && (i < 8)) {
                        if (board[j][i] == ChessPiece::W_PAWN) {
                            b_passedPawn = false;
                            break;
                        }
                    }
                }
            }
        }
        if (color == Color::WHITE)
        {
            for (int j = y - 1; (b_passedPawn && (j > 0)); --j) {
                for (int i = x - 1; i <= x + 1; ++i) {
                    if ((i >= 0) && (i < 8)) {
                        if (board[j][i] == ChessPiece::B_PAWN) {
                            b_passedPawn = false;
                            break;
                        }
                    }
                }
            }
        }

        float passedPawnarr[8] = {0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 3.0f, 14.0f, 0.0f};

        return VALUE + (((color == Color::WHITE ? (b_passedPawn ? passedPawnarr[7-y] : pawnEvalWhite[y][x]) 
            : (b_passedPawn ? passedPawnarr[y] : pawnEvalBlack[y][x])))/static_calibrate);
    }
    
};

Pawn::Pawn() : color(Color::EMPTY)
{

}



// Король
float kingEvalWhite[8][8] = {

    -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f ,
        -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f,
        -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f,
        -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f,
        -2.0f, -3.0f, -3.0f, -4.0f, -4.0f, -3.0f, -3.0f, -2.0f,
        -1.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -1.0f,
        2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 2.0f,
        2.0f, 3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 3.0f, 2.0f
};

float kingEvalBlack[8][8] = {

     2.0f, 3.0f, 1.0f, 0.0f, 0.0f, 1.0f, 3.0f, 2.0f,
     2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 2.0f,
    -1.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -2.0f, -1.0f,
    -2.0f, -3.0f, -3.0f, -4.0f, -4.0f, -3.0f, -3.0f, -2.0f,
    -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f,
    -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f,
    -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f,
    -3.0f, -4.0f, -4.0f, -5.0f, -5.0f, -4.0f, -4.0f, -3.0f          
};

// В конце игры у короля - приоритет центра.
float kingEvalEndshpil[8][8] = {

   0.0f, 0.6f, 1.2f, 1.8f, 1.8f, 1.2f, 0.6f, 0.0f,
   0.6f, 1.2f, 1.8f, 2.4f, 2.4f, 1.8f, 1.2f, 0.6f,
   1.2f, 1.8f, 2.4f, 3.0f, 3.0f, 2.4f, 1.8f, 1.2f,
   1.8f, 2.4f, 3.0f, 3.6f, 3.6f, 3.0f, 2.4f, 1.8f,
   1.8f, 2.4f, 3.0f, 3.6f, 3.6f, 3.0f, 2.4f, 1.8f,
   1.2f, 1.8f, 2.4f, 3.0f, 3.0f, 2.4f, 1.8f, 1.2f,
   0.6f, 1.2f, 1.8f, 2.4f, 2.4f, 1.8f, 1.2f, 0.6f,
   0.0f, 0.6f, 1.2f, 1.8f, 1.8f, 1.2f, 0.6f, 0.0f
};

// В малофигурных окончаниях можно существенно увеличить глубину дерева,
// а то программа себя очень глупо ведет. Тем более что быстродействие позволяет.
__int8 number_figures(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList) {
    __int8 iret = 0;

    // Проходим по доске
    //for (__int8 y = 0; y < 8; ++y) {
      //  for (__int8 x = 0; x < 8; ++x) {
        //    if (board[y][x] != ChessPiece::EMPTY) ++iret;
        //}
   // }
    iret = wList.n + bList.n;
    return iret;
}

// Коническая оценка для плавного перехода из мидэндшпиля в эндшпиль.
// 11.12.2022
float calculation_phase_for_conic_rate(ChessPiece board[8][8], MOVESL& wList, MOVESL& bList) {
    __int8 iret = 0;

    //const float pawn_phase = 0.0; // Пешка
    const float knight_phase = 1.0; // Конь
    const float bishop_phase = 1.0; // Слон
    const float rook_phase = 2.0; // Ладья
    const float queen_phase = 4.0; // Ферзь.

    //const float all_phase = /*pawn_phase * 16 +*/ knight_phase * 4 + bishop_phase * 4 + rook_phase * 4 + queen_phase * 2;//24

    const float all_phase = 24.0f;

    float phase = all_phase;

    // Проходим по доске
    /*for (__int8 y = 0; y < 8; ++y) {
        for (__int8 x = 0; x < 8; ++x) {
            if (board[y][x] != ChessPiece::EMPTY) {
                switch (board[y][x]) {
                case ChessPiece::B_KNIGHT:
                    phase -= knight_phase;
                    break;
                case ChessPiece::W_KNIGHT:
                    phase -= knight_phase;
                    break;
                case ChessPiece::B_BISHOP:
                    phase -= bishop_phase;
                    break;
                case ChessPiece::W_BISHOP:
                    phase -= bishop_phase;
                    break;
                case ChessPiece::B_QUEEN:
                    phase -= queen_phase;
                    break;
                case ChessPiece::W_QUEEN:
                    phase -= queen_phase;
                    break;
                case ChessPiece::B_ROOK:
                    phase -= rook_phase;
                    break;
                case ChessPiece::W_ROOK:
                    phase -= rook_phase;
                    break;
                //case ChessPiece::B_PAWN : 
                  //  phase -= pawn_phase;
                    //break;
                //case ChessPiece::W_PAWN:
                  //  phase -= pawn_phase;
                    //break;
                }
            }
        }
    }*/

    for (__int8 k = 0; k < bList.n; ++k) {

        __int8 j = bList.moves[k].y;
        __int8 i = bList.moves[k].x;

        switch (board[j][i]) {
        case ChessPiece::B_KNIGHT:
            phase -= knight_phase;
            break;
        case ChessPiece::B_BISHOP:
            phase -= bishop_phase;
            break;
        case ChessPiece::B_QUEEN:
            phase -= queen_phase;
            break;
        case ChessPiece::B_ROOK:
            phase -= rook_phase;
            break;
        }
    }

    for (__int8 k = 0; k < wList.n; ++k) {

        __int8 j = wList.moves[k].y;
        __int8 i = wList.moves[k].x;

        switch (board[j][i]) {
        case ChessPiece::W_KNIGHT:
            phase -= knight_phase;
            break;
        case ChessPiece::W_BISHOP:
            phase -= bishop_phase;
            break;
        case ChessPiece::W_QUEEN:
            phase -= queen_phase;
            break;
        case ChessPiece::W_ROOK:
            phase -= rook_phase;
            break;
        }
    }
    
   // 13 - 139.2 импирический порог

    // 256 - Окончание партии
    // 0 - дебют партии.
    return (all_phase < 1.0f ? 0.0f : (phase*256.0f+0.5f*all_phase)/all_phase);
}


// Шахматный король.
class King  {

public:
    Color color;
    float VALUE = 900.0f;// 0; // стоимость Короля.

    King();
    King(Color icol) {
        color = icol;
    }

    float get_value() {
        return VALUE;
    }

    ChessPiece get() {
        return (color == Color::WHITE ? ChessPiece::W_KING : ChessPiece::B_KING);
    }
    MOVES get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O);

    // Стоимость короля.
    float rate(ChessPiece board[8][8], MOVESL wList, MOVESL bList, __int8 x, __int8 y) {
        // Устаревшее описание.
        //return VALUE + (number_figures(board,wList,bList) <= KING_CENTER_PRIORITY_NUMBER_FIGURES ? kingEvalEndshpil[y][x] : (color== Color::WHITE ? kingEvalWhite[y][x] : kingEvalBlack[y][x]));

        // 256 - Окончание партии
        // 0 - дебют партии.
        float phase = calculation_phase_for_conic_rate(board, wList, bList); // фаза коническая оценка.

        // Плавный переход от дебюта к эндшпилю.
        //return VALUE + ((((color == Color::WHITE ? kingEvalWhite[y][x] : kingEvalBlack[y][x])) * ((256.0 - phase)/256.0)) + (kingEvalEndshpil[y][x] * (phase/256.0)));

       // Применяем сигмоиду
        //const float rastvor_koridor = 0.1f;// При 0.04 ближе к линейной функции. При 1.0 очень резкий переход при значении phase=128.0. 
        //return VALUE + (((color == Color::WHITE ? kingEvalWhite[y][x] : kingEvalBlack[y][x])) * (1.0/(1.0+exp(rastvor_koridor*(phase-128.0)))) + (kingEvalEndshpil[y][x] * (1.0 / (1.0 + exp(-rastvor_koridor * (phase - 128.0))))));
   
        // Больше значения KING_CENTER_PRIORITY_WEIGHT (==139 импирический порог) эндшпиль и Король идет в атаку.
        // Оценки мидэндшпиля и эндшпиля не сочетаемы не совместимы и плавный переход по видимому не лучшая идея.
        return VALUE + (((phase >= KING_CENTER_PRIORITY_WEIGHT ? kingEvalEndshpil[y][x] : (color == Color::WHITE ? kingEvalWhite[y][x] : kingEvalBlack[y][x])))/static_calibrate);
    
    }
};

King::King() : color(Color::EMPTY)
{

}


// Чтобы продлить шахи нужно проверить есть ли шах.
// Проверяет есть ли шах.
// Ускоренная версия 26.08.2022 Было 11.02% стало 8%.
bool InCheck(ChessPiece new_board[8][8], MOVESL& wList, MOVESL& bList,
    PREVIOS_MOVE previos_moves, Color color, 
    bool WHITE_O_O, bool WHITE_O_O_O, 
    bool BLACK_O_O, bool BLACK_O_O_O) {


    bool shah = false;

    // По правилам игры мы не можем сами встать под шах.
    bool attack[8][8];
    for (__int8 j = 0; j <= 7; ++j) {
        for (__int8 i = 0; i <= 7; ++i) {
            attack[j][i] = false; // Инициализация все поля свободны.
        }
    }

    // Вычисляем поля атаки, поля под которые королю нельзя вставать.
    if (color == Color::BLACK) {
        
        // Положение чёрного Короля.
        __int8 iKing = -1;
        __int8 jKing = -1;

        for (__int8 k = 0; k < bList.n; ++k) {

            __int8 j = bList.moves[k].y;
            __int8 i = bList.moves[k].x;

            if (new_board[j][i] == ChessPiece::B_KING) {
               
                iKing = i;
                jKing = j;

                 break;
               
            }
        }

         // Мы делаем ход чёной фигурой
    // формируем поля атаки белых фигур
    // Проверяем находится ли наш чёрный король под атакой 
    // вражеских белых фигур после нашего хода пешкой.

        for (__int8 k = 0; k < wList.n; ++k) {

            __int8 j = wList.moves[k].y;
            __int8 i = wList.moves[k].x;

            if (new_board[j][i] == ChessPiece::W_KING) {
                // Поля которые бьёт белый Король.
                if (j - 1 >= 0) attack[j - 1][i] = true;
                if (j + 1 <= 7) attack[j + 1][i] = true;
                if (i - 1 >= 0) attack[j][i - 1] = true;
                if (i + 1 <= 7) attack[j][i + 1] = true;
                if ((j - 1 >= 0) && (i - 1 >= 0)) attack[j - 1][i - 1] = true;
                if ((j + 1 <= 7) && (i + 1 <= 7)) attack[j + 1][i + 1] = true;
                if ((j + 1 <= 7) && (i - 1 >= 0)) attack[j + 1][i - 1] = true;
                if ((j - 1 >= 0) && (i + 1 <= 7)) attack[j - 1][i + 1] = true;
            }
            else if (new_board[j][i] == ChessPiece::W_PAWN) {
                // Пешка нападает наискосок.
                if ((j - 1 >= 0) && (i - 1 >= 0)) attack[j - 1][i - 1] = true;
                if ((j - 1 >= 0) && (i + 1 <= 7)) attack[j - 1][i + 1] = true;
            }
            else if (new_board[j][i] == ChessPiece::W_ROOK) {

                if ((iKing == i) || (jKing == j)) {
                    // Только если белая Ладья стоит на одной линии с Королём.

                    MOVES m;// (14);
                    m.n = 0;
                    Rook rook;
                    rook.color = Color::WHITE;
                    // true - терминальный вызов генератора.
                    m = rook.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                    for (__int8 k = 0; k < m.n; ++k) {
                        // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                        attack[m.moves[k].y][m.moves[k].x] = true;
                    }
                }
            }
            else if (new_board[j][i] == ChessPiece::W_QUEEN) {

                if ((abs(iKing - i) == abs(jKing - j))|| (iKing == i) || (jKing == j)) {
                    // Ферзь на одной вертикали или горизонтали или диагонали с Королём.

                    MOVES m;
                    m.n = 0;
                    Queen queen;
                    queen.color = Color::WHITE;
                    // true - терминальный вызов генератора.
                    m = queen.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                    for (__int8 k = 0; k < m.n; ++k) {
                        // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                        attack[m.moves[k].y][m.moves[k].x] = true;
                    }
                }
            }
            else if (new_board[j][i] == ChessPiece::W_BISHOP) {

                if (abs(iKing - i) == abs(jKing - j)) {
                    // Если Король на одной диагонали со слоном.

                    MOVES m; // (14);
                    m.n = 0;
                    Bishop bishop;
                    bishop.color = Color::WHITE;
                    // true - терминальный вызов генератора.
                    m = bishop.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                    for (__int8 k = 0; k < m.n; ++k) {
                        // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                        attack[m.moves[k].y][m.moves[k].x] = true;
                        // printf("bishop x=%d y=%d\n", m.moves[k].x, m.moves[k].y);
                    }
                }
            }
            else if (new_board[j][i] == ChessPiece::W_KNIGHT) {
                MOVES m; // (8);
                m.n = 0;
                Knight knight;
                knight.color = Color::WHITE;
                // terminal(true) - терминальный вызов генератора.
                m = knight.get_moves_terminal((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }

      
        // Если чёрный король под атакой белых фигур.
        if (attack[jKing][iKing]) {
              // Король под шахом.
              shah = true;
        }
      

    }
   


    if (color == Color::WHITE) {
        
        // Позиция белого короля.
        __int8 iKing = -1;
        __int8 jKing = -1;

        for (__int8 k = 0; k < wList.n; ++k) {

            __int8 j = wList.moves[k].y;
            __int8 i = wList.moves[k].x;

            if (new_board[j][i] == ChessPiece::W_KING) {

                iKing = i;
                jKing = j;

                break;

            }
        }



        for (__int8 k = 0; k < bList.n; ++k) {

            __int8 j = bList.moves[k].y;
            __int8 i = bList.moves[k].x;

            if (new_board[j][i] == ChessPiece::B_KING) {
                // Поля на которые атакует чёрный король.
                if (j - 1 >= 0)  attack[j - 1][i] = true;
                if (j + 1 <= 7)  attack[j + 1][i] = true;
                if (i - 1 >= 0)  attack[j][i - 1] = true;
                if (i + 1 <= 7)  attack[j][i + 1] = true;
                if ((j - 1 >= 0) && (i - 1 >= 0)) attack[j - 1][i - 1] = true;
                if ((j + 1 <= 7) && (i + 1 <= 7)) attack[j + 1][i + 1] = true;
                if ((j + 1 <= 7) && (i - 1 >= 0)) attack[j + 1][i - 1] = true;
                if ((j - 1 >= 0) && (i + 1 <= 7)) attack[j - 1][i + 1] = true;
            }
            else if (new_board[j][i] == ChessPiece::B_PAWN) {
                // Пешка нападает наискосок.
                if ((j + 1 <= 7) && (i - 1 >= 0))  attack[j + 1][i - 1] = true;
                if ((j + 1 <= 7) && (i + 1 <= 7))  attack[j + 1][i + 1] = true;
            }
            else if (new_board[j][i] == ChessPiece::B_ROOK) {

                if ((iKing == i) || (jKing == j)) {
                    // Только если Ладья стоит на одной линии с Королём.

                    MOVES m; // (14);
                    m.n = 0;
                    Rook rook;
                    rook.color = Color::BLACK;
                    m = rook.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                    for (__int8 k = 0; k < m.n; ++k) {
                        // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                        attack[m.moves[k].y][m.moves[k].x] = true;
                    }
                }
            }
            else if (new_board[j][i] == ChessPiece::B_QUEEN) {

                if ((abs(iKing - i) == abs(jKing - j)) || (iKing == i) || (jKing == j)) {
                    // Ферзь на одной вертикали или горизонтали или диагонали с Королём.

                    MOVES m;
                    m.n = 0;
                    Queen queen;
                    queen.color = Color::BLACK;
                    m = queen.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                    for (__int8 k = 0; k < m.n; ++k) {
                        // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                        attack[m.moves[k].y][m.moves[k].x] = true;
                    }
                }
            }
            else if (new_board[j][i] == ChessPiece::B_BISHOP) {

                if (abs(iKing - i) == abs(jKing - j)) {
                    // Если Король на одной диагонали со слоном.

                    MOVES m; // (14);
                    m.n = 0;
                    Bishop bishop;
                    bishop.color = Color::BLACK;
                    m = bishop.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                    for (__int8 k = 0; k < m.n; ++k) {
                        // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                        attack[m.moves[k].y][m.moves[k].x] = true;
                    }
                }
            }
            else if (new_board[j][i] == ChessPiece::B_KNIGHT) {
                MOVES m; // (8);
                m.n = 0;
                Knight knight;
                knight.color = Color::BLACK;
                m = knight.get_moves_terminal((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
    
    
        // Если белый король под атакой чёрных фигур.
        if ((jKing >= 0) && (jKing <= 7) && (iKing >= 0) && (iKing <= 7)) {
            if (attack[jKing][iKing]) {
                // Король под шахом.
                shah = true;
            }
        }
       
    }
    

    return shah;
}


// Чтобы продлить шахи нужно проверить есть ли шах.
// Проверяет есть ли шах.
bool InCheck0(ChessPiece new_board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, Color color, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O) {
    

    // По правилам игры мы не можем сами встать под шах.
    bool attack[8][8];
    for (__int8 j = 0; j <= 7; ++j) {
        for (__int8 i = 0; i <= 7; ++i) {
            attack[j][i] = false; // Инициализация все поля свободны.
        }
    }

    // Вычисляем поля атаки, поля под которые королю нельзя вставать.
    if (color == Color::BLACK) {
        // for (__int8 j = 0; j <= 7; ++j) {
          //   for (__int8 i = 0; i <= 7; ++i) {
            //     if (get_color1(new_board, i, j) == Color::WHITE) {

        for (__int8 k = 0; k < wList.n; ++k) {

            __int8 j = wList.moves[k].y;
            __int8 i = wList.moves[k].x;

            if (new_board[j][i] == ChessPiece::W_KING) {
                // поля на которые атакует белый Король.
                if (j - 1 >= 0) attack[j - 1][i] = true;
                if (j + 1 <= 7) attack[j + 1][i] = true;
                if (i - 1 >= 0) attack[j][i - 1] = true;
                if (i + 1 <= 7) attack[j][i + 1] = true;
                if ((j - 1 >= 0) && (i - 1 >= 0)) attack[j - 1][i - 1] = true;
                if ((j + 1 <= 7) && (i + 1 <= 7)) attack[j + 1][i + 1] = true;
                if ((j + 1 <= 7) && (i - 1 >= 0)) attack[j + 1][i - 1] = true;
                if ((j - 1 >= 0) && (i + 1 <= 7)) attack[j - 1][i + 1] = true;
            }
            else if (new_board[j][i] == ChessPiece::W_PAWN) {
                // Пешка нападает наискосок.
                if ((j - 1 >= 0) && (i - 1 >= 0)) attack[j - 1][i - 1] = true;
                if ((j - 1 >= 0) && (i + 1 <= 7)) attack[j - 1][i + 1] = true;
            }
            else if (new_board[j][i] == ChessPiece::W_ROOK) {

                MOVES m; // (14);
                m.n = 0;
                Rook rook;
                rook.color = Color::WHITE;
                // true - терминальный вызов.
                m = rook.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
            else if (new_board[j][i] == ChessPiece::W_QUEEN) {

                MOVES m;
                m.n = 0;
                Queen queen;
                queen.color = Color::WHITE;
                // true - терминальный вызов.
                m = queen.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
            else if (new_board[j][i] == ChessPiece::W_BISHOP) {

                MOVES m; // (14);
                m.n = 0;
                Bishop bishop;
                bishop.color = Color::WHITE;
                // true - терминальный вызов.
                m = bishop.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
            else if (new_board[j][i] == ChessPiece::W_KNIGHT) {
                MOVES m; // (8);
                m.n = 0;
                Knight knight;
                knight.color = Color::WHITE;
                // true - терминальный вызов.
                m = knight.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }

    }
       


    if (color == Color::WHITE) {
        

        for (int k = 0; k < bList.n; ++k) {

            __int8 j = bList.moves[k].y;
            __int8 i = bList.moves[k].x;

            if (new_board[j][i] == ChessPiece::B_KING) {
                // Поля атаки чёрного Короля
                if (j - 1 >= 0)  attack[j - 1][i] = true;
                if (j + 1 <= 7)  attack[j + 1][i] = true;
                if (i - 1 >= 0)  attack[j][i - 1] = true;
                if (i + 1 <= 7)  attack[j][i + 1] = true;
                if ((j - 1 >= 0) && (i - 1 >= 0)) attack[j - 1][i - 1] = true;
                if ((j + 1 <= 7) && (i + 1 <= 7)) attack[j + 1][i + 1] = true;
                if ((j + 1 <= 7) && (i - 1 >= 0)) attack[j + 1][i - 1] = true;
                if ((j - 1 >= 0) && (i + 1 <= 7)) attack[j - 1][i + 1] = true;
            }
            else if (new_board[j][i] == ChessPiece::B_PAWN) {
                // Чёрная пешка нападает наискосок.
                if ((j + 1 <= 7) && (i - 1 >= 0))  attack[j + 1][i - 1] = true;
                if ((j + 1 <= 7) && (i + 1 <= 7))  attack[j + 1][i + 1] = true;
            }
            else if (new_board[j][i] == ChessPiece::B_ROOK) {

                MOVES m; // (14);
                m.n = 0;
                Rook rook;
                rook.color = Color::BLACK;
                // true - терминальный вызов.
                m = rook.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
            else if (new_board[j][i] == ChessPiece::B_QUEEN) {

                MOVES m;
                m.n = 0;
                Queen queen;
                queen.color = Color::BLACK;
                // true - терминальный вызов.
                m = queen.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
            else if (new_board[j][i] == ChessPiece::B_BISHOP) {

                MOVES m; // (14);
                m.n = 0;
                Bishop bishop;
                bishop.color = Color::BLACK;
                // true - терминальный вызов.
                m = bishop.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
            else if (new_board[j][i] == ChessPiece::B_KNIGHT) {
                MOVES m; // (8);
                m.n = 0;
                Knight knight;
                knight.color = Color::BLACK;
                // true - терминальный вызов.
                m = knight.get_moves((new_board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                    attack[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
    }
        //    }
      //  }
    //}

    bool shah = false;
   // for (__int8 j = 0; j <= 7; ++j) {
     //   for (__int8 i = 0; i <= 7; ++i) {
            if (color == Color::WHITE) {

                for (__int8 k = 0; k < wList.n; ++k) {

                    __int8 j = wList.moves[k].y;
                    __int8 i = wList.moves[k].x;

                    // Белый король под боем одной из чёрных фигур.
                    if (new_board[j][i] == ChessPiece::W_KING) {
                        if (attack[j][i]) {
                            // Король под шахом.
                            shah = true;
                            break;
                        }
                    }
                }
            }
            // Мы делаем ход чёной фигурой
            // формируем поля атаки белых фигур
            // Проверяем находится ли наш чёрный король под атакой 
            // вражеских белых фигур после нашего хода пешкой.
            if (color == Color::BLACK) {

                for (int k = 0; k < bList.n; ++k) {

                    __int8 j = bList.moves[k].y;
                    __int8 i = bList.moves[k].x;

                    // Чёрный король под боем одной из белых фигур.
                    if (new_board[j][i] == ChessPiece::B_KING) {
                        if (attack[j][i]) {
                            // Король под шахом.
                            shah = true;
                            break;
                        }
                    }
                }
            }
        //}
    //}

    return shah;
}


void append_chess_move(Color color, ChessPiece new_board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, MOVES& mgl, MOVES_NODE xy_to, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O)
{

    if (0) {

        mgl.append(xy_to.x, xy_to.y);
    }
    else
    {
        

        if (!InCheck(new_board, wList, bList, previos_moves, color, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O)) {
            // Добавляем ход конём только в том случае если король не под шахом.
            mgl.append(xy_to.x, xy_to.y);
        }
    }
}


// 13.08.2022 Добавил ходы Короля.
// 14.08.2022 добавил ходы Ладьи, Ферзя и слона, коня.
// Для пешек все ходы (и взятия и простые).
MOVES Board::get_moves(__int8 x, __int8 y)
{
    MOVES m;
    m.n = 0;

    
    //printf("B_Bishop %d \n", m.n);

    if ((x >= 0) && (y >= 0) && (x < 8) && (y < 8)) {

        bool bterminal = false;

        // Ходы пешек.
        if (this->board[y][x] == ChessPiece::W_PAWN) {
            Pawn pawn;
            pawn.color = Color::WHITE;
            m = pawn.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O, true);
        }
        if (this->board[y][x] == ChessPiece::B_PAWN) {
            Pawn pawn;
            pawn.color = Color::BLACK;
            m = pawn.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O, true);
        }
        // Ходы Короля.
        if (this->board[y][x] == ChessPiece::W_KING) {
            King king;
            king.color = Color::WHITE;
            m = king.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        if (this->board[y][x] == ChessPiece::B_KING) {
            King king;
            king.color = Color::BLACK;
            m = king.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        // Ходы Ладьи.
        if (this->board[y][x] == ChessPiece::W_ROOK) {
            Rook rook;
            rook.color = Color::WHITE;
            m = rook.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        if (this->board[y][x] == ChessPiece::B_ROOK) {
            Rook rook;
            rook.color = Color::BLACK;
            m = rook.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }

        // Ходы Слона.
        if (this->board[y][x] == ChessPiece::W_BISHOP) {
            Bishop bishop;
            bishop.color = Color::WHITE;
            m = bishop.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        if (this->board[y][x] == ChessPiece::B_BISHOP) {
            Bishop bishop;
            bishop.color = Color::BLACK;

            m = bishop.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }

        // Ходы Ферзя.
        if (this->board[y][x] == ChessPiece::W_QUEEN) {
            Queen queen;
            queen.color = Color::WHITE;
            m = queen.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        if (this->board[y][x] == ChessPiece::B_QUEEN) {
            Queen queen;
            queen.color = Color::BLACK;
            m = queen.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }

        // Ходы коня
        if (this->board[y][x] == ChessPiece::W_KNIGHT) {
            Knight knight;
            knight.color = Color::WHITE;
            m = knight.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        if (this->board[y][x] == ChessPiece::B_KNIGHT) {
            Knight knight;
            knight.color = Color::BLACK;
            m = knight.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
    }


    return m;
}


// 13.08.2022 Добавил ходы Короля.
// 14.08.2022 добавил ходы Ладьи, Ферзя и слона, коня.
//для пешек только взятия.
MOVES Board::get_moves_white(__int8 x, __int8 y)
{
    MOVES m;
    m.n = 0;


    //printf("B_Bishop %d \n", m.n);

    if ((x >= 0) && (y >= 0) && (x < 8) && (y < 8)) {

        bool bterminal = false;

        // Ходы пешек.
        if (this->board[y][x] == ChessPiece::W_PAWN) {
            Pawn pawn;
            pawn.color = Color::WHITE;
            m = pawn.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O, false);
        }
        else 
       
        // Ходы Короля.
        if (this->board[y][x] == ChessPiece::W_KING) {
            King king;
            king.color = Color::WHITE;
            m = king.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        else 
        // Ходы Ладьи.
        if (this->board[y][x] == ChessPiece::W_ROOK) {
            Rook rook;
            rook.color = Color::WHITE;
            m = rook.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        else

        // Ходы Слона.
        if (this->board[y][x] == ChessPiece::W_BISHOP) {
            Bishop bishop;
            bishop.color = Color::WHITE;
            m = bishop.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        else

        // Ходы Ферзя.
        if (this->board[y][x] == ChessPiece::W_QUEEN) {
            Queen queen;
            queen.color = Color::WHITE;
            m = queen.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        else 

        // Ходы коня
        if (this->board[y][x] == ChessPiece::W_KNIGHT) {
            Knight knight;
            knight.color = Color::WHITE;
            m = knight.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        
    }


    return m;
}

// 13.08.2022 Добавил ходы Короля.
// 14.08.2022 добавил ходы Ладьи, Ферзя и слона, коня.
//для пешек только взятия.
MOVES Board::get_moves_black(__int8 x, __int8 y)
{
    MOVES m;
    m.n = 0;


    //printf("B_Bishop %d \n", m.n);

    if ((x >= 0) && (y >= 0) && (x < 8) && (y < 8)) {

        bool bterminal = false;

        // Ходы пешек.
       
        if (this->board[y][x] == ChessPiece::B_PAWN) {
            Pawn pawn;
            pawn.color = Color::BLACK;
            m = pawn.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O, false);
        }
        else
        // Ходы Короля.
        if (this->board[y][x] == ChessPiece::B_KING) {
            King king;
            king.color = Color::BLACK;
            m = king.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        else
        // Ходы Ладьи.
        if (this->board[y][x] == ChessPiece::B_ROOK) {
            Rook rook;
            rook.color = Color::BLACK;
            m = rook.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        else
        // Ходы Слона.
        if (this->board[y][x] == ChessPiece::B_BISHOP) {
            Bishop bishop;
            bishop.color = Color::BLACK;

            m = bishop.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        else
        // Ходы Ферзя.
        if (this->board[y][x] == ChessPiece::B_QUEEN) {
            Queen queen;
            queen.color = Color::BLACK;
            m = queen.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
        else
        // Ходы коня
        if (this->board[y][x] == ChessPiece::B_KNIGHT) {
            Knight knight;
            knight.color = Color::BLACK;
            m = knight.get_moves((this->board), (this->wList), (this->bList), (this->previos_moves), x, y, bterminal, this->WHITE_O_O, this->WHITE_O_O_O, this->BLACK_O_O, this->BLACK_O_O_O);
        }
    }


    return m;
}

// 07.08.2022  - Ходы для короля.
// 25.08.2022 - Доработал с учётом рокировки.
MOVES King::get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves, __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O)
{
    Board board_loc;
    MOVES m1; // (10);
    m1.n = 0;

    // По правилам игры мы не можем сами встать под шах.
    bool attack[8][8]; 
    for (__int8 j = 0; j <= 7; ++j) {
        for (__int8 i = 0; i <= 7; ++i) {
            attack[j][i] = false; // Инициализация все поля свободны.
        }
    }

    // Вычисляем поля атаки, поля под которые королю нельзя вставать.
    if (color == Color::BLACK) {
        for (__int8 j = 0; j <= 7; ++j) {
            for (__int8 i = 0; i <= 7; ++i) {
                if (get_color1(board, i, j) == Color::WHITE) {
                    if (board[j][i] == ChessPiece::W_KING) {
                        // Поля на которые атакует былый Король.
                        if (j - 1 >= 0) attack[j - 1][i] = true;
                        if (j + 1 <= 7) attack[j + 1][i] = true;
                        if (i - 1 >= 0) attack[j][i - 1] = true;
                        if (i + 1 <= 7) attack[j][i + 1] = true;
                        if ((j - 1 >= 0) && (i - 1 >= 0)) attack[j - 1][i - 1] = true;
                        if ((j + 1 <= 7) && (i + 1 <= 7)) attack[j + 1][i + 1] = true;
                        if ((j + 1 <= 7) && (i - 1 >= 0)) attack[j + 1][i - 1] = true;
                        if ((j - 1 >= 0) && (i + 1 <= 7)) attack[j - 1][i + 1] = true;
                    }
                    else if (board[j][i] == ChessPiece::W_PAWN) {
                        // Белая пешка нападает наискосок.
                        if ((j - 1 >= 0) && (i - 1 >= 0)) attack[j - 1][i - 1] = true;
                        if ((j - 1 >= 0) && (i + 1 <= 7)) attack[j - 1][i + 1] = true;
                    }
                    else if (board[j][i] == ChessPiece::W_ROOK) {

                        MOVES m; // (14);
                        m.n = 0;
                        Rook rook;
                        rook.color = Color::WHITE;
                        m = rook.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                        for (__int8 k = 0; k < m.n; ++k) {
                            // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                            attack[m.moves[k].y][m.moves[k].x] = true;

                        }
                    }
                    else if (board[j][i] == ChessPiece::W_QUEEN) {

                        MOVES m;
                        m.n = 0;
                        Queen queen;
                        queen.color = Color::WHITE;
                        m = queen.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                        for (__int8 k = 0; k < m.n; ++k) {
                            // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                            attack[m.moves[k].y][m.moves[k].x] = true;
                        }
                    }
                    else if (board[j][i] == ChessPiece::W_BISHOP) {

                        MOVES m; // (14);
                        m.n = 0;
                        Bishop bishop;
                        bishop.color = Color::WHITE;
                        m = bishop.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                        for (__int8 k = 0; k < m.n; ++k) {
                            // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                            attack[m.moves[k].y][m.moves[k].x] = true;
                        }
                    }
                    else if (board[j][i] == ChessPiece::W_KNIGHT) {
                        MOVES m; // (8);
                        m.n = 0;
                        Knight knight;
                        knight.color = Color::WHITE;
                        m = knight.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                        for (__int8 k = 0; k < m.n; ++k) {
                            // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                            attack[m.moves[k].y][m.moves[k].x] = true;
                        }
                    }
                }

                
            }
        }
    }


    if (color == Color::WHITE) {
        for (__int8 j = 0; j <= 7; ++j) {
            for (__int8 i = 0; i <= 7; ++i) {
                if (get_color1(board, i, j) == Color::BLACK) {
                    if (board[j][i] == ChessPiece::B_KING) {
                        if (j - 1 >= 0)  attack[j - 1][i] = true;
                        if (j + 1 <= 7)  attack[j + 1][i] = true;
                        if (i - 1 >= 0)  attack[j][i - 1] = true;
                        if (i + 1 <= 7)  attack[j][i + 1] = true;
                        if ((j - 1 >= 0) && (i - 1 >= 0)) attack[j - 1][i - 1] = true;
                        if ((j + 1 <= 7) && (i + 1 <= 7)) attack[j + 1][i + 1] = true;
                        if ((j + 1 <= 7) && (i - 1 >= 0)) attack[j + 1][i - 1] = true;
                        if ((j - 1 >= 0) && (i + 1 <= 7)) attack[j - 1][i + 1] = true;
                    }
                    else if (board[j][i] == ChessPiece::B_PAWN) {
                        // Пешка нападает наискосок.
                        if ((j + 1 <= 7) && (i - 1 >= 0))  attack[j + 1][i - 1] = true;
                        if ((j + 1 <= 7) && (i + 1 <= 7))  attack[j + 1][i + 1] = true;
                    }
                    else if (board[j][i] == ChessPiece::B_ROOK) {

                        MOVES m; // (14);
                        m.n = 0;
                        Rook rook;
                        rook.color = Color::BLACK;
                        m = rook.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                        for (__int8 k = 0; k < m.n; ++k) {
                            // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                            attack[m.moves[k].y][m.moves[k].x] = true;
                        }
                    }
                    else if (board[j][i] == ChessPiece::B_QUEEN) {

                        MOVES m;
                        m.n = 0;
                        Queen queen;
                        queen.color = Color::BLACK;
                        m = queen.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                        for (__int8 k = 0; k < m.n; ++k) {
                            // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                            attack[m.moves[k].y][m.moves[k].x] = true;
                        }
                    }
                    else if (board[j][i] == ChessPiece::B_BISHOP) {

                        MOVES m; // (14);
                        m.n = 0;
                        Bishop bishop;
                        bishop.color = Color::BLACK;
                        m = bishop.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                        for (__int8 k = 0; k < m.n; ++k) {
                            // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                            attack[m.moves[k].y][m.moves[k].x] = true;
                        }
                    }
                    else if (board[j][i] == ChessPiece::B_KNIGHT) {
                        MOVES m; // (8);
                        m.n = 0;
                        Knight knight;
                        knight.color = Color::BLACK;
                        m = knight.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                        for (__int8 k = 0; k < m.n; ++k) {
                            // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                            attack[m.moves[k].y][m.moves[k].x] = true;
                        }
                    }
                }
            }
        }
    }

    for (__int8 j = y - 1; j <= y + 1; ++j) {
        for (__int8 i = x - 1; i <= x + 1; ++i) {
            if ((i == x) && (j == y)) continue;
            if ((i >= 0) && (i <= 7) && (j >= 0) && (j <= 7)) {
                if ((color == Color::BLACK) && (get_color1(board, i, j) != Color::BLACK)) {

                    // Нельзя вставать под шах.
                    if (!attack[j][i]) {

                        if (bterminal) {
                            m1.append(i, j);
                        }
                        else
                        {
                            // Мы можем съесть фигуру противника только в том случае 
                           // если она не защищена другой фигурой противника.

                            MOVES_NODE xy_from;
                            MOVES_NODE xy_to;
                            xy_from.x = x;
                            xy_from.y = y;
                            xy_to.x = i;
                            xy_to.y = j;

                            Board new_board;

                            // Копирование доски.
                            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                             // Предыдущий ход на этой новой  доске после сделанного хода.
                            new_board.previos_moves.xy_from = xy_from;
                            new_board.previos_moves.xy_to = xy_to;
                            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m1, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


                        }
                    }
                }
                if ((color == Color::WHITE) && (get_color1(board, i, j) != Color::WHITE)) {
                    if (!attack[j][i]) {

                        if (bterminal) {
                            m1.append(i, j);
                        }
                        else
                        {

                            // Мы можем съесть фигуру противника только в том случае 
                            // если она не защищена другой фигурой противника.

                            MOVES_NODE xy_from;
                            MOVES_NODE xy_to;
                            xy_from.x = x;
                            xy_from.y = y;
                            xy_to.x = i;
                            xy_to.y = j;

                            Board new_board;

                            // Копирование доски.
                            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                             // Предыдущий ход на этой новой  доске после сделанного хода.
                            new_board.previos_moves.xy_from = xy_from;
                            new_board.previos_moves.xy_to = xy_to;
                            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m1, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


                        }
                    }
                }
            }

        }
    }

    // 25.08.2022 - рокировка.
    if ((color == Color::WHITE)&&(!attack[7][4]) && (!attack[7][5]) && (!attack[7][6]) /* && (!attack[7][7])*/ &&
    (board[7][4]==ChessPiece::W_KING)&& (board[7][7] == ChessPiece::W_ROOK)&&(!WHITE_O_O)&&
        (board[7][5] == ChessPiece::EMPTY)&& (board[7][6] == ChessPiece::EMPTY))
    {
        // Рокировка задаётся ходом Короля.
        // Белая рокировка в короткую сторону.
        m1.append(6, 7);
    }

    if ((color == Color::WHITE) && /*(!attack[7][0]) && (!attack[7][1]) &&*/ (!attack[7][2]) && (!attack[7][3]) && (!attack[7][4]) &&
        (board[7][4] == ChessPiece::W_KING) && (board[7][0] == ChessPiece::W_ROOK) && (!WHITE_O_O_O) &&
        (board[7][1] == ChessPiece::EMPTY) && (board[7][2] == ChessPiece::EMPTY) && (board[7][3] == ChessPiece::EMPTY))
    {
        // Рокировка задаётся ходом Короля.
        // Белая рокировка в длинную сторону.
        m1.append(2, 7);
    }

    if ((color == Color::BLACK) && (!attack[0][4]) && (!attack[0][5]) && (!attack[0][6]) /* && (!attack[0][7])*/ &&
        (board[0][4] == ChessPiece::B_KING) && (board[0][7] == ChessPiece::B_ROOK) && (!BLACK_O_O) &&
        (board[0][5] == ChessPiece::EMPTY) && (board[0][6] == ChessPiece::EMPTY))
    {
        // Рокировка задаётся ходом Короля.
        // Чёрная рокировка в короткую сторону.
        m1.append(6, 0);
    }

    if ((color == Color::BLACK) && /*(!attack[0][0]) && (!attack[0][1]) &&*/ (!attack[0][2]) && (!attack[0][3]) && (!attack[0][4]) &&
        (board[0][4] == ChessPiece::B_KING) && (board[0][0] == ChessPiece::B_ROOK) && (!BLACK_O_O_O) &&
        (board[0][1] == ChessPiece::EMPTY) && (board[0][2] == ChessPiece::EMPTY) && (board[0][3] == ChessPiece::EMPTY))
    {
        // Рокировка задаётся ходом Короля.
        // Чёрная рокировка в длинную сторону.
        m1.append(2, 0);
    }

    return m1;
}


// 07.08.2022  - Ходы для пешек.
// 18.08.2022 Можно делать только те ходы пешкой чтобы король не был под шахом. Реализовано 18,08,2022
MOVES Pawn::get_moves(ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, PREVIOS_MOVE previos_moves,
    __int8 x, __int8 y, bool bterminal, bool WHITE_O_O, bool WHITE_O_O_O, bool BLACK_O_O, bool BLACK_O_O_O, bool ball)
{
    // ball все перемещения и взятия и простые продвижения вперед.


    
    MOVES m;  // (4);
    m.n = 0;

    // Пешка достигла конца доски и больше как пешка ходить не может.
    if ((y == 7) || (y == 0)) return m;

    if (ball) {

        // пешка ходит на один ход вперед если поле впереди свободно.
        if ((color == Color::BLACK) && (y < 7) && (get_color1(board, x, y + 1) == Color::EMPTY)) {

            if (bterminal) {
                m.append(x, y + 1);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = x;
                xy_to.y = y + 1;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }


        }
        // Со стартовой позиции пешка может пойти на две клетки если позии [x,y+1] и [x,y+2] не заняты.
        if ((color == Color::BLACK) && (y == 1) && (get_color1(board, x, y + 1) == Color::EMPTY) &&
            (get_color1(board, x, y + 2) == Color::EMPTY)) {

            if (bterminal) {
                m.append(x, y + 2);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = x;
                xy_to.y = y + 2;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }


        }

        // пешка ходит на один ход вперед если поле впереди свободно.
        if ((color == Color::WHITE) && (y > 0) && (get_color1(board, x, y - 1) == Color::EMPTY)) {

            if (bterminal) {
                m.append(x, y - 1);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = x;
                xy_to.y = y - 1;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }


        }

        // Со стартовой позиции пешка может пойти на две клетки если позии [x,y - 1] и [x,y - 2] не заняты.
        if ((color == Color::WHITE) && (y == 6) && (get_color1(board, x, y - 1) == Color::EMPTY) &&
            (get_color1(board, x, y - 2) == Color::EMPTY)) {

            if (bterminal) {
                m.append(x, y - 2);
            }
            else
            {

                MOVES_NODE xy_from;
                MOVES_NODE xy_to;
                xy_from.x = x;
                xy_from.y = y;
                xy_to.x = x;
                xy_to.y = y - 2;

                Board new_board;

                // Копирование доски.
                new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                // Предыдущий ход на этой новой  доске после сделанного хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

                append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            }



        }
    }
    // Взятие белой фигуры влево.
    if ((x > 0) && (color == Color::BLACK) && (get_color1(board, x - 1, y + 1) == Color::WHITE)) {

        if (bterminal) {
            m.append(x - 1, y + 1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = x - 1;
            xy_to.y = y + 1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }

    }

    // Взятие белой фигуры вправо.
    if ((x < 7) && (color == Color::BLACK) && (get_color1(board, x + 1, y + 1) == Color::WHITE)) {

        if (bterminal) {
            m.append(x + 1, y + 1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = x + 1;
            xy_to.y = y + 1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }

    }

    // Взятие чёрной фигуры влево.
    if ((x > 0) && (color == Color::WHITE) && (get_color1(board, x - 1, y - 1) == Color::BLACK)) {

        if (bterminal) {
            m.append(x - 1, y - 1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = x - 1;
            xy_to.y = y - 1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }

    }

    // Взятие чёрной фигуры вправо.
    if ((x < 7) && (color == Color::WHITE) && (get_color1(board, x + 1, y - 1) == Color::BLACK)) {

        if (bterminal) {
            m.append(x + 1, y - 1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = x + 1;
            xy_to.y = y - 1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }

    }

    
    // Взятие на проходе
    if ((y == 3) && (color == Color::WHITE) && (previos_moves.figure == ChessPiece::B_PAWN) && (previos_moves.xy_from.y == 1) &&
        (previos_moves.xy_to.y == 3)
        && (x-1>=0)&&(x - 1 == previos_moves.xy_from.x) && (x - 1 == previos_moves.xy_to.x)) {

        if (bterminal) {
            m.append(x - 1, y - 1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = x - 1;
            xy_to.y = y - 1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }
    }
    // Взятие на проходе
    if ((y == 3) && (color == Color::WHITE) && (previos_moves.figure == ChessPiece::B_PAWN) && (previos_moves.xy_from.y == 1) &&
        (previos_moves.xy_to.y == 3)
        && (x+1<=7) && (x + 1 == previos_moves.xy_from.x) && (x + 1 == previos_moves.xy_to.x)) {

        if (bterminal) {
            m.append(x + 1, y - 1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = x + 1;
            xy_to.y = y - 1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


        }

    }

    // Взятие на проходе
    if ((y == 4) && (color == Color::BLACK) && (previos_moves.figure == ChessPiece::W_PAWN) && (previos_moves.xy_from.y == 6) &&
        (previos_moves.xy_to.y == 4)
        && (x-1 >= 0) && (x - 1 == previos_moves.xy_from.x) && (x - 1 == previos_moves.xy_to.x)) {
        if (bterminal) {
            m.append(x - 1, y + 1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = x - 1;
            xy_to.y = y + 1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);           

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

        }
    }
    // Взятие на проходе
    if ((y == 4) && (color == Color::BLACK) && (previos_moves.figure == ChessPiece::W_PAWN) && (previos_moves.xy_from.y == 6) &&
        (previos_moves.xy_to.y == 4)
        && (x+1 <= 7) && (x + 1 == previos_moves.xy_from.x) && (x + 1 == previos_moves.xy_to.x)) {

        if (bterminal) {
            m.append(x + 1, y + 1);
        }
        else
        {

            MOVES_NODE xy_from;
            MOVES_NODE xy_to;
            xy_from.x = x;
            xy_from.y = y;
            xy_to.x = x + 1;
            xy_to.y = y + 1;

            Board new_board;

            // Копирование доски.
            new_board.copy(board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после сделанного хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = new_board.board[xy_to.y][xy_to.x];

            append_chess_move(color, new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, m, xy_to, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

        }
    }
    

    return m;
}

// Поля которыми белые фигуры атакуют черных фигур
void generate_mobility_WHITE(float attack_WHITE[8][8], ChessPiece board[8][8], MOVESL& wList, MOVESL& bList,
    PREVIOS_MOVE previos_moves,
    bool WHITE_O_O, bool WHITE_O_O_O,
    bool BLACK_O_O, bool BLACK_O_O_O);


// Поля которыми чёрные фигуры атакуют белых фигур
void generate_mobility_BLACK(float attack_BLACK[8][8], ChessPiece board[8][8], MOVESL& wList, MOVESL& bList,
    PREVIOS_MOVE previos_moves,
    bool WHITE_O_O, bool WHITE_O_O_O,
    bool BLACK_O_O, bool BLACK_O_O_O);


// Для верних Чёрных.
float upSt[8][8] = {
    {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f},
    {0.9f,1.0f,1.1f,1.2f,1.3f,1.4f,1.5f,1.6f},
    {1.7f,1.8f,1.9f,2.0f,2.1f,2.2f,2.3f,2.4f},
    {2.5f,2.6f,2.7f,2.8f,2.9f,3.0f,3.1f,3.2f},
    {3.3f,3.4f,3.5f,3.6f,3.7f,3.8f,3.9f,4.0f},
    {4.1f,4.2f,4.3f,4.4f,4.5f,4.6f,4.7f,4.8f},
    {4.9f,5.0f,5.1f,5.2f,5.3f,5.4f,5.5f,5.6f},
    {5.7f,5.8f, 5.9f,6.0f,6.1f,6.2f,6.3f,6.4f}
};

// Для нижних: (белых)
float botSt[8][8] =
{
    {6.4f, 6.3f, 6.2f, 6.1f, 6.0f, 5.9f, 5.8f, 5.7f},
    {5.6f, 5.5f, 5.4f, 5.3f, 5.2f, 5.1f, 5.0f, 4.9f},
    {4.8f,4.7f, 4.6f, 4.5f, 4.4f, 4.3f, 4.2f, 4.1f},
    {4.0f,3.9f, 3.8f, 3.7f,3.6f,3.5f,3.4f,3.3f},
    {3.2f, 3.1f, 3.0f, 2.9f, 2.8f,2.7f,2.6f,2.5f},
    {2.4f, 2.3f,2.2f,2.1f,2.0f,1.9f,1.8f, 1.7f},
    {1.6f, 1.5f, 1.4f,1.3f,1.2f,1.1f, 1.0f, 0.9f},
    {0.8f, 0.7f, 0.6f, 0.5f, 0.4f, 0.3f, 0.2f, 0.1f}
};

// Когда нет пешек для всех фигур обеих сторон используется одна общая оценка с приоритетом центра.
float endGSt[8][8] =
{
    {0.1f, 0.2f, 0.3f, 0.4f, 0.4f, 0.3f, 0.2f, 0.1f},
    {0.9f, 1.0f, 1.1f, 1.2f, 1.2f, 1.1f, 1.0f, 0.9f},
    {1.7f, 1.8f, 1.9f, 2.0f, 2.0f, 1.9f, 1.8f, 1.7f},
    {2.5f, 2.6f, 2.7f, 2.8f, 2.8f, 2.7f, 2.6f, 2.5f},
    {2.5f, 2.6f, 2.7f, 2.8f, 2.8f, 2.7f, 2.6f, 2.5f},
    {1.7f, 1.8f, 1.9f, 2.0f, 2.0f, 1.9f, 1.8f, 1.7f},
    {0.9f, 1.0f, 1.1f, 1.2f, 1.2f, 1.1f, 1.0f, 0.9f},
    {0.1f, 0.2f, 0.3f, 0.4f, 0.4f, 0.3f, 0.2f, 0.1f}
};

// Оценка позиции для белых или чёрных на доске.
float Board::rate(Color color) 
{

    float res = 0.0;
    

    if (0) {
        // Простая оценочная функция, книга Евгения Корнилова.
        // Простая оценочная функция хоть и затрачивает на себя меньше времени,
        // но замедляет Альфа-Бета алгоритм т.к. отсечения работают очень плохо и алгоритм вынужден 
        // просматривать больше узлов и медленней работать. ???
        
        bool bpawn = false;

        if (color == Color::BLACK)
        {
            for (__int8 k = 0; k < bList.n; ++k) {

                __int8 y = bList.moves[k].y;
                __int8 x = bList.moves[k].x;

                if (board[y][x] == ChessPiece::B_PAWN) {
                    Pawn p = Pawn(Color::BLACK);
                    //res += p.rate(board, x, y);
                    res += p.VALUE;
                    bpawn=true;
                }
                else if (board[y][x] == ChessPiece::B_KING) {
                    King k = King(Color::BLACK);
                    //res += k.rate(board, wList, bList, x, y);
                    res += k.VALUE;
                }
                else if (board[y][x] == ChessPiece::B_QUEEN) {
                    Queen q = Queen(Color::BLACK);
                    //res += q.rate(board, x, y);
                    res += q.VALUE;
                }
                else if (board[y][x] == ChessPiece::B_BISHOP) {
                    Bishop bishop = Bishop(Color::BLACK);
                    //res += bishop.rate(board, x, y);
                    res += bishop.VALUE;
                }
                else if (board[y][x] == ChessPiece::B_ROOK) {
                    Rook rook = Rook(Color::BLACK);
                    //res += rook.rate(board, x, y);
                    res += rook.VALUE;
                }
                else if (board[y][x] == ChessPiece::B_KNIGHT) {
                    Knight kn = Knight(Color::BLACK);
                    //res += kn.rate(board, x, y);
                    res += kn.VALUE;
                }
            }

            if (!bpawn) {
                for (__int8 k = 0; k < wList.n; ++k) {

                    __int8 y = wList.moves[k].y;
                    __int8 x = wList.moves[k].x;

                    if (board[y][x] == ChessPiece::W_PAWN) {
                        bpawn = true;
                        break;
                    }
                }
            }

            if (!bpawn) {
                for (__int8 k = 0; k < bList.n; ++k) {

                    __int8 y = bList.moves[k].y;
                    __int8 x = bList.moves[k].x;

                    res += endGSt[y][x];
                }
            }
            else {
                for (__int8 k = 0; k < bList.n; ++k) {

                    __int8 y = bList.moves[k].y;
                    __int8 x = bList.moves[k].x;

                    res += upSt[y][x];
                }
            }
        }
   
        if (color == Color::WHITE)
        {
            for (__int8 k = 0; k < wList.n; ++k) {

                __int8 y = wList.moves[k].y;
                __int8 x = wList.moves[k].x;


                if (board[y][x] == ChessPiece::W_PAWN) {
                    Pawn p = Pawn(Color::WHITE);
                    //res += p.rate(board, x, y);
                    res += p.VALUE;
                    bpawn = true;
                }
                else if (board[y][x] == ChessPiece::W_KING) {
                    King k = King(Color::WHITE);
                    //res += k.rate(board, wList, bList, x, y);
                    res += k.VALUE;
                }
                else if (board[y][x] == ChessPiece::W_QUEEN) {
                    Queen q = Queen(Color::WHITE);
                    //res += q.rate(board, x, y);
                    res += q.VALUE;
                }
                else if (board[y][x] == ChessPiece::W_BISHOP) {
                    Bishop bishop = Bishop(Color::WHITE);
                    //res += bishop.rate(board, x, y);
                    res += bishop.VALUE;
                }
                else if (board[y][x] == ChessPiece::W_ROOK) {
                    Rook rook = Rook(Color::WHITE);
                    //res += rook.rate(board, x, y);
                    res += rook.VALUE;
                }
                else if (board[y][x] == ChessPiece::W_KNIGHT) {
                    Knight kn = Knight(Color::WHITE);
                    //res += kn.rate(board, x, y);
                    res += kn.VALUE;
                }

                if (!bpawn) {
                    for (__int8 k = 0; k < bList.n; ++k) {

                        __int8 y = bList.moves[k].y;
                        __int8 x = bList.moves[k].x;

                        if (board[y][x] == ChessPiece::B_PAWN) {
                            bpawn = true;
                            break;
                        }
                    }
                }

                if (!bpawn) {
                    for (__int8 k = 0; k < wList.n; ++k) {

                        __int8 y = wList.moves[k].y;
                        __int8 x = wList.moves[k].x;

                        res += endGSt[y][x];
                    }
                }
                else {
                    for (__int8 k = 0; k < wList.n; ++k) {

                        __int8 y = wList.moves[k].y;
                        __int8 x = wList.moves[k].x;

                        res += botSt[y][x];
                    }
                }
                
            }

        }
    
       

    }
    else {

        __int8 pawn_x_position[8];
        for (__int8 x = 0; x < 8; ++x)
        {
            pawn_x_position[x] = 0;
        }

        // Проходим последовательно по всем клеткам доски
        // и к общей оценке добавляем оценку конкретных фигур.
        // у каждой фигуры в классе фигуры есть свой метод оценки. 



        //for (__int8 y = 0; y < 8; ++y) {
          //  for (__int8 x = 0; x < 8; ++x) {
            //    if (get_color1(board, x, y) != color)
              //      continue;

        __int8 two_Bishop = 0;
        if (color == Color::BLACK)
        {
#ifdef MOBILITY_USER_SETTING
            /*__int8 mobility = 0; // мобильность.

            bool attack_BLACK[8][8];
            for (__int8 x = 0; x < 8; ++x)
            {
                for (__int8 y = 0; y < 8; ++y)
                {
                    attack_BLACK[y][x] = false;
                }
            }

            generate_attack_BLACK(attack_BLACK, board, wList, bList, previos_moves, WHITE_O_O,  WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);


            for (__int8 x = 0; x < 8; ++x)
            {
                for (__int8 y = 0; y < 8; ++y)
                {
                    if (attack_BLACK[y][x]) {
                        ++mobility;
                    }
                }
            }
            res+= ((1.0f* mobility)/32.0f)* 10.0f * 0.4f; // Бонус за активность фигур на доске.
            */

            // Мобильность с учётом весов фигур и своенных линий и диагоналей.
            float attack_BLACK[8][8];
            for (__int8 x = 0; x < 8; ++x)
            {
                for (__int8 y = 0; y < 8; ++y)
                {
                    attack_BLACK[y][x] = 0.0f;
                }
            }
            generate_mobility_BLACK(attack_BLACK, board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            for (__int8 x = 0; x < 8; ++x)
            {
                for (__int8 y = 0; y < 8; ++y)
                {
                    res += attack_BLACK[y][x]/ static_calibrate;
                }
            }
#endif

            //float phase = calculation_phase_for_conic_rate(board, wList, bList); // фаза коническая оценка.
            //if ((phase < KING_CENTER_PRIORITY_WEIGHT) && (BLACK_O_O || BLACK_O_O_O)) {
                // Начальная стадия партии и рокировка уже сделана.
              //  res += 3.0f; // Бонус за сделанную рокировку и безопасность короля.
            //}

            for (__int8 k = 0; k < bList.n; ++k) {

                __int8 y = bList.moves[k].y;
                __int8 x = bList.moves[k].x;

                if (board[y][x] == ChessPiece::B_PAWN) {
                    Pawn p = Pawn(Color::BLACK);
                    res += p.rate(board, x, y);
                    pawn_x_position[x]++;
                }
                else if (board[y][x] == ChessPiece::B_KING) {
                    King k = King(Color::BLACK);
                    res += k.rate(board, wList, bList, x, y);
                }
                else if (board[y][x] == ChessPiece::B_QUEEN) {
                    Queen q = Queen(Color::BLACK);
                    res += q.rate(board, x, y);
                }
                else if (board[y][x] == ChessPiece::B_BISHOP) {
                    Bishop bishop = Bishop(Color::BLACK);
                    res += bishop.rate(board, x, y);
                    ++two_Bishop;
                }
                else if (board[y][x] == ChessPiece::B_ROOK) {
                    Rook rook = Rook(Color::BLACK);
                    res += rook.rate(board, x, y);
                }
                else if (board[y][x] == ChessPiece::B_KNIGHT) {
                    Knight kn = Knight(Color::BLACK);
                    res += kn.rate(board, x, y);
                }
            }



            // Преимущество двух слонов. Треть пешки.
            if (two_Bishop == 2) res += 10.0f * 0.33f / static_calibrate;

            /*for (__int8 k = 0; k < bList.n; ++k) {

                // Цена большая больше пол пешки, не противоречит ли это тому что позиционная оценка всегда меньше материальной.

                __int8 y = bList.moves[k].y;
                __int8 x = bList.moves[k].x;

                if (board[y][x] == ChessPiece::B_PAWN) {
                    // Вилка пешкой на фигуру 24.09.2023
                    if ((x >= 1) && (x <= 6)) {
                        if ((board[y + 1][x - 1] == ChessPiece::W_KNIGHT) && (board[y + 1][x + 1] == ChessPiece::W_KNIGHT)) {
                            // Вилка пешкой на двух коней.
                            Knight kn = Knight(Color::BLACK);
                            res += kn.VALUE;
                        }
                        else if ((board[y + 1][x - 1] == ChessPiece::W_ROOK) && (board[y + 1][x + 1] == ChessPiece::W_ROOK)) {
                            // Вилка пешкой на двух ладей.
                            Rook rook = Rook(Color::BLACK);
                            res += rook.VALUE;
                        }
                        // Если вилка на коня и слона то слон нападает на пешку и нужно ещё проверить защищена ли пешка.
                        // Аналогично с Ферзём нужно проверить защищена ли пешка. И вообще говоря сколько раз защищена.

                        // Также смотри. Сделана вилка на две ладьи а следующим ходом сьедается пешка делающая вилку например королем.
                        // В общем эти вопросы не стат оценки а анализа вглубину по дереву скорее.
                    }
                }
            }*/
            
        }

        



        if (color == Color::WHITE)
        {
#ifdef MOBILITY_USER_SETTING
            /* __int8 mobility = 0;

             bool attack_WHITE[8][8];
             for (__int8 x = 0; x < 8; ++x)
             {
                 for (__int8 y = 0; y < 8; ++y)
                 {
                     attack_WHITE[y][x] = false;
                 }
             }

             generate_attack_WHITE(attack_WHITE, board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

             for (__int8 x = 0; x < 8; ++x)
             {
                 for (__int8 y = 0; y < 8; ++y)
                 {
                     if (attack_WHITE[y][x]) {
                         ++mobility;
                     }
                 }
             }

             res += ((1.0f* mobility) / 32.0f) * 10.0f * 0.4f; //  Бонус за активность фигур на доске.
             */

             // Мобильность с учётом весов фигур и своенных линий и диагоналей.
            float attack_WHITE[8][8];
            for (__int8 x = 0; x < 8; ++x)
            {
                for (__int8 y = 0; y < 8; ++y)
                {
                    attack_WHITE[y][x] = 0.0f;
                }
            }

            generate_mobility_WHITE(attack_WHITE, board, wList, bList, previos_moves, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            for (__int8 x = 0; x < 8; ++x)
            {
                for (__int8 y = 0; y < 8; ++y)
                {
                    res += attack_WHITE[y][x] / static_calibrate;
                }
            }

#endif

            //float phase = calculation_phase_for_conic_rate(board, wList, bList); // фаза коническая оценка.
            //if ((phase < KING_CENTER_PRIORITY_WEIGHT) && (WHITE_O_O || WHITE_O_O_O)) {
                // Начальная стадия партии и рокировка уже сделана.
              //  res += 3.0f; // Бонус за сделанную рокировку и безопасность короля.
            //}

            two_Bishop = 0;

            for (__int8 k = 0; k < wList.n; ++k) {

                __int8 y = wList.moves[k].y;
                __int8 x = wList.moves[k].x;


                if (board[y][x] == ChessPiece::W_PAWN) {
                    Pawn p = Pawn(Color::WHITE);
                    res += p.rate(board, x, y);
                    pawn_x_position[x]++;
                }
                else if (board[y][x] == ChessPiece::W_KING) {
                    King k = King(Color::WHITE);
                    res += k.rate(board, wList, bList, x, y);
                }
                else if (board[y][x] == ChessPiece::W_QUEEN) {
                    Queen q = Queen(Color::WHITE);
                    res += q.rate(board, x, y);
                }
                else if (board[y][x] == ChessPiece::W_BISHOP) {
                    Bishop bishop = Bishop(Color::WHITE);
                    res += bishop.rate(board, x, y);
                    ++two_Bishop;
                }
                else if (board[y][x] == ChessPiece::W_ROOK) {
                    Rook rook = Rook(Color::WHITE);
                    res += rook.rate(board, x, y);
                }
                else if (board[y][x] == ChessPiece::W_KNIGHT) {
                    Knight kn = Knight(Color::WHITE);
                    res += kn.rate(board, x, y);
                }
            }



            // Преимущество двух слонов. Треть пешки.
            if (two_Bishop == 2) res += 10.0f * 0.33f / static_calibrate;

        }

        /*
        1. пара слонов – это плюс треть пешки,
        2. король прикрыт своими пешками – плюс пол-пешки,
        3. конь или ферзь близки к королю (своему или чужому) – плюс сотая пешки,
        4. слабая или отставшая пешка – минус четверть пешки,
        5. ладья на полуоткрытой вертикали – плюс пять сотых пешки,
        6. ладья на открытой вертикали – плюс одна десятая пешки,
        7. пара ладей на седьмой горизонтали – плюс пол-пешки,
        8. висящая фигура – минус четверть пешки, две висящих фигуры – минус пешка, три или больше – минус три пешки.

        */
        //}
    //}

    //for (__int8 x = 0; x < 8; ++x) {
//#pragma omp parallel for
  //      for (int tid = 0; tid < 8; ++tid) {
    //        pawn_x_position[x] += pawn_x_position_omp[tid][x];
      //  }
    //}

    // Сдвоенные пешки уменьшают оценку позиции.
    // количество пешек
        __int8 ipawn = 0;
        // количество вертикалей в которых есть хоть одна пешка.
        __int8 iverticalpawn = 0;
        for (__int8 x = 0; x < 8; ++x) {
            ipawn += pawn_x_position[x];
            if (pawn_x_position[x] > 0) {
                iverticalpawn++;
            }
        }
        // Количество сдвоенных пешек есть количество пешек минус количество вертикалей с пешками.
        __int8 ipawn2 = ipawn - iverticalpawn;
        res -= 2.5f * (ipawn2) / static_calibrate; // для каждой сдвоенной пешки вычитаем два с половиной балла из оценки.

        // одинокая пешка снижает оценку позиции
        for (__int8 i = 1; i <= 6; ++i) {

            if ((pawn_x_position[i] > 0) && (pawn_x_position[i - 1] == 0) && (pawn_x_position[i + 1] == 0)) {
                res -= 2.0f / static_calibrate;
            }
        }
    }
    return res;
}

// По передаваемому цвету color возвращает цвет врага.
Color Color_invert(Color color)
{    
        return (color == Color::WHITE ? Color::BLACK : Color::WHITE);
}


// Поля которыми чёрные фигуры атакуют белых фигур
void generate_mobility_BLACK(float attack_BLACK[8][8], ChessPiece board[8][8], MOVESL& wList, MOVESL& bList,
    PREVIOS_MOVE previos_moves,
    bool WHITE_O_O, bool WHITE_O_O_O,
    bool BLACK_O_O, bool BLACK_O_O_O)
{
    for (__int8 k = 0; k < bList.n; ++k) {

        __int8 j = bList.moves[k].y;
        __int8 i = bList.moves[k].x;

       /* if (board[j][i] == ChessPiece::B_KING) {
            // Поля на которые атакует чёрный король.
            if (j - 1 >= 0)  if (get_color1(board, i, j - 1) != Color::BLACK) attack_BLACK[j - 1][i] = true;
            if (j + 1 <= 7)  if (get_color1(board, i, j + 1) != Color::BLACK) attack_BLACK[j + 1][i] = true;
            if (i - 1 >= 0) if (get_color1(board, i - 1, j) != Color::BLACK) attack_BLACK[j][i - 1] = true;
            if (i + 1 <= 7) if (get_color1(board, i + 1, j) != Color::BLACK) attack_BLACK[j][i + 1] = true;
            if ((j - 1 >= 0) && (i - 1 >= 0)) if (get_color1(board, i - 1, j - 1) != Color::BLACK) attack_BLACK[j - 1][i - 1] = true;
            if ((j + 1 <= 7) && (i + 1 <= 7)) if (get_color1(board, i + 1, j + 1) != Color::BLACK) attack_BLACK[j + 1][i + 1] = true;
            if ((j + 1 <= 7) && (i - 1 >= 0)) if (get_color1(board, i - 1, j + 1) != Color::BLACK) attack_BLACK[j + 1][i - 1] = true;
            if ((j - 1 >= 0) && (i + 1 <= 7)) if (get_color1(board, i + 1, j - 1) != Color::BLACK) attack_BLACK[j - 1][i + 1] = true;
        }
        else if (board[j][i] == ChessPiece::B_PAWN) {
            // Пешка нападает наискосок.
            if ((j + 1 <= 7) && (i - 1 >= 0)) if (get_color1(board, i - 1, j + 1) != Color::BLACK)  attack_BLACK[j + 1][i - 1] = true;
            if ((j + 1 <= 7) && (i + 1 <= 7)) if (get_color1(board, i + 1, j + 1) != Color::BLACK) attack_BLACK[j + 1][i + 1] = true;
        }
        else*/
        
        // C Королем уже было учтено где VALUE. Два раза учитывать неправильно. 2,августа,2023
        if (0&&(flag_POSICIONING_GAME) && (calculation_phase_for_conic_rate((board), wList, bList) <= POSICIONING_GAME_POROG))
        {
            if (board[j][i] == ChessPiece::B_KING) {
                // Отрицательная мобильность короля в начале партии (штраф за мобильность).

                // Поля на которые атакует чёрный король.
                if (j - 1 >= 0)  if (get_color1(board, i, j - 1) != Color::BLACK) attack_BLACK[j - 1][i] -= 0.9f;
                if (j + 1 <= 7)  if (get_color1(board, i, j + 1) != Color::BLACK) attack_BLACK[j + 1][i] -= 0.9f;
                if (i - 1 >= 0) if (get_color1(board, i - 1, j) != Color::BLACK) attack_BLACK[j][i - 1] -= 0.9f;
                if (i + 1 <= 7) if (get_color1(board, i + 1, j) != Color::BLACK) attack_BLACK[j][i + 1] -= 0.9f;
                if ((j - 1 >= 0) && (i - 1 >= 0)) if (get_color1(board, i - 1, j - 1) != Color::BLACK) attack_BLACK[j - 1][i - 1] -= 0.9f;
                if ((j + 1 <= 7) && (i + 1 <= 7)) if (get_color1(board, i + 1, j + 1) != Color::BLACK) attack_BLACK[j + 1][i + 1] -= 0.9f;
                if ((j + 1 <= 7) && (i - 1 >= 0)) if (get_color1(board, i - 1, j + 1) != Color::BLACK) attack_BLACK[j + 1][i - 1] -= 0.9f;
                if ((j - 1 >= 0) && (i + 1 <= 7)) if (get_color1(board, i + 1, j - 1) != Color::BLACK) attack_BLACK[j - 1][i + 1] -= 0.9f;
            }
        }
        
        
        if (board[j][i] == ChessPiece::B_ROOK) {
            {
                // Только если Ладья стоит на одной линии с Королём.

                MOVES m; // (14);
                m.n = 0;
                Rook rook;
                rook.color = Color::BLACK;
                m = rook.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                    attack_BLACK[m.moves[k].y][m.moves[k].x] += (attack_BLACK[m.moves[k].y][m.moves[k].x] > 0.1f ? 0.5f : 0.3f); // Сдвоенная ладья 0,2 бонус
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_QUEEN) {

            

            if ((!flag_POSICIONING_GAME)||(calculation_phase_for_conic_rate((board), wList, bList)> POSICIONING_GAME_POROG))
            {
                // Ферзь на одной вертикали или горизонтали или диагонали с Королём.

                MOVES m;
                m.n = 0;
                Queen queen;
                queen.color = Color::BLACK;
                m = queen.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                    attack_BLACK[m.moves[k].y][m.moves[k].x] += (attack_BLACK[m.moves[k].y][m.moves[k].x] > 0.1f ?0.5f : 0.3f); // Ферзь сдвоен с ладьёй или слоном.
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_BISHOP) {


            {
                // Если Король на одной диагонали со слоном.

                MOVES m; // (14);
                m.n = 0;
                Bishop bishop;
                bishop.color = Color::BLACK;
                m = bishop.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                    attack_BLACK[m.moves[k].y][m.moves[k].x] += (attack_BLACK[m.moves[k].y][m.moves[k].x] > 0.1f ?  0.6f : 0.4f); // Ферзь сдвоен со слоном
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_KNIGHT) {
            MOVES m; // (8);
            m.n = 0;
            Knight knight;
            knight.color = Color::BLACK;
            m = knight.get_moves_terminal((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            for (__int8 k = 0; k < m.n; ++k) {
                // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                if (get_color1(board, m.moves[k].x, m.moves[k].y) != Color::BLACK)
                {
                    attack_BLACK[m.moves[k].y][m.moves[k].x] += 0.9f;
                }
            }
        }
    }
}

// Поля которыми белые фигуры атакуют черных фигур
void generate_mobility_WHITE(float attack_WHITE[8][8], ChessPiece board[8][8], MOVESL& wList, MOVESL& bList,
    PREVIOS_MOVE previos_moves,
    bool WHITE_O_O, bool WHITE_O_O_O,
    bool BLACK_O_O, bool BLACK_O_O_O)
{
    for (__int8 k = 0; k < wList.n; ++k) {

        __int8 j = wList.moves[k].y;
        __int8 i = wList.moves[k].x;

        /*if (board[j][i] == ChessPiece::W_KING) {
            // Поля на которые атакует белый король.
            if (j - 1 >= 0) if (get_color1(board, i, j - 1) != Color::WHITE) attack_WHITE[j - 1][i] = true;
            if (j + 1 <= 7) if (get_color1(board, i, j + 1) != Color::WHITE) attack_WHITE[j + 1][i] = true;
            if (i - 1 >= 0) if (get_color1(board, i - 1, j) != Color::WHITE) attack_WHITE[j][i - 1] = true;
            if (i + 1 <= 7) if (get_color1(board, i + 1, j) != Color::WHITE) attack_WHITE[j][i + 1] = true;
            if ((j - 1 >= 0) && (i - 1 >= 0)) if (get_color1(board, i - 1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i - 1] = true;
            if ((j + 1 <= 7) && (i + 1 <= 7)) if (get_color1(board, i + 1, j + 1) != Color::WHITE) attack_WHITE[j + 1][i + 1] = true;
            if ((j + 1 <= 7) && (i - 1 >= 0)) if (get_color1(board, i - 1, j + 1) != Color::WHITE) attack_WHITE[j + 1][i - 1] = true;
            if ((j - 1 >= 0) && (i + 1 <= 7)) if (get_color1(board, i + 1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i + 1] = true;
        }
        else if (board[j][i] == ChessPiece::W_PAWN) {
            // Пешка нападает наискосок.
            if ((j - 1 >= 0) && (i - 1 >= 0)) if (get_color1(board, i - 1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i - 1] = true;
            if ((j - 1 >= 0) && (i + 1 <= 7)) if (get_color1(board, i + 1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i + 1] = true;
        }
        else*/ 
        
        // C Королем уже было учтено где VALUE. Два раза учитывать неправильно. 2,августа,2023
        if (0&&(flag_POSICIONING_GAME) && (calculation_phase_for_conic_rate((board), wList, bList) <= POSICIONING_GAME_POROG))
        {
            // Отрицательная мобильность короля в начале партии (штраф за мобильность).

            if (board[j][i] == ChessPiece::W_KING) {
                // Поля на которые атакует белый король.
                if (j - 1 >= 0) if (get_color1(board, i, j - 1) != Color::WHITE) attack_WHITE[j - 1][i] -= 0.9f;
                if (j + 1 <= 7) if (get_color1(board, i, j + 1) != Color::WHITE) attack_WHITE[j + 1][i] -= 0.9f;
                if (i - 1 >= 0) if (get_color1(board, i - 1, j) != Color::WHITE) attack_WHITE[j][i - 1] -= 0.9f;
                if (i + 1 <= 7) if (get_color1(board, i + 1, j) != Color::WHITE) attack_WHITE[j][i + 1] -= 0.9f;
                if ((j - 1 >= 0) && (i - 1 >= 0)) if (get_color1(board, i - 1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i - 1] -= 0.9f;
                if ((j + 1 <= 7) && (i + 1 <= 7)) if (get_color1(board, i + 1, j + 1) != Color::WHITE) attack_WHITE[j + 1][i + 1] -= 0.9f;
                if ((j + 1 <= 7) && (i - 1 >= 0)) if (get_color1(board, i - 1, j + 1) != Color::WHITE) attack_WHITE[j + 1][i - 1] -= 0.9f;
                if ((j - 1 >= 0) && (i + 1 <= 7)) if (get_color1(board, i + 1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i + 1] -= 0.9f;
            }
        }
        
        if (board[j][i] == ChessPiece::W_ROOK) {


            {
                // Только если Ладья стоит на одной линии с Королём.

                MOVES m; // (14);
                m.n = 0;
                Rook rook;
                rook.color = Color::WHITE;
                m = rook.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                    attack_WHITE[m.moves[k].y][m.moves[k].x] += (attack_WHITE[m.moves[k].y][m.moves[k].x]>0.1f ? 0.5f : 0.3f); // Сдвоенная ладья 0.2f бонус
                }
            }
        }
        else if (board[j][i] == ChessPiece::W_QUEEN) {

            if ((!flag_POSICIONING_GAME) || (calculation_phase_for_conic_rate((board), wList, bList) > POSICIONING_GAME_POROG))
            {
                // Ферзь на одной вертикали или горизонтали или диагонали с Королём.

                MOVES m;
                m.n = 0;
                Queen queen;
                queen.color = Color::WHITE;
                m = queen.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                    attack_WHITE[m.moves[k].y][m.moves[k].x] += (attack_WHITE[m.moves[k].y][m.moves[k].x]>0.1f ? 0.5f : 0.3f); // Ферзь сдвоенный с ладьёй или слоном бонус 0,2а
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_BISHOP) {


            {
                // Если Король на одной диагонали со слоном.

                MOVES m; // (14);
                m.n = 0;
                Bishop bishop;
                bishop.color = Color::WHITE;
                m = bishop.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                    attack_WHITE[m.moves[k].y][m.moves[k].x] += (attack_WHITE[m.moves[k].y][m.moves[k].x]>0.1f ? 0.6f : 0.4f); // Слон сдвоенный с ферзём бонус 0,2f
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_KNIGHT) {
            MOVES m; // (8);
            m.n = 0;
            Knight knight;
            knight.color = Color::WHITE;
            m = knight.get_moves_terminal((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            for (__int8 k = 0; k < m.n; ++k) {
                // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                if (get_color1(board, m.moves[k].x, m.moves[k].y) != Color::WHITE) {
                    attack_WHITE[m.moves[k].y][m.moves[k].x] += 0.9f;
                }
            }
        }
    }
}

// Поля которыми чёрные фигуры атакуют белых фигур
void generate_attack_BLACK(bool attack_BLACK[8][8], ChessPiece board[8][8], MOVESL& wList, MOVESL& bList,
    PREVIOS_MOVE previos_moves,
    bool WHITE_O_O, bool WHITE_O_O_O,
    bool BLACK_O_O, bool BLACK_O_O_O)
{
    for (__int8 k = 0; k < bList.n; ++k) {

        __int8 j = bList.moves[k].y;
        __int8 i = bList.moves[k].x;

        if (board[j][i] == ChessPiece::B_KING) {
            // Поля на которые атакует чёрный король.
            if (j - 1 >= 0)  if (get_color1(board, i, j-1)!= Color::BLACK) attack_BLACK[j - 1][i] = true;
            if (j + 1 <= 7)  if (get_color1(board, i, j + 1) != Color::BLACK) attack_BLACK[j + 1][i] = true;
            if (i - 1 >= 0) if (get_color1(board, i-1, j) != Color::BLACK) attack_BLACK[j][i - 1] = true;
            if (i + 1 <= 7) if (get_color1(board, i+1, j ) != Color::BLACK) attack_BLACK[j][i + 1] = true;
            if ((j - 1 >= 0) && (i - 1 >= 0)) if (get_color1(board, i-1, j - 1) != Color::BLACK) attack_BLACK[j - 1][i - 1] = true;
            if ((j + 1 <= 7) && (i + 1 <= 7)) if (get_color1(board, i+1, j + 1) != Color::BLACK) attack_BLACK[j + 1][i + 1] = true;
            if ((j + 1 <= 7) && (i - 1 >= 0)) if (get_color1(board, i-1, j + 1) != Color::BLACK) attack_BLACK[j + 1][i - 1] = true;
            if ((j - 1 >= 0) && (i + 1 <= 7)) if (get_color1(board, i+1, j - 1) != Color::BLACK) attack_BLACK[j - 1][i + 1] = true;
        }
        else if (board[j][i] == ChessPiece::B_PAWN) {
            // Пешка нападает наискосок.
            if ((j + 1 <= 7) && (i - 1 >= 0)) if (get_color1(board, i - 1, j + 1) != Color::BLACK)  attack_BLACK[j + 1][i - 1] = true;
            if ((j + 1 <= 7) && (i + 1 <= 7)) if (get_color1(board, i + 1, j + 1) != Color::BLACK) attack_BLACK[j + 1][i + 1] = true;
        }
        else if (board[j][i] == ChessPiece::B_ROOK) {             
            {
                // Только если Ладья стоит на одной линии с Королём.

                MOVES m; // (14);
                m.n = 0;
                Rook rook;
                rook.color = Color::BLACK;
                m = rook.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                    attack_BLACK[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_QUEEN) {

            
            {
                // Ферзь на одной вертикали или горизонтали или диагонали с Королём.

                MOVES m;
                m.n = 0;
                Queen queen;
                queen.color = Color::BLACK;
                m = queen.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                    attack_BLACK[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_BISHOP) {

            
            {
                // Если Король на одной диагонали со слоном.

                MOVES m; // (14);
                m.n = 0;
                Bishop bishop;
                bishop.color = Color::BLACK;
                m = bishop.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                    attack_BLACK[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_KNIGHT) {
            MOVES m; // (8);
            m.n = 0;
            Knight knight;
            knight.color = Color::BLACK;
            m = knight.get_moves_terminal((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            for (__int8 k = 0; k < m.n; ++k) {
                // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                if (get_color1(board, m.moves[k].x, m.moves[k].y) != Color::BLACK)
                {
                    attack_BLACK[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
    }
}

// Поля которыми белые фигуры атакуют черных фигур
void generate_attack_WHITE(bool attack_WHITE[8][8], ChessPiece board[8][8], MOVESL& wList, MOVESL& bList,
    PREVIOS_MOVE previos_moves, 
    bool WHITE_O_O, bool WHITE_O_O_O,
    bool BLACK_O_O, bool BLACK_O_O_O)
{
    for (__int8 k = 0; k < wList.n; ++k) {

        __int8 j = wList.moves[k].y;
        __int8 i = wList.moves[k].x;

        if (board[j][i] == ChessPiece::W_KING) {
            // Поля на которые атакует чёрный король.
            if (j - 1 >= 0) if (get_color1(board, i, j - 1) != Color::WHITE) attack_WHITE[j - 1][i] = true;
            if (j + 1 <= 7) if (get_color1(board, i, j + 1) != Color::WHITE) attack_WHITE[j + 1][i] = true;
            if (i - 1 >= 0) if (get_color1(board, i-1, j ) != Color::WHITE) attack_WHITE[j][i - 1] = true;
            if (i + 1 <= 7) if (get_color1(board, i+1, j ) != Color::WHITE) attack_WHITE[j][i + 1] = true;
            if ((j - 1 >= 0) && (i - 1 >= 0)) if (get_color1(board, i-1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i - 1] = true;
            if ((j + 1 <= 7) && (i + 1 <= 7)) if (get_color1(board, i+1, j + 1) != Color::WHITE) attack_WHITE[j + 1][i + 1] = true;
            if ((j + 1 <= 7) && (i - 1 >= 0)) if (get_color1(board, i-1, j + 1) != Color::WHITE) attack_WHITE[j + 1][i - 1] = true;
            if ((j - 1 >= 0) && (i + 1 <= 7)) if (get_color1(board, i+1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i + 1] = true;
        }
        else if (board[j][i] == ChessPiece::W_PAWN) {
            // Пешка нападает наискосок.
            if ((j - 1 >= 0) && (i - 1 >= 0)) if (get_color1(board, i-1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i - 1] = true;
            if ((j - 1 >= 0) && (i + 1 <= 7)) if (get_color1(board, i+1, j - 1) != Color::WHITE) attack_WHITE[j - 1][i + 1] = true;
        }
        else if (board[j][i] == ChessPiece::W_ROOK) {


            {
                // Только если Ладья стоит на одной линии с Королём.

                MOVES m; // (14);
                m.n = 0;
                Rook rook;
                rook.color = Color::WHITE;
                m = rook.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует одна из вражеских ладей.
                    attack_WHITE[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
        else if (board[j][i] == ChessPiece::W_QUEEN) {


            {
                // Ферзь на одной вертикали или горизонтали или диагонали с Королём.

                MOVES m;
                m.n = 0;
                Queen queen;
                queen.color = Color::WHITE;
                m = queen.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует вражеский ферзь.
                    attack_WHITE[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_BISHOP) {


            {
                // Если Король на одной диагонали со слоном.

                MOVES m; // (14);
                m.n = 0;
                Bishop bishop;
                bishop.color = Color::WHITE;
                m = bishop.get_moves((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

                for (__int8 k = 0; k < m.n; ++k) {
                    // Поля куда ходить королём нельзя потому что атакует один из вражеских слонов.
                    attack_WHITE[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
        else if (board[j][i] == ChessPiece::B_KNIGHT) {
            MOVES m; // (8);
            m.n = 0;
            Knight knight;
            knight.color = Color::WHITE;
            m = knight.get_moves_terminal((board), wList, bList, previos_moves, i, j, true, WHITE_O_O, WHITE_O_O_O, BLACK_O_O, BLACK_O_O_O);

            for (__int8 k = 0; k < m.n; ++k) {
                // Поля куда ходить королём нельзя потому что атакует один из вражеских коней.
                if (get_color1(board, m.moves[k].x, m.moves[k].y) != Color::WHITE) {
                    attack_WHITE[m.moves[k].y][m.moves[k].x] = true;
                }
            }
        }
    }
}


// Король съеден.
bool eatKing(Board &board) {

    bool bKW = false, bKB = false;

    //for (__int8 y_1 = 0; y_1 < 8; ++y_1) {
      //  for (__int8 x_1 = 0; x_1 < 8; ++x_1) {
        //    if (board.board[y_1][x_1] == ChessPiece::W_KING) bKW = true;
          //  if (board.board[y_1][x_1] == ChessPiece::B_KING) bKB = true;

        //} 
    //}
    for (int k = 0; k < board.bList.n; ++k) {

        __int8 y = board.bList.moves[k].y;
        __int8 x = board.bList.moves[k].x;

        if (board.board[y][x] == ChessPiece::B_KING) {
            bKB = true;
            break;
        }
    }

    for (int k = 0; k < board.wList.n; ++k) {

        __int8 y = board.wList.moves[k].y;
        __int8 x = board.wList.moves[k].x;

        if (board.board[y][x] == ChessPiece::W_KING) {
            bKW = true;
            break;
        }
    }

    return (!(bKW && bKB));
}

// Форсированный вариант.
float Quies(float alpha, float beta, Color my_color, Color enemy_color, Board board, __int8 iply);

// forsing ==false Без формированного варианта. 
// forsing = true с форсированным вариантом.
LIST_MOVES minimax_do(Color my_color, Color enemy_color, __int8 my_depth, Board board, __int8 depth, bool forsing) {

    bool enemy = ((depth % 2) == 1); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    // depth - глубина просчёта он нуля.

    // если исчерпан лимит глубины - вернём статическую оценку позиции.
    if ((depth == my_depth) || (eatKing(board))) {

        // Либо съели короля либо достигли глубины перебора.

        // Считает материал с точки зрения белых,
        // она суммирует вес всех белых фигур, а 
        // потом вычитает из них вес всех чёрных фигур.
        // Оценка для Чёрных наоборот.

        LIST_MOVES m;
        m.n = 1;
        const float multiplyer = 1.0f; //1.1f;

        

        if (forsing) {

            // Терминальный вызов в листе.
            m.moves[0].rate = Quies(-10000.0, 10000.0, my_color, enemy_color, board, 0);
            //m.moves[0].rate = Quies1(-10000.0, 10000.0, my_color, enemy_color, board, 0);
        }
        else {
            // Вызывалось для my_color.
            // Если у нас минимакс то максимум то минимум, мы то максимизируем то минимизируем
            m.moves[0].rate = board.rate(my_color) - board.rate(enemy_color) * multiplyer;
           // Если мы переворачиваем и только максимизируем в алгоритме как у Евгения Корнилова.
            // m.moves[0].rate = (color == my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer : board.rate(enemy_color) - board.rate(my_color) * multiplyer);
        }
#ifndef CODE_FOR_DLL
        //std::cout << "static  evaluation = " << board.rate(my_color) << " "<< board.rate(enemy_color) << " "<< board.rate(enemy_color) * multiplyer <<" " << m.moves[0].rate << std::endl;
        //board.print();
        //system("PAUSE");
#endif

        m.moves[0].xy_from = init(None);
        m.moves[0].xy_to = init(None);

        return m;
    }


    LIST_MOVES rates;
    rates.n = 0;
    float rate;
    float rate_flag;

    if (enemy) {
        //min
        rate = +9999.0f - 10.0f * depth;
        rate_flag = rate - 1.0f;
    }
    else {
        // max
        rate = -9999.0f + 10.0f * depth;
        rate_flag = rate + 1.0f;
    }


    MOVES figList;
    if (color == Color::WHITE) {

        figList.n = board.wList.n;
        for (__int8 k_1 = 0; k_1 < board.wList.n; ++k_1) {
            figList.moves[k_1] = board.wList.moves[k_1];
        }

    }
    if (color == Color::BLACK) {

        figList.n = board.bList.n;
        for (__int8 k_1 = 0; k_1 < board.bList.n; ++k_1) {
            figList.moves[k_1] = board.bList.moves[k_1];
        }
    }


    // Проходим по доске
    //for (__int8 y = 0; y < 8; ++y) {
        //for (__int8 x = 0; x < 8; ++x) {
            //if (get_color1(board.board, x, y) != color) {
                // Если фигура чужая или у нас пустая клетка то переходим к следующей клетки доски.
              //  continue;
            //}

    for (__int8 k_1 = 0; k_1 < figList.n; ++k_1) {

        __int8 y = figList.moves[k_1].y;
        __int8 x = figList.moves[k_1].x;



        MOVES_NODE xy_from;
        xy_from = init(x, y);

#ifndef CODE_FOR_DLL
        //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y)  << std::endl;
#endif

            // получим все позиции перемещения
        MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции board[y][x].
#ifndef CODE_FOR_DLL
            //std::cout << "ChessMan="<< (int)(board.board[y][x]) << "number moves=" << moves_current_list.n << std::endl;
            //printf("ChessMan= %d number moves = %d\n", (int)(board.board[y][x]), moves_current_list.n);
#endif

            // Проверяем каждый сгенерированный код.
        for (__int8 i = 0; i < moves_current_list.n; ++i) {

            MOVES_NODE xy_to = moves_current_list.moves[i]; // Куда походить фигурой.



            Board new_board;

            // Копирование доски.
            new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = board.board[y][x];

#ifndef CODE_FOR_DLL
            if (0 && (depth == 0)) {
                //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
               // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                printf("depth = 0 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                new_board.print();

                system("PAUSE");
            }
            if (0 && (depth == 1)) {
                //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
               // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                printf("depth = 1 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                new_board.print();

                system("PAUSE");
            }
#endif

            // Место куда атаковали не пусто, там стоит вражеская фигура ?
            bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
            if (color == Color::WHITE) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 1) &&
                    (board.previos_moves.xy_to.y == 3) &&
                    (xy_from.y == 3) && (xy_to.y == 2)) {
                    captured = true; // взятие на проходе.
                }
            }
            if (color == Color::BLACK) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 6) &&
                    (board.previos_moves.xy_to.y == 4) &&
                    (xy_from.y == 4) && (xy_to.y == 5)) {
                    captured = true; // взятие на проходе.
                }
            }

            // Сьели Короля.
            // if (captured && (board.board[xy_to.y][xy_to.x] == ChessPiece::W_KING) || (board.board[xy_to.y][xy_to.x] == ChessPiece::B_KING)) {

             //   rate = ((enemy) ? -1000.0f : 1000.0f); // Вражеский король съеден (мат).
            //}
            //else
            //{
            LIST_MOVES m;
            m.n = 0;

            bool forsing=false;
            m = minimax_do(my_color, enemy_color, my_depth, new_board, depth + 1, forsing);

#ifndef CODE_FOR_DLL
            // std::cout << "i=" << m.n  << " rate=" << (m.n > 0 ? m.moves[0].rate : 0) << std::endl;
#endif

            if (m.n == 0) continue;


            if (enemy) {
                //min
                // rate = (rate < m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)

            }
            else {
                //max

                // rate = (rate > m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)

            }

            rate = m.moves[0].rate;

#ifndef CODE_FOR_DLL
            if (depth == 0) {
                //std::cout << "\n rate= " << rate << std::endl;
               // printf("\n rate=  %f\n", rate);
            }
#endif


            if ((captured) && (!enemy)) {
                // rate += my_depth - depth; // Добавим немного агрессии
            }
            //}

            // Доска  копируется в памяти много раз подряд.




            rates.n++;

            if (rates.n - 1 > POOL_SIZE - 1) {
#ifndef CODE_FOR_DLL
                std::cout << "error! moves limit <POOL_SIZE\n";
                system("PAUSE");
#endif
            }

            rates.moves[rates.n - 1].rate = rate;
            // std::cout << "i=" << rates.n - 1 << " rate=" << rate << std::endl;
            // printf("i=%d rate= %f\n",rates.n - 1, rate);

            rates.moves[rates.n - 1].xy_from = xy_from;
            rates.moves[rates.n - 1].xy_to = xy_to;

        }

    }
    //  }
  //}


    if (depth == 0) {
        return rates;
    }
    if (rates.n == 0) {

        if (fabs(rate) > fabs(rate_flag)) {
            // Цель игры мат королю противника.
            rates.n = 1;
            if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
                // Мы под шахом и нам некуда идти -> мат.
                rates.moves[0].rate = rate;
            }
            else {
                // Некуда идти и мы не под шахом ничья.
                rate = 0.0; // Пат
                rates.moves[0].rate = rate;
            }
            rates.moves[0].xy_from.x = None;
            rates.moves[0].xy_from.y = None;
            rates.moves[0].xy_to.x = None;
            rates.moves[0].xy_to.y = None;
        }

        return rates; // Как бы возврат None так как rates.n == 0.
    }

    LIST_MOVES m;
    m.n = 1;


    {

        __int8 id_found = 0;
        float rate_found = rates.moves[0].rate;

        // Мы находимся на некоторой глубине вложенности

        if (enemy) {

            // Поиск минимума  
            // Враг черные при первом ходе белых или наборот при первом ходе черных.

            for (__int8 i = 1; i < rates.n; ++i) {
                if (rates.moves[i].rate < rate_found) {
                    rate_found = rates.moves[i].rate;
                    id_found = i;
                }
            }

        }
        else {
            // Поиск максимума   
            // Белые  при первом ходе белых или наборот при первом ходе черных.

            for (__int8 i = 1; i < rates.n; ++i) {
                if (rates.moves[i].rate > rate_found) {
                    rate_found = rates.moves[i].rate;
                    id_found = i;
                }
            }
        }

#ifndef CODE_FOR_DLL
        if (0) {
            //std::cout << "\n";
            printf("\n");
            for (__int8 i = 1; i < rates.n; ++i) {
                if (fabs(rates.moves[i].rate - rate_found) < 0.01) {
                    //std::cout << i << " ";
                    printf("%d ", i);
                }
            }
            //std::cout << "\n";
            printf("\n");
        }
#endif


        m.moves[0].rate = rates.moves[id_found].rate;

        // m.moves[0].xy_from=init(None);
         //m.moves[0].xy_to=init(None);

        m.moves[0].xy_from = rates.moves[id_found].xy_from;
        m.moves[0].xy_to = rates.moves[id_found].xy_to;
    }
    return m;

    //return ;
}


// Простейший самый первый параллельный алгоритм. 14.01.2023
LIST_MOVES minimax_do_parallel(Color my_color, Color enemy_color, __int8 my_depth, Board board, __int8 depth) {

    bool enemy = (bool)(depth % 2); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    // depth - глубина просчёта он нуля.

    // если исчерпан лимит глубины - вернём статическую оценку позиции.
    /*if ((depth == my_depth) || (eatKing(board))) {

        // Либо съели короля либо достигли глубины перебора.

        // Считает материал с точки зрения белых,
        // она суммирует вес всех белых фигур, а 
        // потом вычитает из них вес всех чёрных фигур.
        // Оценка для Чёрных наоборот.

        LIST_MOVES m;
        m.n = 1;
        const float multiplyer = 1.1f; //1.1f;

        // Вызывалось для my_color.
         // Если у нас минимакс то максимум то минимум, мы то максимизируем то минимизируем
        m.moves[0].rate = board.rate(my_color) - board.rate(enemy_color) * multiplyer;
        // Если мы переворачиваем и только максимизируем в алгоритме.
        // m.moves[0].rate = (color==my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer: board.rate(enemy_color)- board.rate(my_color)*multiplyer);

#ifndef CODE_FOR_DLL
        //std::cout << "static  evaluation = " << board.rate(my_color) << " "<< board.rate(enemy_color) << " "<< board.rate(enemy_color) * multiplyer <<" " << m.moves[0].rate << std::endl;
        //board.print();
        //system("PAUSE");
#endif

        m.moves[0].xy_from = init(None);
        m.moves[0].xy_to = init(None);

        return m;
    }*/


    LIST_MOVES rates;
    rates.n = 0;
    float rate;
    float rate_flag;

    if (enemy) {
        //min
        rate = +9999.0f - 10.0f * depth;
        rate_flag = rate - 1.0f;
    }
    else {
        // max
        rate = -9999.0f + 10.0f * depth;
        rate_flag = rate + 1.0f;
    }


    MOVES figList;
    if (color == Color::WHITE) {

        figList.n = board.wList.n;
        for (__int8 k_1 = 0; k_1 < board.wList.n; ++k_1) {
            figList.moves[k_1] = board.wList.moves[k_1];
        }

    }
    if (color == Color::BLACK) {

        figList.n = board.bList.n;
        for (__int8 k_1 = 0; k_1 < board.bList.n; ++k_1) {
            figList.moves[k_1] = board.bList.moves[k_1];
        }
    }


    // Проходим по доске
    //for (__int8 y = 0; y < 8; ++y) {
        //for (__int8 x = 0; x < 8; ++x) {
            //if (get_color1(board.board, x, y) != color) {
                // Если фигура чужая или у нас пустая клетка то переходим к следующей клетки доски.
              //  continue;
            //}

    // Оценка сверху.
    PREVIOS_MOVE move_now[987];// 16 ферзей 56клеток каждый.
    int icount_all_moves = 0;

    for (__int8 k_1 = 0; k_1 < figList.n; ++k_1) {

        __int8 y = figList.moves[k_1].y;
        __int8 x = figList.moves[k_1].x;

        MOVES_NODE xy_from;
        xy_from = init(x, y);

        // получим все позиции перемещения
        MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции board[y][x].

           // Проверяем каждый сгенерированный код.
        for (__int8 i = 0; i < moves_current_list.n; ++i) {
            move_now[icount_all_moves].xy_from.x = x;
            move_now[icount_all_moves].xy_from.y = y;
            move_now[icount_all_moves].xy_to.x = moves_current_list.moves[i].x;
            move_now[icount_all_moves].xy_to.y = moves_current_list.moves[i].y;
            icount_all_moves++;
            if (icount_all_moves  >= 987) {
                std::cout << "parallel limit ischerpan\n";
                system("pause");
            }
        }
    }

    //for (__int8 k_1 = 0; k_1 < figList.n; ++k_1)
    
    std::cout << "number moves =" << icount_all_moves << std::endl;
    rates.n = icount_all_moves;
    int id_thread[40] = { 0 };// 2*xeon 2630v4.
    bool flag_in[POOL_SIZE] = { false };


#pragma omp parallel for schedule (dynamic)
    for (int k_i = 0; k_i < icount_all_moves; ++k_i)
    {

        __int8 y = move_now[k_i].xy_from.y;
        __int8 x = move_now[k_i].xy_from.x;



        MOVES_NODE xy_from;
        xy_from = init(x, y);

#ifndef CODE_FOR_DLL
        //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y)  << std::endl;
#endif

            // получим все позиции перемещения
        //MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции board[y][x].
#ifndef CODE_FOR_DLL
            //std::cout << "ChessMan="<< (int)(board.board[y][x]) << "number moves=" << moves_current_list.n << std::endl;
            //printf("ChessMan= %d number moves = %d\n", (int)(board.board[y][x]), moves_current_list.n);
#endif

            // Проверяем каждый сгенерированный код.
        //for (__int8 i = 0; i < moves_current_list.n; ++i)
        {

            MOVES_NODE xy_to = init(move_now[k_i].xy_to.x, move_now[k_i].xy_to.y);// moves_current_list.moves[i]; // Куда походить фигурой.



            Board new_board;

            // Копирование доски.
            new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = board.board[y][x];

#ifndef CODE_FOR_DLL
            if (0 && (depth == 0)) {
                //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
               // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                printf("depth = 0 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                new_board.print();

                system("PAUSE");
            }
            if (0 && (depth == 1)) {
                //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
               // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                printf("depth = 1 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                new_board.print();

                system("PAUSE");
            }
#endif

            // Место куда атаковали не пусто, там стоит вражеская фигура ?
            bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
            if (color == Color::WHITE) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 1) &&
                    (board.previos_moves.xy_to.y == 3) &&
                    (xy_from.y == 3) && (xy_to.y == 2)) {
                    captured = true; // взятие на проходе.
                }
            }
            if (color == Color::BLACK) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 6) &&
                    (board.previos_moves.xy_to.y == 4) &&
                    (xy_from.y == 4) && (xy_to.y == 5)) {
                    captured = true; // взятие на проходе.
                }
            }

            // Сьели Короля.
            // if (captured && (board.board[xy_to.y][xy_to.x] == ChessPiece::W_KING) || (board.board[xy_to.y][xy_to.x] == ChessPiece::B_KING)) {

             //   rate = ((enemy) ? -1000.0f : 1000.0f); // Вражеский король съеден (мат).
            //}
            //else
            //{
            LIST_MOVES m;
            m.n = 0;

            bool forsing = true;
            m = minimax_do(my_color, enemy_color, my_depth, new_board, depth + 1, forsing);


#ifndef CODE_FOR_DLL
            // std::cout << "i=" << m.n  << " rate=" << (m.n > 0 ? m.moves[0].rate : 0) << std::endl;
#endif

            if (m.n == 0) continue;


            if (enemy) {
                //min
               // rate = (rate < m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)

            }
            else {
                //max

               // rate = (rate > m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)

            }

            float rate31 = m.moves[0].rate;

#ifndef CODE_FOR_DLL
            if (depth == 0) {
                //std::cout << "\n rate= " << rate << std::endl;
               // printf("\n rate=  %f\n", rate);
            }
#endif


            if ((captured) && (!enemy)) {
                // rate += my_depth - depth; // Добавим немного агрессии
          //  }
            }

            // Доска  копируется в памяти много раз подряд.




           // rates.n++;

          //  if (rates.n - 1 > POOL_SIZE - 1)
          //  {
//#ifndef CODE_FOR_DLL
              //  std::cout << "error! moves limit <POOL_SIZE\n";
              //  system("PAUSE");
//#endif
          //  }

            rates.moves[std::min(POOL_SIZE - 1,id_thread[omp_get_thread_num()]+omp_get_thread_num())].rate = rate31;
            // std::cout << "i=" << rates.n - 1 << " rate=" << rate << std::endl;
            // printf("i=%d rate= %f\n",rates.n - 1, rate);

            rates.moves[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())].xy_from = xy_from;
            rates.moves[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())].xy_to = xy_to;
            flag_in[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())] = true;
            id_thread[omp_get_thread_num()] += omp_get_num_threads();
        }

    }
    //  }
  //}

    // Плотная упаковка от нуля до rates.n-1
    int imesto0 = 0;
    int imesto1 = 0;
    for (int k_i = 0; k_i < POOL_SIZE-1; ++k_i)
    {

        if (flag_in[imesto1]) {
            if (imesto1 > imesto0) {
                rates.moves[imesto0] = rates.moves[imesto1];
                imesto0++;
                imesto1++;
            }
        }
        else {
            ++imesto1;
        }
       
    }

    if (depth == 0) {
        return rates;
    }
    if (rates.n == 0) {

        if (fabs(rate) > fabs(rate_flag)) {
            // Цель игры мат королю противника.
            rates.n = 1;
            if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
                // Мы под шахом и нам некуда идти -> мат.
                rates.moves[0].rate = rate;
            }
            else {
                // Некуда идти и мы не под шахом ничья.
                rate = 0.0; // Пат
                rates.moves[0].rate = rate;
            }
            rates.moves[0].xy_from.x = None;
            rates.moves[0].xy_from.y = None;
            rates.moves[0].xy_to.x = None;
            rates.moves[0].xy_to.y = None;
        }

        return rates; // Как бы возврат None так как rates.n == 0.
    }

    LIST_MOVES m;
    m.n = 1;


    {

        __int8 id_found = 0;
        float rate_found = rates.moves[0].rate;

        // Мы находимся на некоторой глубине вложенности

        if (enemy) {

            // Поиск минимума  
            // Враг черные при первом ходе белых или наборот при первом ходе черных.

            for (__int8 i = 1; i < rates.n; ++i) {
                if (rates.moves[i].rate < rate_found) {
                    rate_found = rates.moves[i].rate;
                    id_found = i;
                }
            }

        }
        else {
            // Поиск максимума   
            // Белые  при первом ходе белых или наборот при первом ходе черных.

            for (__int8 i = 1; i < rates.n; ++i) {
                if (rates.moves[i].rate > rate_found) {
                    rate_found = rates.moves[i].rate;
                    id_found = i;
                }
            }
        }

#ifndef CODE_FOR_DLL
        if (0) {
            //std::cout << "\n";
            printf("\n");
            for (__int8 i = 1; i < rates.n; ++i) {
                if (fabs(rates.moves[i].rate - rate_found) < 0.01) {
                    //std::cout << i << " ";
                    printf("%d ", i);
                }
            }
            //std::cout << "\n";
            printf("\n");
        }
#endif


        m.moves[0].rate = rates.moves[id_found].rate;

        // m.moves[0].xy_from=init(None);
         //m.moves[0].xy_to=init(None);

        m.moves[0].xy_from = rates.moves[id_found].xy_from;
        m.moves[0].xy_to = rates.moves[id_found].xy_to;
    }
    return m;

    //return ;
}





// используется для SEE 
// только равные или выигрышные взятия.
float rate_fig(ChessPiece figure)
{
    float ret = 0.0f;
    if ((figure == ChessPiece::W_PAWN) || (figure == ChessPiece::B_PAWN)) {
        ret = Pawn(Color::WHITE).VALUE;
    }
    if ((figure == ChessPiece::W_QUEEN) || (figure == ChessPiece::B_QUEEN)) {
        ret = Queen(Color::WHITE).VALUE;
    }
    if ((figure == ChessPiece::W_BISHOP) || (figure == ChessPiece::B_BISHOP)) {
        ret = Bishop(Color::WHITE).VALUE;
    }
    if ((figure == ChessPiece::W_KING) || (figure == ChessPiece::B_KING)) {
        ret = King(Color::WHITE).VALUE;
    }
    if ((figure == ChessPiece::W_ROOK) || (figure == ChessPiece::B_ROOK)) {
        ret = Rook(Color::WHITE).VALUE;
    }
    if ((figure == ChessPiece::W_KNIGHT) || (figure == ChessPiece::B_KNIGHT)) {
        ret = Knight(Color::WHITE).VALUE;
    }
    return ret;
}


// alpha_beta_algorithm
// Некоторые называют это искуственным интеллектом игры в шахматы.
LIST_MOVES minimax(Color color, __int8 thinking_depth, Board board);

float fig_rate(Color clr, ChessPiece board[8][8], MOVESL& wList, MOVESL& bList, __int8 x, __int8 y);

// Генератор ходов и сортировка ходов.
void GenerateAllMoves(MOVES &figList, LIST_NODE_MOVES make_moves[], MOVES moves_current_list[], 
    Color color, Board board, int &n_01, int &i_01, int &i_02, Color my_color, Color enemy_color, __int8 my_depth, bool enemy, __int8 iply, bool bparallel,
    bool bprincipal_variation_search, float rate_start)
{
    float key[16] = { -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f, -9999.0f };

    if (color == Color::WHITE) {

        figList.n = board.wList.n;
        for (__int8 k_1 = 0; k_1 < board.wList.n; ++k_1) {
            figList.moves[k_1] = board.wList.moves[k_1];

            key[k_1] = fig_rate(color, board.board, board.wList, board.bList, board.wList.moves[k_1].x, board.wList.moves[k_1].y);
        }

        // Сортировка фигур по убыванию rates (цены фигуры: Король, ферзь, ладья, слон, конь, пешка).
        for (__int8 k_1 = 0; k_1 < board.wList.n; ++k_1) {
            for (__int8 l_1 = 1; l_1 < board.wList.n; ++l_1) {
                if (key[l_1 - 1] < key[l_1]) {
                    // swap
                    float ftmp = key[l_1 - 1];
                    key[l_1 - 1] = key[l_1];
                    key[l_1] = ftmp;
                    // swap
                    MOVES_NODE mn_tmp = figList.moves[l_1 - 1];
                    figList.moves[l_1 - 1] = figList.moves[l_1];
                    figList.moves[l_1] = mn_tmp;
                }
            }
        }

    }
    if (color == Color::BLACK) {

        figList.n = board.bList.n;
        for (__int8 k_1 = 0; k_1 < board.bList.n; ++k_1) {
            figList.moves[k_1] = board.bList.moves[k_1];

            key[k_1] = fig_rate(color, board.board, board.wList, board.bList, board.bList.moves[k_1].x, board.bList.moves[k_1].y);

        }

        // Сортировка фигур по убыванию rates (цены фигуры: Король, ферзь, ладья, слон, конь, пешка)
        for (__int8 k_1 = 0; k_1 < board.bList.n; ++k_1) {
            for (__int8 l_1 = 1; l_1 < board.bList.n; ++l_1) {
                if (key[l_1 - 1] < key[l_1]) {
                    // swap
                    float ftmp = key[l_1 - 1];
                    key[l_1 - 1] = key[l_1];
                    key[l_1] = ftmp;
                    // swap
                    MOVES_NODE mn_tmp = figList.moves[l_1 - 1];
                    figList.moves[l_1 - 1] = figList.moves[l_1];
                    figList.moves[l_1] = mn_tmp;
                }
            }
        }
    }

    for (__int8 k_1 = 0; k_1 < figList.n; ++k_1) {

        __int8 y = figList.moves[k_1].y;
        __int8 x = figList.moves[k_1].x;

        // получим все позиции перемещения
        moves_current_list[k_1] = board.get_moves(x, y);//список возможных ходов из позиции [x,y].

        n_01 += moves_current_list[k_1].n;
    }

    if (bprincipal_variation_search) {

        // все допустимые ходы без сортировки очень быстрого Principal Variation Search

        for (__int8 k_1 = figList.n - 1; k_1 >= 0; --k_1) {

            __int8 y = figList.moves[k_1].y;
            __int8 x = figList.moves[k_1].x;

            // Проверяем каждый сгенерированный код.
            for (__int8 i = 0; i < moves_current_list[k_1].n; ++i) {

                MOVES_NODE xy_to = moves_current_list[k_1].moves[i]; // Куда походить фигурой.  


                make_moves[i_01].xy_from.x = x;
                make_moves[i_01].xy_from.y = y;
                make_moves[i_01].xy_to.x = xy_to.x;
                make_moves[i_01].xy_to.y = xy_to.y;

                make_moves[i_01].rate = 0.0;

                i_01++;
            }
        }
    }
    else {

        for (__int8 k_1 = figList.n - 1; k_1 >= 0; --k_1) {

            __int8 y = figList.moves[k_1].y;
            __int8 x = figList.moves[k_1].x;

            MOVES_NODE xy_from;
            xy_from.x = x;
            xy_from.y = y;

            // получим все позиции перемещения
            //MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции [x,y].
             // Проверяем каждый сгенерированный код.
            for (__int8 i = 0; i < moves_current_list[k_1].n; ++i) {

                MOVES_NODE xy_to = moves_current_list[k_1].moves[i]; // Куда походить фигурой.  

                // Место куда атаковали не пусто, там стоит вражеская фигура ?
                bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
                bool eat_Pawn_exit = false;
                bool bPawn2Queen = false;
                if (color == Color::WHITE) {
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                        (xy_to.y == 0)) {
                        // пешка стала ферзём.
                        bPawn2Queen = true;
                        captured = true;
                    }
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                        (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                        (xy_to.x == board.previos_moves.xy_to.x) &&
                        (xy_to.x == board.previos_moves.xy_from.x) &&
                        ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                        (board.previos_moves.xy_from.y == 1) &&
                        (board.previos_moves.xy_to.y == 3) &&
                        (xy_from.y == 3) && (xy_to.y == 2)) {
                        captured = true; // взятие на проходе.
                        eat_Pawn_exit = true;
                    }
                }
                if (color == Color::BLACK) {
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                        (xy_to.y == 7)) {
                        // пешка стала ферзём.
                        bPawn2Queen = true;
                        captured = true;
                    }
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                        (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                        (xy_to.x == board.previos_moves.xy_to.x) &&
                        (xy_to.x == board.previos_moves.xy_from.x) &&
                        ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                        (board.previos_moves.xy_from.y == 6) &&
                        (board.previos_moves.xy_to.y == 4) &&
                        (xy_from.y == 4) && (xy_to.y == 5)) {
                        captured = true; // взятие на проходе.
                        eat_Pawn_exit = true;

                    }
                }

                if (captured) {


                    make_moves[i_01].xy_from.x = x;
                    make_moves[i_01].xy_from.y = y;
                    make_moves[i_01].xy_to.x = xy_to.x;
                    make_moves[i_01].xy_to.y = xy_to.y;
                    //MVV/LVA наиболее ценная жертва наименее ценный нападающий.
                    // Ценность того что съели. Чем больше тем лучше.
                    float rate_eat_figure;
                    if (eat_Pawn_exit) {
                        // Взятие на проход.
                        rate_eat_figure = fig_rate(get_color1(board.board, board.previos_moves.xy_to.x, board.previos_moves.xy_to.y), board.board, board.wList, board.bList, board.previos_moves.xy_to.x, board.previos_moves.xy_to.y);
                    }
                    else {
                        // // Ценность того что съели. Чем больше тем лучше.
                        rate_eat_figure = fig_rate(get_color1(board.board, xy_to.x, xy_to.y), board.board, board.wList, board.bList, xy_to.x, xy_to.y);

                    }
                    if (bPawn2Queen) {
                        Queen q;

                        rate_eat_figure = q.get_value();// Приобрели ферзя.
                    }
                    // Ценность того чем едим.
                    float rate_fig = 1000.0f - fig_rate(get_color1(board.board, x, y), board.board, board.wList, board.bList, x, y);
                    make_moves[i_01].rate = rate_eat_figure * 1000.0f + rate_fig;// Ключ для сортировки по убыванию.
                    i_01++;

                }
            }

        }


        // Сортировка по убыванию.
        for (__int8 k_1 = 0; k_1 < i_01; ++k_1) {
            for (__int8 l_1 = 1; l_1 < i_01; ++l_1) {
                if (make_moves[l_1 - 1].rate < make_moves[l_1].rate) {
                    //swap
                    LIST_NODE_MOVES tmp = make_moves[l_1 - 1];
                    make_moves[l_1 - 1] = make_moves[l_1];
                    make_moves[l_1] = tmp;
                }
            }
        }


        i_02 = i_01;

        // статическая оценка текущей позиции.
        float multiplyer = 1.0f;
        // Для вычисления прироста стратегической оценки.
        float val0 = (color==my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer: board.rate(enemy_color)- board.rate(my_color)*multiplyer);

        for (__int8 k_1 = 0; k_1 < figList.n; ++k_1) {

            __int8 y = figList.moves[k_1].y;
            __int8 x = figList.moves[k_1].x;

            MOVES_NODE xy_from;
            xy_from.x = x;
            xy_from.y = y;

            // получим все позиции перемещения
            //MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции [x,y].
             // Проверяем каждый сгенерированный код.
            for (__int8 i = 0; i < moves_current_list[k_1].n; ++i) {

                MOVES_NODE xy_to = moves_current_list[k_1].moves[i]; // Куда походить фигурой.  

                // Место куда атаковали не пусто, там стоит вражеская фигура ?
                bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
                bool eat_Pawn_exit = false;
                bool bPawn2Queen = false;
                if (color == Color::WHITE) {
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                        (xy_to.y == 0)) {
                        // пешка стала ферзём.
                        bPawn2Queen = true;
                        captured = true;
                    }
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                        (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                        (xy_to.x == board.previos_moves.xy_to.x) &&
                        (xy_to.x == board.previos_moves.xy_from.x) &&
                        ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                        (board.previos_moves.xy_from.y == 1) &&
                        (board.previos_moves.xy_to.y == 3) &&
                        (xy_from.y == 3) && (xy_to.y == 2)) {
                        captured = true; // взятие на проходе.
                        eat_Pawn_exit = true;
                    }
                }
                if (color == Color::BLACK) {
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                        (xy_to.y == 7)) {
                        // пешка стала ферзём.
                        bPawn2Queen = true;
                        captured = true;
                    }
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                        (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                        (xy_to.x == board.previos_moves.xy_to.x) &&
                        (xy_to.x == board.previos_moves.xy_from.x) &&
                        ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                        (board.previos_moves.xy_from.y == 6) &&
                        (board.previos_moves.xy_to.y == 4) &&
                        (xy_from.y == 4) && (xy_to.y == 5)) {
                        captured = true; // взятие на проходе.
                        eat_Pawn_exit = true;
                    }
                }



                if (!captured) {




                    Board new_board;

                    // Копирование доски.
                    new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                    MOVES_NODE xy_from;
                    xy_from.x = x;
                    xy_from.y = y;

                    new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                     // Предыдущий ход на этой новой  доске после хода.
                    new_board.previos_moves.xy_from = xy_from;
                    new_board.previos_moves.xy_to = xy_to;
                    new_board.previos_moves.figure = board.board[y][x];

                   // float val1 = new_board.rate(my_color) - new_board.rate(enemy_color) * multiplyer;
                    float val1 = (color == my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer : board.rate(enemy_color) - board.rate(my_color) * multiplyer);

                    make_moves[i_01].xy_from.x = x;
                    make_moves[i_01].xy_from.y = y;
                    make_moves[i_01].xy_to.x = xy_to.x;
                    make_moves[i_01].xy_to.y = xy_to.y;

                    if (InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color), new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O)) {
                        make_moves[i_01].rate = 1000.0f;// Шах лучший ход.
                    }
                    else {
                        // приращение стратегической оценки. 
                        // Оно обычно малое меньше цены пешки.
                        // Оно положительно если мы приобретаем,
                        // и отрицательно если мы теряем в статической оценке позиции.
                        make_moves[i_01].rate = val1 - val0;
                    }

                    i_01++;

                }
            }

        }


        if (i_01 != n_01) {
            printf("error propusk\n");
            system("pause");
        }

        //if (make_moves != nullptr)
        {
            // Сортировка по убыванию.
            for (__int8 k_1 = i_02; k_1 < i_01; ++k_1) {
                for (__int8 l_1 = i_02 + 1; l_1 < i_01; ++l_1) {
                    if (make_moves[l_1 - 1].rate < make_moves[l_1].rate) {
                        //swap
                        LIST_NODE_MOVES tmp = make_moves[l_1 - 1];
                        make_moves[l_1 - 1] = make_moves[l_1];
                        make_moves[l_1] = tmp;
                    }
                }
            }
        }

        // true - принудительная обрезка дерева вариантов для глубины 6. 15,01,2023
        const bool bLite_tree = false;// true;

        if (1 && (my_depth == 6) && (iply < 2))
        {
            // 15.01.2023
            // Быстрый просчёт на четыре полухода используетcя для правильной 
            // сортировки ходов при alpha-beta алгоритме на первых двух полуходах.
            // Применяется только на первых двух уровнях при глубине 6. Запускается на глубину 4. 
            // Этот приём позволяет создать хорошую сортировку ходов для alpha-beta и ускорить её существенно.
            int idepth_test = 4;


            if (!enemy) {
                // по убыванию.
                 // Ответ Чёрных.

                 // Генерация списка возможных ходов с оценкой позиции:
                Board chess_board_loc;
                chess_board_loc.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);
                // Мы защищаем доску от изменений, вдруг мы что то испортим при запуске alpha-beta minimax
                //LIST_MOVES rates7 = minimax(my_color, 2, chess_board_loc);
                LIST_MOVES rates7;
                if (bparallel) {
                    rates7 = minimax_do_parallel(my_color, enemy_color, idepth_test, chess_board_loc, iply);
                }
                else {
                    // Нельзя вызывать паралельное внутри параллельного.
                    rates7 = minimax_do(my_color, enemy_color, idepth_test, chess_board_loc, iply, true);
                }

                // Сортировка по убыванию рейтинга
                for (__int8 id_rate = 1; id_rate < rates7.n; ++id_rate)
                {
                    for (__int8 id_rate0 = 0; id_rate0 < rates7.n - id_rate; ++id_rate0)
                    {
                        if (rates7.moves[id_rate0].rate < rates7.moves[id_rate0 + 1].rate) {

                            // swap
                            LIST_NODE_MOVES tmp;
                            tmp = rates7.moves[id_rate0];
                            rates7.moves[id_rate0] = rates7.moves[id_rate0 + 1];
                            rates7.moves[id_rate0 + 1] = tmp;


                        }
                    }
                }

                for (__int8 id_rate0 = 0; id_rate0 < rates7.n; ++id_rate0)
                {
                    // В порядке убывания rates что было получено при анализе дерева глубиной 4 полухода.
                    make_moves[id_rate0] = rates7.moves[id_rate0];
                }

                if (bLite_tree) {
                    // Отрежем половину тихих ходов.

                    int id_moves = 1;
                    for (__int8 id_rate0 = 1; id_rate0 < rates7.n; ++id_rate0)
                    {
                        // Считаем количество ходов отличающихся от лучшего на одну сантипешку.
                        if (fabs(rates7.moves[id_rate0].rate - rates7.moves[0].rate) < 1.0) {
                            ++id_moves;
                        }
                    }

                    //i_01 = n_01 = std::min(rates7.n, id_moves + 6);// Только лучшие ходы принудительно.
                }
            }
            else {

                // по возрастанию.
                // Ответ Белых.

                // Генерация списка возможных ходов с оценкой позиции:
                Board chess_board_loc;
                chess_board_loc.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);
                // Мы защищаем доску от изменений, вдруг мы что то испортим при запуске alpha-beta minimax
                //LIST_MOVES rates7 = minimax(my_color, 2, chess_board_loc);
                LIST_MOVES rates7;
                if (bparallel) {
                    rates7 = minimax_do_parallel(my_color, enemy_color, idepth_test, chess_board_loc, iply); // передаём нечетное значение
                }
                else {
                    // Нельзя запускать параллельное внутри параллельного
                    rates7 = minimax_do(my_color, enemy_color, idepth_test, chess_board_loc, iply, true);
                }

                // Сортировка по возрастанию рейтинга
                for (__int8 id_rate = 1; id_rate < rates7.n; ++id_rate)
                {
                    for (__int8 id_rate0 = 0; id_rate0 < rates7.n - id_rate; ++id_rate0)
                    {
                        if (rates7.moves[id_rate0].rate > rates7.moves[id_rate0 + 1].rate) {

                            // swap
                            LIST_NODE_MOVES tmp;
                            tmp = rates7.moves[id_rate0];
                            rates7.moves[id_rate0] = rates7.moves[id_rate0 + 1];
                            rates7.moves[id_rate0 + 1] = tmp;


                        }
                    }
                }

                for (__int8 id_rate0 = 0; id_rate0 < rates7.n; ++id_rate0)
                {
                    // В порядке убывания rates что было получено при анализе дерева глубиной 4 полухода.
                    make_moves[id_rate0] = rates7.moves[id_rate0];
                }

                if (bLite_tree) {
                    // Отрежем половину тихих ходов.

                    int id_moves = 1;
                    for (__int8 id_rate0 = 1; id_rate0 < rates7.n; ++id_rate0)
                    {
                        // Считаем количество ходов отличающихся от лучшего на одну сантипешку.
                        if (fabs(rates7.moves[id_rate0].rate - rates7.moves[0].rate) < 1.0) {
                            ++id_moves;
                        }
                    }

                    //i_01 = n_01 = std::min(rates7.n, id_moves + 6);// Только лучшие ходы принудительно.
                }
            }
        }
        else if ((my_depth == 6) && (iply >= 2)) {

            if (bLite_tree) {
                // Отрежем половину тихих ходов.

                //i_01 = n_01 = std::min(i_01, static_cast<int>(0.5 * i_01) + 3);// Только лучшие ходы принудительно.
            }

        }


        if (0 && (!enemy) && (iply == 0) && (my_depth == 6)) {

            // Ответ Чёрных.

            // Когда мы только запускаем поиск на глубину 3 хода (6 полуходов)
            // Мы предварительное запускаем быстрый поиск на 2хода (4 полухода) чтобы сразу отсечь большинство непереспективных ходов.


            // Генерация списка возможных ходов с оценкой позиции:
            Board chess_board_loc;
            chess_board_loc.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);
            // Мы защищаем доску от изменений, вдруг мы что то испортим при запуске alpha-beta minimax
            LIST_MOVES rates7 = minimax(my_color, 4, chess_board_loc);

            //printf("rates.n=%d\n", rates.n);

            int id_moves = 0;



            // Сортировка по убыванию рейтинга
            for (__int8 id_rate = 1; id_rate < rates7.n; ++id_rate)
            {
                for (__int8 id_rate0 = 0; id_rate0 < rates7.n - id_rate; ++id_rate0)
                {
                    if (rates7.moves[id_rate0].rate < rates7.moves[id_rate0 + 1].rate) {

                        // swap
                        LIST_NODE_MOVES tmp;
                        tmp = rates7.moves[id_rate0];
                        rates7.moves[id_rate0] = rates7.moves[id_rate0 + 1];
                        rates7.moves[id_rate0 + 1] = tmp;


                    }
                }
            }

            int inumber_equivalent_moves = 1; // Число эквивалентных ответов.


                // При if ((rates.n > 3)&&(ilevel_Gameuser == 5))
                // Компьютер может выбрать любой в окрестности сантипешка
                // отличающийся от сильного хода по силе ход, не самый сильный.

            id_moves = 1;
            for (__int8 id_rate0 = 1; id_rate0 < rates7.n; ++id_rate0)
            {
                // Считаем количество ходов отличающихся от лучшего на одну сантипешку.
                if (fabs(rates7.moves[id_rate0].rate - rates7.moves[0].rate) < 1.0) {
                    ++id_moves;
                }
            }

            if (0) {
                //n_01=i_01 = id_moves;
                for (__int8 id_rate0 = 0; id_rate0 < id_moves; ++id_rate0)
                {
                    //  make_moves[id_rate0] = rates7.moves[id_rate0];
                }
            }
            else {
                for (__int8 id_rate0 = 0; id_rate0 < rates7.n; ++id_rate0)
                {
                    // В порядке убывания rates что было получено при анализе дерева глубиной 4 полухода.
                    make_moves[id_rate0] = rates7.moves[id_rate0];
                }
            }

            std::cout << "best move for black:" << std::endl;
            for (__int8 id_rate = 0; id_rate < std::min(4 + std::max(inumber_equivalent_moves, std::min(5, rates7.n)), rates7.n); ++id_rate)
            {
                std::cout << rates7.moves[id_rate].rate << " " << static_cast<char>('a' + rates7.moves[id_rate].xy_from.x);
                std::cout << 8 - rates7.moves[id_rate].xy_from.y << static_cast<char>('a' + rates7.moves[id_rate].xy_to.x);
                std::cout << 8 - rates7.moves[id_rate].xy_to.y << std::endl;
            }
            std::cout << std::endl;
        }

        //delete[]  moves_current_list;

        // Beam search
        // Если возможных ответов всего 1, 2 или 3 то мы их все оставляем иначе отрубаем ровно половину.
        // 22.01.2023 Beam search не работает (программа отдаёт в жерву фигуры и даже ферзя) вместо Beam search нужно использовать Principal Variation search.
        // Principal Variation Search см. Евгений Корнилов стр. 47.
        //i_01 = n_01 = (i_01 <= 4 ? 4 :  i_01 / 2);// Это для Beam search

    }

    for (__int8 k_1 = 0; k_1 < i_01; ++k_1) 
    {
        // Присваиваем правильную заглушку для будующей сортировки ходов.
        make_moves[k_1].rate = rate_start;

    }

}

// https://www.geeksforgeeks.org/minimax-algorithm-in-game-theory-set-4-alpha-beta-pruning/
// alpha - beta алгоритм.
LIST_MOVES alpha_beta_do(float alpha, float beta, Color my_color, Color enemy_color, __int8 my_depth, Board board, __int8 depth, __int8 iply,
    bool bprincipal_variation_search) {

    ++number_chess_positions;

    // depth может не менятся на плюс один из-за итеративного углубления.
    // iply всегда меняется на плюс 1.

    bool enemy = ((iply % 2)==1); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    

    // depth - глубина просчёта он нуля.

    // если исчерпан лимит глубины - вернём статическую оценку позиции.
    if ((depth >= my_depth) || (eatKing(board))||(iply>=30)) {

        // Либо съели короля либо достигли глубины перебора.

        // Считает материал с точки зрения белых,
        // она суммирует вес всех белых фигур, а 
        // потом вычитает из них вес всех чёрных фигур.
        // Оценка для Чёрных наоборот.

        LIST_MOVES m;
        m.n = 1;
        const float multiplyer = 1.0f; // 1.1f;

        // Вызывалось для my_color.
        // статическая оценка.
        // 44.2% 
        // Добавляем немного агрессии добавляем баллы за взятие фигур противника.
        // Если у нас минимакс то максимум то минимум, мы то максимизируем то минимизируем 
        m.moves[0].rate = board.rate(my_color) - board.rate(enemy_color) * multiplyer;
        // Если мы переворачиваем и в алгоритме только максимизируем как у Евгения Корнилова.
        //m.moves[0].rate = (color == my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer : board.rate(enemy_color) - board.rate(my_color) * multiplyer);

#ifndef CODE_FOR_DLL
       //std::cout << "static  evaluation = " << board.rate(my_color) << " "<< board.rate(enemy_color) << " "<< board.rate(enemy_color) * multiplyer <<" " << m.moves[0].rate << std::endl;
       //board.print();
       //system("PAUSE");
#endif

        m.moves[0].xy_from = init(None);
        m.moves[0].xy_to = init(None);

        return m;
    }


    LIST_MOVES rates;
    rates.n = 0;
    float rate;
    float rate_flag;
    if (enemy) {
        //min
        rate = +9999.0f-10.0f* iply;
        rate_flag = rate - 1.0f;
    }
    else {
        // max
        rate = -9999.0f+10.0f*iply;
        rate_flag = rate + 1.0f;
    }

   
    MOVES figList;   

    // Проходим по доске
  //for (__int8 y = 0; y < 8; ++y) {
      //for (__int8 x = 0; x < 8; ++x) {
          //if (get_color1(board.board, x, y) != color) {
              // Если фигура чужая или у нас пустая клетка то переходим к следующей клетки доски.
            //  continue;
          //}

    int n_01 = 0;
    MOVES moves_current_list[POOL_SIZEL];
    
    LIST_NODE_MOVES make_moves[POOL_SIZE];

    int i_01 = 0, i_02 = 0;    
   
    GenerateAllMoves(figList, make_moves, moves_current_list,  color,  board, n_01, i_01, i_02, my_color, enemy_color, my_depth,  enemy, iply, false, bprincipal_variation_search, rate);

   

    if (n_01 > 0) {
        if (n_01 > POOL_SIZE) {
            printf("increase POOL_SIZE %d %d\n", n_01, POOL_SIZE);
            system("PAUSE");
        }
        LIST_MOVES rates;
        //printf("%d %d %d\n", rates.POOL_SIZE_loc, i_01, n_01);
        rates.n = 0;

        //system("PAUSE");

        // Сканируем ходы

    //for (__int8 k_1 = 0; k_1 < figList.n; ++k_1)
    for (int k_1 = 0; k_1 < i_01; ++k_1)
    {

       
        __int8 y = make_moves[k_1].xy_from.y;
        __int8 x = make_moves[k_1].xy_from.x;

        MOVES_NODE xy_from;
        xy_from = init(x, y);

#ifndef CODE_FOR_DLL
        //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y)  << std::endl;
#endif

            // получим все позиции перемещения
        //MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции [x,y].
        // Проверяем каждый сгенерированный код.
        //for (__int8 i = 0; i < moves_current_list.n; ++i) 
        {

            MOVES_NODE xy_to;// = moves_current_list.moves[i]; // Куда походить фигурой.               

            xy_to.x = make_moves[k_1].xy_to.x;
            xy_to.y = make_moves[k_1].xy_to.y;

            Board new_board;

            // Копирование доски.
            new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = board.board[y][x];

            // Место куда атаковали не пусто, там стоит вражеская фигура ?
            bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
            bool captured_see = false;
            // true цена того что съели больше или равна цене того чем съели.
            if (captured) captured_see = (rate_fig(board.board[xy_to.y][xy_to.x]) >= rate_fig(new_board.board[xy_to.y][xy_to.x]) ? true : false);

            if (color == Color::WHITE) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 1) &&
                    (board.previos_moves.xy_to.y == 3) &&
                    (xy_from.y == 3) && (xy_to.y == 2)) {
                    captured = true; // взятие на проходе.
                    captured_see = true;
                }
            }
            if (color == Color::BLACK) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 6) &&
                    (board.previos_moves.xy_to.y == 4) &&
                    (xy_from.y == 4) && (xy_to.y == 5)) {
                    captured = true; // взятие на проходе.
                    captured_see = true;
                }
            }
           // Сьели Короля.
            //if (captured && (board.board[xy_to.y][xy_to.x] == ChessPiece::B_KING) || (board.board[xy_to.y][xy_to.x] == ChessPiece::W_KING)) {

              //  rate = ((enemy) ? -1000.0f : 1000.0f); // Вражеский король сьеден (мат).
            //}
        //    else
            {
                LIST_MOVES m;
                m.n = 0;

                //alpha - наш максимум.
                // beta - максимум противника.

                //m = alpha_beta_do(alpha, beta, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1);
                // Было взятие - итеративное углубление.
                // captured - Он почемуто жертвует фигуры (выбрасывает фигуры). Коня за пешку.
                // Попробуем captured_see только равные или победившие взятия.
                //m = alpha_beta_do(alpha, beta,   my_color, enemy_color, my_depth, new_board, (captured_see ? depth : depth + 1),iply+1);
                //  детектор шахов InCheck(new_board.board, new_board.previos_moves, color)
                // В малофигурном окончании правило captured_see неверно и заменено на captured так как Король тоже может есть.
               // m = alpha_beta_do(alpha, beta, my_color, enemy_color, my_depth, new_board, 
                 //   (captured_see|| InCheck(new_board.board, new_board.previos_moves, color)||
                   //     (captured&& number_figures(new_board.board)<=ENDSHPIL_NUMBER_FIGURES) ? depth : depth + 1), iply + 1);
                // Нам же не нужно чтобы он слона за пешку отдал. Без SEE он просто отдаёт материал.
                //Color_invert(color) правильное продление шахов с invert color так как на новой доске и цвет новый,
                // но продление шахов инициирует всё новые шахи так же как продление взятий инициирует новые взятия иногда неравноценные жертвы
                // фигуры выбрасываются.
               // m = alpha_beta_do(alpha, beta, my_color, enemy_color, my_depth, new_board,
                 //   (captured_see  /* || InCheck(new_board.board, new_board.previos_moves, Color_invert(color))*/ ? depth : depth + 1), iply + 1);


                // Principial Variation Search
                //if (k_1 < 4)
                {

                    m = alpha_beta_do(alpha, beta, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1,false);
                }
                /*else {

                    // Быстрый просчёт с нулевым окном
                    if (enemy) {
                        //min

                        m = alpha_beta_do(beta - 1.0f, beta, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1,true);
                    }
                    else {
                        //max
                        m = alpha_beta_do(alpha, alpha + 1.0f, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1,true);
                    }

                    // если эвристика не сработала, и ход улучшает
                    // alpha, то мы пересчитаем с полным окном
                    if ((m.moves[0].rate > alpha) && (m.moves[0].rate < beta)) {

                        Board new_board2;

                        // Копирование доски.
                        new_board2.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                        new_board2.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                         // Предыдущий ход на этой новой  доске после хода.
                        new_board2.previos_moves.xy_from = xy_from;
                        new_board2.previos_moves.xy_to = xy_to;
                        new_board2.previos_moves.figure = board.board[y][x];

                        m = alpha_beta_do(alpha, beta, my_color, enemy_color, my_depth, new_board2, depth + 1, iply + 1,false);
                    }
                }*/

                //if (iply == 0) {

                  //  std::cout << "rate is = " << m.moves[0].rate << std::endl;
                    //getchar();
                //}


                if (m.n == 0) {
                    continue;
                }

                if (enemy) {
                    //min
                   // rate = (rate < m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)
                    rate = std::min(rate, m.moves[0].rate);

                }
                else {
                    //max

                    //rate = (rate > m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)
                    rate = std::max(rate, m.moves[0].rate);
                }

                // Оценка позиции не лучший ход.
                //17.12.2022 закоментировал rate = m.moves[0].rate; // (best_move)

                if ((captured) && (!enemy)) {
                   // rate += my_depth - depth; // Добавим немного агрессии
                }

                if (enemy) {
                    //min

                    //beta = (rate < beta ? rate : beta);
                    beta = std::min( beta, rate);
                }
                else {
                    // max

                    //if (rate > alpha) alpha = rate;
                    //alpha = (rate > alpha ? rate : alpha);
                    alpha = std::max( alpha, rate);


                }

                //if (depth == 0) std::cout << "\n rate= " << rate << std::endl;

                if (alpha >= beta) {

                    // ОТСЕЧЕНИЕ

#ifndef CODE_FOR_DLL
                    //if (depth == 0) std::cout << "alpha-beta\n";
#endif
                    // rate = beta;// отсечение
                     // Возвратить rate (best_move).


                    rates.n++;

                    if (rates.n - 1 > POOL_SIZE - 1) {
#ifndef CODE_FOR_DLL
                        std::cout << "error! moves limit <POOL_SIZE\n";
                        system("PAUSE");
#endif
                    }

                    rates.moves[rates.n - 1].rate = rate;

                    rates.moves[rates.n - 1].xy_from = xy_from;
                    rates.moves[rates.n - 1].xy_to = xy_to;

                    goto ALPHA_BETA_CONT;
                }


            }



            rates.n++;

            if (rates.n - 1 > POOL_SIZE - 1) {
#ifndef CODE_FOR_DLL
                std::cout << "error! moves limit <POOL_SIZE\n";
                system("PAUSE");
#endif
            }

            rates.moves[rates.n - 1].rate = rate;

            rates.moves[rates.n - 1].xy_from = xy_from;
            rates.moves[rates.n - 1].xy_to = xy_to;

        }


    }
       // }
    //}


    ALPHA_BETA_CONT : // результат отсечения досрочное прерывание сканирования

    if (depth == 0) {

       /* if (fabs(rate) > 9998.0) {
            // Цель игры мат королю противника.
            rates.n = 1;
            rates.moves[0].rate = rate;
            rates.moves[0].xy_from.x = None;
            rates.moves[0].xy_from.y = None;
            rates.moves[0].xy_to.x = None;
            rates.moves[0].xy_to.y = None;
        }*/

        return rates;
    }

#ifndef CODE_FOR_DLL
    if (depth == 1) {
     //   std::cout << "1 BLACK evalute " << rates.n << " " << (rates.n > 0 ? rates.moves[0].rate : 0) << std::endl;
    }
#endif

    if (rates.n == 0) {

        if (fabs(rate) > fabs(rate_flag)) {
            // Цель игры мат королю противника.
            rates.n = 1;
            if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
                // Мы под шахом и нам некуда идти -> мат.
                rates.moves[0].rate = rate;
            }
            else {
                // Некуда идти и мы не под шахом ничья.
                rate = 0.0; // Пат
                rates.moves[0].rate = rate;
            }
            rates.moves[0].xy_from.x = None;
            rates.moves[0].xy_from.y = None;
            rates.moves[0].xy_to.x = None;
            rates.moves[0].xy_to.y = None;
        }

        return rates; // Как бы возврат None так как rates.n == 0.
    }

    LIST_MOVES m;
    m.n = 1;

    {


        __int8 id_found = 0;
        float rate_found = rates.moves[0].rate;

        // Мы находимся на некоторой глубине вложенности

        if (enemy) {

            // Поиск минимума  
            // Враг черные при первом ходе белых или наборот при первом ходе черных.

            for (__int8 i = 1; i < rates.n; ++i) {
                if (rates.moves[i].rate < rate_found) {
                    rate_found = rates.moves[i].rate;
                    id_found = i;
                }
            }

        }
        else {
            // Поиск максимума   
            // Белые  при первом ходе белых или наборот при первом ходе черных.

            for (__int8 i = 1; i < rates.n; ++i) {
                if (rates.moves[i].rate > rate_found) {
                    rate_found = rates.moves[i].rate;
                    id_found = i;
                }
            }
        }

        //if (0) {
            //std::cout << "\n";
            //for (__int8 i = 1; i < rates.n; ++i) {
              //  if (fabs(rates.moves[i].rate - rate_found) < 0.01) {
                //    std::cout << i << " ";
                //}
            //}
            //std::cout << "\n";
        //}


        m.moves[0].rate = rates.moves[id_found].rate;

        // m.moves[0].xy_from=init(None);
         //m.moves[0].xy_to=init(None);

        m.moves[0].xy_from = rates.moves[id_found].xy_from;
        m.moves[0].xy_to = rates.moves[id_found].xy_to;
    }
    if (depth == 1) {
       // std::cout << "2 BLACK evalute " <<  (m.n > 0 ? m.moves[0].rate : 0) << std::endl;
    }
   // system("PAUSE");

    return m;
    }
    else {



    LIST_MOVES rates;

    rates.n = 0;

    //if (rates.n == 0) {

    if (fabs(rate) > fabs(rate_flag)) {
        // Цель игры мат королю противника.
        rates.n = 1;
        if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
            // Мы под шахом и нам некуда идти -> мат.
            rates.moves[0].rate = rate;
        }
        else {
            // Некуда идти и мы не под шахом ничья.
            rate = 0.0; // Пат
            rates.moves[0].rate = rate;
        }
        rates.moves[0].xy_from.x = None;
        rates.moves[0].xy_from.y = None;
        rates.moves[0].xy_to.x = None;
        rates.moves[0].xy_to.y = None;
    }

    return rates; // Как бы возврат None так как rates.n == 0.

    }


    //return ;
}


float fig_rate(Color clr, ChessPiece board[8][8], MOVESL &wList, MOVESL &bList, __int8 x, __int8 y) {

    float rate = 0.0; // Цена пустой клетки

    if (clr == Color::WHITE) {

        if (board[y][x] == ChessPiece::W_PAWN) {
            Pawn p = Pawn(Color::WHITE);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::W_BISHOP) {
            Bishop p = Bishop(Color::WHITE);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::W_KNIGHT) {
            Knight p = Knight(Color::WHITE);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::W_ROOK) {
            Rook p = Rook(Color::WHITE);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::W_QUEEN) {
            Queen p = Queen(Color::WHITE);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::W_KING) {
            King p = King(Color::WHITE);
            rate = p.rate(board, wList, bList, x, y);
        }
    }
    if (clr == Color::BLACK) {

        if (board[y][x] == ChessPiece::B_PAWN) {
            Pawn p = Pawn(Color::BLACK);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::B_BISHOP) {
            Bishop p = Bishop(Color::BLACK);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::B_KNIGHT) {
            Knight p = Knight(Color::BLACK);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::B_ROOK) {
            Rook p = Rook(Color::BLACK);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::B_QUEEN) {
            Queen p = Queen(Color::BLACK);
            rate = p.rate(board, x, y);
        }
        else if (board[y][x] == ChessPiece::B_KING) {
            King p = King(Color::BLACK);
            rate = p.rate(board, wList, bList, x, y);
        }
    }
    return rate;
}


// Форсированный вариант.
// Без сортировки ходов. Порядок взятий не упорядочивается.
float Quies1(float alpha, float beta, Color my_color, Color enemy_color, Board board, __int8 iply) {
    // depth может не менятся на плюс один из-за итеративного углубления.
    // iply всегда меняется на плюс 1.

    bool enemy = ((iply % 2) == 1); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    const float multiplyer = 1.0f; //1.1f;

    // статическая оценка текущей позиции.
    // Если у нас минимакс то максимум то минимум, мы то максимизируем то минимизируем 
    float val= board.rate(my_color) - board.rate(enemy_color) * multiplyer;
    // Если мы переворачиваем и только максимизируем в алгоритме как у Евгения Корнилова.
   // float val =  (color == my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer : board.rate(enemy_color) - board.rate(my_color) * multiplyer);

    if (enemy) {
        //min

        beta = (val < beta ? val : beta);
    }
    else {
        // max

        alpha = (val > alpha ? val : alpha);

    }

    MOVES figList;
    if (color == Color::WHITE) {

        figList.n = board.wList.n;
        for (__int8 k_1 = 0; k_1 < board.wList.n; ++k_1) {
            figList.moves[k_1] = board.wList.moves[k_1];
        }

    }
    if (color == Color::BLACK) {

        figList.n = board.bList.n;
        for (__int8 k_1 = 0; k_1 < board.bList.n; ++k_1) {
            figList.moves[k_1] = board.bList.moves[k_1];
        }
    }


    // Проходим по доске
    //for (__int8 y = 0; y < 8; ++y) {
        //for (__int8 x = 0; x < 8; ++x) {
            //if (get_color1(board.board, x, y) != color) {
                // Если фигура чужая или у нас пустая клетка то переходим к следующей клетки доски.
              //  continue;
            //}

    for (__int8 k_1 = 0; k_1 < figList.n; ++k_1) {

        __int8 y = figList.moves[k_1].y;
        __int8 x = figList.moves[k_1].x;


        MOVES_NODE xy_from;
        xy_from = init(x, y);

#ifndef CODE_FOR_DLL
        //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y)  << std::endl;
#endif

            // получим все позиции перемещения
        MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции [x,y].
        // Проверяем каждый сгенерированный код.
        for (__int8 i = 0; i < moves_current_list.n; ++i) {

            MOVES_NODE xy_to = moves_current_list.moves[i]; // Куда походить фигурой.  

            Board new_board;

            // Копирование доски.
            new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = board.board[y][x];

            // Место куда атаковали не пусто, там стоит вражеская фигура ?
            bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
            if (color == Color::WHITE) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 1) &&
                    (board.previos_moves.xy_to.y == 3) &&
                    (xy_from.y == 3) && (xy_to.y == 2)) {
                    captured = true; // взятие на проходе.                   
                }
            }
            if (color == Color::BLACK) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 6) &&
                    (board.previos_moves.xy_to.y == 4) &&
                    (xy_from.y == 4) && (xy_to.y == 5)) {
                    captured = true; // взятие на проходе.                   
                }
            }

            if (captured) {

                val = Quies1(alpha, beta, my_color, enemy_color, new_board, iply + 1);

                if (enemy) {
                    //min

                    beta = (val < beta ? val : beta);
                }
                else {
                    // max

                    alpha = (val > alpha ? val : alpha);

                }

            }

        }


    }
        //}
    //}

    if (enemy) {
        //min

        return beta;
    }
    else {
        // max

        return  alpha;

    }

}

// 11.08.2023 Решил сделать по образцу как у Евгения Корнилова.
float Evaluate(Color my_color, Color enemy_color, Board board, __int8 iply) {

    bool enemy = ((iply % 2) == 1); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    const float multiplyer = 1.0f; //1.1f;

    // Если мы только максимизируем как у Евгения Корнилова. Т.е. мы каждый раз переворачиваем и только максимизируем.
    float val = (color == my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer : board.rate(enemy_color) - board.rate(my_color) * multiplyer);

    return val;
}





// Форсированный вариант.
// С сортировкой ходов (взятий): наиболее ценная жертва - наименее ценный нападающий (MVV/LVA).
// 11.08.2023 Решил сделать по образцу как у Евгения Корнилова.
float QuiesEvgenyiKornilov(float alpha, float beta, Color my_color, Color enemy_color, Board board, __int8 iply) {
    // depth может не менятся на плюс один из-за итеративного углубления.
    // iply всегда меняется на плюс 1.

    bool enemy = ((iply % 2) == 1); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    const float multiplyer = 1.0f; //1.1f;

    // статическая оценка текущей позиции.
    // Если у нас минимакс то максимум то минимум, мы то максимизируем то минимизируем
    //float val = board.rate(my_color) - board.rate(enemy_color) * multiplyer;
    // Если мы только максимизируем как у Евгения Корнилова. Т.е. мы каждый раз переворачиваем и только максимизируем.
    //float val = (color == my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer : board.rate(enemy_color) - board.rate(my_color) * multiplyer);
    float val = Evaluate(my_color, enemy_color, board, iply); // Статическая оценка текущей позиции.

    // max
    if (val > alpha) alpha = val;

    if (alpha >= beta) return alpha; // alpha-beta отсечение.

    MOVES figList;

    int n_01 = 0;
    MOVES moves_current_list[POOL_SIZEL];

    if (color == Color::WHITE) {

        figList.n = board.wList.n;
        for (int k_1 = 0; k_1 < board.wList.n; ++k_1) {
            figList.moves[k_1] = board.wList.moves[k_1];
        }

        for (int k_1 = 0; k_1 < figList.n; ++k_1) {

            __int8 y = figList.moves[k_1].y;
            __int8 x = figList.moves[k_1].x;

            // получим все позиции перемещения
            moves_current_list[k_1] = board.get_moves_white(x, y);//список возможных ходов из позиции [x,y].

            n_01 += moves_current_list[k_1].n;
        }

    }
    if (color == Color::BLACK) {

        figList.n = board.bList.n;
        for (int k_1 = 0; k_1 < board.bList.n; ++k_1) {
            figList.moves[k_1] = board.bList.moves[k_1];
        }

        for (int k_1 = 0; k_1 < figList.n; ++k_1) {

            __int8 y = figList.moves[k_1].y;
            __int8 x = figList.moves[k_1].x;

            // получим все позиции перемещения
            moves_current_list[k_1] = board.get_moves_black(x, y);//список возможных ходов из позиции [x,y].

            n_01 += moves_current_list[k_1].n;
        }
    }


   

   

    LIST_NODE_MOVES make_moves[POOL_SIZE];
    int i_01 = 0;
    for (int k_1 = figList.n - 1; k_1 >= 0; --k_1) {

        __int8 y = figList.moves[k_1].y;
        __int8 x = figList.moves[k_1].x;

        MOVES_NODE xy_from;
        xy_from.x = x;
        xy_from.y = y;

        // получим все позиции перемещения
        //MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции [x,y].
         // Проверяем каждый сгенерированный код.
        for (int i = 0; i < moves_current_list[k_1].n; ++i) {

            MOVES_NODE xy_to = moves_current_list[k_1].moves[i]; // Куда походить фигурой.  

            // Место куда атаковали не пусто, там стоит вражеская фигура ?
            bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
            bool eat_Pawn_exit = false;
            bool bPawn2Queen = false;
            if (color == Color::WHITE) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                    (xy_to.y == 0)) {
                    // пешка стала ферзём.
                    bPawn2Queen = true;
                    captured = true;
                }

                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 1) &&
                    (board.previos_moves.xy_to.y == 3) &&
                    (xy_from.y == 3) && (xy_to.y == 2)) {
                    captured = true; // взятие на проходе.
                    eat_Pawn_exit = true;

                }
            }
            if (color == Color::BLACK) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                    (xy_to.y == 7)) {
                    // пешка стала ферзём.
                    bPawn2Queen = true;
                    captured = true;
                }
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 6) &&
                    (board.previos_moves.xy_to.y == 4) &&
                    (xy_from.y == 4) && (xy_to.y == 5)) {
                    captured = true; // взятие на проходе.
                    eat_Pawn_exit = true;

                }
            }

            // Накапливаем только взятия
            if (captured) {
                make_moves[i_01].xy_from.x = x;
                make_moves[i_01].xy_from.y = y;
                make_moves[i_01].xy_to.x = xy_to.x;
                make_moves[i_01].xy_to.y = xy_to.y;
                //MVV/LVA наиболее ценная жертва наименее ценный нападающий.
                // Ценность того что съели. Чем больше тем лучше.
                float rate_eat_figure;
                if (eat_Pawn_exit) {
                    // Взятие на проходе
                    rate_eat_figure = fig_rate(get_color1(board.board, board.previos_moves.xy_to.x, board.previos_moves.xy_to.y),
                        board.board, board.wList, board.bList, board.previos_moves.xy_to.x, board.previos_moves.xy_to.y);
                }
                else {
                    // Ценность того что съели. Чем больше тем лучше.
                    rate_eat_figure = fig_rate(get_color1(board.board, xy_to.x, xy_to.y), board.board, board.wList, board.bList, xy_to.x, xy_to.y);

                }
                if (bPawn2Queen) {
                    Queen q;
                    rate_eat_figure = q.get_value();// Приобрели ферзя.
                }
                // Стоимость того чем едим.
                float rate_fig = 1000.0f - fig_rate(get_color1(board.board, x, y), board.board, board.wList, board.bList, x, y);
                make_moves[i_01].rate = rate_eat_figure * 1000.0f + rate_fig;// Ключ для сортировки по убыванию.
                i_01++;
            }
        }

    }
    // Сортировка взятий по убыванию.
     //MVV/LVA наиболее ценная жертва наименее ценный нападающий.
    for (int k_1 = 0; k_1 < i_01; ++k_1) {
        for (int l_1 = 1; l_1 < i_01; ++l_1) {
            if (make_moves[l_1 - 1].rate < make_moves[l_1].rate) {
                //swap
                LIST_NODE_MOVES tmp = make_moves[l_1 - 1];
                make_moves[l_1 - 1] = make_moves[l_1];
                make_moves[l_1] = tmp;
            }
        }
    }

    // Рассмотрим только взятия   
    for (int k_1 = 0; k_1 < i_01; ++k_1)
    {
        __int8 y = make_moves[k_1].xy_from.y;
        __int8 x = make_moves[k_1].xy_from.x;

        MOVES_NODE xy_from;
        xy_from = init(x, y);

        MOVES_NODE xy_to;
        xy_to.x = make_moves[k_1].xy_to.x;
        xy_to.y = make_moves[k_1].xy_to.y;

        Board new_board;

        // Копирование доски.
        new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

        new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

        // Предыдущий ход на этой новой  доске после хода.
        new_board.previos_moves.xy_from = xy_from;
        new_board.previos_moves.xy_to = xy_to;
        new_board.previos_moves.figure = board.board[y][x];

           
        val = -QuiesEvgenyiKornilov(-beta, -alpha, my_color, enemy_color, new_board, iply + 1);

        if (val > alpha) alpha = val;

        if (alpha >= beta) return alpha; // alpha-beta отсечение.
    
    }
  

    if (n_01 == 0) {

        if ((eatKing(board)) ||
            (InCheck(board.board, board.wList, board.bList, board.previos_moves, color,
                board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O))) {

            // Если некуда идти и король под шахом или король съеден.
            // То мат.

            float rate;

           
            // max
            // На некуда ходить, мы под шахом нам МАТ.
            // Возвращаем нашу минимальную оценку мы же максимайзер (мы max)
            // Нам хорошо что чем больше тем лучше.
            rate = -9999.0f + 10.0f * iply;

            
            return rate;
        }
        else {
            // Идти некуда но король не под шахом.
            return 0.0f; // Пат
        }
    }

        // max
        return  alpha;
}

// Alpha - Beta алгоритм поиска лучшего хода в шахматах.
// 11.08.2023 Решил сделать по образцу как у Евгения Корнилова.
LIST_MOVES AlphaBeta(float alpha, float beta, Color my_color, Color enemy_color,
    __int8 my_depth, Board board, __int8 depth, __int8 iply, __int8 ishah, bool bprincipal_variation_search,
    bool doNullMove, bool isExtensiens, int tid, bool captured_see_memo, bool captured_memo, int i_taxi_count, int jKing_security_count) {

    // depth может не менятся на плюс один из-за итеративного углубления.
    // iply всегда меняется на плюс 1.

    number_chess_positions_arr[tid] += 1;

    bool enemy = ((iply % 2) == 1); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    // Если шах то мы продлеваем все ветки на глубину 1.
    //if (InCheck(board.board, board.previos_moves, (color))) depth-=2;

    // depth - глубина просчёта он нуля.

    // Futility pruning 23.09.2023    
    if (Futility_pruning &&((THINKING_DEPTH == 8)||(THINKING_DEPTH == 6))) {
        // Автор идеи - Эрнст Хайнц, применивший её в своей программе DarkThough.

        // Норберт Винер 8 полуходов

        // Если два последних полухода
        if (depth >= my_depth - 2) {

            if (!captured_memo) {// не взятие

                // Узел не расширяется и не нулевой ход.
                // не взятие и не шах.
                if ((!doNullMove) && (!isExtensiens)) {

                    float evalute23 = Evaluate(my_color, enemy_color, board, iply); // Статическая оценка текущей позиции.


                    Pawn pawn_probe; // Пешка цена которой используется.

                    const float margin = 0.5f * pawn_probe.VALUE; // Пол пешки, вес пешки в программе 10.0f

                    if (evalute23 - margin >= beta)
                    {

                        LIST_MOVES m;
                        m.n = 1;

                        // Терминальный вызов в листе.
                        m.moves[0].rate = beta;
                        m.moves[0].xy_from = init(None);
                        m.moves[0].xy_to = init(None);

                        return m;

                    }
                }
            }
        }
    }


    // Razoring 23.09.2023    
    if (Razoring && ((THINKING_DEPTH == 8) )) {
       

        // Норберт Винер 8 полуходов

        // Если два последних полухода
        if (depth >= my_depth - 4) {

            if (!captured_memo) {// не взятие

            // Узел не расширяется и не нулевой ход.
            // не взятие и не шах.
                if ((!doNullMove) && (!isExtensiens)) {

                    float evalute23 = Evaluate(my_color, enemy_color, board, iply); // Статическая оценка текущей позиции.


                    Pawn Queen_probe; // Королева цена которой используется.

                    const float margin = 1.0f * Queen_probe.VALUE; // Цена Королевы

                    if (evalute23 - margin >= beta)
                    {

                        depth++; // Сокращаем глубину поиска.

                        /*LIST_MOVES m;
                        m.n = 1;

                        // Терминальный вызов в листе.
                        m.moves[0].rate = beta;
                        m.moves[0].xy_from = init(None);
                        m.moves[0].xy_to = init(None);

                        return m;
                        */
                    }
                }
            }
        }
    }

    // если исчерпан лимит глубины - вернём статическую оценку позиции.
    if ((depth >= my_depth) || (eatKing(board)) || (iply >= 20)) {

        // Либо съели короля либо достигли глубины перебора.

        // Считает материал с точки зрения белых,
        // она суммирует вес всех белых фигур, а 
        // потом вычитает из них вес всех чёрных фигур.
        // Оценка для Чёрных наоборот.

        LIST_MOVES m;
        m.n = 1;
        //const float multiplyer = 1.0f; //1.1f;

        // Вызывалось для my_color.
        // статическая оценка.
        // Если у нас минимакс то максимум то минимум, мы то максимизируем то минимизируем
        // m.moves[0].rate=Evaluate(my_color, enemy_color, board, iply); // Статическая оценка текущей позиции.
        
        // Терминальный вызов в листе.
        m.moves[0].rate = QuiesEvgenyiKornilov(alpha, beta, my_color, enemy_color, board, iply);
        m.moves[0].xy_from = init(None);
        m.moves[0].xy_to = init(None);

        return m;
    }

    int inCheck_diapazon_Start = 0, inCheck_diapazon_End = 0; // диапазон тихих ходов в которых объявлен шах.

    // Вычисляем коническую оценку.
    // 256 - Окончание партии
    // 0 - дебют партии.
    float phase = calculation_phase_for_conic_rate(board.board, board.wList, board.bList); // фаза коническая оценка.


    // max
    float rate = -9999.0f + 10.0f * iply;
    float rate_flag = rate + 1.0f;
   
    MOVES figList;

    LIST_MOVES rates;
    rates.n = 0;

    int n_01 = 0;
    MOVES moves_current_list[POOL_SIZEL];

    LIST_NODE_MOVES make_moves[POOL_SIZE];

    int i_01 = 0, i_02 = 0;

    bool bnull_move = false;

    //null move
    // Нельзя применять в эндшпиле (точнее при цукцванге), нужно сравнение с конической оценкой.
    // Цукцванг очень часто возникает в эндшпиле.
    if ((!doNullMove) && (!isExtensiens)) {

        // Пустой ход делается не более одного раза в ветке.
        // Эвристика пустого хода не прменяется при шахе.

        if ((THINKING_DEPTH == 6) || (THINKING_DEPTH == 7) || (THINKING_DEPTH == 8) || (THINKING_DEPTH == 10))
        {
            LIST_MOVES m;
            m.n = 0;

            Board new_board;

            // Копирование доски.
            new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

           
            if (phase < KING_CENTER_PRIORITY_WEIGHT) {
                // начало или середина партиии, не эндшпиль - коническая оценка.

                // +2 == R коэффициент затиухания (для THINKING_DEPTH == 6).
                // +4 == R коэффициент затиухания (для THINKING_DEPTH == 8).

                bool bExtensions00 = false;

                // bool bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                  //   new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O);

                 //m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, (
                 //captured_see  || оставить закоментированным
                 //bExtensions0  ? depth + 2 : depth + 1 + 2), iply + 1, ishah + 1, false, bExtensions0);


                bool  doNullMove0 = true;
                // Полное окно.
                // m = AlphaBeta(-beta, -alpha, my_color, enemy_color, my_depth, new_board, (THINKING_DEPTH == 6 ? depth + 1 + 2 : depth + 1 + 4), iply + 1, ishah + 1, false, doNullMove0, bExtensions00, tid);

                // Нулевое окно. 21.06.2023.
                // Небольшое ускорение за счёт нулевого окна с 286с до 266с. 7% времени сократилось только за счёт нулевого окна.
                // Допуск одна тысячная пешки 0.01f. Т.к. цена пешки в моей программе 10.0f.
                // Мы не сгенерировали своего перемещения и дали опоненту фактически сделать ход два раза.
                

                 m = AlphaBeta(-beta, -(beta-0.01f), my_color, enemy_color, my_depth, new_board, (THINKING_DEPTH == 6 ? depth + 1 + 2 : depth + 1 + 4), iply + 1, ishah + 1, false, doNullMove0, bExtensions00, tid, false, false, i_taxi_count, jKing_security_count);

                for (int i_9 = 0; i_9 < m.n; ++i_9) m.moves[i_9].rate *= -1.0f;

                // Мы отдали ход противнику, противник сделал два хода. Если наша оценка после пропуска хода всё ещё хороша то 
                // это беспереспектиная ветвь дерева перебора.
               
                    //max


                    //rate = std::max(rate, m.moves[0].rate);

                   // 18.июля.2023
                   // Внимание никакой сортировки здесь ненадо!!! Она ломает программу, мы отдаем ферзя например за так на уровне Каисса.


                    if (m.moves[0].rate >= beta) {



#ifndef CODE_FOR_DLL
                        //  if (depth == 0) std::cout << "alpha-beta\n";
#endif
                        // rate = beta;// отсечение
                         // Возвратить rate (best_move).


                        rates.n=1;



                        //rates.moves[rates.n - 1].rate = rate;
                        //rates.moves[rates.n - 1].rate = std::max(rate, m.moves[0].rate);

                        rates.moves[0].rate = beta;

                        MOVES_NODE null_move;
                        null_move.x = None;
                        null_move.y = None;

                        rates.moves[0].xy_from = null_move;
                        rates.moves[0].xy_to = null_move;

                        bnull_move = true;

                        goto ALPHA_BETA_CONT_NM;

                    }
               

            }
        }
    }


    // Если эвристика нулевого хода сработала (отсечение произошло), то мы даже пропускаем генерацию и сортировку ходов, это тоже должно экономить время поиска лучшео хода.
    GenerateAllMoves(figList, make_moves, moves_current_list, color, board, n_01, i_01, i_02, my_color, enemy_color, my_depth, enemy, iply, false, bprincipal_variation_search, rate);

    

    {



        if ((killer_euristick[tid][iply].xy_from.x != None) && (killer_euristick[tid][iply].xy_from.y != None) &&
            (killer_euristick[tid][iply].xy_to.x != None) && (killer_euristick[tid][iply].xy_to.y != None)) {

            bool bcontinue_killer = true;
            // Убийственный ход найден в первой части и так сильных ходов (найден среди взятий).
            // Поэтому его не нужно двигать вперед, он и так впереди.
            for (int k_1 = 0; k_1 < i_02; ++k_1)
            {
                if ((make_moves[k_1].xy_from.x == killer_euristick[tid][iply].xy_from.x) &&
                    (make_moves[k_1].xy_from.y == killer_euristick[tid][iply].xy_from.y) &&
                    (make_moves[k_1].xy_to.x == killer_euristick[tid][iply].xy_to.x) &&
                    (make_moves[k_1].xy_to.y == killer_euristick[tid][iply].xy_to.y))
                {
                    bcontinue_killer = false;
                    break;
                }
            }

            if (bcontinue_killer && (n_01 > 0)) {
                int ifound_killer = -1;
                // Сильный ход может быть только тихим ходом. Все взятия остаются первыми.
                // С номера i_02 начинается тихий ход.
                for (int k_1 = i_02; k_1 < i_01; ++k_1)
                {
                    if ((make_moves[k_1].xy_from.x == killer_euristick[tid][iply].xy_from.x) &&
                        (make_moves[k_1].xy_from.y == killer_euristick[tid][iply].xy_from.y) &&
                        (make_moves[k_1].xy_to.x == killer_euristick[tid][iply].xy_to.x) &&
                        (make_moves[k_1].xy_to.y == killer_euristick[tid][iply].xy_to.y))
                    {
                        ifound_killer = k_1;
                        break;
                    }
                }
                // на первое место ставим лучший ход.
                if (ifound_killer > 0) {
                    for (int k_1 = ifound_killer; k_1 > i_02; --k_1)
                    {
                        make_moves[k_1] = make_moves[k_1 - 1];
                    }
                    make_moves[i_02].rate = 0.0;
                    make_moves[i_02].xy_from.x = killer_euristick[tid][iply].xy_from.x;
                    make_moves[i_02].xy_from.y = killer_euristick[tid][iply].xy_from.y;
                    make_moves[i_02].xy_to.x = killer_euristick[tid][iply].xy_to.x;
                    make_moves[i_02].xy_to.y = killer_euristick[tid][iply].xy_to.y;
                }
            }

            inCheck_diapazon_Start = i_02;
            if (bcontinue_killer) ++inCheck_diapazon_Start;
            // Есть идея продвинуть шахи вперёд среди оставшихся ходов.
            // Но их наличие очень ресурсоёмко вычислить.

            if (1) {
                bool bcheck_[POOL_SIZE];
                for (int k_1 = inCheck_diapazon_Start; k_1 < i_01; ++k_1)
                {
                    bcheck_[k_1] = false;
                }
                for (int k_1 = inCheck_diapazon_Start; k_1 < i_01; ++k_1)
                {

                    __int8 y = make_moves[k_1].xy_from.y;
                    __int8 x = make_moves[k_1].xy_from.x;

                    MOVES_NODE xy_from;
                    xy_from = init(x, y);

                    MOVES_NODE xy_to;

                    xy_to.x = make_moves[k_1].xy_to.x;
                    xy_to.y = make_moves[k_1].xy_to.y;

                    Board new_board;

                    // Копирование доски.
                    new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                    new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                    // Предыдущий ход на этой новой  доске после хода.
                    new_board.previos_moves.xy_from = xy_from;
                    new_board.previos_moves.xy_to = xy_to;
                    new_board.previos_moves.figure = board.board[y][x];

                    bcheck_[k_1] = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                        new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O);
                }
                inCheck_diapazon_End = inCheck_diapazon_Start;
                // Продвигаем все шахи вперед в списке ходов.
                // Шахи находятся после взятий и после эвристики нулевого хода.
                for (int k_1 = inCheck_diapazon_Start; k_1 < i_01; ++k_1)
                {
                    if (bcheck_[k_1]) {
                        LIST_NODE_MOVES tmpM = make_moves[k_1];
                        for (int k_2 = k_1 - 1; k_2 >= inCheck_diapazon_End; --k_2)
                        {
                            make_moves[k_2 + 1] = make_moves[k_2];
                        }
                        if (k_1 != inCheck_diapazon_End) {
                            make_moves[inCheck_diapazon_End] = tmpM;
                            ++inCheck_diapazon_End;
                        }
                    }
                }
            }


        }
    }

    if (n_01 > 0) {

        if (n_01 > POOL_SIZE) {
            printf("increase POOL_SIZE %d %d\n", n_01, POOL_SIZE);
            system("PAUSE");
        }


        //system("PAUSE");

        // Сканируем ходы
        //for (__int8 k_1 = 0; k_1 < figList.n; ++k_1)
        for (int k_1 = 0; k_1 < i_01; ++k_1)
        {


            __int8 y = make_moves[k_1].xy_from.y;
            __int8 x = make_moves[k_1].xy_from.x;

            MOVES_NODE xy_from;
            xy_from = init(x, y);



            // получим все позиции перемещения
       // MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции [x,y].
        // Проверяем каждый сгенерированный код.
        //for (__int8 i = 0; i < moves_current_list.n; ++i) 
            {

                //MOVES_NODE xy_to = moves_current_list.moves[i]; // Куда походить фигурой.               

                MOVES_NODE xy_to;

                xy_to.x = make_moves[k_1].xy_to.x;
                xy_to.y = make_moves[k_1].xy_to.y;

#ifndef CODE_FOR_DLL
                //if (iply == 0) {
                    //-->printf("%d %d iply = 0 from  %d %d to %d %d\n", k_1, n_01, (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));
                    //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
                    //board.print();
                    //system("PAUSE");
                //}
#endif

                Board new_board;

                // Копирование доски.
                new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                // Предыдущий ход на этой новой  доске после хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = board.board[y][x];

                float rate_Current;


#ifndef CODE_FOR_DLL
                if (0 && (iply == 0)) {
                    //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
                   // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                    printf("iply = 0 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                    new_board.print();

                    system("PAUSE");
                }
                if (0 && (depth == 1)) {
                    //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
                   // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                    printf("depth = 1 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                    new_board.print();

                    system("PAUSE");
                }
#endif

                // Место куда атаковали не пусто, там стоит вражеская фигура ?
                bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
                bool captured_see = false;
                // true цена того что съели больше или равна цене того чем съели.
                if (captured) captured_see = (rate_fig(board.board[xy_to.y][xy_to.x]) >= rate_fig(new_board.board[xy_to.y][xy_to.x]) ? true : false);
                if (color == Color::WHITE) {
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                        (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                        (xy_to.x == board.previos_moves.xy_to.x) &&
                        (xy_to.x == board.previos_moves.xy_from.x) &&
                        ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                        (board.previos_moves.xy_from.y == 1) &&
                        (board.previos_moves.xy_to.y == 3) &&
                        (xy_from.y == 3) && (xy_to.y == 2)) {
                        captured = true; // взятие на проходе.
                        captured_see = true;

                    }
                }
                if (color == Color::BLACK) {
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                        (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                        (xy_to.x == board.previos_moves.xy_to.x) &&
                        (xy_to.x == board.previos_moves.xy_from.x) &&
                        ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                        (board.previos_moves.xy_from.y == 6) &&
                        (board.previos_moves.xy_to.y == 4) &&
                        (xy_from.y == 4) && (xy_to.y == 5)) {
                        captured = true; // взятие на проходе.
                        captured_see = true;
                    }
                }


                // Сьели Короля.
                //if (captured && (board.board[xy_to.y][xy_to.x] == ChessPiece::B_KING) || (board.board[xy_to.y][xy_to.x] == ChessPiece::W_KING)) {

                  //  rate = ((enemy) ? -1000.0f : 1000.0f); // Вражеский король сьеден (мат).
                //}
            //    else
                {
                    LIST_MOVES m;
                    m.n = 0;

                    //alpha - наш максимум.
                    // beta - максимум противника.

                    bool bExtensions0 = false;

                    bool isTaxi = false;
                    // Включаем продление при опасном сокращении расстояния от нашего ферзя до Короля противника.

                    if ((color == Color::WHITE) && (board.board[y][x] == ChessPiece::W_QUEEN)) {
                        char kingX, kingY;
                        for (int i_07 = 0; i_07 < new_board.bList.n; ++i_07) {
                            if (new_board.board[new_board.bList.moves[i_07].y][new_board.bList.moves[i_07].x] == ChessPiece::B_KING) {
                                kingX = new_board.bList.moves[i_07].x;
                                kingY = new_board.bList.moves[i_07].y;

                                float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));

                                if (new_taxi < 4.0) {
                                    // Попали в квадрат Короля.
                                    float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                    if (new_taxi < old_taxi) isTaxi = true;
                                    if ((new_taxi == old_taxi) && (xy_to.x + xy_to.y < xy_from.x + xy_from.y)) isTaxi = true;

                                }
                                break;
                            }
                        }
                    }

                    if ((color == Color::BLACK) && (board.board[y][x] == ChessPiece::B_QUEEN)) {
                        char kingX, kingY;
                        for (int i_07 = 0; i_07 < new_board.wList.n; ++i_07) {
                            if (new_board.board[new_board.wList.moves[i_07].y][new_board.wList.moves[i_07].x] == ChessPiece::W_KING) {
                                kingX = new_board.wList.moves[i_07].x;
                                kingY = new_board.wList.moves[i_07].y;

                                float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));

                                if (new_taxi < 4.0) {
                                    float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                    if (new_taxi < old_taxi) isTaxi = true;
                                    if ((new_taxi == old_taxi) && (xy_to.x + xy_to.y < xy_from.x + xy_from.y)) isTaxi = true;
                                }
                                break;
                            }
                        }
                    }

                    if (iKingSecurity == 0) {
                        isTaxi = false;
                    }

                    bool isSquareKingActivity = false;
                    // Включаем продления если детектированы действия в квадрате Короля.

                    {
                        char kingX, kingY;
                        for (int i_07 = 0; i_07 < new_board.bList.n; ++i_07) {
                            if (new_board.board[new_board.bList.moves[i_07].y][new_board.bList.moves[i_07].x] == ChessPiece::B_KING) {
                                kingX = new_board.bList.moves[i_07].x;
                                kingY = new_board.bList.moves[i_07].y;

                                // Попали в квадрат Короля.

                                float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));


                                float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                if (new_taxi < 1.47) isSquareKingActivity = true;
                                if (old_taxi < 1.47) isSquareKingActivity = true;

                                break;
                            }
                        }
                    }

                    {
                        char kingX, kingY;
                        for (int i_07 = 0; i_07 < new_board.wList.n; ++i_07) {
                            if (new_board.board[new_board.wList.moves[i_07].y][new_board.wList.moves[i_07].x] == ChessPiece::W_KING) {
                                kingX = new_board.wList.moves[i_07].x;
                                kingY = new_board.wList.moves[i_07].y;

                                // Попали в квадрат Короля.

                                float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));


                                float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                if (new_taxi < 1.47) isSquareKingActivity = true;
                                if (old_taxi < 1.47) isSquareKingActivity = true;

                                break;
                            }
                        }
                    }

                    bool bMiddleGame = true;// начало или середина игры ? 
                    // 10.08.2023  В эндшпиле отключаются все продления кроме шахов.
                    //const float conic_rate = calculation_phase_for_conic_rate(board.board, board.wList, board.bList);// Коническая оценка.
                    const float conic_rate = phase;
                    if (conic_rate > KING_CENTER_PRIORITY_WEIGHT) {
                        // эндшпиль.
                        bMiddleGame = false;
                    }


                    if (THINKING_DEPTH <= 4) {
                        if (bQueekGame) {

                            // Продление при шахе, а также если ответных ходов один, два или три всего.
                            if (k_1 < inCheck_diapazon_Start) {
                                bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                    new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O);// || (n_01 < 1);
                            }
                            else {
                                bExtensions0 = ((k_1 >= inCheck_diapazon_Start) && (k_1 < inCheck_diapazon_End));// Экономичное определение шаха.
                            }
                        }
                        else {
                            // 07.08.2023 На depth 4 анализ занимает 2с.
                            // Продление при шахе, а также если всего только один ответный ход + продление при выгодном взятии фигуры (до глубины шестёрка). 
                            bExtensions0 = (k_1 < inCheck_diapazon_Start ? InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O) :
                                ((k_1 >= inCheck_diapazon_Start) && (k_1 < inCheck_diapazon_End))) // Экономичное определение шаха.
                                || ((bMiddleGame) && ((n_01 <= 1) || ((n_01 < 3) && (iply < 7)) || (iply < 0 ? captured_see /* || captured_see_memo */ : false) ||// Включение capture see побуждает брать фигуры.
                                    // Белая пешка на последней горизонтали.
                                    ((color == Color::WHITE) && (board.board[y][x] == ChessPiece::W_PAWN) && (xy_to.y == 1)) ||
                                    // Чёрная пешка на последней горизонтали.
                                    ((color == Color::BLACK) && (board.board[y][x] == ChessPiece::B_PAWN) && (xy_to.y == 6)) ||
                                    (iply < 0 ? isSquareKingActivity : false)));

                            // продлеваем при шахе до хода, после хода. Продлеваем если только два ответа. Продлеваем если выгодно съели фигуру.
                           /*bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                               new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O) ||
                               InCheck(board.board, board.wList, board.bList, board.previos_moves, color,
                                   board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O) || (n_01 <= 2) || captured_see || captured_see_memo;*/
                        }
                    }
                    else {

                        if (bQueekGame) {
                            // Продление только при шахе.
                            if (k_1 < inCheck_diapazon_Start) {
                                bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                    new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O);
                            }
                            else {
                                bExtensions0 = ((k_1 >= inCheck_diapazon_Start) && (k_1 < inCheck_diapazon_End)); // Экономичное определение шаха.
                            }
                        }
                        else {
                            // Продление только при шахе.
                            bExtensions0 = (k_1 < inCheck_diapazon_Start ? InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O) :
                                ((k_1 >= inCheck_diapazon_Start) && (k_1 < inCheck_diapazon_End))) || // Экономичное определение шаха.
                                ((bMiddleGame) && ((n_01 <= 1)||((n_01<3)&&(iply<7)) || (iply < 0 ? captured_see : false) ||// Включение capture see побуждает брать фигуры.
                                    // Белая пешка (фигура которой ходили) на последней горизонтали.
                                    ((color == Color::WHITE) && (board.board[y][x] == ChessPiece::W_PAWN) && (xy_to.y == 1)) ||
                                    // Чёрная пешка на последней горизонтали.
                                    ((color == Color::BLACK) && (board.board[y][x] == ChessPiece::B_PAWN) && (xy_to.y == 6))
                                    || (iply < 0 ? isSquareKingActivity : false)));
                        }
                    }


                    // Principial Variation Search
                    //if (k_1 < 4) 
                    {

                        //m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1, ishah + 1, false, doNullMove, false);

                        // depth - 1 слишком глубокое дерево получается очень долго считает варианты.
                        // Продлевать взятия нельзя программа очень долго думает видно взятий очень много.


                        m = AlphaBeta(-beta, -alpha, my_color, enemy_color, my_depth, new_board,
                            (/*captured_see  ||*/ bExtensions0 || ((bMiddleGame) && (i_taxi_count < 4 ? isTaxi : false)) ? depth : depth + 1), iply + 1, ishah + 1,
                            false, doNullMove, bExtensions0 || ((bMiddleGame) && (i_taxi_count < 4 ? isTaxi : false)), tid, captured_see, captured, (isTaxi ? i_taxi_count + 1 : i_taxi_count),
                            (isSquareKingActivity ? jKing_security_count + 1 : jKing_security_count));

                        for (int i_9 = 0; i_9 < m.n; ++i_9) m.moves[i_9].rate *= -1.0f; // Меняем знак.

                    }
                    /*else {

                        // Быстрый просчёт с нулевым окном
                        if (enemy) {
                            //min

                            m = alpha_beta_do1(beta - 1.0f, beta, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1, ishah + 1, true, doNullMove, bExtensions0);
                        }
                        else {
                            //max
                            m = alpha_beta_do1(alpha, alpha + 1.0f, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1, ishah + 1,true, doNullMove, bExtensions0);
                        }


                        // если эвристика не сработала, и ход улучшает
                        // alpha, то мы пересчитаем с полным окном
                        if ((m.moves[0].rate > alpha) && (m.moves[0].rate < beta)) {

                            Board new_board2;

                            // Копирование доски.
                            new_board2.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                            new_board2.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                             // Предыдущий ход на этой новой  доске после хода.
                            new_board2.previos_moves.xy_from = xy_from;
                            new_board2.previos_moves.xy_to = xy_to;
                            new_board2.previos_moves.figure = board.board[y][x];


                            m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board2, depth + 1, iply + 1, ishah + 1,false, doNullMove, bExtensions0);
                        }
                    }*/


                    // Было взятие - итеративное углубление.
                    // captured - Он почемуто жертвует фигуры (выбрасывает фигуры). Коня за пешку.
                    // Попробуем captured_see только равные или победившие взятия.
                    //m = alpha_beta_do1(alpha, beta,   my_color, enemy_color, my_depth, new_board, (captured_see ? depth : depth + 1),iply+1);
                    //  детектор шахов InCheck(new_board.board, new_board.previos_moves, color, doNullMove)
                    // В малофигурном окончании правило captured_see неверно и заменено на captured так как Король тоже может есть.
                   // m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, 
                     //   (captured_see|| InCheck(new_board.board, new_board.previos_moves, color)||
                       //     (captured&& number_figures(new_board.board)<=ENDSHPIL_NUMBER_FIGURES) ? depth : depth + 1), iply + 1, doNullMove);
                    // Нам же не нужно чтобы он слона за пешку отдал. Без SEE он просто отдаёт материал.
                    //Color_invert(color) правильное продление шахов с invert color так как на новой доске и цвет новый,
                    // но продление шахов инициирует всё новые шахи так же как продление взятий инициирует новые взятия иногда неравноценные жертвы
                    // фигуры выбрасываются.
                    //-->m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board,
                        //(captured_see   || InCheck(new_board.board, new_board.previos_moves, Color_invert(color)) ? depth : depth + 1), iply + 1, doNullMove);

                   // m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board,
                     //   (captured_see  || ((ishah==0)&&InCheck(new_board.board, new_board.previos_moves, Color_invert(color))) ? depth -1 : depth + 1),
                       // iply + 1, (InCheck(new_board.board, new_board.previos_moves, Color_invert(color)) ? ishah+1:ishah), doNullMove);

                    rate_Current = m.moves[0].rate;
                    //std::cout << "rate_Current " << m.moves[0].rate<<" alpha = "<<alpha <<"beta = "<< beta << "\n";

                    if (m.n == 0) {
                        continue;
                    }



                    
                        //max

                        // 18.июля.2023
                        // Внимание никакой сортировки здесь ненадо!!! Она ломает программу, мы отдаем ферзя например за так на уровне Каисса.

                        //rate = (rate > m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)
                        rate = std::max(rate, m.moves[0].rate);
             

                        //if (rate > alpha) alpha = rate;
                        //alpha = (rate > alpha ? rate : alpha);
                        alpha = std::max(rate, alpha);


                    

                    //if (depth == 0) std::cout << "\n rate= " << rate << std::endl;

                    if (alpha >= beta) {
#ifndef CODE_FOR_DLL
                        //  if (depth == 0) std::cout << "alpha-beta\n";
#endif
                        // rate = beta;// отсечение
                         // Возвратить rate (best_move).


                        rates.n++;

                        if (rates.n - 1 > n_01 - 1) {
#ifndef CODE_FOR_DLL
                            printf("error! moves limit < n_01\n");
                            system("PAUSE");
#endif
                        }

                        if (iply == 0) {
                            // Корень дерева. Нам нужны все оценки а не только их экстремальное значение.
                            rates.moves[rates.n - 1].rate = rate_Current;
                        }
                        else {
                            rates.moves[rates.n - 1].rate = rate;
                        }


                        rates.moves[rates.n - 1].xy_from = xy_from;
                        rates.moves[rates.n - 1].xy_to = xy_to;

                        goto ALPHA_BETA_CONT;
                    }


                }



                rates.n++;

                if (rates.n - 1 > n_01 - 1) {
#ifndef CODE_FOR_DLL
                    printf("error! moves limit < n_01\n");
                    system("PAUSE");
#endif
                }

                if (iply == 0) {
                    // Корень дерева. Нам нужны все оценки а не только их экстремальное значение.
                    rates.moves[rates.n - 1].rate = rate_Current;
                }
                else {
                    rates.moves[rates.n - 1].rate = rate;
                }


                rates.moves[rates.n - 1].xy_from = xy_from;
                rates.moves[rates.n - 1].xy_to = xy_to;

            }

        }
        // }
     //}



    ALPHA_BETA_CONT: // результат отсечения досрочное прерывание сканирования

        // Освобождение оперативной памяти.
      //  if (make_moves != nullptr) {
        //    delete[] make_moves;
          //  make_moves = nullptr;
        //}

        if (depth == 0) {

            /* if (fabs(rate) > 9998.0) {
                 // Цель игры мат королю противника.
                 rates.n = 1;
                 rates.moves[0].rate = rate;
                 rates.moves[0].xy_from.x = None;
                 rates.moves[0].xy_from.y = None;
                 rates.moves[0].xy_to.x = None;
                 rates.moves[0].xy_to.y = None;
             }*/

             /*std::cout << "iply" << (int)(iply) << " " << (enemy ? "min" : "max") << "\n";
             for (int id_rate = 0; id_rate < rates.n; ++id_rate)
             {
                 std::cout << rates.moves[id_rate].rate << " " << static_cast<char>('a' + rates.moves[id_rate].xy_from.x);
                 std::cout << 8 - rates.moves[id_rate].xy_from.y << static_cast<char>('a' + rates.moves[id_rate].xy_to.x);
                 std::cout << 8 - rates.moves[id_rate].xy_to.y << std::endl;
             }
             std::cout << std::endl;
             getchar();*/

            return rates;
        }

#ifndef CODE_FOR_DLL
        if (depth == 1) {
            //   std::cout << "1 BLACK evalute " << rates.n << " " << (rates.n > 0 ? rates.moves[0].rate : 0) << std::endl;
        }
#endif

        if (rates.n == 0) {

            if (fabs(rate) > fabs(rate_flag)) {
                // Цель игры мат королю противника.
                rates.n = 1;
                if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
                    // Мы под шахом и нам некуда идти -> мат.
                      // max
            // На некуда ходить, мы под шахом нам МАТ.
            // Возвращаем нашу минимальную оценку мы же максимайзер (мы max)
            // Нам хорошо что чем больше тем лучше.
                    rate = -9999.0f + 10.0f * iply;
                    rates.moves[0].rate = rate;
                }
                else {
                    // Некуда идти и мы не под шахом ничья.
                    rate = 0.0; // Пат
                    rates.moves[0].rate = rate;
                }
                rates.moves[0].xy_from.x = None;
                rates.moves[0].xy_from.y = None;
                rates.moves[0].xy_to.x = None;
                rates.moves[0].xy_to.y = None;
            }

            return rates; // Как бы возврат None так как rates.n == 0.
        }

        LIST_MOVES m;
        m.n = 1;

        {


            int id_found = 0;
            float rate_found = rates.moves[0].rate;

            // Мы находимся на некоторой глубине вложенности

           
                // Поиск максимума   
                // Белые  при первом ходе белых или наборот при первом ходе черных.

                for (int i = 1; i < rates.n; ++i) {
                    if (rates.moves[i].rate > rate_found) {
                        rate_found = rates.moves[i].rate;
                        id_found = i;
                    }
                }
            

            //if (0) {
                //std::cout << "\n";
                //for (__int8 i = 1; i < rates.n; ++i) {
                  //  if (fabs(rates.moves[i].rate - rate_found) < 0.01) {
                    //    std::cout << i << " ";
                    //}
                //}
                //std::cout << "\n";
            //}


            m.moves[0].rate = rates.moves[id_found].rate;

            // std::cout <<"iply ="<< (int)(iply) <<" rate found="<< rates.moves[id_found].rate<< (enemy ? "min" : "max") << "\n";

             // m.moves[0].xy_from=init(None);
              //m.moves[0].xy_to=init(None);

            m.moves[0].xy_from = rates.moves[id_found].xy_from;
            m.moves[0].xy_to = rates.moves[id_found].xy_to;

            // Запоминаем эвристику убийцы.
            // 
            // 


            // killer_euristick[tid][iply].rate = 0.0; // не используется
            killer_euristick[tid][iply].xy_from.x = m.moves[0].xy_from.x;
            killer_euristick[tid][iply].xy_from.y = m.moves[0].xy_from.y;
            killer_euristick[tid][iply].xy_to.x = m.moves[0].xy_to.x;
            killer_euristick[tid][iply].xy_to.y = m.moves[0].xy_to.y;
        }
        if (depth == 1) {
            // std::cout << "2 BLACK evalute " <<  (m.n > 0 ? m.moves[0].rate : 0) << std::endl;
        }
        // system("PAUSE");

        return m;
    }
    else {



        LIST_MOVES rates;

        rates.n = 0;

        //if (rates.n == 0) {

        if (fabs(rate) > fabs(rate_flag)) {
            // Цель игры мат королю противника.
            rates.n = 1;
            if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
                // Мы под шахом и нам некуда идти -> мат.
                 // Мы под шахом и нам некуда идти -> мат.
                      // max
            // На некуда ходить, мы под шахом нам МАТ.
            // Возвращаем нашу минимальную оценку мы же максимайзер (мы max)
            // Нам хорошо что чем больше тем лучше.
                rate = -9999.0f + 10.0f * iply;
                rates.moves[0].rate = rate;
            }
            else {
                // Некуда идти и мы не под шахом ничья.
                rate = 0.0; // Пат
                rates.moves[0].rate = rate;
            }
            rates.moves[0].xy_from.x = None;
            rates.moves[0].xy_from.y = None;
            rates.moves[0].xy_to.x = None;
            rates.moves[0].xy_to.y = None;
        }

        return rates; // Как бы возврат None так как rates.n == 0.

    }


ALPHA_BETA_CONT_NM: // результат отсечения досрочное прерывание сканирования

    // Освобождение оперативной памяти.
  //  if (make_moves != nullptr) {
    //    delete[] make_moves;
      //  make_moves = nullptr;
    //}

    if (bnull_move) {

        /* if (fabs(rate) > 9998.0) {
             // Цель игры мат королю противника.
             rates.n = 1;
             rates.moves[0].rate = rate;
             rates.moves[0].xy_from.x = None;
             rates.moves[0].xy_from.y = None;
             rates.moves[0].xy_to.x = None;
             rates.moves[0].xy_to.y = None;
         }*/

         /*  std::cout << "iply 3 " << (int)(iply) << " " << (enemy ? "min" : "max") << "\n";
           for (int id_rate = 0; id_rate < rates.n; ++id_rate)
           {
               std::cout << rates.moves[id_rate].rate << " " << static_cast<char>('a' + rates.moves[id_rate].xy_from.x);
               std::cout << 8 - rates.moves[id_rate].xy_from.y << static_cast<char>('a' + rates.moves[id_rate].xy_to.x);
               std::cout << 8 - rates.moves[id_rate].xy_to.y << std::endl;
           }
           std::cout << std::endl;
           getchar();*/

        return rates;
    }


    std::cout << "AlphaBeta error undefined situation...\n";
    system("pause");

    //return ;
}



// Форсированный вариант.
// С сортировкой ходов (взятий): наиболее ценная жертва - наименее ценный нападающий (MVV/LVA).
float Quies(float alpha, float beta, Color my_color, Color enemy_color, Board board, __int8 iply) {
    // depth может не менятся на плюс один из-за итеративного углубления.
    // iply всегда меняется на плюс 1.

    bool enemy = ((iply % 2) == 1); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    const float multiplyer = 1.0f; //1.1f;

    // статическая оценка текущей позиции.
    // Если у нас минимакс то максимум то минимум, мы то максимизируем то минимизируем
    float val = board.rate(my_color) - board.rate(enemy_color) * multiplyer;
    // Если мы только максимизируем как у Евгения Корнилова. Т.е. мы каждый раз переворачиваем и только максимизируем.
    //float val = (color == my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer : board.rate(enemy_color) - board.rate(my_color) * multiplyer);

    if (enemy) {
        //min

        beta = (val < beta ? val : beta);
    }
    else {
        // max

        alpha = (val > alpha ? val : alpha);

    }

    if (alpha >= beta) return alpha; // alpha-beta отсечение.

    MOVES figList;
    if (color == Color::WHITE) {

        figList.n = board.wList.n;
        for (int k_1 = 0; k_1 < board.wList.n; ++k_1) {
            figList.moves[k_1] = board.wList.moves[k_1];
        }

    }
    if (color == Color::BLACK) {

        figList.n = board.bList.n;
        for (int k_1 = 0; k_1 < board.bList.n; ++k_1) {
            figList.moves[k_1] = board.bList.moves[k_1];
        }
    }


    // Проходим по доске
    //for (__int8 y = 0; y < 8; ++y) {
        //for (__int8 x = 0; x < 8; ++x) {
            //if (get_color1(board.board, x, y) != color) {
                // Если фигура чужая или у нас пустая клетка то переходим к следующей клетки доски.
              //  continue;
            //}

    int n_01 = 0;
    MOVES moves_current_list[POOL_SIZEL];

    for (int k_1 = 0; k_1 < figList.n; ++k_1) {

        __int8 y = figList.moves[k_1].y;
        __int8 x = figList.moves[k_1].x;

        // получим все позиции перемещения
        moves_current_list[k_1] = board.get_moves(x, y);//список возможных ходов из позиции [x,y].

        n_01 += moves_current_list[k_1].n;
    }

    LIST_NODE_MOVES make_moves[POOL_SIZE];
    int i_01 = 0;
    for (int k_1 = figList.n - 1; k_1 >= 0; --k_1) {

        __int8 y = figList.moves[k_1].y;
        __int8 x = figList.moves[k_1].x;

        MOVES_NODE xy_from;
        xy_from.x = x;
        xy_from.y = y;

        // получим все позиции перемещения
        //MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции [x,y].
         // Проверяем каждый сгенерированный код.
        for (int i = 0; i < moves_current_list[k_1].n; ++i) {

            MOVES_NODE xy_to = moves_current_list[k_1].moves[i]; // Куда походить фигурой.  

            // Место куда атаковали не пусто, там стоит вражеская фигура ?
            bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
            bool eat_Pawn_exit = false;
            bool bPawn2Queen = false;
            if (color == Color::WHITE) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                    (xy_to.y == 0)) {
                    // пешка стала ферзём.
                    bPawn2Queen = true;
                    captured = true;
                }

                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 1) &&
                    (board.previos_moves.xy_to.y == 3) &&
                    (xy_from.y == 3) && (xy_to.y == 2)) {
                    captured = true; // взятие на проходе.
                    eat_Pawn_exit = true;

                }
            }
            if (color == Color::BLACK) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                    (xy_to.y == 7)) {
                    // пешка стала ферзём.
                    bPawn2Queen = true;
                    captured = true;
                }
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 6) &&
                    (board.previos_moves.xy_to.y == 4) &&
                    (xy_from.y == 4) && (xy_to.y == 5)) {
                    captured = true; // взятие на проходе.
                    eat_Pawn_exit = true;

                }
            }

            // Накапливаем только взятия
            if (captured) {
                make_moves[i_01].xy_from.x = x;
                make_moves[i_01].xy_from.y = y;
                make_moves[i_01].xy_to.x = xy_to.x;
                make_moves[i_01].xy_to.y = xy_to.y;
                //MVV/LVA наиболее ценная жертва наименее ценный нападающий.
                // Ценность того что съели. Чем больше тем лучше.
                float rate_eat_figure;
                if (eat_Pawn_exit) {
                    // Взятие на проходе
                    rate_eat_figure = fig_rate(get_color1(board.board, board.previos_moves.xy_to.x, board.previos_moves.xy_to.y),
                        board.board, board.wList, board.bList, board.previos_moves.xy_to.x, board.previos_moves.xy_to.y);
                }
                else {
                    // Ценность того что съели. Чем больше тем лучше.
                    rate_eat_figure = fig_rate(get_color1(board.board, xy_to.x, xy_to.y), board.board, board.wList, board.bList, xy_to.x, xy_to.y);

                }
                if (bPawn2Queen) {
                    Queen q;
                    rate_eat_figure = q.get_value();// Приобрели ферзя.
                }
                // Стоимость того чем едим.
                float rate_fig = 1000.0f - fig_rate(get_color1(board.board, x, y), board.board, board.wList, board.bList, x, y);
                make_moves[i_01].rate = rate_eat_figure * 1000.0f + rate_fig;// Ключ для сортировки по убыванию.
                i_01++;
            }
        }

    }
    // Сортировка взятий по убыванию.
     //MVV/LVA наиболее ценная жертва наименее ценный нападающий.
    for (int k_1 = 0; k_1 < i_01; ++k_1) {
        for (int l_1 = 1; l_1 < i_01; ++l_1) {
            if (make_moves[l_1 - 1].rate < make_moves[l_1].rate) {
                //swap
                LIST_NODE_MOVES tmp = make_moves[l_1 - 1];
                make_moves[l_1 - 1] = make_moves[l_1];
                make_moves[l_1] = tmp;
            }
        }
    }

    //delete[]  moves_current_list;

    // Рассмотрим только взятия
    //for (__int8 k_1 = 0; k_1 < figList.n; ++k_1)
    for (int k_1 = 0; k_1 < i_01; ++k_1)
    {


        __int8 y = make_moves[k_1].xy_from.y;
        __int8 x = make_moves[k_1].xy_from.x;
    



        MOVES_NODE xy_from;
        xy_from = init(x, y);

#ifndef CODE_FOR_DLL
        //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y)  << std::endl;
#endif

            // получим все позиции перемещения
        //MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции [x,y].
        // Проверяем каждый сгенерированный код.
        //for (__int8 i = 0; i < moves_current_list.n; ++i) 
        {

          //  MOVES_NODE xy_to = moves_current_list.moves[i]; // Куда походить фигурой.  

            MOVES_NODE xy_to;
            xy_to.x= make_moves[k_1].xy_to.x;
            xy_to.y = make_moves[k_1].xy_to.y;

            Board new_board;

            // Копирование доски.
            new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

            new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

             // Предыдущий ход на этой новой  доске после хода.
            new_board.previos_moves.xy_from = xy_from;
            new_board.previos_moves.xy_to = xy_to;
            new_board.previos_moves.figure = board.board[y][x];

            // Место куда атаковали не пусто, там стоит вражеская фигура ?
            //bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
            /*if (color == Color::WHITE) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 1) &&
                    (board.previos_moves.xy_to.y == 3)&&
                    (xy_from.y==3)&&(xy_to.y==2)) {
                    captured = true; // взятие на проходе.

                }
            }
            if (color == Color::BLACK) {
                if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                    (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                    (xy_to.x == board.previos_moves.xy_to.x) &&
                    (xy_to.x == board.previos_moves.xy_from.x) &&
                    ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                    (board.previos_moves.xy_from.y == 6) &&
                    (board.previos_moves.xy_to.y == 4)&&
                    (xy_from.y == 4) && (xy_to.y == 5)) {
                    captured = true; // взятие на проходе.

                }
            }*/

            //if (captured) 
            {

                val = Quies(alpha, beta, my_color, enemy_color, new_board, iply + 1);

                if (enemy) {
                    //min

                    beta = (val < beta ? val : beta);
                }
                else {
                    // max

                    alpha = (val > alpha ? val : alpha);

                }

                if (alpha >= beta) return alpha; // alpha-beta отсечение.

            }

        }

        

    }
        //}
    //}

    //delete[] make_moves;
    //make_moves = nullptr;

    if (n_01 == 0) {

        if ((eatKing(board)) || (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O))) {

            // Если некуда идти и король под шахом или король съеден.
            // То мат.

            float rate;

            if (enemy) {
                //min
                rate = +9999.0f - 10.0f * iply;

            }
            else {
                // max
                rate = -9999.0f + 10.0f * iply;

            }
            return rate;
        }
        else {
            // Идти некуда но король не под шахом.
            return 0.0f; // Пат
        }
    }

    //delete[] make_moves;

    if (enemy) {
        //min

        return beta;
    }
    else {
        // max

        return  alpha;

    }

}



// alpha - beta алгоритм.
LIST_MOVES alpha_beta_do1(float alpha, float beta, Color my_color, Color enemy_color,
    __int8 my_depth, Board board, __int8 depth, __int8 iply, __int8 ishah, bool bprincipal_variation_search,
    bool doNullMove, bool isExtensiens, int tid, bool captured_see_memo, int i_taxi_count, int jKing_security_count) {

    // depth может не менятся на плюс один из-за итеративного углубления.
    // iply всегда меняется на плюс 1.

    number_chess_positions_arr[tid]+=1;

    bool enemy = ((iply % 2)==1); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    // Если шах то мы продлеваем все ветки на глубину 1.
    //if (InCheck(board.board, board.previos_moves, (color))) depth-=2;

    // depth - глубина просчёта он нуля.

    // если исчерпан лимит глубины - вернём статическую оценку позиции.
    if ((depth >= my_depth) || (eatKing(board)) || (iply >= 20)) {

        // Либо съели короля либо достигли глубины перебора.

        // Считает материал с точки зрения белых,
        // она суммирует вес всех белых фигур, а 
        // потом вычитает из них вес всех чёрных фигур.
        // Оценка для Чёрных наоборот.

        LIST_MOVES m;
        m.n = 1;
        const float multiplyer = 1.0f; //1.1f;

        // Вызывалось для my_color.
        // статическая оценка.
        // Если у нас минимакс то максимум то минимум, мы то максимизируем то минимизируем
        //m.moves[0].rate = board.rate(my_color) - board.rate(enemy_color) * multiplyer;
        //m.moves[0].rate =(color==my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer: board.rate(enemy_color)- board.rate(my_color)*multiplyer);

        // Терминальный вызов в листе.
        m.moves[0].rate = Quies(alpha, beta, my_color, enemy_color, board, iply);
        //m.moves[0].rate = Quies1(alpha, beta, my_color, enemy_color, board, iply);

#ifndef CODE_FOR_DLL
        //std::cout << "static  evaluation = " << board.rate(my_color) << " "<< board.rate(enemy_color) << " "<< board.rate(enemy_color) * multiplyer <<" " << m.moves[0].rate << std::endl;
        //board.print();
        //system("PAUSE");
#endif

        m.moves[0].xy_from = init(None);
        m.moves[0].xy_to = init(None);

        return m;
    }



    float rate;
    float rate_flag;
    if (enemy) {
        //min
        rate = +9999.0f - 10.0f * iply;
        rate_flag = rate - 1.0f;
    }
    else {
        // max
        rate = -9999.0f + 10.0f * iply;
        rate_flag = rate + 1.0f;
    }


    MOVES figList;

    // Проходим по доске
    //for (__int8 y = 0; y < 8; ++y) {
        //for (__int8 x = 0; x < 8; ++x) {
            //if (get_color1(board.board, x, y) != color) {
                // Если фигура чужая или у нас пустая клетка то переходим к следующей клетки доски.
              //  continue;
            //}


    LIST_MOVES rates;
    //printf("%d %d %d\n", rates.POOL_SIZE_loc, i_01, n_01);
    rates.n = 0;



    int n_01 = 0;
    MOVES moves_current_list[POOL_SIZEL];

    LIST_NODE_MOVES make_moves[POOL_SIZE];

    int i_01 = 0, i_02 = 0;

    bool bnull_move = false;

    //null move
    // Нельзя применять в эндшпиле (точнее при цукцванге), нужно сравнение с конической оценкой.
    // Цукцванг очень часто возникает в эндшпиле.
    if ((!doNullMove)&&(!isExtensiens)) {

        // Пустой ход делается не более одного раза в ветке.
        // Эвристика пустого хода не прменяется при шахе.

        if ((THINKING_DEPTH == 6) || (THINKING_DEPTH == 8) || (THINKING_DEPTH == 10) || (THINKING_DEPTH == 7))
        {
            LIST_MOVES m;
            m.n = 0;

            Board new_board;

            // Копирование доски.
            new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

            // Вычисляем коническую оценку.
            // 256 - Окончание партии
           // 0 - дебют партии.
            float phase = calculation_phase_for_conic_rate(new_board.board, new_board.wList, new_board.bList); // фаза коническая оценка.

            if (phase < KING_CENTER_PRIORITY_WEIGHT) {
                // начало или середина партиии, не эндшпиль - коническая оценка.

                // +2 == R коэффициент затиухания (для THINKING_DEPTH == 6).
                // +4 == R коэффициент затиухания (для THINKING_DEPTH == 8).

                bool bExtensions00 = false;

               // bool bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                 //   new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O);

                //m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, (
                //captured_see  || оставить закоментированным
                //bExtensions0  ? depth + 2 : depth + 1 + 2), iply + 1, ishah + 1, false, bExtensions0);

                
                bool  doNullMove0 = true;
                // Полное окно.
                // m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, (THINKING_DEPTH == 6 ? depth + 1 + 2 : depth + 1 + 4), iply + 1, ishah + 1, false, doNullMove0, bExtensions00, tid);

                // Нулевое окно. 21.06.2023.
                // Небольшое ускорение за счёт нулевого окна с 286с до 266с. 7% времени сократилось только за счёт нулевого окна.
                // Допуск одна тысячная пешки 0.01f. Т.к. цена пешки в моей программе 10.0f.
                // Мы не сгенерировали своего перемещения и дали опоненту фактически сделать ход два раза.
                if (enemy) {
                    m = alpha_beta_do1(alpha, alpha + 0.01f, my_color, enemy_color, my_depth, new_board, (THINKING_DEPTH == 6 ? depth + 1 + 2 : depth + 1 + 4), iply + 1, ishah + 1, false, doNullMove0, bExtensions00, tid, false, i_taxi_count, jKing_security_count);

                }
                else {
                   
                    m = alpha_beta_do1(beta - 0.01f, beta, my_color, enemy_color, my_depth, new_board, (THINKING_DEPTH == 6 ? depth + 1 + 2 : depth + 1 + 4), iply + 1, ishah + 1, false, doNullMove0, bExtensions00, tid, false, i_taxi_count, jKing_security_count);

                }

                //for (int i_9 = 0; i_9 < m.n; ++i_9) m.moves[i_9].rate *= -1.0f;

                // Мы отдали ход противнику, противник сделал два хода. Если наша оценка после пропуска хода всё ещё хороша то 
                // это беспереспектиная ветвь дерева перебора.
                if (enemy) {
                    //min

                    //rate = std::min(rate, m.moves[0].rate);

                   // 18.июля.2023
                   // Внимание никакой сортировки здесь ненадо!!! Она ломает программу, мы отдаем ферзя например за так на уровне Каисса.



                    if (alpha >= std::min(rate, m.moves[0].rate)) {



#ifndef CODE_FOR_DLL
                        //  if (depth == 0) std::cout << "alpha-beta\n";
#endif
                        // rate = beta;// отсечение
                         // Возвратить rate (best_move).


                        rates.n++;



                        //rates.moves[rates.n - 1].rate = rate;
                        rates.moves[rates.n - 1].rate = std::min(rate, m.moves[0].rate);

                        MOVES_NODE null_move;
                        null_move.x = None;
                        null_move.y = None;

                        rates.moves[rates.n - 1].xy_from = null_move;
                        rates.moves[rates.n - 1].xy_to = null_move;

                        bnull_move = true;

                        goto ALPHA_BETA_CONT_NM;
                    }

                }
                else {
                    //max


                    //rate = std::max(rate, m.moves[0].rate);

                   // 18.июля.2023
                   // Внимание никакой сортировки здесь ненадо!!! Она ломает программу, мы отдаем ферзя например за так на уровне Каисса.


                    if (std::max(rate, m.moves[0].rate) >= beta) {



#ifndef CODE_FOR_DLL
                        //  if (depth == 0) std::cout << "alpha-beta\n";
#endif
                        // rate = beta;// отсечение
                         // Возвратить rate (best_move).


                        rates.n++;



                        //rates.moves[rates.n - 1].rate = rate;
                        rates.moves[rates.n - 1].rate = std::max(rate, m.moves[0].rate);

                        MOVES_NODE null_move;
                        null_move.x = None;
                        null_move.y = None;

                        rates.moves[rates.n - 1].xy_from = null_move;
                        rates.moves[rates.n - 1].xy_to = null_move;

                        bnull_move = true;

                        goto ALPHA_BETA_CONT_NM;

                    }
                }

            }
        }
    }
   
   
    // Если эвристика нулевого хода сработала (отсечение произошло), то мы даже пропускаем генерацию и сортировку ходов, это тоже должно экономить время поиска лучшео хода.
    GenerateAllMoves(figList, make_moves, moves_current_list, color, board, n_01, i_01, i_02, my_color, enemy_color, my_depth, enemy, iply, false, bprincipal_variation_search, rate);
    
    {
       


        if ((killer_euristick[tid][iply].xy_from.x != None) && (killer_euristick[tid][iply].xy_from.y != None) &&
            (killer_euristick[tid][iply].xy_to.x != None) && (killer_euristick[tid][iply].xy_to.y != None)) {

            bool bcontinue_killer = true;
            // Убийственный ход найден в первой части и так сильных ходов (найден среди взятий).
            // Поэтому его не нужно двигать вперед, он и так впереди.
            for (int k_1 = 0; k_1 < i_02; ++k_1)
            {
                if ((make_moves[k_1].xy_from.x == killer_euristick[tid][iply].xy_from.x) &&
                    (make_moves[k_1].xy_from.y == killer_euristick[tid][iply].xy_from.y) &&
                    (make_moves[k_1].xy_to.x == killer_euristick[tid][iply].xy_to.x) &&
                    (make_moves[k_1].xy_to.y == killer_euristick[tid][iply].xy_to.y))
                {
                    bcontinue_killer = false;
                    break;
                }
            }
            
            if (bcontinue_killer&&(n_01 > 0)) {
                int ifound_killer = -1;
                // Сильный ход может быть только тихим ходом. Все взятия остаются первыми.
                // С номера i_02 начинается тихий ход.
                for (int k_1 = i_02; k_1 < i_01; ++k_1)
                {
                    if ((make_moves[k_1].xy_from.x == killer_euristick[tid][iply].xy_from.x) &&
                        (make_moves[k_1].xy_from.y == killer_euristick[tid][iply].xy_from.y) &&
                        (make_moves[k_1].xy_to.x == killer_euristick[tid][iply].xy_to.x) &&
                        (make_moves[k_1].xy_to.y == killer_euristick[tid][iply].xy_to.y))
                    {
                        ifound_killer = k_1;
                        break;
                    }
                }
                // на первое место ставим лучший ход.
                if (ifound_killer > 0) {
                    for (int k_1 = ifound_killer; k_1 > i_02; --k_1)
                    {
                        make_moves[k_1] = make_moves[k_1 - 1];
                    }
                    make_moves[i_02].rate = 0.0;
                    make_moves[i_02].xy_from.x = killer_euristick[tid][iply].xy_from.x;
                    make_moves[i_02].xy_from.y = killer_euristick[tid][iply].xy_from.y;
                    make_moves[i_02].xy_to.x = killer_euristick[tid][iply].xy_to.x;
                    make_moves[i_02].xy_to.y = killer_euristick[tid][iply].xy_to.y;
                }
            }
        
            int i_03 = i_02;
            if (bcontinue_killer) ++i_03;
            // Есть идея продвинуть шахи вперёд среди оставшихся ходов.
            // Но их наличие очень ресурсоёмко вычислить.

            if (1) {
                bool bcheck_[POOL_SIZE];
                for (int k_1 = i_03; k_1 < i_01; ++k_1)
                {
                    bcheck_[k_1] = false;
                }
                for (int k_1 = i_03; k_1 < i_01; ++k_1)
                {

                    __int8 y = make_moves[k_1].xy_from.y;
                    __int8 x = make_moves[k_1].xy_from.x;

                    MOVES_NODE xy_from;
                    xy_from = init(x, y);

                    MOVES_NODE xy_to;

                    xy_to.x = make_moves[k_1].xy_to.x;
                    xy_to.y = make_moves[k_1].xy_to.y;

                    Board new_board;

                    // Копирование доски.
                    new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                    new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                    // Предыдущий ход на этой новой  доске после хода.
                    new_board.previos_moves.xy_from = xy_from;
                    new_board.previos_moves.xy_to = xy_to;
                    new_board.previos_moves.figure = board.board[y][x];

                    bcheck_[k_1] = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                        new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O);
                }
                int i_04 = i_03;
                // Продвигаем все шахи вперед в списке ходов.
                // Шахи находятся после взятий и после эвристики нулевого хода.
                for (int k_1 = i_03; k_1 < i_01; ++k_1)
                {
                    if (bcheck_[k_1]) {
                        LIST_NODE_MOVES tmpM = make_moves[k_1];
                        for (int k_2 = k_1-1; k_2 >=i_04 ; --k_2)
                        {
                            make_moves[k_2 + 1] = make_moves[k_2];
                        }                        
                        if (k_1 != i_04) {
                            make_moves[i_04] = tmpM;
                            ++i_04;
                        }
                    }
                }
            }


        }
    }

    if (n_01 > 0) {

        if (n_01 > POOL_SIZE) {
            printf("increase POOL_SIZE %d %d\n",n_01,POOL_SIZE);
            system("PAUSE");
        }
        

        //system("PAUSE");

        // Сканируем ходы
        //for (__int8 k_1 = 0; k_1 < figList.n; ++k_1)
        for (int k_1 = 0; k_1 < i_01; ++k_1)
        {


            __int8 y = make_moves[k_1].xy_from.y;
            __int8 x = make_moves[k_1].xy_from.x;

            MOVES_NODE xy_from;
            xy_from = init(x, y);



            // получим все позиции перемещения
       // MOVES moves_current_list = board.get_moves(x, y);//список возможных ходов из позиции [x,y].
        // Проверяем каждый сгенерированный код.
        //for (__int8 i = 0; i < moves_current_list.n; ++i) 
            {

                //MOVES_NODE xy_to = moves_current_list.moves[i]; // Куда походить фигурой.               

                MOVES_NODE xy_to;

                xy_to.x = make_moves[k_1].xy_to.x;
                xy_to.y = make_moves[k_1].xy_to.y;

#ifndef CODE_FOR_DLL
                //if (iply == 0) {
                    //-->printf("%d %d iply = 0 from  %d %d to %d %d\n", k_1, n_01, (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));
                    //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
                    //board.print();
                    //system("PAUSE");
                //}
#endif

                Board new_board;

                // Копирование доски.
                new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после хода.
                new_board.previos_moves.xy_from = xy_from;
                new_board.previos_moves.xy_to = xy_to;
                new_board.previos_moves.figure = board.board[y][x];

                float rate_Current;


#ifndef CODE_FOR_DLL
                if (0 && (iply == 0)) {
                    //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
                   // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                    printf("iply = 0 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                    new_board.print();

                    system("PAUSE");
                }
                if (0 && (depth == 1)) {
                    //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
                   // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                    printf("depth = 1 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                    new_board.print();

                    system("PAUSE");
                }
#endif

                // Место куда атаковали не пусто, там стоит вражеская фигура ?
                bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
                bool captured_see = false;
                // true цена того что съели больше или равна цене того чем съели.
                if (captured) captured_see = (rate_fig(board.board[xy_to.y][xy_to.x]) >= rate_fig(new_board.board[xy_to.y][xy_to.x]) ? true : false);
                if (color == Color::WHITE) {
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                        (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                        (xy_to.x == board.previos_moves.xy_to.x) &&
                        (xy_to.x == board.previos_moves.xy_from.x) &&
                        ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                        (board.previos_moves.xy_from.y == 1) &&
                        (board.previos_moves.xy_to.y == 3) &&
                        (xy_from.y == 3) && (xy_to.y == 2)) {
                        captured = true; // взятие на проходе.
                        captured_see = true;

                    }
                }
                if (color == Color::BLACK) {
                    if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                        (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                        (xy_to.x == board.previos_moves.xy_to.x) &&
                        (xy_to.x == board.previos_moves.xy_from.x) &&
                        ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                        (board.previos_moves.xy_from.y == 6) &&
                        (board.previos_moves.xy_to.y == 4) &&
                        (xy_from.y == 4) && (xy_to.y == 5)) {
                        captured = true; // взятие на проходе.
                        captured_see = true;
                    }
                }


                // Сьели Короля.
                //if (captured && (board.board[xy_to.y][xy_to.x] == ChessPiece::B_KING) || (board.board[xy_to.y][xy_to.x] == ChessPiece::W_KING)) {

                  //  rate = ((enemy) ? -1000.0f : 1000.0f); // Вражеский король сьеден (мат).
                //}
            //    else
                {
                    LIST_MOVES m;
                    m.n = 0;

                    //alpha - наш максимум.
                    // beta - максимум противника.

                    bool bExtensions0 = false;

                    bool isTaxi = false;
                    // Включаем продление при опасном сокращении расстояния от нашего ферзя до Короля противника.

                    if ((color == Color::WHITE) && (board.board[y][x] == ChessPiece::W_QUEEN)) {
                        char kingX, kingY;
                        for (int i_07 = 0; i_07 < new_board.bList.n; ++i_07) {
                            if (new_board.board[new_board.bList.moves[i_07].y][new_board.bList.moves[i_07].x] == ChessPiece::B_KING) {
                                kingX = new_board.bList.moves[i_07].x;
                                kingY = new_board.bList.moves[i_07].y;

                                float new_taxi = sqrtf((xy_to.x-kingX)*(xy_to.x - kingX)+ (xy_to.y - kingY) * (xy_to.y - kingY));

                                if (new_taxi < 4.0) {
                                    // Попали в квадрат Короля.
                                    float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                    if (new_taxi < old_taxi) isTaxi = true;
                                    if ((new_taxi==old_taxi)&&(xy_to.x+ xy_to.y< xy_from.x+ xy_from.y)) isTaxi = true;
                                    
                                }
                                break;
                            }
                        }
                    }

                    if ((color == Color::BLACK) && (board.board[y][x] == ChessPiece::B_QUEEN)) {
                        char kingX, kingY;
                        for (int i_07 = 0; i_07 < new_board.wList.n; ++i_07) {
                            if (new_board.board[new_board.wList.moves[i_07].y][new_board.wList.moves[i_07].x] == ChessPiece::W_KING) {
                                kingX = new_board.wList.moves[i_07].x;
                                kingY = new_board.wList.moves[i_07].y;

                                float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));

                                if (new_taxi < 4.0) {
                                    float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                    if (new_taxi < old_taxi) isTaxi = true;
                                    if ((new_taxi == old_taxi) && (xy_to.x + xy_to.y < xy_from.x + xy_from.y)) isTaxi = true;
                                }
                                break;
                            }
                        }
                    }
                    
                    if (iKingSecurity == 0) {
                        isTaxi = false;
                    }

                    bool isSquareKingActivity = false;
                    // Включаем продления если детектированы действия в квадрате Короля.

                    {
                        char kingX, kingY;
                        for (int i_07 = 0; i_07 < new_board.bList.n; ++i_07) {
                            if (new_board.board[new_board.bList.moves[i_07].y][new_board.bList.moves[i_07].x] == ChessPiece::B_KING) {
                                kingX = new_board.bList.moves[i_07].x;
                                kingY = new_board.bList.moves[i_07].y;

                                // Попали в квадрат Короля.

                                float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));

                                
                                float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                if (new_taxi < 1.47) isSquareKingActivity = true;
                                if (old_taxi < 1.47) isSquareKingActivity = true;
                                                                
                                break;
                            }
                        }
                    }

                    {
                        char kingX, kingY;
                        for (int i_07 = 0; i_07 < new_board.wList.n; ++i_07) {
                            if (new_board.board[new_board.wList.moves[i_07].y][new_board.wList.moves[i_07].x] == ChessPiece::W_KING) {
                                kingX = new_board.wList.moves[i_07].x;
                                kingY = new_board.wList.moves[i_07].y;

                                // Попали в квадрат Короля.

                                float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));

                                
                                    float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                    if (new_taxi < 1.47) isSquareKingActivity = true;
                                    if (old_taxi < 1.47) isSquareKingActivity = true;
                                
                                break;
                            }
                        }
                    }

                    bool bMiddleGame = true;// начало или середина игры ? 
                    // 10.08.2023  В эндшпиле отключаются все продления кроме шахов.
                    const float conic_rate = calculation_phase_for_conic_rate(board.board, board.wList, board.bList);// Коническая оценка.
                    if (conic_rate > KING_CENTER_PRIORITY_WEIGHT) {
                        // эндшпиль.
                        bMiddleGame = false;
                    }


                    if (THINKING_DEPTH <= 4) {
                        if (bQueekGame) {

                            // Продление при шахе, а также если ответных ходов один, два или три всего. 
                            bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O);// || (n_01 < 1);
                        }
                        else {
                            // 07.08.2023 На depth 4 анализ занимает 2с.
                            // Продление при шахе, а также если всего только один ответный ход + продление при выгодном взятии фигуры (до глубины шестёрка). 
                            bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O)
                                || ((bMiddleGame)&&((n_01 <= 1) || (iply < 7 ? captured_see /* || captured_see_memo */ : false) ||
                                // Белая пешка на последней горизонтали.
                                ((color == Color::WHITE) && (board.board[y][x] == ChessPiece::W_PAWN) && (xy_to.y == 1)) ||
                                // Чёрная пешка на последней горизонтали.
                                ((color == Color::BLACK) && (board.board[y][x] == ChessPiece::B_PAWN) && (xy_to.y == 6))||
                                    (iply < 0 ? isSquareKingActivity : false)));

                            // продлеваем при шахе до хода, после хода. Продлеваем если только два ответа. Продлеваем если выгодно съели фигуру.
                           /*bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                               new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O) ||
                               InCheck(board.board, board.wList, board.bList, board.previos_moves, color,
                                   board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O) || (n_01 <= 2) || captured_see || captured_see_memo;*/
                        }
                    }
                    else {

                        if (bQueekGame) {
                            // Продление только при шахе.
                            bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O);
                        }
                        else {
                            // Продление только при шахе.
                            bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O) ||
                                ((bMiddleGame) && ((n_01 <= 1) || (iply < 6 ? captured_see : false) ||
                                // Белая пешка на последней горизонтали.
                                ((color == Color::WHITE) && (board.board[y][x] == ChessPiece::W_PAWN) && (xy_to.y == 1)) ||
                                // Чёрная пешка на последней горизонтали.
                                ((color == Color::BLACK) && (board.board[y][x] == ChessPiece::B_PAWN) && (xy_to.y == 6)) 
                                    || (iply < 0 ? isSquareKingActivity : false)));
                        }
                    }
                  

                    // Principial Variation Search
                    //if (k_1 < 4) 
                    {

                        //m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1, ishah + 1, false, doNullMove, false);

                        // depth - 1 слишком глубокое дерево получается очень долго считает варианты.
                        // Продлевать взятия нельзя программа очень долго думает видно взятий очень много.
                       

                        m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, 
                            (/*captured_see  ||*/ bExtensions0 || ((bMiddleGame) && (i_taxi_count < 4 ? isTaxi : false)) ? depth : depth + 1), iply + 1, ishah + 1,
                            false, doNullMove, bExtensions0 || ((bMiddleGame) && (i_taxi_count < 4 ? isTaxi : false)), tid, captured_see, (isTaxi ? i_taxi_count+1 : i_taxi_count),
                            (isSquareKingActivity ? jKing_security_count+1 : jKing_security_count));

                       // for (int i_9 = 0; i_9 < m.n; ++i_9) m.moves[i_9].rate *= -1.0f;

                    }
                    /*else {

                        // Быстрый просчёт с нулевым окном
                        if (enemy) {
                            //min

                            m = alpha_beta_do1(beta - 1.0f, beta, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1, ishah + 1, true, doNullMove, bExtensions0);
                        }
                        else {
                            //max
                            m = alpha_beta_do1(alpha, alpha + 1.0f, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1, ishah + 1,true, doNullMove, bExtensions0);
                        }


                        // если эвристика не сработала, и ход улучшает
                        // alpha, то мы пересчитаем с полным окном
                        if ((m.moves[0].rate > alpha) && (m.moves[0].rate < beta)) {

                            Board new_board2;

                            // Копирование доски.
                            new_board2.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                            new_board2.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                             // Предыдущий ход на этой новой  доске после хода.
                            new_board2.previos_moves.xy_from = xy_from;
                            new_board2.previos_moves.xy_to = xy_to;
                            new_board2.previos_moves.figure = board.board[y][x];


                            m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board2, depth + 1, iply + 1, ishah + 1,false, doNullMove, bExtensions0);
                        }
                    }*/


                    // Было взятие - итеративное углубление.
                    // captured - Он почемуто жертвует фигуры (выбрасывает фигуры). Коня за пешку.
                    // Попробуем captured_see только равные или победившие взятия.
                    //m = alpha_beta_do1(alpha, beta,   my_color, enemy_color, my_depth, new_board, (captured_see ? depth : depth + 1),iply+1);
                    //  детектор шахов InCheck(new_board.board, new_board.previos_moves, color, doNullMove)
                    // В малофигурном окончании правило captured_see неверно и заменено на captured так как Король тоже может есть.
                   // m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, 
                     //   (captured_see|| InCheck(new_board.board, new_board.previos_moves, color)||
                       //     (captured&& number_figures(new_board.board)<=ENDSHPIL_NUMBER_FIGURES) ? depth : depth + 1), iply + 1, doNullMove);
                    // Нам же не нужно чтобы он слона за пешку отдал. Без SEE он просто отдаёт материал.
                    //Color_invert(color) правильное продление шахов с invert color так как на новой доске и цвет новый,
                    // но продление шахов инициирует всё новые шахи так же как продление взятий инициирует новые взятия иногда неравноценные жертвы
                    // фигуры выбрасываются.
                    //-->m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board,
                        //(captured_see   || InCheck(new_board.board, new_board.previos_moves, Color_invert(color)) ? depth : depth + 1), iply + 1, doNullMove);

                   // m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board,
                     //   (captured_see  || ((ishah==0)&&InCheck(new_board.board, new_board.previos_moves, Color_invert(color))) ? depth -1 : depth + 1),
                       // iply + 1, (InCheck(new_board.board, new_board.previos_moves, Color_invert(color)) ? ishah+1:ishah), doNullMove);

                    rate_Current = m.moves[0].rate;
                    //std::cout << "rate_Current " << m.moves[0].rate<<" alpha = "<<alpha <<"beta = "<< beta << "\n";

                    if (m.n == 0) {
                        continue;
                    }

                    

                    if (enemy) {

                        // 18.июля.2023
                         // Внимание никакой сортировки здесь ненадо!!! Она ломает программу, мы отдаем ферзя например за так на уровне Каисса.


                        //min
                       // rate = (rate < m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)
                        rate = std::min(rate, m.moves[0].rate);

                    }
                    else {
                        //max

                        // 18.июля.2023
                        // Внимание никакой сортировки здесь ненадо!!! Она ломает программу, мы отдаем ферзя например за так на уровне Каисса.

                        //rate = (rate > m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)
                        rate = std::max(rate, m.moves[0].rate);
                    }

                    // Оценка позиции не лучший ход.
                    //17.12.2022 закоментировал rate = m.moves[0].rate; // (best_move)

                    if ((captured) && (!enemy)) {
                        //rate += my_depth - depth; // Добавим немного агрессии
                    }

                    if (enemy) {
                        //min

                        //beta = (rate < beta ? rate : beta);
                        beta = std::min(rate, beta);
                    }
                    else {
                        // max

                        //if (rate > alpha) alpha = rate;
                        //alpha = (rate > alpha ? rate : alpha);
                        alpha = std::max(rate, alpha);


                    }

                    //if (depth == 0) std::cout << "\n rate= " << rate << std::endl;

                    if (alpha >= beta) {
#ifndef CODE_FOR_DLL
                        //  if (depth == 0) std::cout << "alpha-beta\n";
#endif
                        // rate = beta;// отсечение
                         // Возвратить rate (best_move).


                        rates.n++;

                        if (rates.n - 1 > n_01 - 1) {
#ifndef CODE_FOR_DLL
                            printf("error! moves limit < n_01\n");
                            system("PAUSE");
#endif
                        }

                        if (iply == 0) {
                            // Корень дерева. Нам нужны все оценки а не только их экстремальное значение.
                            rates.moves[rates.n - 1].rate = rate_Current;
                        }
                        else {
                            rates.moves[rates.n - 1].rate = rate;
                        }
                        

                        rates.moves[rates.n - 1].xy_from = xy_from;
                        rates.moves[rates.n - 1].xy_to = xy_to;

                        goto ALPHA_BETA_CONT;
                    }


                }



                rates.n++;

                if (rates.n - 1 > n_01 - 1) {
#ifndef CODE_FOR_DLL
                    printf("error! moves limit < n_01\n");
                    system("PAUSE");
#endif
                }

                if (iply == 0) {
                    // Корень дерева. Нам нужны все оценки а не только их экстремальное значение.
                    rates.moves[rates.n - 1].rate = rate_Current;
                }
                else {
                    rates.moves[rates.n - 1].rate = rate;
                }
              

                rates.moves[rates.n - 1].xy_from = xy_from;
                rates.moves[rates.n - 1].xy_to = xy_to;

            }

        }
        // }
     //}



    ALPHA_BETA_CONT: // результат отсечения досрочное прерывание сканирования

        // Освобождение оперативной памяти.
      //  if (make_moves != nullptr) {
        //    delete[] make_moves;
          //  make_moves = nullptr;
        //}

        if (depth == 0) {

            /* if (fabs(rate) > 9998.0) {
                 // Цель игры мат королю противника.
                 rates.n = 1;
                 rates.moves[0].rate = rate;
                 rates.moves[0].xy_from.x = None;
                 rates.moves[0].xy_from.y = None;
                 rates.moves[0].xy_to.x = None;
                 rates.moves[0].xy_to.y = None;
             }*/

            /*std::cout << "iply" << (int)(iply) << " " << (enemy ? "min" : "max") << "\n";
            for (int id_rate = 0; id_rate < rates.n; ++id_rate)
            {
                std::cout << rates.moves[id_rate].rate << " " << static_cast<char>('a' + rates.moves[id_rate].xy_from.x);
                std::cout << 8 - rates.moves[id_rate].xy_from.y << static_cast<char>('a' + rates.moves[id_rate].xy_to.x);
                std::cout << 8 - rates.moves[id_rate].xy_to.y << std::endl;
            }
            std::cout << std::endl;
            getchar();*/ 

            return rates;
        }

#ifndef CODE_FOR_DLL
        if (depth == 1) {
            //   std::cout << "1 BLACK evalute " << rates.n << " " << (rates.n > 0 ? rates.moves[0].rate : 0) << std::endl;
        }
#endif

        if (rates.n == 0) {

            if (fabs(rate) > fabs(rate_flag)) {
                // Цель игры мат королю противника.
                rates.n = 1;
                if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
                    // Мы под шахом и нам некуда идти -> мат.
                    rates.moves[0].rate = rate;
                }
                else {
                    // Некуда идти и мы не под шахом ничья.
                    rate = 0.0; // Пат
                    rates.moves[0].rate = rate;
                }
                rates.moves[0].xy_from.x = None;
                rates.moves[0].xy_from.y = None;
                rates.moves[0].xy_to.x = None;
                rates.moves[0].xy_to.y = None;
            }

            return rates; // Как бы возврат None так как rates.n == 0.
        }

        LIST_MOVES m;
        m.n = 1;

        {


            int id_found = 0;
            float rate_found = rates.moves[0].rate;

            // Мы находимся на некоторой глубине вложенности

            if (enemy) {

                // Поиск минимума  
                // Враг черные при первом ходе белых или наборот при первом ходе черных.

                for (int i = 1; i < rates.n; ++i) {
                    if (rates.moves[i].rate < rate_found) {
                        rate_found = rates.moves[i].rate;
                        id_found = i;
                    }
                }

            }
            else {
                // Поиск максимума   
                // Белые  при первом ходе белых или наборот при первом ходе черных.

                for (int i = 1; i < rates.n; ++i) {
                    if (rates.moves[i].rate > rate_found) {
                        rate_found = rates.moves[i].rate;
                        id_found = i;
                    }
                }
            }

            //if (0) {
                //std::cout << "\n";
                //for (__int8 i = 1; i < rates.n; ++i) {
                  //  if (fabs(rates.moves[i].rate - rate_found) < 0.01) {
                    //    std::cout << i << " ";
                    //}
                //}
                //std::cout << "\n";
            //}


            m.moves[0].rate = rates.moves[id_found].rate;

           // std::cout <<"iply ="<< (int)(iply) <<" rate found="<< rates.moves[id_found].rate<< (enemy ? "min" : "max") << "\n";

            // m.moves[0].xy_from=init(None);
             //m.moves[0].xy_to=init(None);

            m.moves[0].xy_from = rates.moves[id_found].xy_from;
            m.moves[0].xy_to = rates.moves[id_found].xy_to;

            // Запоминаем эвристику убийцы.
            // 
            // 
           

            // killer_euristick[tid][iply].rate = 0.0; // не используется
            killer_euristick[tid][iply].xy_from.x = m.moves[0].xy_from.x;
            killer_euristick[tid][iply].xy_from.y = m.moves[0].xy_from.y;
            killer_euristick[tid][iply].xy_to.x = m.moves[0].xy_to.x;
            killer_euristick[tid][iply].xy_to.y = m.moves[0].xy_to.y;
        }
        if (depth == 1) {
            // std::cout << "2 BLACK evalute " <<  (m.n > 0 ? m.moves[0].rate : 0) << std::endl;
        }
        // system("PAUSE");

        return m;
    }
    else {

    

    LIST_MOVES rates;
   
    rates.n = 0;

    //if (rates.n == 0) {

        if (fabs(rate) > fabs(rate_flag)) {
            // Цель игры мат королю противника.
            rates.n = 1;
            if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
                // Мы под шахом и нам некуда идти -> мат.
                rates.moves[0].rate = rate;
            }
            else {
                // Некуда идти и мы не под шахом ничья.
                rate = 0.0; // Пат
                rates.moves[0].rate = rate;
            }
            rates.moves[0].xy_from.x = None;
            rates.moves[0].xy_from.y = None;
            rates.moves[0].xy_to.x = None;
            rates.moves[0].xy_to.y = None;
        }

        return rates; // Как бы возврат None так как rates.n == 0.
    
   }
    
   
ALPHA_BETA_CONT_NM: // результат отсечения досрочное прерывание сканирования

    // Освобождение оперативной памяти.
  //  if (make_moves != nullptr) {
    //    delete[] make_moves;
      //  make_moves = nullptr;
    //}

    if (bnull_move) {

        /* if (fabs(rate) > 9998.0) {
             // Цель игры мат королю противника.
             rates.n = 1;
             rates.moves[0].rate = rate;
             rates.moves[0].xy_from.x = None;
             rates.moves[0].xy_from.y = None;
             rates.moves[0].xy_to.x = None;
             rates.moves[0].xy_to.y = None;
         }*/

      /*  std::cout << "iply 3 " << (int)(iply) << " " << (enemy ? "min" : "max") << "\n";
        for (int id_rate = 0; id_rate < rates.n; ++id_rate)
        {
            std::cout << rates.moves[id_rate].rate << " " << static_cast<char>('a' + rates.moves[id_rate].xy_from.x);
            std::cout << 8 - rates.moves[id_rate].xy_from.y << static_cast<char>('a' + rates.moves[id_rate].xy_to.x);
            std::cout << 8 - rates.moves[id_rate].xy_to.y << std::endl;
        }
        std::cout << std::endl;
        getchar();*/

        return rates;
    }
   
   
    std::cout << "alpha beta do1 error undefined situation...\n";
    system("pause");

   //return ;
}



// параллельный alpha - beta алгоритм 21.01.2023.
// https://github.com/hennimohammed/parallel_chess_engine
// Root splitting (разделение дочерних узлов корня на паралельные секции).
// Ставим schedule (dynamic) указывает что  динамическое планирование между потоками будет применено,
// что означает, что если поток завершит назначенные ему итерации раньше других, ему будут назначены
// некоторые итерации из другого потока, что позволит лучше использовать доступные потоки.
// alpha и beta обновляем в критической секции #pragma omp critical 
LIST_MOVES parallel_chess_engine1(float alpha, float beta, Color my_color, Color enemy_color,
    __int8 my_depth, Board board, __int8 depth, __int8 iply, __int8 ishah) {

    // depth может не менятся на плюс один из-за итеративного углубления.
    // iply всегда меняется на плюс 1.

    bool enemy = ((iply % 2) == 1); // нечетный полуход враг(Чёрные).
    Color color = (enemy ? enemy_color : my_color);

    // Если шах то мы продлеваем все ветки на глубину 1.
    //if (InCheck(board.board, board.previos_moves, (color))) depth-=2;

    // depth - глубина просчёта он нуля.

    // если исчерпан лимит глубины - вернём статическую оценку позиции.
    if ((depth >= my_depth) || (eatKing(board)) || (iply >= 30)) {

        // Либо съели короля либо достигли глубины перебора.

        // Считает материал с точки зрения белых,
        // она суммирует вес всех белых фигур, а 
        // потом вычитает из них вес всех чёрных фигур.
        // Оценка для Чёрных наоборот.

        LIST_MOVES m;
        m.n = 1;
        const float multiplyer = 1.0f; //1.1f;

        // Вызывалось для my_color.
        // статическая оценка.
        //m.moves[0].rate = board.rate(my_color) - board.rate(enemy_color) * multiplyer;
        //m.moves[0].rate = (color==my_color ? board.rate(my_color) - board.rate(enemy_color) * multiplyer: board.rate(enemy_color)- board.rate(my_color)*multiplyer);

        // Терминальный вызов в листе.
        m.moves[0].rate = Quies(alpha, beta, my_color, enemy_color, board, iply);
        //m.moves[0].rate = Quies1(alpha, beta, my_color, enemy_color, board, iply);

#ifndef CODE_FOR_DLL
        //std::cout << "static  evaluation = " << board.rate(my_color) << " "<< board.rate(enemy_color) << " "<< board.rate(enemy_color) * multiplyer <<" " << m.moves[0].rate << std::endl;
        //board.print();
        //system("PAUSE");
#endif

        m.moves[0].xy_from = init(None);
        m.moves[0].xy_to = init(None);

        return m;
    }



    float rate;
    float rate_flag;
    if (enemy) {
        //min
        rate = +9999.0f - 10.0f * iply;
        rate_flag = rate - 1.0f;
    }
    else {
        // max
        rate = -9999.0f + 10.0f * iply;
        rate_flag = rate + 1.0f;
    }


    MOVES figList;

    // Проходим по доске
    //for (__int8 y = 0; y < 8; ++y) {
        //for (__int8 x = 0; x < 8; ++x) {
            //if (get_color1(board.board, x, y) != color) {
                // Если фигура чужая или у нас пустая клетка то переходим к следующей клетки доски.
              //  continue;
            //}

    int n_01 = 0;
    MOVES moves_current_list[POOL_SIZEL];

    LIST_NODE_MOVES make_moves[POOL_SIZE];

    int i_01 = 0, i_02 = 0;


    GenerateAllMoves(figList, make_moves, moves_current_list, color, board, n_01, i_01, i_02, my_color, enemy_color, my_depth, enemy, iply,true,false, rate);

    if (n_01 > 0) {
        if (n_01 > POOL_SIZE) {
            printf("increase POOL_SIZE %d %d\n", n_01, POOL_SIZE);
            system("PAUSE");
        }
        LIST_MOVES rates;
        //printf("%d %d %d\n", rates.POOL_SIZE_loc, i_01, n_01);
        rates.n = 0;

        int rates_sum_n = 0;


       
        std::cout << "number moves =" << i_01 << std::endl;
       // rates.n = i_01;
        int id_thread[40] = { 0 };// 2*xeon 2630v4.
        bool flag_in[POOL_SIZE] = { false };

        float rate_calc[40] = { 0.0f };// 2*xeon 2630v4.


        //system("PAUSE");

        // Сканируем ходы
        //for (__int8 k_1 = 0; k_1 < figList.n; ++k_1)
        
#ifdef _OPENMP

        omp_set_num_threads(number_cores);//4 или 8

#endif

        bool b_we_should_be_continue = true;

        //shared(ishah, iply, depth, my_depth, enemy_color, my_color, alpha, beta) 
        //reduction(+ : rates_sum_n)

#pragma omp parallel for  schedule (dynamic)
        for (int k_1 = 0; k_1 < i_01; ++k_1)
        {

            if (b_we_should_be_continue) {


                int tid = 0;
#ifdef _OPENMP
                tid = omp_get_thread_num();
#endif

                __int8 y = make_moves[k_1].xy_from.y;
                __int8 x = make_moves[k_1].xy_from.x;

                MOVES_NODE xy_from;
                xy_from = init(x, y);

               
            // Проверяем каждый сгенерированный код.
            //for (__int8 i = 0; i < moves_current_list.n; ++i) 
                {

                    //MOVES_NODE xy_to = moves_current_list.moves[i]; // Куда походить фигурой.               

                    MOVES_NODE xy_to;

                    xy_to.x = make_moves[k_1].xy_to.x;
                    xy_to.y = make_moves[k_1].xy_to.y;

#ifndef CODE_FOR_DLL
                //if (iply == 0) {
                    //-->printf("%d %d iply = 0 from  %d %d to %d %d\n", k_1, n_01, (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));
                    //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
                    //board.print();
                    //system("PAUSE");
                //}
#endif

                    Board new_board;


//#pragma omp critical  (create_new_board)
                    {// Убрал эту критическую секцию и стало выполнятся на 1 секунду быстрее 

                        // Копирование доски.
                        new_board.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                        new_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                         // Предыдущий ход на этой новой  доске после хода.
                        new_board.previos_moves.xy_from = xy_from;
                        new_board.previos_moves.xy_to = xy_to;
                        new_board.previos_moves.figure = board.board[y][x];
                    }

                    float rate_Current;

#ifndef CODE_FOR_DLL
                    if (0 && (iply == 0)) {
                        //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
                       // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                        printf("iply = 0 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                        new_board.print();

                        system("PAUSE");
                    }
                    if (0 && (depth == 1)) {
                        //std::cout << "from " << (int)(xy_from.x) << " " << (int)(xy_from.y) << std::endl;
                       // std::cout << "to " << (int)(xy_to.x) << " " << (int)(xy_to.y) << std::endl;
                        printf("depth = 1 from  %d %d to %d %d\n", (int)(xy_from.x), (int)(xy_from.y), (int)(xy_to.x), (int)(xy_to.y));

                        new_board.print();

                        system("PAUSE");
                    }
#endif

                    // Место куда атаковали не пусто, там стоит вражеская фигура ?
                    bool captured = (board.board[xy_to.y][xy_to.x] == ChessPiece::EMPTY ? false : true);
                    bool captured_see = false;
                    // true цена того что съели больше или равна цене того чем съели.
                    if (captured) captured_see = (rate_fig(board.board[xy_to.y][xy_to.x]) >= rate_fig(new_board.board[xy_to.y][xy_to.x]) ? true : false);
                    if (color == Color::WHITE) {
                        if ((board.board[xy_from.y][xy_from.x] == ChessPiece::W_PAWN) &&
                            (board.previos_moves.figure == ChessPiece::B_PAWN) &&
                            (xy_to.x == board.previos_moves.xy_to.x) &&
                            (xy_to.x == board.previos_moves.xy_from.x) &&
                            ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                            (board.previos_moves.xy_from.y == 1) &&
                            (board.previos_moves.xy_to.y == 3) &&
                            (xy_from.y == 3) && (xy_to.y == 2)) {
                            captured = true; // взятие на проходе.
                            captured_see = true;

                        }
                    }
                    if (color == Color::BLACK) {
                        if ((board.board[xy_from.y][xy_from.x] == ChessPiece::B_PAWN) &&
                            (board.previos_moves.figure == ChessPiece::W_PAWN) &&
                            (xy_to.x == board.previos_moves.xy_to.x) &&
                            (xy_to.x == board.previos_moves.xy_from.x) &&
                            ((xy_to.x == xy_from.x + 1) || (xy_to.x == xy_from.x - 1)) &&
                            (board.previos_moves.xy_from.y == 6) &&
                            (board.previos_moves.xy_to.y == 4) &&
                            (xy_from.y == 4) && (xy_to.y == 5)) {
                            captured = true; // взятие на проходе.
                            captured_see = true;
                        }
                    }


                    // Сьели Короля.
                    //if (captured && (board.board[xy_to.y][xy_to.x] == ChessPiece::B_KING) || (board.board[xy_to.y][xy_to.x] == ChessPiece::W_KING)) {

                      //  rate = ((enemy) ? -1000.0f : 1000.0f); // Вражеский король сьеден (мат).
                    //}
                //    else
                    {
                        LIST_MOVES m;
                        m.n = 0;

                        //alpha - наш максимум.
                        // beta - максимум противника.

                        bool isTaxi = false;
                        // Включаем продление при опасном сокращении расстояния от нашего ферзя до Короля противника.

                        if ((color == Color::WHITE) && (board.board[y][x] == ChessPiece::W_QUEEN)) {
                            char kingX, kingY;
                            for (int i_07 = 0; i_07 < new_board.bList.n; ++i_07) {
                                if (new_board.board[new_board.bList.moves[i_07].y][new_board.bList.moves[i_07].x] == ChessPiece::B_KING) {
                                    kingX = new_board.bList.moves[i_07].x;
                                    kingY = new_board.bList.moves[i_07].y;

                                    float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));

                                    if (new_taxi < 4.0) {
                                        // Попали в квадрат Короля.
                                        float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                        if (new_taxi < old_taxi) isTaxi = true;
                                        if ((new_taxi == old_taxi) && (xy_to.x + xy_to.y < xy_from.x + xy_from.y)) isTaxi = true;

                                    }
                                    break;
                                }
                            }
                        }

                        if ((color == Color::BLACK) && (board.board[y][x] == ChessPiece::B_QUEEN)) {
                            char kingX, kingY;
                            for (int i_07 = 0; i_07 < new_board.wList.n; ++i_07) {
                                if (new_board.board[new_board.wList.moves[i_07].y][new_board.wList.moves[i_07].x] == ChessPiece::W_KING) {
                                    kingX = new_board.wList.moves[i_07].x;
                                    kingY = new_board.wList.moves[i_07].y;

                                    float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));

                                    if (new_taxi < 4.0) {
                                        float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                        if (new_taxi < old_taxi) isTaxi = true;
                                        if ((new_taxi == old_taxi) && (xy_to.x + xy_to.y < xy_from.x + xy_from.y)) isTaxi = true;
                                    }
                                    break;
                                }
                            }
                        }

                        if (iKingSecurity == 0) {
                            // Безопасность Короля не учитывается не продлевается.
                            isTaxi = false;
                        }

                        bool isSquareKingActivity = false;
                        // Включаем продления если детектированы действия в квадрате Короля.

                        {
                            char kingX, kingY;
                            for (int i_07 = 0; i_07 < new_board.bList.n; ++i_07) {
                                if (new_board.board[new_board.bList.moves[i_07].y][new_board.bList.moves[i_07].x] == ChessPiece::B_KING) {
                                    kingX = new_board.bList.moves[i_07].x;
                                    kingY = new_board.bList.moves[i_07].y;

                                    // Попали в квадрат Короля.

                                    float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));


                                    float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                    if (new_taxi < 1.47) isSquareKingActivity = true;
                                    if (old_taxi < 1.47) isSquareKingActivity = true;

                                    break;
                                }
                            }
                        }

                        {
                            char kingX, kingY;
                            for (int i_07 = 0; i_07 < new_board.wList.n; ++i_07) {
                                if (new_board.board[new_board.wList.moves[i_07].y][new_board.wList.moves[i_07].x] == ChessPiece::W_KING) {
                                    kingX = new_board.wList.moves[i_07].x;
                                    kingY = new_board.wList.moves[i_07].y;

                                    // Попали в квадрат Короля.

                                    float new_taxi = sqrtf((xy_to.x - kingX) * (xy_to.x - kingX) + (xy_to.y - kingY) * (xy_to.y - kingY));


                                    float old_taxi = sqrtf((xy_from.x - kingX) * (xy_from.x - kingX) + (xy_from.y - kingY) * (xy_from.y - kingY));

                                    if (new_taxi < 1.47) isSquareKingActivity = true;
                                    if (old_taxi < 1.47) isSquareKingActivity = true;

                                    break;
                                }
                            }
                        }

                        bool bMiddleGame = true;// начало или середина игры.
                        // 30.07.2023 усиление эндшпиля.
                        const float conic_rate = calculation_phase_for_conic_rate(board.board, board.wList, board.bList);
                        if (conic_rate > KING_CENTER_PRIORITY_WEIGHT) {
                            bMiddleGame = false;
                        }

                        bool bExtensions0;

                        if (bQueekGame) {
                            // Были ли продления при шахе ?
                            //bool bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                              //  new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O)||
                                //InCheck(board.board, board.wList, board.bList, board.previos_moves, color,
                                  //  board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                            bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                  new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O);
                        }
                        else {
                            // Были ли продления при шахе ?
                                bExtensions0 = InCheck(new_board.board, new_board.wList, new_board.bList, new_board.previos_moves, Color_invert(color),
                                new_board.WHITE_O_O, new_board.WHITE_O_O_O, new_board.BLACK_O_O, new_board.BLACK_O_O_O) ||
                                ((bMiddleGame)&&((n_01 <= 1) || (iply < 6 ? captured_see /* || captured_see_memo */ : false) ||
                                // Белая пешка на последней горизонтали.
                                ((color == Color::WHITE) && (board.board[y][x] == ChessPiece::W_PAWN) && (xy_to.y == 1)) ||
                                // Чёрная пешка на последней горизонтали.
                                ((color == Color::BLACK) && (board.board[y][x] == ChessPiece::B_PAWN) && (xy_to.y == 6)) ||
                                 (iply < 1 ? isSquareKingActivity : false)));
                        }
                        bool doNullMove0 = false; // Не более одной эвристики нулевого хода в ветке.

                        // Principial Variation Search
                        //if (k_1 < 8)
                        {

                           // m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1, ishah + 1,false, doNullMove0, bExtensions0);

                            // depth - 1 слишком глубокое дерево получается очень долго считает варианты.
                            // Продлевать взятия нельзя программа очень долго думает видно взятий очень много.
                            // шах можно продлевать, по времени приемлемо и качество игры лучше.

                            

                            m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, (/*captured_see  ||*/ bExtensions0||isTaxi  ? depth  : depth + 1),
                                iply + 1, ishah + 1, false, doNullMove0, bExtensions0 || isTaxi,tid, captured_see,(isTaxi ? 1 : 0),
                                (isSquareKingActivity ? 1 : 0));

                           
                        }
                        /*else {

                            // Быстрый просчёт с нулевым окном
                            if (enemy) {
                                //min

                                m = alpha_beta_do1(beta-1.0f, beta, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1, ishah + 1,true, doNullMove0, bExtensions0);
                            }
                            else {
                                //max
                                m = alpha_beta_do1(alpha, alpha + 1.0f, my_color, enemy_color, my_depth, new_board, depth + 1, iply + 1, ishah + 1,true, doNullMove0, bExtensions0);
                            }

                            
                           
                            // если эвристика не сработала, и ход улучшает
                            // alpha, то мы пересчитаем с полным окном
                            if ((m.moves[0].rate > alpha) && (m.moves[0].rate < beta)) {

                                Board new_board2;

                                // Копирование доски.
                                new_board2.copy(board.board, board.wList, board.bList, board.previos_moves, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O);

                                new_board2.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                                 // Предыдущий ход на этой новой  доске после хода.
                                new_board2.previos_moves.xy_from = xy_from;
                                new_board2.previos_moves.xy_to = xy_to;
                                new_board2.previos_moves.figure = board.board[y][x];

                                m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board2, depth + 1, iply + 1, ishah + 1,false, false, false);
                            }
                        }*/
                        // Было взятие - итеративное углубление.
                        // captured - Он почемуто жертвует фигуры (выбрасывает фигуры). Коня за пешку.
                        // Попробуем captured_see только равные или победившие взятия.
                        //m = alpha_beta_do1(alpha, beta,   my_color, enemy_color, my_depth, new_board, (captured_see ? depth : depth + 1),iply+1, false);
                        //  детектор шахов InCheck(new_board.board, new_board.previos_moves, color)
                        // В малофигурном окончании правило captured_see неверно и заменено на captured так как Король тоже может есть.
                       // m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board, 
                         //   (captured_see|| InCheck(new_board.board, new_board.previos_moves, color)||
                           //     (captured&& number_figures(new_board.board)<=ENDSHPIL_NUMBER_FIGURES) ? depth : depth + 1), iply + 1, false);
                        // Нам же не нужно чтобы он слона за пешку отдал. Без SEE он просто отдаёт материал.
                        //Color_invert(color) правильное продление шахов с invert color так как на новой доске и цвет новый,
                        // но продление шахов инициирует всё новые шахи так же как продление взятий инициирует новые взятия иногда неравноценные жертвы
                        // фигуры выбрасываются.
                        //-->m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board,
                            //(captured_see   || InCheck(new_board.board, new_board.previos_moves, Color_invert(color)) ? depth : depth + 1), iply + 1, false);

                       // m = alpha_beta_do1(alpha, beta, my_color, enemy_color, my_depth, new_board,
                         //   (captured_see  || ((ishah==0)&&InCheck(new_board.board, new_board.previos_moves, Color_invert(color))) ? depth -1 : depth + 1),
                           // iply + 1, (InCheck(new_board.board, new_board.previos_moves, Color_invert(color)) ? ishah+1:ishah), false);

                        rate_Current = m.moves[0].rate;

                        if (m.n == 0) {
                            continue;
                        }

                        rate_calc[omp_get_thread_num()] = m.moves[0].rate;

#pragma omp critical  (update_alpha_beta)
                        {

                            if (enemy) {
                                //min
                               // rate = (rate < m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)
                               // rate = std::min(rate, m.moves[0].rate);
                                rate = std::min(rate, rate_calc[omp_get_thread_num()]);

                            }
                            else {
                                //max

                                //rate = (rate > m.moves[0].rate ? rate : m.moves[0].rate);// (best_move)
                                //rate = std::max(rate, m.moves[0].rate);
                                rate = std::max(rate, rate_calc[omp_get_thread_num()]);
                            }

                            // Оценка позиции не лучший ход.
                            //17.12.2022 закоментировал rate = m.moves[0].rate; // (best_move)

                            if ((captured) && (!enemy)) {
                                //rate += my_depth - depth; // Добавим немного агрессии
                            }

                            if (enemy) {
                                //min

                                //beta = (rate < beta ? rate : beta);
                                beta = std::min(rate, beta);
                            }
                            else {
                                // max

                                //if (rate > alpha) alpha = rate;
                                //alpha = (rate > alpha ? rate : alpha);
                                alpha = std::max(rate, alpha);


                            }

                        }



                            //if (depth == 0) std::cout << "\n rate= " << rate << std::endl;

                            if (alpha >= beta) {
#ifndef CODE_FOR_DLL
                                //  if (depth == 0) std::cout << "alpha-beta\n";
#endif
                         // rate = beta;// отсечение
                         // Возвратить rate (best_move).

#pragma omp critical (add_move1)
                                {

                                rates.n++;
                                //rates_sum_n = rates_sum_n + 1;

                                //if (rates.n - 1 > n_01 - 1) {
#ifndef CODE_FOR_DLL
                                  //  printf("error! moves limit < n_01\n");
                                    //system("PAUSE");
#endif
                               // }

                                rates.moves[rates.n - 1].rate = rate;
                                //rates.moves[rates.n - 1].rate = rate_Current;

                                rates.moves[rates.n - 1].xy_from = xy_from;
                                rates.moves[rates.n - 1].xy_to = xy_to;
                                flag_in[rates.n - 1] = true;


                                //rates.moves[rates.n - 1].rate = rate;

                                //rates.moves[rates.n - 1].xy_from = xy_from;
                                //rates.moves[rates.n - 1].xy_to = xy_to;
                                //flag_in[rates.n - 1] = true;

                                //rates.moves[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())].rate = rate;
                                // std::cout << "i=" << rates.n - 1 << " rate=" << rate << std::endl;
                                // printf("i=%d rate= %f\n",rates.n - 1, rate);

                                //rates.moves[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())].xy_from = xy_from;
                                //rates.moves[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())].xy_to = xy_to;
                                //flag_in[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())] = true;
                                //id_thread[omp_get_thread_num()] += omp_get_num_threads();



                                    //goto ALPHA_BETA_CONT;
                                    b_we_should_be_continue = false;
                                }

                        }


                    

                    }


                   #pragma omp critical (add_move2)
                    {
                         rates.n++;
                        //rates_sum_n = rates_sum_n + 1;

                         //if (rates.n - 1 > n_01 - 1) {
 //#ifndef CODE_FOR_DLL
                           //  printf("error! moves limit < n_01\n");
                           //  system("PAUSE");
 //#endif
                         //}

                         rates.moves[rates.n - 1].rate = rate;
                         //rates.moves[rates.n - 1].rate = rate_Current;

                         rates.moves[rates.n - 1].xy_from = xy_from;
                         rates.moves[rates.n - 1].xy_to = xy_to;
                         flag_in[rates.n - 1] = true;

                        //rates.moves[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())].rate = rate;
                        // std::cout << "i=" << rates.n - 1 << " rate=" << rate << std::endl;
                        // printf("i=%d rate= %f\n",rates.n - 1, rate);

                        //rates.moves[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())].xy_from = xy_from;
                        //rates.moves[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())].xy_to = xy_to;
                        //flag_in[std::min(POOL_SIZE - 1, id_thread[omp_get_thread_num()] + omp_get_thread_num())] = true;
                        //id_thread[omp_get_thread_num()] += omp_get_num_threads();

                    }
                }

            }
        }
        // }
     //}



   

 // Плотная упаковка от нуля до rates.n-1
       /* 
       // Добавления все последовательные т.к. они все в critical section.
       int imesto0 = 0;
        int imesto1 = 0;
        for (int k_i = 0; k_i < POOL_SIZE - 1; ++k_i)
        {

            if (flag_in[imesto1]) {
                if (imesto1 > imesto0) {
                    rates.moves[imesto0] = rates.moves[imesto1];
                    imesto0++;
                    imesto1++;
                }
            }
            else {
                ++imesto1;
            }

        }*/

       // rates.n = rates_sum_n;

        // Освобождение оперативной памяти.
      //  if (make_moves != nullptr) {
        //    delete[] make_moves;
          //  make_moves = nullptr;
        //}


        // ALPHA_BETA_CONT: // результат отсечения досрочное прерывание сканирования

        if (depth == 0) {

            /* if (fabs(rate) > 9998.0) {
                 // Цель игры мат королю противника.
                 rates.n = 1;
                 rates.moves[0].rate = rate;
                 rates.moves[0].xy_from.x = None;
                 rates.moves[0].xy_from.y = None;
                 rates.moves[0].xy_to.x = None;
                 rates.moves[0].xy_to.y = None;
             }*/



            return rates;
        }

#ifndef CODE_FOR_DLL
        if (depth == 1) {
            //   std::cout << "1 BLACK evalute " << rates.n << " " << (rates.n > 0 ? rates.moves[0].rate : 0) << std::endl;
        }
#endif

        if (rates.n == 0) {

            if (fabs(rate) > fabs(rate_flag)) {
                // Цель игры мат королю противника.
                rates.n = 1;
                if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
                    // Мы под шахом и нам некуда идти -> мат.
                    rates.moves[0].rate = rate;
                }
                else {
                    // Некуда идти и мы не под шахом ничья.
                    rate = 0.0; // Пат
                    rates.moves[0].rate = rate;
                }
                rates.moves[0].xy_from.x = None;
                rates.moves[0].xy_from.y = None;
                rates.moves[0].xy_to.x = None;
                rates.moves[0].xy_to.y = None;
            }

            return rates; // Как бы возврат None так как rates.n == 0.
        }

        LIST_MOVES m;
        m.n = 1;

        {


            int id_found = 0;
            float rate_found = rates.moves[0].rate;

            // Мы находимся на некоторой глубине вложенности

            if (enemy) {

                // Поиск минимума  
                // Враг черные при первом ходе белых или наборот при первом ходе черных.

                for (int i = 1; i < rates.n; ++i) {
                    if (rates.moves[i].rate < rate_found) {
                        rate_found = rates.moves[i].rate;
                        id_found = i;
                    }
                }

            }
            else {
                // Поиск максимума   
                // Белые  при первом ходе белых или наборот при первом ходе черных.

                for (int i = 1; i < rates.n; ++i) {
                    if (rates.moves[i].rate > rate_found) {
                        rate_found = rates.moves[i].rate;
                        id_found = i;
                    }
                }
            }

            //if (0) {
                //std::cout << "\n";
                //for (__int8 i = 1; i < rates.n; ++i) {
                  //  if (fabs(rates.moves[i].rate - rate_found) < 0.01) {
                    //    std::cout << i << " ";
                    //}
                //}
                //std::cout << "\n";
            //}


            m.moves[0].rate = rates.moves[id_found].rate;

            // m.moves[0].xy_from=init(None);
             //m.moves[0].xy_to=init(None);

            m.moves[0].xy_from = rates.moves[id_found].xy_from;
            m.moves[0].xy_to = rates.moves[id_found].xy_to;
        }
        if (depth == 1) {
            // std::cout << "2 BLACK evalute " <<  (m.n > 0 ? m.moves[0].rate : 0) << std::endl;
        }
        // system("PAUSE");

        return m;
    }
    else {



        LIST_MOVES rates;

        rates.n = 0;

        //if (rates.n == 0) {

        if (fabs(rate) > fabs(rate_flag)) {
            // Цель игры мат королю противника.
            rates.n = 1;
            if (InCheck(board.board, board.wList, board.bList, board.previos_moves, color, board.WHITE_O_O, board.WHITE_O_O_O, board.BLACK_O_O, board.BLACK_O_O_O)) {
                // Мы под шахом и нам некуда идти -> мат.
                rates.moves[0].rate = rate;
            }
            else {
                // Некуда идти и мы не под шахом ничья.
                rate = 0.0; // Пат
                rates.moves[0].rate = rate;
            }
            rates.moves[0].xy_from.x = None;
            rates.moves[0].xy_from.y = None;
            rates.moves[0].xy_to.x = None;
            rates.moves[0].xy_to.y = None;
        }

        return rates; // Как бы возврат None так как rates.n == 0.

    }
    //return ;
}


// alpha_beta_algorithm
// Некоторые называют это искуственным интеллектом игры в шахматы.
LIST_MOVES minimax(Color color, __int8 thinking_depth, Board board) {

    // 30.07.2023 усиление эндшпиля.
    const float conic_rate = calculation_phase_for_conic_rate(board.board, board.wList, board.bList);
    if (conic_rate > KING_CENTER_PRIORITY_WEIGHT) {
        // перешли в эндшпиль. Усиление анализа вариантов в эндшпиле.
        if (THINKING_DEPTH < 6) {
           // THINKING_DEPTH = 6;// 12;
        }
    }
    //if (conic_rate > ENDSHPIL_INCREASE_DEPTH_POROUG) {
            // перешли в глубокий эндшпиль
      //      THINKING_DEPTH = 8;// 12; не тянет
       // }

    number_chess_positions = 0;
  
    for (int tid = 0; tid < number_cores; ++tid) {
        for (int iply = 0; iply < max_h; ++iply) {
            killer_euristick[tid][iply].rate = 0.0; // не используется
            killer_euristick[tid][iply].xy_from.x = None;
            killer_euristick[tid][iply].xy_from.y = None;
            killer_euristick[tid][iply].xy_to.x = None;
            killer_euristick[tid][iply].xy_to.y = None;
            number_chess_positions_arr[tid] = 0;
        }
    }

    Color my_color = color;
    // Вражеский цвет должен быть другого знака.
    Color enemy_color = Color_invert(color); // Цвет врага.

    __int8 my_depth = thinking_depth;
    
    //if (number_figures(board.board, board.wList, board.bList) == ENDSHPIL_NUMBER_FIGURES) {
        // всего 8 фигур на доске.
        // малофигурное окончание существенно увеличиваем глубину перебора.
       // my_depth = 8;
    //}
    //if (number_figures(board.board, board.wList, board.bList) == 4) {
       //-----> my_depth = 8;
    //}


#ifdef CODE_FOR_DLL
    // Ответ за белых думать не должен а должен просто проверить мат, пат.
    if (color == Color::WHITE) {
        if (breduction_white) {
            my_depth = 1;
        }
    }
#endif  

    const bool bprincipal_variation_search = false;

    LIST_MOVES rates;
    
    if (ilevel_Game == 2) {

        // С форсированным вариантом.

        const char depth = 0;
        const char iply = 0;
        const char ishah = 0;



         //rates = alpha_beta_do1(-10000.0, 10000.0, my_color, enemy_color, my_depth, board, 0, 0);
       
         //rates = alpha_beta_do1(-10000.0, 10000.0, my_color, enemy_color, my_depth, board, depth, iply, ishah, bprincipal_variation_search, false,false,0,false,0,0);
         
         // 11.08.2023 Сделано как у Евгения Корнилова.
        // rates = AlphaBeta(-10000.0, 10000.0, my_color, enemy_color, my_depth, board, depth, iply, ishah, bprincipal_variation_search, false, false, 0, false, 0, 0);

         // 23.09.2023
         // Поиск стремления
        float scope= Evaluate(my_color, enemy_color, board, iply); // Статическая оценка текущей позици
        Pawn pawn_val;
        float window = 0.5f * pawn_val.VALUE;
        float alpha = scope - window;
        float beta = scope + window;
        rates = AlphaBeta(alpha, beta, my_color, enemy_color, my_depth, board, depth, iply, ishah, bprincipal_variation_search, false, false, 0, false, false, 0, 0);

        int id_found = 0;
        float rate_found = rates.moves[0].rate;

        // Мы находимся на некоторой глубине вложенности


            // Поиск максимума   
            // Белые  при первом ходе белых или наборот при первом ходе черных.

        for (int i = 1; i < rates.n; ++i) {
            if (rates.moves[i].rate > rate_found) {
                rate_found = rates.moves[i].rate;
                id_found = i;
            }
        }

        scope= rates.moves[id_found].rate;

        if (scope <= alpha) {
            alpha = -10000.0;
            rates = AlphaBeta(-10000.0, beta, my_color, enemy_color, my_depth, board, depth, iply, ishah, bprincipal_variation_search, false, false, 0, false, false, 0, 0);
        }
        else if (scope >= beta) {
            beta = 10000.0;
            rates = AlphaBeta(alpha, 10000.0, my_color, enemy_color, my_depth, board, depth, iply, ishah, bprincipal_variation_search, false, false, 0, false, false, 0, 0);
        }

         for (int tid = 0; tid < number_cores; ++tid) {
             number_chess_positions += number_chess_positions_arr[tid];
         }
    }
    else {

        

         const char depth = 0;
         const char iply = 0;

         if ((THINKING_DEPTH == 6)||(THINKING_DEPTH == 8)) {

             const char ishah = 0;

             // Распараллеленный минимакс на 40 потоков. Каждый ход в отдельном потоке.
             // rates = minimax_do_parallel(my_color, enemy_color, my_depth, board, 0);
             // rates = alpha_beta_do1(-10000.0, 10000.0, my_color, enemy_color, my_depth, board, depth, iply, ishah);
             // параллельный alpha-beta algorithm 
             rates =  parallel_chess_engine1(-10000.0, 10000.0, my_color, enemy_color, my_depth, board, depth, iply, ishah);
             for (int tid = 0; tid < number_cores; ++tid) {
                 number_chess_positions += number_chess_positions_arr[tid];
             }
         }
         else {

             // Без форсированного варианта.

             //rates = minimax_do(my_color, enemy_color, my_depth, board, 0);
             rates = alpha_beta_do(-10000.0, 10000.0, my_color, enemy_color, my_depth, board, depth, iply, bprincipal_variation_search);
         }
    }

#ifndef CODE_FOR_DLL
   //std::cout << "n=" << rates.n << std::endl;
  // printf("n=%d\n", rates.n);
   //system("PAUSE");
#endif

    

    return rates;
}


// Игра в шахматы со стартовой позиции.
void Game() {

    // Второй (наивысший 13.09.2022) уровень игры. В честь советской программы Каисса и первого чемпионата мира среди машин. Четыре полухода с формированным вариантом.
    // Только четыре полухода с форсированым вариантом. На фотографии Михаил Донской - один из авторов программы Каисса.
    THINKING_DEPTH = 4;
    ilevel_Game = 2;

    Board chess_board;

    chess_board.init();
    //chess_board.board[1][2] = Pawn(Color::BLACK).get();
    //chess_board.board[0][3] = King(Color::BLACK).get();
    //chess_board.board[7][3] = King(Color::WHITE).get();

    // simple start position on the board
    chess_board.board[1][0] = Pawn(Color::BLACK).get();
    chess_board.board[1][1] = Pawn(Color::BLACK).get();
    chess_board.board[1][2] = Pawn(Color::BLACK).get();
    chess_board.board[1][3] = King(Color::BLACK).get();
    chess_board.board[5][1] = Pawn(Color::WHITE).get();
    chess_board.board[5][3] = King(Color::WHITE).get();
    chess_board.board[2][2] = Rook(Color::WHITE).get(); // Ладья.

    chess_board.init2();

    chess_board.print();// Печатаем исходное состояние доски.

    Color color = Color::WHITE; // Ход чёрных
    const __int8 number_of_half_passes = 8; // Число полуходов в партии (длительность партии).

    const __int8 iprint = -1;

    for (__int8 i = 0; i < number_of_half_passes; ++i) {

        // Кандидат на будущий лучший ход.
        float  max_rate = -9999;
        MOVES_NODE xy_from;
        MOVES_NODE xy_to;
        xy_from=init(None);
        xy_to=init(None);
    

        // Генерация списка возможных ходов с оценкой позиции:
        LIST_MOVES rates = minimax(color, THINKING_DEPTH, chess_board);
        // Поиск в цикле наилучшего хода из списка сгенерированных.
        for (int id_rate=0; id_rate < rates.n; ++id_rate)
        {
#ifndef CODE_FOR_DLL
            if (i == iprint) {
                //std::cout << rates.moves[id_rate].rate << "\n";
                //std::cout << "from =" << rates.moves[id_rate].xy_from.x << " " << rates.moves[id_rate].xy_from.y << " to=" << rates.moves[id_rate].xy_to.x << " " << rates.moves[id_rate].xy_to.y << std::endl;
                printf("%f \n from =%d %d to= %d %d\n", rates.moves[id_rate].rate, rates.moves[id_rate].xy_from.x, rates.moves[id_rate].xy_from.y, rates.moves[id_rate].xy_to.x, rates.moves[id_rate].xy_to.y);
            }

            //******
            if (i == iprint) {
                // Устарело надо писать копирование развёрнуто.
                Board cb;
                // Копирование доски.
                for (__int8 y_1 = 0; y_1 < 8; ++y_1) {
                    for (__int8 x_1 = 0; x_1 < 8; ++x_1) {
                        cb.board[y_1][x_1] = chess_board.board[y_1][x_1];
                    }
                }
                cb.previos_moves.xy_from = chess_board.previos_moves.xy_from;
                cb.previos_moves.xy_to = chess_board.previos_moves.xy_to;
                cb.previos_moves.figure = chess_board.previos_moves.figure;

                MOVES_NODE xy_from1 = rates.moves[id_rate].xy_from;
                MOVES_NODE xy_to1 = rates.moves[id_rate].xy_to;
                cb.move(xy_from1, xy_to1);

                cb.previos_moves.xy_from = xy_from1;
                cb.previos_moves.xy_to = xy_to1;
                cb.previos_moves.figure = chess_board.board[xy_to1.y][xy_to1.x];
                cb.print();
            }
           //*****
#endif

           if (rates.moves[id_rate].rate < max_rate) {
              continue;
           }
           // запоминаем кандидата на наилучший ход.
           if (max_rate < rates.moves[id_rate].rate) 
           {
               max_rate = rates.moves[id_rate].rate;
               xy_from = rates.moves[id_rate].xy_from;              
               xy_to = rates.moves[id_rate].xy_to;
              
           }
           
#ifndef CODE_FOR_DLL
           if ((xy_from.x == None) || (xy_from.y == None)) {
               printf("end; \n");
               system("pause");

               exit(0);
           }
#endif
           
        }

      // if (i== iprint) system("PAUSE");
       // Предыдущий ход на этой старой  доске.
       //chess_board.previos_moves.xy_from = chess_board.previos_moves.xy_from;
       //chess_board.previos_moves.xy_to = chess_board.previos_moves.xy_to;
       //chess_board.previos_moves.figure = chess_board.previos_moves.figure;

       chess_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

        // Предыдущий ход на этой новой  доске после хода.
       chess_board.previos_moves.xy_from = xy_from;
       chess_board.previos_moves.xy_to = xy_to;
       chess_board.previos_moves.figure = chess_board.board[xy_to.y][xy_to.x];

      

        // смена цвета.
        color = Color_invert(color);
            
        chess_board.print();// Печатаем состояние доски после
#ifndef CODE_FOR_DLL
        std::cout << max_rate << "\n";
        if (i == iprint) {
            //std::cout << "from =" << xy_from.x << " " << xy_from.y << " to=" << xy_to.x << " " << xy_to.y << std::endl;
           // printf("from =%d %d to= %d %d\n", xy_from.x, xy_from.y, xy_to.x, xy_to.y);
        }
#endif

    }
}

#ifndef CODE_FOR_DLL
/*
int main()
{
#ifdef _OPENMP

    omp_set_num_threads(number_cores); // Не более 40 потоков.

#endif

    //_setmode(_fileno(stdout), _O_U8TEXT);
    //_setmode(_fileno(stdin), _O_U8TEXT);

    _isatty(_fileno(stdout));
    _setmode(_fileno(stdout), _O_WTEXT);

    //SetConsoleCP(CP_UTF8);
    //SetConsoleOutputCP(CP_UTF8);

    //SetConsoleCP(1251); SetConsoleOutputCP(1251);
    //SetConsoleOutputCP(65001);

    //cout.imbue(locale("rus_RUS.866"));
    //page.imbue(locale(locale(), new std::codecvt_utf8<wint_t>));

    if (0) {
        // Урок 1

        Board board;

        board.init();
        board.board[1][2] = Pawn(Color::BLACK).get();
        board.board[0][3] = King(Color::BLACK).get();
        board.board[7][3] = King(Color::WHITE).get();
        board.init2();

        board.print();// Печатаем исходное состояние доски.

        MOVES m = board.get_moves(2, 1);//список возможных ходов из позиции [2,1].
        MOVES_NODE xy_from;
        xy_from=init(2,1);

        MOVES_NODE xy_to = (m.n > 0 ? m.moves[0] : xy_from);

        if (m.n == 0) std::cout << "Error! empty move pawn\n";

        board.move(xy_from, xy_to,41); // Двигаем фигуру по первому попавшемуся ходу.
        board.previos_moves.xy_from=xy_from;
        board.previos_moves.xy_to=xy_to;
        board.previos_moves.figure = ChessPiece::B_PAWN;

        board.print();// Печатаем состояние доски после.
    }
    else {
        // Урок 3

        clock_t start = clock();

        // Алгоритм игры в шахматы, бот играет сам с собой
        // начиная со стартовой позиции на доске.
        Game();

        clock_t end = clock();
        double seconds = (double)(end - start) / CLOCKS_PER_SEC;
        std::cout << "The time: " << seconds << " seconds\n";
    }
    system("PAUSE");
}
*/
#endif

#ifdef CODE_FOR_DLL

// cb - шахматная доска.
// clr цвет (true - ход Чёрных пока это всегда так, false - ход белых).
// move1 ответный ход Чёрных и информация о предыдущем ходе.
// O_O - информация о рокировке.
// rate_out2 - информация об оценке позиции возвращаемая в интерфейс.
extern "C" EXPORT void analiticChess(int* cb, bool clr,
    int* move1, bool* O_O, float* rate_out2)
//int main_for_dll(int* cb, bool clr,
  //  int* move1, bool* O_O)
{

#ifdef _OPENMP

    omp_set_num_threads(number_cores); // Не более 40 потоков.

#endif

    // Сейчас ход Чёрных. Алгоритм минимакса должен ответить за чёрных.

    clock_t start = clock();

    

    // Задаём доску.

    Board chess_board;

    chess_board.init();


   // int cb[64] = { 9, 11, 10, 8, 7, 10, 11, 9, 12 , 12 , 12 , 12 , 12 , 12 ,12 ,12 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 3, 5, 4, 2, 1, 4, 5, 3};

   // int cb[64] = { 0, 7, 0, 0, 0, 0, 0, 11, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   /* int cb[64] =
      { 7, 0, 0, 0, 0, 0, 0, 3, 
        0, 0, 0, 0, 0, 0, 0, 3,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 11,
        0, 0, 0, 0, 0, 0, 1, 0,
        10, 0, 0, 0, 0, 0, 0, 0 };
        */
    //W_KING = 1, W_QUEEN = 2, W_ROOK = 3, W_BISHOP = 4, W_KNIGHT = 5, W_PAWN=  6, B_KING = 7, B_QUEEN = 8, B_ROOK = 9, B_BISHOP = 10, B_KNIGHT = 11, B_PAWN = 12
    /*int cb[64] =
    { 9, 0, 0, 0, 7, 10, 0, 9,
      12, 12, 12, 4, 12, 12, 12, 12,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 12, 6, 0, 0, 0,
      0, 0, 0, 11, 11, 0, 6, 0,
      0, 0, 0, 0, 0, 2, 0, 6,
      6, 6, 6, 0, 0, 6, 0, 0,
      3, 5, 4, 0, 1, 0, 0, 3 };*//*
      //W_KING = 1, W_QUEEN = 2, W_ROOK = 3, W_BISHOP = 4, W_KNIGHT = 5, W_PAWN=  6, B_KING = 7, B_QUEEN = 8, B_ROOK = 9, B_BISHOP = 10, B_KNIGHT = 11, B_PAWN = 12
      int cb[64] =
      { 0, 7, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 9, 6, 0, 0,
        12, 0, 0, 0, 0, 1, 0, 12,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 12, 0, 0, 0, 0, 0, 0,
        0, 4, 0, 0, 0, 0, 0, 0,
        6, 0, 6, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0 };*/



    int k = 0;
    
    // Текущая расстановка фигур на доске,
    // так чтобы расстановка соответствовала пользовательской,
    // переданной из интерфейса.
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            switch (cb[k]) {
            case 1:
                chess_board.board[i][j] = King(Color::WHITE).get();
                break;
            case 7:
                chess_board.board[i][j] = King(Color::BLACK).get();
                break;
            case 6:
                chess_board.board[i][j] = Pawn(Color::WHITE).get();
                break;
            case 12:
                chess_board.board[i][j] = Pawn(Color::BLACK).get();
                break;
            case 5:
                chess_board.board[i][j] = Knight(Color::WHITE).get();
                break;
            case 11:
                chess_board.board[i][j] = Knight(Color::BLACK).get();
                break;
            case 3:
                chess_board.board[i][j] = Rook(Color::WHITE).get();
                break;
            case 9:
                chess_board.board[i][j] = Rook(Color::BLACK).get();
                break;
            case 4:
                chess_board.board[i][j] = Bishop(Color::WHITE).get();
                break;
            case 10:
                chess_board.board[i][j] = Bishop(Color::BLACK).get();
                break;
            case 2:
                chess_board.board[i][j] = Queen(Color::WHITE).get();
                break;
            case 8:
                chess_board.board[i][j] = Queen(Color::BLACK).get();
                break;
            }
          
            ++k;        
        }
    }

     chess_board.init2();

     if (move1[16] > 0) {

         // При опасном сокращении расстояния между нашим ферзём и королем противника 
         // глубина вариантов продлевается.

         iKingSecurity = 1;
     }

     if (move1[17] > 0) {

         // Тихая позиционная игра.

         flag_POSICIONING_GAME = true;
     }

     if (move1[18] > 0) {
         Futility_pruning = true;
     }
     if (move1[19] > 0) {
         Razoring = true;
     }

     if ((move1[20] > 0) && (move1[20] <= 21)) {
         // Вероятность ошибки ЭВМ мы просто переключаемся на слабого Тьюринга.
         probability_barier = move1[20];
     }

     // Информация о рокировке (булев флаг).
     chess_board.WHITE_O_O = O_O[0]; // была ли рокировка белых в короткую сторону
     chess_board.WHITE_O_O_O = O_O[1]; // была ли рокировка белых в длинную сторону
     chess_board.BLACK_O_O = O_O[2]; // была ли рокировка чёрных в короткую сторону
     chess_board.BLACK_O_O_O = O_O[3]; // была ли рокировка чёрных в длинную сторону

     // предыдущий ход противника (обычно это ход белых который был перед будущим ответом компа).
     chess_board.previos_moves.xy_from.x = move1[4];
     chess_board.previos_moves.xy_from.y = move1[5];
     chess_board.previos_moves.xy_to.x = move1[6];
     chess_board.previos_moves.xy_to.y = move1[7];

     if ((move1[6] >= 0) && (move1[6] <= 7) && (move1[7] >= 0) && (move1[7] <= 7)) 
     {
         chess_board.previos_moves.figure = chess_board.board[move1[7]][move1[6]]; // Фигура которой ходили на предыдущем ходу
     }
     else {
         // Эо может являться признаком первого хода в партии.
         chess_board.previos_moves.figure = ChessPiece::EMPTY;
     }


     bool bfound = false;

     // Опцию дебютного справочника можно просто отключить если выставить ноль.
     // Дебютный справочник сделан только для чёрных if (clr) ...
     if (1&& clr) {
         // Сначала нужно проверить если вариант в базе и если есть то ответить по заготовленному варианту.
         // Хранить позиции партий текстовом файле и ответы на них одноходовые. Вначале хранить число позиций с фигурами.
         // База данных в текстовом файле, можно её постепенно наполнять. Если найдено несколько одинаковых позиций выбрать случайный ответ.
         // Назвать это всё PaulMorphy_database.txt На позицию хранить не более 7 альтернативных ответов.

         // Первые несколько ходов можно сделать по учебнику шахматной игры.
         // Перечислим ответы за чёрных:
         //1. Французская защита : 
         //1. e2-e4 > e7-e6
         //2. d2-d4 -> d7-d5
         //3.1. ed (разменный вариант).
         //3.2 e5 (система Нимцовича).
         //3.3 Nc3 -> {Nc6; de; Cb4; c5; Nf6; Nd2}

         // здесь много теории.
         // 1. e2-e4 -> e7-e5

         // Ускоренный дракон
         // 1. e2-e4 -> c7-c5
         // 2. Nf3 g6
         // 3. d4 cxd4
         // 4. Nxd4 Cg7


         /*FILE* fp_database0 = NULL;
         errno_t err_database0;
         err_database0 = fopen_s(&fp_database0, "PaulMorphy_position.txt", "w");
         for (int i429 = 0; i429 < 64; ++i429) {
             fprintf_s(fp_database0, "%d ", cb[i429]);
             if ((i429 + 1) % 8 == 0) fprintf_s(fp_database0, "\n");
         }
         fclose(fp_database0);
         */

         FILE* fp_database = NULL;
         errno_t err_database;
         //err_database = fopen_s(&fp_database, "\\chess_Data\\Plugins\\x86_64\\debut_book.txt", "r");
         //err_database = fopen_s(&fp_database, "\\debut_book.txt", "r");
         err_database = fopen_s(&fp_database, "debut_book.txt", "r");



         if ((err_database == 0)&&(fp_database!=NULL))
         {
             // Файл успешно открыт.

             // Пока ищет первый найденный ответ
             int inumber_positions = 0;
             int din = 0;
             fscanf_s(fp_database, "%d", &din);
             inumber_positions = din;


             int isize_identical = 0;
             int** move_memo = new int*[7];
             for (int i429 = 0; i429 < 7; ++i429) {
                 move_memo[i429] = new int[9];
             }

             int cb_copy[64] = { 0 };
             for (int i428 = 0; i428 < inumber_positions; ++i428) {
                 //int* cb_copy = new int[64];

                 for (int i429 = 0; i429 < 64; ++i429) {
                     fscanf_s(fp_database, "%d", &din);
                     cb_copy[i429] = din;
                 }
                 // Представление досок в файле базы данных полностью совпадает с программным представлением, 
                 // перенумерация Не нужна.
                 bool bidentical = true;
                 for (int i429 = 0; i429 < 64; ++i429) {
                     if (cb_copy[i429] != cb[i429]) {
                         bidentical = false;
                         break;
                     }
                 }
                 if ((isize_identical<7)&&(bidentical)) {
                     bfound = true;

                     fscanf_s(fp_database, "%d", &din);
                     move_memo[isize_identical][0] = din;
                     fscanf_s(fp_database, "%d", &din);
                     move_memo[isize_identical][1] = din;
                     fscanf_s(fp_database, "%d", &din);
                     move_memo[isize_identical][2] = din;
                     fscanf_s(fp_database, "%d", &din);
                     move_memo[isize_identical][3] = din;

                    // for (int i429 = 0; i429 < 5; ++i429) {
                      //   fscanf_s(fp_database, "%d", &din);
                     //}

                     /*FILE* fp_database1 = NULL;
                     errno_t err_database1;
                     err_database1 = fopen_s(&fp_database1, "PaulMorphy_move.txt", "w");
                     for (int i429 = 0; i429 < 4; ++i429) {
                         fprintf_s(fp_database1, "%d ", move1[i429]);
                     }
                     fclose(fp_database1);
                     */

                     // Информация о рокировке.
                     fscanf_s(fp_database, "%d", &din);
                     move_memo[isize_identical][4] = din;
                     fscanf_s(fp_database, "%d", &din);
                     move_memo[isize_identical][5] = din;
                     fscanf_s(fp_database, "%d", &din);
                     move_memo[isize_identical][6] = din;
                     fscanf_s(fp_database, "%d", &din);
                     move_memo[isize_identical][7] = din;

                     //move1[9] :  0 - партия не окончена, 1 - мат, 2 - пат.
                     fscanf_s(fp_database, "%d", &din); // плюс одно считывание за move[9].

                     move1[9] = 0;

                     isize_identical++;

                 }
                 else {

                     for (int i429 = 0; i429 < 9; ++i429) {
                         fscanf_s(fp_database, "%d", &din);
                     }
                 }

                 //delete[] cb_copy;
             }

             if (bfound) {

                 std::mt19937 gen{ std::random_device()() };
                 int id_out = gen() % isize_identical;

                 move1[0] = move_memo[id_out][0];
                 move1[1] = move_memo[id_out][1];
                 move1[2] = move_memo[id_out][2];
                 move1[3] = move_memo[id_out][3];

                 O_O[0] = (move_memo[id_out][4] == 0 ? false : true);
                 O_O[1] = (move_memo[id_out][5] == 0 ? false : true);
                 O_O[2] = (move_memo[id_out][6] == 0 ? false : true);
                 O_O[3] = (move_memo[id_out][7] == 0 ? false : true);
             }

             for (int i429 = 0; i429 < 7; ++i429) {
                 delete[]  move_memo[i429];
             }
             delete[]  move_memo;

             fclose(fp_database);
         }
         else {
             FILE* fp_database1 = NULL;
             errno_t err_database1;
             err_database1 = fopen_s(&fp_database1, "database_errno_open.txt", "w");
            
                 fprintf_s(fp_database1, "%d ", err_database);
             
             fclose(fp_database1);
         }
     }

     if (!bfound) {

         switch (move1[10]) {
         case 0:
             // Нулевой уровень игры. Бумажная машина Алана Тьюринга. Два полухода без форсированного варианта.
             // Только два полухода без форсированого варианта, пропускает детский мат.
             THINKING_DEPTH = 2;
             ilevel_Game = 0;
             break;
         case 1:
             // Первый уровень игры. В честь Клода Шеннона. Четыре полухода без форсированного варианта.
             // Только четыре полухода без форсированого варианта.
             THINKING_DEPTH = 4;
             ilevel_Game = 0;
             break;
         case 2:
             // Второй уровень игры. В честь Джон фон Нейман. Два полухода с форсированным вариантом.
             // Только два полухода с форсированым вариантом.
             THINKING_DEPTH = 2;
             ilevel_Game = 2;
             break;
         case 3:
             // Третий уровень игры. В честь Кена Томпсона. Шесть полуходов с форсированным вариантом.
             // Только шесть полуходов с форсированным вариантом.
             // Используется эвристика нулевого хода.
             THINKING_DEPTH = 6;
             ilevel_Game = 2;
             break;
         case 4:
             // Четвёртый (наивысший 13.09.2022) уровень игры. В честь советской программы Каисса и первого чемпионата мира среди машин.
             //  Четыре полухода с формированным вариантом.
             // Только четыре полухода с форсированым вариантом. На фотографии Михаил Донской.
             THINKING_DEPTH = 4;
             ilevel_Game = 2;
             break;
         case 5:
             // Пятый (наивысший 22.07.2023) уровень игры. В честь отца кбернетики Норберта Винера. Восемь полуходов с форсированным вариантом.
             // Только восемь полуходов с форсированным вариантом.
             // Используется эвристика нулевого хода.
             THINKING_DEPTH = 8;
             ilevel_Game = 2;
             break;
         }

         Color color = Color::BLACK;
         if (clr == false) {
             color = Color::WHITE;
         }
         //const __int8 number_of_half_passes = 8; // Число полуходов в партии (длительность партии).

         const __int8 iprint = -1;

         int  i = 0;

         // Кандидат на будущий лучший ход.
         float  max_rate = -9999;
         MOVES_NODE xy_from;
         MOVES_NODE xy_to;
         xy_from = init(None);
         xy_to = init(None);

         {

             // Генерация списка возможных ходов с оценкой позиции:
             LIST_MOVES rates = minimax(color, THINKING_DEPTH, chess_board);

             //std::cout << rates.n<< std::endl;

             // Поиск в цикле наилучшего хода из списка сгенерированных.
             for (int id_rate = 0; id_rate < rates.n; ++id_rate)
             {

                 //std::cout << "current rate = " << rates.moves[id_rate].rate << std::endl;

                 if (rates.moves[id_rate].rate < max_rate) {
                     continue;
                 }
                 // запоминаем кандидата на наилучший ход.
                 if (max_rate < rates.moves[id_rate].rate)
                 {
                     
                     max_rate = rates.moves[id_rate].rate;
                     xy_from = rates.moves[id_rate].xy_from;
                     xy_to = rates.moves[id_rate].xy_to;

                 }
             }

             if (clr) {

                 // Запускается слабый Алан Тьюринг в probability_barier процентах случаев (ослабление игры ЭВМ). 
                 // Но только для Каисса, Кен Томпсон, Норберт Виннер.
                 if ((bBOT_INTERESTING) && (move1[10] > 2)) {
                     std::mt19937 gen{ std::random_device()() };
                     std::uniform_int_distribution<> dis(0, 99);
                     int probability_of_the_best_move = dis(gen); // Вероятность лучшего хода в процентах.
                     int isize = dis(gen);
                     for (int i6 = 0; i6 < isize; i6++) {
                         probability_of_the_best_move = dis(gen);
                     }


                     // Имитация неидеальности игры машиной. В probability_barier процентах случаев (ослабление игры ЭВМ) ход выбирается некачественно. 
                     // Это чтобы человеку было интересней играть, т.к. никто не любит постоянно проигрывать.
                     // Это запланированное ослабление игры ЭВМ. 1 october 2023




                     // мы просто запустим наислабейшего Тьюринга
                     // probability_barier - задаёт пользователь шахматной программы.
                     if (probability_of_the_best_move < probability_barier) {// между 15% и 21%.

                         // Нулевой уровень игры. Бумажная машина Алана Тьюринга. Два полухода без форсированного варианта.
                         // Только два полухода без форсированого варианта, пропускает детский мат.
                         THINKING_DEPTH = 2;
                         ilevel_Game = 0;

                         i = 0;

                         // Кандидат на будущий лучший ход.
                         max_rate = -9999;
                         xy_from = init(None);
                         xy_to = init(None);

                         // Генерация списка возможных ходов с оценкой позиции:
                         rates = minimax(color, THINKING_DEPTH, chess_board);

                         //std::cout << rates.n<< std::endl;



                         // Поиск в цикле наилучшего хода из списка сгенерированных.
                         for (int id_rate = 0; id_rate < rates.n; ++id_rate)
                         {

                             //std::cout << "current rate = " << rates.moves[id_rate].rate << std::endl;

                             if (rates.moves[id_rate].rate < max_rate) {
                                 continue;
                             }
                             // запоминаем кандидата на наилучший ход.
                             if (max_rate < rates.moves[id_rate].rate)
                             {

                                 max_rate = rates.moves[id_rate].rate;
                                 xy_from = rates.moves[id_rate].xy_from;
                                 xy_to = rates.moves[id_rate].xy_to;

                             }
                         }

                     }

                 }
             }
             rate_out2[0] = max_rate;

             //chess_board.move(xy_from, xy_to,42); // Двигаем фигуру по лучшему найденному ходу.

              // смена цвета.
             //color = Color_invert(color);

             //chess_board.print();// Печатаем состояние доски после



             int xfrom1, yfrom1, xto1, yto1;

             xfrom1 = xy_from.x;
             yfrom1 = xy_from.y;
             xto1 = xy_to.x;
             yto1 = xy_to.y;

             if ((rates.n == 0) || ((rates.n == 1) &&
                 (rates.moves[0].xy_from.x == None) &&
                 (rates.moves[0].xy_from.y == None) &&
                 (rates.moves[0].xy_to.x == None) &&
                 (rates.moves[0].xy_to.y == None))) {
             }
             else {
                 if (clr) {
                     if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::B_KING) && (xy_from.y == 0) && (xy_to.y == 0) && (xy_from.x == 4) && (xy_to.x == 6)) {
                         // Чёрные сделали рокировку в короткую сторону.
                         chess_board.BLACK_O_O = true;
                         chess_board.BLACK_O_O_O = true;
                     }
                     else if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::B_KING) && (xy_from.y == 0) && (xy_to.y == 0) && (xy_from.x == 4) && (xy_to.x == 2)) {
                         // Чёрные сделали рокировку в длинную сторону.
                         chess_board.BLACK_O_O = true;
                         chess_board.BLACK_O_O_O = true;
                     }
                 }
                 else {
                     if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::W_KING) && (xy_from.y == 7) && (xy_to.y == 7) && (xy_from.x == 4) && (xy_to.x == 6)) {
                         // Чёрные сделали рокировку в короткую сторону.
                         chess_board.WHITE_O_O = true;
                         chess_board.WHITE_O_O_O = true;
                     }
                     else if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::W_KING) && (xy_from.y == 7) && (xy_to.y == 7) && (xy_from.x == 4) && (xy_to.x == 2)) {
                         // Чёрные сделали рокировку в длинную сторону.
                         chess_board.WHITE_O_O = true;
                         chess_board.WHITE_O_O_O = true;
                     }
                 }
             }

             //std::cout << xfrom1 << " " << yfrom1 << " " << xto1 << " " << yto1 << std::endl;

             /* xfrom1 = 4;
              yfrom1 = 1;
              xto1 = 4;
              yto1 = 2;*/

             move1[0] = xfrom1;
             move1[1] = yfrom1;
             move1[2] = xto1;
             move1[3] = yto1;


             // Информация о рокировке.
             O_O[0] = chess_board.WHITE_O_O;
             O_O[1] = chess_board.WHITE_O_O_O;
             O_O[2] = chess_board.BLACK_O_O;
             O_O[3] = chess_board.BLACK_O_O_O;

             if ((rates.n == 0) || ((rates.n == 1) &&
                 (rates.moves[0].xy_from.x == None) &&
                 (rates.moves[0].xy_from.y == None) &&
                 (rates.moves[0].xy_to.x == None) &&
                 (rates.moves[0].xy_to.y == None))) {

                 // Конец партии
                 int stop = 1;

                 if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                     //printf("  Мат\n");
                     stop = 1; // Мат
                 }
                 else {
                     if (!eatKing(chess_board)) {

                         //printf(" Пат\n");
                         stop = 2; // Пат
                     }
                     else {
                         // Мат Короля съели
                         stop = 1; // Мат
                     }
                 }


                 move1[9] = stop;
             }
             else {
                 // Партия не окончена.
                 move1[9] = 0;
             }
         }

         if (clr) {
             if (move1[9] == 0) {
                 // Ответная реакция белых, может быть им мат

                 chess_board.move(xy_from, xy_to); // Двигаем фигуру по лучшему найденному ходу.

                 // Запоминаем предыдущий ход.
                 chess_board.previos_moves.xy_from = xy_from;
                 chess_board.previos_moves.xy_to = xy_to;
                 chess_board.previos_moves.figure = chess_board.board[xy_to.y][xy_to.x];

                 // смена цвета.
                 color = Color_invert(color);

                 // Генерация списка возможных ходов с оценкой позиции:
                 LIST_MOVES rates = minimax(color, THINKING_DEPTH, chess_board);

                 //std::cout << rates.n<< std::endl;

                 if ((rates.n == 0) || ((rates.n == 1) &&
                     (rates.moves[0].xy_from.x == None) &&
                     (rates.moves[0].xy_from.y == None) &&
                     (rates.moves[0].xy_to.x == None) &&
                     (rates.moves[0].xy_to.y == None))) {

                     // Конец партии
                     int stop = 3;

                     if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                         //printf("  Мат\n");
                         stop = 3; // Мат
                     }
                     else {
                         if (!eatKing(chess_board)) {

                             //printf(" Пат\n");
                             stop = 4; // Пат
                         }
                         else {
                             // Короля съели.
                             stop = 3; // Мат
                         }
                     }

                     move1[9] = stop;
                 }
             }


             if (move1[11] > 0) {
                 // Движок даёт подсказку хода за белых по требованию

                 int ilevel_Game_memo = ilevel_Game;
                 int THINKING_DEPTH_memo = THINKING_DEPTH;

                 ilevel_Game = 2; // Включён форсированный вариант.
                 //THINKING_DEPTH = 4; // Каисса 4 полухода и форсированный вариант.

                 breduction_white = false;
                 THINKING_DEPTH = 3 + move1[11];

                 max_rate = -9999.0f;
                 xy_from = init(None);
                 xy_to = init(None);

                 // Компьютер играет только за чёрных. 
                 LIST_MOVES rates1 = minimax(Color::WHITE, THINKING_DEPTH, chess_board);
                 bool b1 = false, b2 = false, b3 = false, b4 = false, b5 = false;

                 //printf("rates.n=%d\n", rates.n);

                 int id_moves = 0;

                 if ((rates1.n == 0) || ((rates1.n == 1) &&
                     (rates1.moves[id_moves].xy_from.x == None) &&
                     (rates1.moves[id_moves].xy_from.y == None) &&
                     (rates1.moves[id_moves].xy_to.x == None) &&
                     (rates1.moves[id_moves].xy_to.y == None))) {

                     if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {

                         // Мат
                         move1[12] = -2;
                         move1[13] = -2;
                         move1[14] = -2;
                         move1[15] = -2;

                     }
                     else {
                         // Пат
                         move1[12] = -2;
                         move1[13] = -2;
                         move1[14] = -2;
                         move1[15] = -2;
                     }
                     //Игра окончена...

                 }
                 else {

                     // Сортировка по убыванию рейтинга
                     for (int id_rate = 1; id_rate < rates1.n; ++id_rate)
                     {
                         for (int id_rate0 = 0; id_rate0 < rates1.n - id_rate; ++id_rate0)
                         {
                             if (rates1.moves[id_rate0].rate < rates1.moves[id_rate0 + 1].rate) {

                                 // swap
                                 LIST_NODE_MOVES tmp;
                                 tmp = rates1.moves[id_rate0];
                                 rates1.moves[id_rate0] = rates1.moves[id_rate0 + 1];
                                 rates1.moves[id_rate0 + 1] = tmp;


                             }
                         }
                     }

                     b1 = true;

                     // Лучший ход найденный четырьмя полуходами с форсированным вариантом.
                     xy_from = rates1.moves[0].xy_from;
                     xy_to = rates1.moves[0].xy_to;

                     int xfrom1, yfrom1, xto1, yto1;

                     xfrom1 = xy_from.x;
                     yfrom1 = xy_from.y;
                     xto1 = xy_to.x;
                     yto1 = xy_to.y;

                     move1[12] = xfrom1;
                     move1[13] = yfrom1;
                     move1[14] = xto1;
                     move1[15] = yto1;

                 }
             }
         }
     }
     else {


         if (clr) {
             if (move1[11] > 0) {
                 // Движок даёт подсказку хода за белых по требованию

                 int ilevel_Game_memo = ilevel_Game;
                 int THINKING_DEPTH_memo = THINKING_DEPTH;

                 ilevel_Game = 2; // Включён форсированный вариант.
                 //THINKING_DEPTH = 4; // Каисса 4 полухода и форсированный вариант.

                 breduction_white = false;
                 THINKING_DEPTH = 3 + move1[11];

                 // Кандидат на будущий лучший ход.
                 float  max_rate = -9999;
                 MOVES_NODE xy_from;
                 MOVES_NODE xy_to;

                 max_rate = -9999.0f;
                 xy_from = init(None);
                 xy_to = init(None);

                 // Компьютер играет только за чёрных. 
                 LIST_MOVES rates1 = minimax(Color::WHITE, THINKING_DEPTH, chess_board);
                 bool b1 = false, b2 = false, b3 = false, b4 = false, b5 = false;

                 //printf("rates.n=%d\n", rates.n);

                 Color color = Color::WHITE;

                 int id_moves = 0;

                 if ((rates1.n == 0) || ((rates1.n == 1) &&
                     (rates1.moves[id_moves].xy_from.x == None) &&
                     (rates1.moves[id_moves].xy_from.y == None) &&
                     (rates1.moves[id_moves].xy_to.x == None) &&
                     (rates1.moves[id_moves].xy_to.y == None))) {

                     if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {

                         // Мат
                         move1[12] = -2;
                         move1[13] = -2;
                         move1[14] = -2;
                         move1[15] = -2;

                     }
                     else {
                         // Пат
                         move1[12] = -2;
                         move1[13] = -2;
                         move1[14] = -2;
                         move1[15] = -2;
                     }
                     //Игра окончена...

                 }
                 else {

                     // Сортировка по убыванию рейтинга
                     for (int id_rate = 1; id_rate < rates1.n; ++id_rate)
                     {
                         for (int id_rate0 = 0; id_rate0 < rates1.n - id_rate; ++id_rate0)
                         {
                             if (rates1.moves[id_rate0].rate < rates1.moves[id_rate0 + 1].rate) {

                                 // swap
                                 LIST_NODE_MOVES tmp;
                                 tmp = rates1.moves[id_rate0];
                                 rates1.moves[id_rate0] = rates1.moves[id_rate0 + 1];
                                 rates1.moves[id_rate0 + 1] = tmp;


                             }
                         }
                     }

                     b1 = true;

                     // Лучший ход найденный четырьмя полуходами с форсированным вариантом.
                     xy_from = rates1.moves[0].xy_from;
                     xy_to = rates1.moves[0].xy_to;

                     int xfrom1, yfrom1, xto1, yto1;

                     xfrom1 = xy_from.x;
                     yfrom1 = xy_from.y;
                     xto1 = xy_to.x;
                     yto1 = xy_to.y;

                     move1[12] = xfrom1;
                     move1[13] = yfrom1;
                     move1[14] = xto1;
                     move1[15] = yto1;

                 }
             }
         }
     }
     
     /*
    
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "The time: " << seconds << " seconds\n";

    //              minimax                               alpha-beta
    //       debug            realese
    // DEPTH time    ratio   time
    //   3    0.056                                      0.009  (3,3)
    //   4    2.195  (39.2)   0.334                      0.06   (4,4)
    //   5   52.383  (23.86)  12.7                       1.189  (4,5)  0.355
    //   6                   276.244  (21.75) (436kb)    5.853  (4,6)  0.739
    //   7                                              55.259  (4,7)  2.171  (471 kb)  
    //   8                                                      (4,8)  3.15
    //   9                                                      (4,9)  3.921
    //  30  форсированный вариант, взятия просматриваем на 30  (4,30)  6.693  (541 кб) 

   //system("PAUSE");*/

   // return 0;
    
}

#endif

#ifndef CODE_FOR_DLL

const int Russian = 2;

// cb - шахматная доска.
// clr цвет (1 - ход Чёрных пока это всегда так.
// Остальные параметры ответный ход Чёрных.
//extern "C" EXPORT void analiticChess(int* cb, bool clr,
  //  int* move1)
int main()
{

#ifdef _OPENMP

    omp_set_num_threads(number_cores); // Не более 40 потоков.

#endif

    // Поддержка кириллицы в консоли Windows
    setlocale(LC_CTYPE, "rus"); // вызов функции настройки локали
    //setlocale(LC_ALL, "");
    system("mode con cols=166 lines=12000");

    int ilanguage = Russian;

    

    if (ilanguage == Russian) {
        std::cout << "Пожалуйста, выберите язык общения с шахматной программой: 1) Английский 2) Русский.\n";
    }
    else {
        std::cout << "Please select your language: 1) English 2) Russian.\n";
    }
    
    
    std::cin >> ilanguage;
    if (ilanguage == Russian) {
        std::cout << " Вы выбрали русский язык.\n";
    }
    else {
        ilanguage = 1;
        std::cout << " Your select english language...\n";
    }

    if (ilanguage == Russian) {
        std::cout << "Добро пожаловать в шахматную компьютерную ретро программу в стилистике 1967 года.\n";
        std::cout << "В программе реализован метод граней и оценок (alpha-beta algorithm Ньюэл, Саймон и \n";
        std::cout << "Шоу 1962г.), сортировка ходов, эвристика пустого хода, служба лучшего хода,\n";
        std::cout << "форсированный вариант. В основе статической оценки программы лежат бонусы за легальную мобильность.\n";
        std::cout << "Пожалуйста, ответьте на ряд вопросов прежде чем начать игру:\n";
    }
    else {
        std::cout << "Welcome to setting computer chess program.\n";
    }

    int istyle_aggressive = 1;

    if (ilanguage == Russian) {
        std::cout << "Выберите предпочитаемый стиль игры:\n";
        std::cout << " 1) агресивный стиль игры, выводим ферзя в бой! 2) тихий позиционный стиль игры.\n";
    }
    else {
        std::cout << "Please, select your game style:\n";
        std::cout << " 1) aggressive Queen is attack! 2) more stable style.\n";
    }

        std::cin >> istyle_aggressive;

        if (istyle_aggressive == 2) {
            // Тихая позиционная игра.
            flag_POSICIONING_GAME = true;
        }        

        if (ilanguage == Russian) {
           std::cout << "Включить продления связанные с безопасностью короля ? 0) нет 1) да\n";
        }
        else {
           std::cout << "Include prolongations linking from king security ? 0) no 1) yes\n";
        }

        int iKingSecurity0;

        std::cin >> iKingSecurity0;

        if (iKingSecurity0 == 1) {
            iKingSecurity = 1;
        }

        int iQueekGame = 1;

        if (ilanguage == Russian) {
            std::cout << "Выберите предпочитаемую скорость игры:\n";
            std::cout << " 1) быстрая игра! (продлеваем только шахи) 2) интересная более медленная игра.\n";
            std::cout << "помимо шахов продления в случаях: 2.1) единственного ответа; 2.2) превращения пешки в ферзи (promotion)\n";
            std::cout << "2.3) выгодного взятия при (iply < 7)\n";
            if (iKingSecurity == 1) {
                std::cout << "*) опасного сокращения расстояния между ферзём и королём противника (не более 4 раз)\n";
            }
            else {
                std::cout << "Продления безопасности короля отключены.\n";
            }
        }
        else {
            std::cout << "Please, select your speed game:\n";
            std::cout << " 1) Queek Game! 2) more interest Game (more time needed).\n";
        }

        std::cin >> iQueekGame;

        if (iQueekGame == 2) {
            // интересная более медленная игра.
            // Продлевааются еще многие вещи кроме шахов, т.е. продлеваются не только шахи.
            bQueekGame = false;
        }
       

    // Сейчас ход Чёрных. Алгоритм минимакса должен ответить за чёрных.

    // Поддержка кириллицы в консоли Windows
    //setlocale(LC_CTYPE, "rus"); // вызов функции настройки локали
    // Меняет разделитель целой и дробной части.
    //setlocale(LC_ALL, "");
    //system("mode con cols=60 lines=12000");
    //system("Color 70");

    // Задаём доску.

    // Второй (наивысший 13.09.2022) уровень игры. В честь советской программы Каисса и первого чемпионата мира среди машин. Четыре полухода с формированным вариантом.
         // Только четыре полухода с форсированым вариантом. На фотографии Михаил Донской.
    THINKING_DEPTH = 2;
    ilevel_Game = 0;

    bool bOklev = false;
    int ilevel_Gameuser = 1;

    while (!bOklev) {

        if (ilanguage == Russian) {
            std::cout << "Пожалуйста, выберите глубину анализа компьютером в полуходах и наличие форсированного варианта:\n";
            std::cout << " 1) ( Алан Тьюринг) глубина два полухода (один полуход за белых и один полуход за чёрных).\n";
            std::cout << " 2) ( Клод  Шеннон) глубина четыре полухода (два полухода за белых и два полухода за чёрных).\n";
            std::cout << "\033[33m 3) (Джон фон Нейман) глубина два полухода плюс форсированный вариант.\033[0m\n";
            std::cout << "\033[33m 4) (Ньюэл, Саймон и Шоу) глубина шесть полуходов (три полухода за белых и три полухода за чёрных).\033[0m\n";
            std::cout << "\033[32m 5) глубина четыре полухода плюс форсированный вариант. Случайный ход из первых трёх линий.\033[0m\n";
            std::cout << "\033[31m 6) (Михаил Донской) глубина четыре полухода плюс форсированный вариант.\033[0m\n";
            std::cout << "\033[31m 7) (Кен Томпсон) глубина шесть полуходов плюс форсированный вариант.\033[0m\n";
            std::cout << "\033[32m 8) глубина шесть полуходов плюс форсированный вариант. Случайный ход из первых трёх линий.\033[0m\n";
            std::cout << "\033[31m 9) (Норберт Виннер) глубина восемь полуходов плюс форсированный вариант.\033[0m\n";
            std::cout << "\033[34m 10) (Адельсон-Вельский) глубина три полухода плюс форсированный вариант.\033[0m\n";
            std::cout << "\033[34m 11) (Каисса) глубина пять полуходов плюс форсированный вариант.\033[0m\n";
            std::cout << "\033[34m 12) (Арлазоров) глубина семь полуходов плюс форсированный вариант.\033[0m\n";
            std::cout << "\033[31m 13) (DeepThought Джудит Полгар) глубина десять полуходов плюс форсированный вариант.\033[0m\n";
        }
        else {
            std::cout << "Plese, select computer search depth and may be increasing depth reshime:\n";
            std::cout << " 1) ( Alan  Turing) depth == 2 (one move for white and one move for black).\n";
            std::cout << " 2) (Kloud Shennon) depth == 4 (two move for white and two move for black).\n";
            std::cout << "\033[33m 3) (Dghon fon Neiman) depth == 2 and increasing depth for eating figure or Shah king.\033[0m\n";
            std::cout << "\033[33m 4) (Newel, Saimon and Show) depth == 6 (three move for white and three move for black).\033[0m\n";
            std::cout << "\033[32m 5) depth == 4 and increasing depth for eating figure or Shah king. random.\033[0m\n";
            std::cout << "\033[31m 6) (Michael Donskoy) depth == 4 and increasing depth for eating figure or Shah king.\033[0m\n";
            std::cout << "\033[31m 7) (Ken Tompson) depth == 6 and increasing depth for eating figure or Shah king.\033[0m\n";
            std::cout << "\033[32m 8) depth == 6 and increasing depth for eating figure or Shah king. random.\033[0m\n";
            std::cout << "\033[31m 9) (Norbert Vinner) depth == 8 and increasing depth for eating figure or Shah king.\033[0m\n";
            std::cout << "\033[34m 10) (Adelson Velskey) depth == 3 and increasing depth for eating figure or Shah king.\033[0m\n";
            std::cout << "\033[34m 11) (Kaissa) depth == 5 and increasing depth for eating figure or Shah king.\033[0m\n";
            std::cout << "\033[34m 12) (Arlazorov) depth == 7 and increasing depth for eating figure or Shah king.\033[0m\n";
            std::cout << "\033[31m 13) (DeepThought Judit Polgar) depth == 10 and increasing depth for eating figure or Shah king.\033[0m\n";
        }
        
        std::cin >> ilevel_Gameuser;

        if ((ilevel_Gameuser >= 1) && (ilevel_Gameuser <= 13))
        {
            bOklev = true;
        }
    }

    if (ilanguage == Russian) {
        std::cout << "Вы выбрали уровень игры = " << ilevel_Gameuser << std::endl;
    }
    else {
        std::cout << " Your select lelevel Game is:" << ilevel_Gameuser << std::endl;
    }

    if ((ilevel_Gameuser == 1) || (ilevel_Gameuser == 3)) THINKING_DEPTH = 2;
    if ((ilevel_Gameuser == 2) || (ilevel_Gameuser == 5) || (ilevel_Gameuser == 6)) THINKING_DEPTH = 4;
    if ((ilevel_Gameuser == 4) || (ilevel_Gameuser == 7) || (ilevel_Gameuser == 8)) THINKING_DEPTH = 6;
    if (ilevel_Gameuser == 13) THINKING_DEPTH = 10;
    if  (ilevel_Gameuser == 9) THINKING_DEPTH = 8;
    if (ilevel_Gameuser == 10) THINKING_DEPTH = 3;
    if (ilevel_Gameuser == 11) THINKING_DEPTH = 5;
    if (ilevel_Gameuser == 12) THINKING_DEPTH = 7;
    if ((ilevel_Gameuser == 3) || (ilevel_Gameuser == 5) || (ilevel_Gameuser == 6) ||
        (ilevel_Gameuser == 7) || (ilevel_Gameuser == 8)|| (ilevel_Gameuser == 9)||
        (ilevel_Gameuser == 10) || (ilevel_Gameuser == 11) || (ilevel_Gameuser == 12)
        || (ilevel_Gameuser == 13))
    {
        ilevel_Game = 2; // Включён форсированный вариант.
    }
    else {
        ilevel_Game = 0; // Без форсированного варианта.
    }

    int bKouch_rechime = 0;

    if (ilanguage == Russian) {

        std::cout << " Желаете получать подсказки о том как сделать ход в партии ?\n";
        std::cout << "0) без подсказок 1) получать помощь шахматного асистента\n";
        std::cout << "2) получать помощь шахматного асистента.\n";
        int tmpAmat = 0;
        std::cin >> tmpAmat;
        if (tmpAmat == 1) {
            std::cout << " По ходу партии Вам будет помогать лёгкая версия электронного помошника.\n";
            bKouch_rechime = 1;
        }
        else if (tmpAmat == 2) {
            std::cout << " По ходу партии Вам будет помогать электронный помошник.\n";
            bKouch_rechime = 2;
        }
        else {
            std::cout << " Вы выбрали режим без подсказок.\n";
        }
    }
    else {

        std::cout << "Plese, select computer Kouch(Teacher) rechime:\n";
        std::cout << "0) None 1) Teacher rechime lite.  2) Teacher rechime full.\n";
        int tmpAmat = 0;
        std::cin >> tmpAmat;
        if (tmpAmat == 1) {
            std::cout << " Your select Teacher rechime lite.\n";
            bKouch_rechime = 1;
        }
        else  if (tmpAmat == 2) {
            std::cout << " Your select Teacher rechime full.\n";
            bKouch_rechime = 2;
        }
        else {
            std::cout << " Your not select Teacher rechime.\n";
        }
    }

    // 3) depth=2 с продлением форсированным вариантом побеждена Андрюхой Василевским 16.12.2022.

    Board chess_board;
    //system("PAUSE");
    chess_board.init();

    bool BotGame = false;


    // int cb[64] = { 9, 11, 10, 8, 7, 10, 11, 9, 12 , 12 , 12 , 12 , 12 , 12 ,12 ,12 , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6, 6, 3, 5, 4, 2, 1, 4, 5, 3};

    // int cb[64] = { 0, 7, 0, 0, 0, 0, 0, 11, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    /* int cb[64] =
       { 7, 0, 0, 0, 0, 0, 0, 3,
         0, 0, 0, 0, 0, 0, 0, 3,
         0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 11,
         0, 0, 0, 0, 0, 0, 1, 0,
         10, 0, 0, 0, 0, 0, 0, 0 };
         */
         //W_KING = 1, W_QUEEN = 2, W_ROOK = 3, W_BISHOP = 4, W_KNIGHT = 5, W_PAWN=  6, B_KING = 7, B_QUEEN = 8, B_ROOK = 9, B_BISHOP = 10, B_KNIGHT = 11, B_PAWN = 12
         /*int cb[64] =
         { 9, 0, 0, 0, 7, 10, 0, 9,
           12, 12, 12, 4, 12, 12, 12, 12,
           0, 0, 0, 0, 0, 0, 0, 0,
           0, 0, 0, 12, 6, 0, 0, 0,
           0, 0, 0, 11, 11, 0, 6, 0,
           0, 0, 0, 0, 0, 2, 0, 6,
           6, 6, 6, 0, 0, 6, 0, 0,
           3, 5, 4, 0, 1, 0, 0, 3 };*/
           //W_KING = 1, W_QUEEN = 2, W_ROOK = 3, W_BISHOP = 4, W_KNIGHT = 5, W_PAWN=  6, B_KING = 7, B_QUEEN = 8, B_ROOK = 9, B_BISHOP = 10, B_KNIGHT = 11, B_PAWN = 12
    /*int cb[64] =
    { 0, 7, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 6, 0, 0,
      12, 0, 0, 0, 0, 1, 0, 12,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 12, 0, 0, 0, 0, 0, 0,
      0, 4, 0, 0, 0, 0, 0, 0,
      6, 0, 6, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 9, 0, 0, 0 };*/
    /*int cb[64] =
    { 0, 7, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 1, 0, 0,
      12, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 6, 0, 0, 0, 0, 0,
      8, 0, 0, 0, 0, 0, 0, 8 };*/
      //W_KING = 1, W_QUEEN = 2, W_ROOK = 3, W_BISHOP = 4, W_KNIGHT = 5, W_PAWN=  6, B_KING = 7, B_QUEEN = 8, B_ROOK = 9, B_BISHOP = 10, B_KNIGHT = 11, B_PAWN = 12
    /*int cb[64] =
    { 0, 0, 0, 3, 0, 0, 0, 0,
      0, 0, 0, 0, 1, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 7, 12, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0 };*/
    /*int cb[64] =
    { 0, 0, 0, 0, 0, 0, 7, 9,
      0, 0, 9, 0, 10, 0, 12, 0,
      12, 0, 0, 0, 6, 12, 1, 12,
      0, 12, 0, 0, 0, 0, 0, 6,
      0, 6, 0, 0, 0, 0, 0, 0,
      6, 4, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 3, 0, 0, 0, 0 };*/
    //W_KING = 1, W_QUEEN = 2, W_ROOK = 3, W_BISHOP = 4, W_KNIGHT = 5, W_PAWN=  6,
    //B_KING = 7, B_QUEEN = 8, B_ROOK = 9, B_BISHOP = 10, B_KNIGHT = 11, B_PAWN = 12
    /*int cb[64] =
    { 9, 0, 10, 8, 7, 10, 11, 9,
      12, 12, 12, 12, 0, 12, 12, 12,
      0, 0, 11, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 12, 0, 0, 0,
      0, 0, 4, 0, 6, 0, 0, 0,
      0, 0, 0, 0, 0, 5, 0, 0,
      6, 6, 6, 6, 0, 6, 6, 6,
      3, 5, 4, 2, 1, 0, 0, 3 };*/
    /*int cb[64] =
    { 9, 11, 10, 8, 7, 10, 11, 9,
      12, 12, 12, 0, 12, 12, 12, 12,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 12, 6, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 4, 0, 0, 0, 0,
      6, 6, 6, 6, 0, 6, 6, 6,
      3, 5, 4, 2, 1, 0, 5, 3 };*/
    /*int cb[64] =
    { 4, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 12, 0, 0, 0, 12, 0, 0,
      0, 0, 0, 0, 7, 6, 12, 0,
      6, 6, 0, 0, 0, 0, 1, 0,
      0, 0, 0, 11, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0 };*/
    /*int cb[64] =
    { 0, 0, 0, 0, 0, 0, 7, 0,
      12, 12, 12, 0, 0, 0, 12, 12,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 9, 0, 0, 0,
      6, 0, 0, 0, 0, 0, 1, 6,
      9, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 8, 0, 0, 0, 0, 10 };*/
    // Проблемная позиция от 4.07.2023 не выявлена.
     /* int cb[64] =
     { 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 5, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 2, 0,
       0, 0, 0, 0, 0, 6, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 7,
       0, 0, 0, 0, 0, 1, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0 };*/
 /*int cb[64] =
{ 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0 };*/
// Duchess-Kaissa 2-й компьютерный
// шахматный чемптонат Торонто, 1977
// Ход белых Фa8. Каисса сыграла Лe8.
/*int cb[64] =
{ 0, 0, 0, 0, 0, 0, 7, 0,
  0, 0, 0, 0, 9, 12, 0, 12,
  2, 12, 0, 8, 0, 10, 12, 0,
  0, 4, 0, 11, 0, 0, 0, 0,
  0, 0, 0, 6, 12, 0, 6, 0,
  0, 0, 0, 0, 4, 0, 0, 6,
  6, 6, 0, 0, 0, 6, 0, 0,
  0, 0, 3, 0, 0, 0, 1, 0 };*/
/*int cb[64] =
{ 0, 0, 0, 0, 3, 0, 0, 0,
  0, 0, 0, 0, 0, 12, 7, 12,
  0, 12, 0, 0, 0, 10, 12, 0,
  0, 4, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 6, 12, 0, 6, 0,
  0, 0, 0, 0, 0, 0, 0, 6,
  6, 6, 11, 0, 0, 6, 0, 0,
  0, 0, 0, 0, 0, 0, 1, 0 };*/
  // Ход черных. Мат в два хода.
 /* int cb[64] =
  { 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 12, 10, 7,
  0, 12, 0, 0, 0, 0, 12, 12,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 6, 12, 6, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 11,
  6, 6, 0, 0, 0, 0, 4, 0,
  3, 0, 0, 0, 0, 0, 5, 1 };*/
// Эффект горизонта из книги Евгения Корнилова.
// Рис. 2.13 Эффект горизонта стр. 102.
// за 6 полуходов.
int cb[64] =
{ 9, 0, 0, 0, 7, 0, 11, 9,
  12, 12, 0, 0, 0, 0, 0, 12,
  0, 0, 0, 0, 12, 0, 12, 0,
  8, 0, 12, 0, 0, 0, 0, 0,
  0, 0, 6, 0, 6, 0, 4, 0,
  0, 0, 0, 0, 0, 0, 0, 6,
  6, 6, 11, 5, 0, 2, 6, 0,
  3, 0, 0, 0, 0, 3, 1, 0 };
// Каисса против любителя
/*int cb[64] =
{ 0, 0, 9, 0, 0, 0, 0, 7,
  0, 12, 0, 0, 0, 0, 0, 12,
  12, 0, 11, 0, 12, 12, 0, 0,
  0, 0, 0, 12, 0, 10, 0, 0,
  6, 0, 0, 6, 0, 8, 0, 0,
  0, 6, 5, 0, 3, 5, 0, 0,
  0, 0, 6, 2, 1, 6, 9, 6,
  0, 0, 0, 0, 0, 0, 0, 3 };*/
  // мат Легаля Конь белых ест 
// пешку на e5.
 /* int cb[64] =
  { 9, 0, 0, 8, 7, 10, 11, 9,
    12, 12, 12, 0, 0, 12, 12, 12,
    0, 0, 11, 12, 0, 0, 0, 0,
    0, 0, 0, 0, 12, 0, 0, 10,
    0, 0, 4, 0, 6, 0, 0, 0,
    0, 0, 5, 0, 0, 5, 0, 6,
    6, 6, 6, 6, 0, 6, 6, 0,
    3, 0, 4, 2, 1, 0, 0, 3 };*/

    /*int cb[64] =
    { 9, 11, 10, 8, 7, 10, 11, 9,
      12, 12, 12, 12, 12, 12, 12, 12,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0,
      6, 6, 6, 6, 6, 6, 6, 6,
      3, 5, 4, 2, 1, 4, 5, 3 };*/


    //bool buserGame = true; // true - Человек играет с машиной; false - Компьютер играет сам с собой.


    int k = 0;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            switch (cb[k]) {
            case 1:
                chess_board.board[i][j] = King(Color::WHITE).get();
                break;
            case 7:
                chess_board.board[i][j] = King(Color::BLACK).get();
                break;
            case 6:
                chess_board.board[i][j] = Pawn(Color::WHITE).get();
                break;
            case 12:
                chess_board.board[i][j] = Pawn(Color::BLACK).get();
                break;
            case 5:
                chess_board.board[i][j] = Knight(Color::WHITE).get();
                break;
            case 11:
                chess_board.board[i][j] = Knight(Color::BLACK).get();
                break;
            case 3:
                chess_board.board[i][j] = Rook(Color::WHITE).get();
                break;
            case 9:
                chess_board.board[i][j] = Rook(Color::BLACK).get();
                break;
            case 4:
                chess_board.board[i][j] = Bishop(Color::WHITE).get();
                break;
            case 10:
                chess_board.board[i][j] = Bishop(Color::BLACK).get();
                break;
            case 2:
                chess_board.board[i][j] = Queen(Color::WHITE).get();
                break;
            case 8:
                chess_board.board[i][j] = Queen(Color::BLACK).get();
                break;
            }
        
            ++k;
        }
    }

    chess_board.init2();

    if (ilanguage == Russian) {
        std::cout << "Вас приветствует шахматная компьютерная ретро программа...\n";
    }
    else
    {
        std::cout << "Welcome to computer chess program...\n";
    }
    chess_board.print();// Печатаем состояние доски после

    if (1) {

        while (1) {

            // Ответ белых Играет человек.
            if (ilanguage == Russian) {
                std::cout << " пожалуйста, введите ваш ход за белых:";
            }
            else {
                std::cout << "please enter your (white) move: \n";
            }

            //bfound = true;
            char s[6];
            std::cin >> s;
            if ((s[0] >= 'a') && (s[0] <= 'h')) {
                chess_board.previos_moves.xy_from.x = s[0] - 'a';
            }
            else {
                std::cout << "error input string...\n";
                system("pause");
                exit(1);
            }
            if ((s[1] >= '1') && (s[1] <= '8')) {
                chess_board.previos_moves.xy_from.y = 8 - (s[1] - '0');
            }
            else {
                std::cout << "error input string...\n";
                system("pause");
                exit(1);
            }
            if ((s[2] >= 'a') && (s[2] <= 'h')) {
                chess_board.previos_moves.xy_to.x = s[2] - 'a';
            }
            else {
                std::cout << "error input string...\n";
                system("pause");
                exit(1);
            }
            if ((s[3] >= '1') && (s[3] <= '8')) {
                chess_board.previos_moves.xy_to.y = 8 - (s[3] - '0');
            }
            else {
                std::cout << "error input string...\n";
                system("pause");
                exit(1);
            }


            {
                int n_01 = 0;
                MOVES moves_current_list[POOL_SIZEL];

                LIST_NODE_MOVES make_moves[POOL_SIZE];

                int i_01 = 0, i_02 = 0;

                Color color = Color::WHITE;

                Color my_color = color;
                // Вражеский цвет должен быть другого знака.
                Color enemy_color = Color_invert(color); // Цвет врага.

                __int8 my_depth = THINKING_DEPTH;

                const char depth = 0;
                const char iply = 0;
                const char ishah = 0;
                const bool enemy = false;

                MOVES figList;

                GenerateAllMoves(figList, make_moves, moves_current_list, color, chess_board, n_01, i_01, i_02, my_color, enemy_color, my_depth, enemy, iply, true, false, 0.0f);

                bool b_we_should_be_continue = true;

                for (int k_1 = 0; k_1 < i_01; ++k_1)
                {

                    if (b_we_should_be_continue) {


                        int tid = 0;
#ifdef _OPENMP
                        tid = omp_get_thread_num();
#endif

                        __int8 y = make_moves[k_1].xy_from.y;
                        __int8 x = make_moves[k_1].xy_from.x;

                        MOVES_NODE xy_from;
                        xy_from = init(x, y);

                        if ((chess_board.previos_moves.xy_from.x == x) && (chess_board.previos_moves.xy_from.y == y)&&
                            (chess_board.previos_moves.xy_to.x== make_moves[k_1].xy_to.x)&&
                            (chess_board.previos_moves.xy_to.y == make_moves[k_1].xy_to.y)) {
                            b_we_should_be_continue = false;
                        }

                    }
                }
            
                if (!b_we_should_be_continue) {
                    break;// такой ход допустим.
                }
                else {
                    if (ilanguage == Russian) {
                        std::cout << "\nВы ввели ход не по правилам шахмат...\n";
                        std::cout << "Введите ход повторно.\n";
                    }
                    else {
                        std::cout << "\nyour move is incorrect...\n";
                    }
                }
            }


        }

       // std::cout << chess_board.previos_moves.xy_from.x << " " << chess_board.previos_moves.xy_from.y << std::endl;

        if (((chess_board.previos_moves.xy_from.x == 1) || (chess_board.previos_moves.xy_from.x == 6)) && (chess_board.previos_moves.xy_from.y == 7)) {
            chess_board.previos_moves.figure = ChessPiece::W_KNIGHT;
        }
        else {
            chess_board.previos_moves.figure = ChessPiece::W_PAWN;
        }

        chess_board.move(chess_board.previos_moves.xy_from, chess_board.previos_moves.xy_to); // Двигаем фигуру по каждому сгенерированному ходу.
    }
    else {

        // e2e4
        chess_board.previos_moves.figure = ChessPiece::W_PAWN;
        chess_board.previos_moves.xy_from.x = 4;
        chess_board.previos_moves.xy_from.y = 6;
        chess_board.previos_moves.xy_to.x = 4;
        chess_board.previos_moves.xy_to.y = 4;
    }
    

    Color color = Color::BLACK;
    const int number_of_half_passes = 320; // Число полуходов в партии (длительность партии).

    const __int8 iprint = -1;

    int  i = 0;

    for (int i = 0; i < number_of_half_passes; ++i) {

        clock_t start = clock();

        // Кандидат на будущий лучший ход.
        float  max_rate = -9999.0f;
        MOVES_NODE xy_from;
        MOVES_NODE xy_to;
        xy_from = init(None);
        xy_to = init(None);

        bool bfound = false;

        // Опцию дебютного справочника можно просто отключить если выставить ноль.
        if (1&& ((i + 1) % 2 != 0)) {
            // Ответ Чёрных
           

            // Сначала нужно проверить если вариант в базе и если есть то ответить по заготовленному варианту.
            // Хранить позиции партий текстовом файле и ответы на них одноходовые. Вначале хранить число позиций с фигурами.
            // База данных в текстовом файле, можно её постепенно наполнять. Если найдено несколько одинаковых позиций выбрать случайный ответ.
            // Назвать это всё PaulMorphy_database.txt На позицию хранить не более 7 альтернативных ответов.

            // Первые несколько ходов можно сделать по учебнику шахматной игры.
            // Перечислим ответы за чёрных:
            //1. Французская защита : 
            //1. e2-e4 > e7-e6
            //2. d2-d4 -> d7-d5
            //3.1. ed (разменный вариант).
            //3.2 e5 (система Нимцовича).
            //3.3 Nc3 -> {Nc6; de; Cb4; c5; Nf6; Nd2}

            // здесь много теории.
            // 1. e2-e4 -> e7-e5

            // Ускоренный дракон
            // 1. e2-e4 -> c7-c5
            // 2. Nf3 g6
            // 3. d4 cxd4
            // 4. Nxd4 Cg7


            /*FILE* fp_database0 = NULL;
            errno_t err_database0;
            err_database0 = fopen_s(&fp_database0, "PaulMorphy_position.txt", "w");
            for (int i429 = 0; i429 < 64; ++i429) {
                fprintf_s(fp_database0, "%d ", cb[i429]);
                if ((i429 + 1) % 8 == 0) fprintf_s(fp_database0, "\n");
            }
            fclose(fp_database0);
            */

            FILE* fp_database = NULL;
            errno_t err_database;
            //err_database = fopen_s(&fp_database, "\\chess_Data\\Plugins\\x86_64\\debut_book.txt", "r");
            //err_database = fopen_s(&fp_database, "\\debut_book.txt", "r");
            err_database = fopen_s(&fp_database, "debut_book.txt", "r");



            if ((err_database == 0) && (fp_database != NULL))
            {
                // Файл успешно открыт.

                // Пока ищет первый найденный ответ
                int inumber_positions = 0;
                int din = 0;
                fscanf_s(fp_database, "%d", &din);
                inumber_positions = din;


                int isize_identical = 0;
                int** move_memo = new int* [7];
                for (int i429 = 0; i429 < 7; ++i429) {
                    move_memo[i429] = new int[9];
                }



                k = 0;

                for (int i = 0; i < 8; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        switch (chess_board.board[i][j]) {
                        case ChessPiece::W_KING:                            
                            cb[k] = 1;
                            break;
                        case ChessPiece::B_KING:                            
                            cb[k] = 7;
                            break;
                        case ChessPiece::W_PAWN:                           
                            cb[k] = 6;
                            break;
                        case ChessPiece::B_PAWN:                           
                            cb[k] = 12;
                            break;
                        case ChessPiece::W_KNIGHT:                     
                            cb[k] = 5;
                            break;
                        case ChessPiece::B_KNIGHT:                           
                            cb[k] = 11;
                            break;
                        case ChessPiece::W_ROOK:                            
                            cb[k] = 3;
                            break;
                        case ChessPiece::B_ROOK:                            
                            cb[k] = 9;
                            break;
                        case ChessPiece::W_BISHOP:                            
                            cb[k] = 4;
                            break;
                        case ChessPiece::B_BISHOP:                           
                            cb[k] = 10;
                            break;
                        case  ChessPiece::W_QUEEN:                            
                            cb[k] = 2;
                            break;
                        case ChessPiece::B_QUEEN:                            
                            cb[k] = 8;
                            break;
                        default: cb[k] = 0;
                            break;
                        }

                        ++k;
                    }
                }

                int cb_copy[64] = { 0 };

                for (int i428 = 0; i428 < inumber_positions; ++i428) {
                    //int* cb_copy = new int[64];

                    for (int i429 = 0; i429 < 64; ++i429) {
                        fscanf_s(fp_database, "%d", &din);
                        cb_copy[i429] = din;
                    }
                    // Представление досок в файле базы данных полностью совпадает с программным представлением, 
                    // перенумерация Не нужна.
                    bool bidentical = true;
                    for (int i429 = 0; i429 < 64; ++i429) {
                        if (cb_copy[i429] != cb[i429]) {
                            bidentical = false;
                            break;
                        }
                    }
                    if ((isize_identical < 7) && (bidentical)) {
                        bfound = true;

                        fscanf_s(fp_database, "%d", &din);
                        move_memo[isize_identical][0] = din;
                        fscanf_s(fp_database, "%d", &din);
                        move_memo[isize_identical][1] = din;
                        fscanf_s(fp_database, "%d", &din);
                        move_memo[isize_identical][2] = din;
                        fscanf_s(fp_database, "%d", &din);
                        move_memo[isize_identical][3] = din;

                        // for (int i429 = 0; i429 < 5; ++i429) {
                          //   fscanf_s(fp_database, "%d", &din);
                         //}

                         /*FILE* fp_database1 = NULL;
                         errno_t err_database1;
                         err_database1 = fopen_s(&fp_database1, "PaulMorphy_move.txt", "w");
                         for (int i429 = 0; i429 < 4; ++i429) {
                             fprintf_s(fp_database1, "%d ", move1[i429]);
                         }
                         fclose(fp_database1);
                         */

                         // Информация о рокировке.
                        fscanf_s(fp_database, "%d", &din);
                        move_memo[isize_identical][4] = din;
                        fscanf_s(fp_database, "%d", &din);
                        move_memo[isize_identical][5] = din;
                        fscanf_s(fp_database, "%d", &din);
                        move_memo[isize_identical][6] = din;
                        fscanf_s(fp_database, "%d", &din);
                        move_memo[isize_identical][7] = din;

                        //move1[9] :  0 - партия не окончена, 1 - мат, 2 - пат.
                        fscanf_s(fp_database, "%d", &din); // плюс одно считывание за move[9].

                        //move1[9] = 0;

                        isize_identical++;

                    }
                    else {

                        for (int i429 = 0; i429 < 9; ++i429) {
                            fscanf_s(fp_database, "%d", &din);
                        }
                    }

                    //delete[] cb_copy;
                }

                if (bfound) {

                    std::mt19937 gen{ std::random_device()() };
                    int id_out = gen() % isize_identical;

                    xy_from.x = move_memo[id_out][0];
                    xy_from.y = move_memo[id_out][1];
                    xy_to.x = move_memo[id_out][2];
                    xy_to.y = move_memo[id_out][3];

                    //std::cout << static_cast<int>(xy_from.x) << " " << static_cast<int>(xy_from.y) << " " << static_cast<int>(xy_to.x) << " " << static_cast<int>(xy_to.y) << std::endl;

                    std::cout  << static_cast<char>('a' + xy_from.x);
                    std::cout << 8 - xy_from.y << static_cast<char>('a' + xy_to.x);
                    std::cout << 8 - xy_to.y << std::endl;


                    // Предыдущий ход на этой старой  доске.
                    //chess_board.previos_moves.xy_from = chess_board.previos_moves.xy_from;
                    //chess_board.previos_moves.xy_to = chess_board.previos_moves.xy_to;
                    //chess_board.previos_moves.figure = chess_board.previos_moves.figure;
                    {
                        if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::B_KING) &&
                            (xy_from.y == 0) && (xy_to.y == 0) && (xy_from.x == 4) && (xy_to.x == 2)) {
                            // Рокировка чёрных в длинную сторону



                            // Завершающий ход ладьи так как рокировка это двойной ход.
                             // from
                            chess_board.bList.delete_move(0, 0);



                            chess_board.bList.append(3, 0);
                            chess_board.board[0][3] = ChessPiece::B_ROOK;
                            chess_board.board[0][0] = ChessPiece::EMPTY;

                            chess_board.BLACK_O_O = true;
                            chess_board.BLACK_O_O_O = true;

                        }

                        if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::B_KING) &&
                            (xy_from.y == 0) && (xy_to.y == 0) && (xy_from.x == 4) && (xy_to.x == 6)) {
                            // Рокировка чёрных в короткую сторону

                             // Завершающий ход ладьи так как рокировка это двойной ход.
                             // from
                            chess_board.bList.delete_move(7, 0);



                            chess_board.bList.append(5, 0);
                            chess_board.board[0][5] = ChessPiece::B_ROOK;
                            chess_board.board[0][7] = ChessPiece::EMPTY;

                            chess_board.BLACK_O_O = true;
                            chess_board.BLACK_O_O_O = true;

                        }

                        if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::W_KING) &&
                            (xy_from.y == 7) && (xy_to.y == 7) && (xy_from.x == 4) && (xy_to.x == 2)) {
                            // Рокировка белых в длинную сторону

                              // Завершающий ход ладьи так как рокировка это двойной ход.
                                     // from
                            chess_board.wList.delete_move(0, 7);



                            chess_board.wList.append(3, 7);
                            chess_board.board[7][3] = ChessPiece::W_ROOK;
                            chess_board.board[7][0] = ChessPiece::EMPTY;

                            chess_board.WHITE_O_O = true;
                            chess_board.WHITE_O_O_O = true;
                        }

                        if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::W_KING) &&
                            (xy_from.y == 7) && (xy_to.y == 7) && (xy_from.x == 4) && (xy_to.x == 6)) {
                            // Рокировка белых в короткую сторону

                              // Завершающий ход ладьи так как рокировка это двойной ход.
                                     // from
                            chess_board.wList.delete_move(7, 7);


                            chess_board.wList.append(5, 7);
                            chess_board.board[7][5] = ChessPiece::W_ROOK;
                            chess_board.board[7][7] = ChessPiece::EMPTY;

                            chess_board.WHITE_O_O = true;
                            chess_board.WHITE_O_O_O = true;

                        }
                    }
                    chess_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                     // Предыдущий ход на этой новой  доске после хода.
                    chess_board.previos_moves.xy_from = xy_from;
                    chess_board.previos_moves.xy_to = xy_to;
                    if ((xy_to.y >= 0) && (xy_to.y <= 7) && (xy_to.x >= 0) && (xy_to.x <= 7)) {
                        chess_board.previos_moves.figure = chess_board.board[xy_to.y][xy_to.x];
                    }
                    else {
                        chess_board.previos_moves.figure = ChessPiece::EMPTY;
                    }
                }

                for (int i429 = 0; i429 < 7; ++i429) {
                    delete[]  move_memo[i429];
                }
                delete[]  move_memo;

                fclose(fp_database);
            }
            else {
                FILE* fp_database1 = NULL;
                errno_t err_database1;
                err_database1 = fopen_s(&fp_database1, "database_errno_open.txt", "w");

                fprintf_s(fp_database1, "%d ", err_database);

                fclose(fp_database1);
            }
        }
        else if (!BotGame) {

            while (1) {

                // Ответ белых Играет человек.
                if (ilanguage == Russian) {
                    std::cout << " пожалуйста, введите ваш ход за белых:";
                }
                else {
                    std::cout << "please enter your (white) move: \n";
                }

                bfound = true;
                char s[6];
                std::cin >> s;
                if ((s[0] >= 'a') && (s[0] <= 'h')) {
                    xy_from.x = s[0] - 'a';
                }
                else {
                    std::cout << "error input string...\n";
                    system("pause");
                    exit(1);
                }
                if ((s[1] >= '1') && (s[1] <= '8')) {
                    xy_from.y = 8 - (s[1] - '0');
                }
                else {
                    std::cout << "error input string...\n";
                    system("pause");
                    exit(1);
                }
                if ((s[2] >= 'a') && (s[2] <= 'h')) {
                    xy_to.x = s[2] - 'a';
                }
                else {
                    std::cout << "error input string...\n";
                    system("pause");
                    exit(1);
                }
                if ((s[3] >= '1') && (s[3] <= '8')) {
                    xy_to.y = 8 - (s[3] - '0');
                }
                else {
                    std::cout << "error input string...\n";
                    system("pause");
                    exit(1);
                }
                //std::cout << "move :" << static_cast<int>(xy_from.x) << " " << static_cast<int>(xy_from.y) << " " << static_cast<int>(xy_to.x) << " " << static_cast<int>(xy_to.y) << std::endl;


                {
                    int n_01 = 0;
                    MOVES moves_current_list[POOL_SIZEL];

                    LIST_NODE_MOVES make_moves[POOL_SIZE];

                    int i_01 = 0, i_02 = 0;

                    Color color = Color::WHITE;

                    Color my_color = color;
                    // Вражеский цвет должен быть другого знака.
                    Color enemy_color = Color_invert(color); // Цвет врага.

                    __int8 my_depth = THINKING_DEPTH;

                    const char depth = 0;
                    const char iply = 0;
                    const char ishah = 0;
                    const bool enemy = false;

                    MOVES figList;

                    GenerateAllMoves(figList, make_moves, moves_current_list, color, chess_board, n_01, i_01, i_02, my_color, enemy_color, my_depth, enemy, iply, true, false, 0.0);

                    bool b_we_should_be_continue = true;

                    for (int k_1 = 0; k_1 < i_01; ++k_1)
                    {

                        if (b_we_should_be_continue) {


                            int tid = 0;
#ifdef _OPENMP
                            tid = omp_get_thread_num();
#endif

                            __int8 y = make_moves[k_1].xy_from.y;
                            __int8 x = make_moves[k_1].xy_from.x;

                            MOVES_NODE xy_from_pattern;
                            xy_from_pattern = init(x, y);

                            if ((xy_from.x == x) && (xy_from.y == y)&&(make_moves[k_1].xy_to.x== xy_to.x)&&(make_moves[k_1].xy_to.y == xy_to.y)) {
                                b_we_should_be_continue = false;
                            }

                        }
                    }

                    if (!b_we_should_be_continue) {
                        break;// такой ход допустим.
                    }
                    else {
                        if (ilanguage == Russian) {
                            std::cout << "\nВы ввели ход не по правилам шахмат...\n";
                            std::cout << "Введите ход повторно.\n";
                        }
                        else {
                            std::cout << "\nyour move is incorrect...\n";
                        }
                    }
                }


            }

           // Предыдущий ход на этой старой  доске.
              //chess_board.previos_moves.xy_from = chess_board.previos_moves.xy_from;
              //chess_board.previos_moves.xy_to = chess_board.previos_moves.xy_to;
              //chess_board.previos_moves.figure = chess_board.previos_moves.figure;
           {
               if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::B_KING) &&
                   (xy_from.y == 0) && (xy_to.y == 0) && (xy_from.x == 4) && (xy_to.x == 2)) {
                   // Рокировка чёрных в длинную сторону



                   // Завершающий ход ладьи так как рокировка это двойной ход.
                    // from
                   chess_board.bList.delete_move(0, 0);



                   chess_board.bList.append(3, 0);
                   chess_board.board[0][3] = ChessPiece::B_ROOK;
                   chess_board.board[0][0] = ChessPiece::EMPTY;

                   chess_board.BLACK_O_O = true;
                   chess_board.BLACK_O_O_O = true;

               }

               if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::B_KING) &&
                   (xy_from.y == 0) && (xy_to.y == 0) && (xy_from.x == 4) && (xy_to.x == 6)) {
                   // Рокировка чёрных в короткую сторону

                    // Завершающий ход ладьи так как рокировка это двойной ход.
                    // from
                   chess_board.bList.delete_move(7, 0);



                   chess_board.bList.append(5, 0);
                   chess_board.board[0][5] = ChessPiece::B_ROOK;
                   chess_board.board[0][7] = ChessPiece::EMPTY;

                   chess_board.BLACK_O_O = true;
                   chess_board.BLACK_O_O_O = true;

               }

               if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::W_KING) &&
                   (xy_from.y == 7) && (xy_to.y == 7) && (xy_from.x == 4) && (xy_to.x == 2)) {
                   // Рокировка белых в длинную сторону

                     // Завершающий ход ладьи так как рокировка это двойной ход.
                            // from
                   chess_board.wList.delete_move(0, 7);



                   chess_board.wList.append(3, 7);
                   chess_board.board[7][3] = ChessPiece::W_ROOK;
                   chess_board.board[7][0] = ChessPiece::EMPTY;

                   chess_board.WHITE_O_O = true;
                   chess_board.WHITE_O_O_O = true;
               }

               if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::W_KING) &&
                   (xy_from.y == 7) && (xy_to.y == 7) && (xy_from.x == 4) && (xy_to.x == 6)) {
                   // Рокировка белых в короткую сторону

                     // Завершающий ход ладьи так как рокировка это двойной ход.
                            // from
                   chess_board.wList.delete_move(7, 7);


                   chess_board.wList.append(5, 7);
                   chess_board.board[7][5] = ChessPiece::W_ROOK;
                   chess_board.board[7][7] = ChessPiece::EMPTY;

                   chess_board.WHITE_O_O = true;
                   chess_board.WHITE_O_O_O = true;

               }
           }
           chess_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

            // Предыдущий ход на этой новой  доске после хода.
           chess_board.previos_moves.xy_from = xy_from;
           chess_board.previos_moves.xy_to = xy_to;
           if ((xy_to.y >= 0) && (xy_to.y <= 7) && (xy_to.x >= 0) && (xy_to.x <= 7)) {
               chess_board.previos_moves.figure = chess_board.board[xy_to.y][xy_to.x];
           }
           else {
               chess_board.previos_moves.figure = ChessPiece::EMPTY;
           }

        }

        if (!bfound) {

           
            const float conic_rate = calculation_phase_for_conic_rate(chess_board.board, chess_board.wList, chess_board.bList);

            if (ilanguage == Russian) {
                std::cout << "коническая оценка = " << conic_rate << std::endl;
            }
            else {
                std::cout << "calculation_phase_for_conic_rate = " << conic_rate << std::endl;
            }
           
            max_rate = -9999.0f;
            xy_from = init(None);
            xy_to = init(None);

            // Генерация списка возможных ходов с оценкой позиции:
           // Board chess_board_loc;
            //chess_board_loc.copy(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O);
            // Мы защищаем доску от изменений, вдруг мы что то испортим при запуске alpha-beta minimax
           // LIST_MOVES rates = minimax(color, THINKING_DEPTH, chess_board);

            // Компьютер играет только за чёрных. 
            LIST_MOVES rates = minimax(Color::BLACK, THINKING_DEPTH, chess_board);

            //printf("rates.n=%d\n", rates.n);

            int id_moves = 0;

            if ((rates.n == 0) || ((rates.n == 1) &&
                (rates.moves[id_moves].xy_from.x == None) &&
                (rates.moves[id_moves].xy_from.y == None) &&
                (rates.moves[id_moves].xy_to.x == None) &&
                (rates.moves[id_moves].xy_to.y == None))) {

                bKouch_rechime = 0; // Сбрасываем Коуч режим так как уже мат.

                if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                    if (ilanguage == Russian) {
                        printf("  Мат.\n");
                    }
                    else {
                        printf("  Mat\n");
                    }
                }
                else {
                    if (ilanguage == Russian) {
                        printf("  Пат.\n");
                    }
                    else {
                        printf(" Stalemate\n");
                    }
                }
                if (ilanguage == Russian) {
                    printf("Игра окончена...\n");
                }
                else {
                    printf("Game over...\n");
                }
                if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                    if (color == Color::BLACK) {
                        if (ilanguage == Russian) {
                            printf("  Победа белых...\n");
                        }
                        else {
                            printf("  White won...\n");
                        }
                    }
                    else {
                        if (ilanguage == Russian) {
                            printf("  Победа черных...\n");
                        }
                        else {
                            printf("  Black won...\n");
                        }
                    }
                }
                else {
                    if (ilanguage == Russian) {
                        printf(" Зафиксирована ничья.\n");
                    }
                    else {
                        printf(" Fixed a draw.\n");
                    }
                }
                i = number_of_half_passes; // конец компьютерной партии
            }
            else {

               

                // Сортировка по убыванию рейтинга
                for (int id_rate = 1; id_rate < rates.n; ++id_rate)
                {
                    for (int id_rate0 = 0; id_rate0 < rates.n - id_rate; ++id_rate0)
                    {
                        if (rates.moves[id_rate0].rate < rates.moves[id_rate0+1].rate) {
                           
                            // swap
                           LIST_NODE_MOVES tmp;
                           tmp = rates.moves[id_rate0];                           
                           rates.moves[id_rate0]= rates.moves[id_rate0+1];                           
                           rates.moves[id_rate0+1] = tmp;
                           
                           
                        }
                    }
                }

                int inumber_equivalent_moves = 1; // Число эквивалентных ответов.

                if ((rates.n > 3) && ((ilevel_Gameuser == 5)|| (ilevel_Gameuser == 8))) {
                    
                    // При if ((rates.n > 3)&&((ilevel_Gameuser == 5)|| (ilevel_Gameuser == 8)))
                    // Компьютер может выбрать любой в окрестности сантипешка
                    // отличающийся от сильного хода по силе ход, не самый сильный.

                    id_moves = 1;
                    for (int id_rate0 = 1; id_rate0 < rates.n; ++id_rate0)
                    {
                        // Считаем количество ходов отличающихся от лучшего на одну сантипешку.
                        if (fabs(rates.moves[id_rate0].rate - rates.moves[0].rate) < 1.0) {
                            ++id_moves;
                        }
                    }
                    inumber_equivalent_moves = id_moves; // Число эквивалентных ответов на ход (отличие менее одной сантипешки).
                    if (inumber_equivalent_moves >= 3) {
                        if (ilanguage == Russian) {
                            std::cout << "всего ходов из данной позиции = " << rates.n << " число ходов в окрестности сантипешка от лучшего = " << id_moves;
                        }
                        else {
                            std::cout << "all moves = " << rates.n << " equivalent moves = " << id_moves;
                        }
                        // Случайный ход один из двух.
                        std::mt19937 gen{ std::random_device()() };
                        id_moves = gen() % 3;
                        if (ilanguage == Russian) {
                            std::cout << "  случайный идентификатор хода равен =" << id_moves<<" из диапазона [0..2].\n";
                            switch (id_moves) {
                            case 0: std::cout << " первая линия движка\n";
                                break;
                            case 1: std::cout << " вторая линия движка\n";
                                break;
                            case 2: std::cout << " третья линия движка\n";
                                break;
                            }
                        }
                        else {
                            std::cout << "  random move is =" << id_moves;
                            switch (id_moves) {
                            case 0: std::cout << " FIRST LINE CHESS ENGINE\n";
                                break;
                            case 1: std::cout << " SECOND LINE CHESS ENGINE\n";
                                break;
                            case 2: std::cout << " THIRD LINE CHESS ENGINE\n";
                                break;
                            }
                        }

                        max_rate = rates.moves[id_moves].rate;
                        xy_from = rates.moves[id_moves].xy_from;
                        xy_to = rates.moves[id_moves].xy_to;
                    }
                    else {
                        max_rate = rates.moves[0].rate;
                        xy_from = rates.moves[0].xy_from;
                        xy_to = rates.moves[0].xy_to;
                    }

                }
                else {
                    // Опыт использования программы показал что первый ход
                    // самый адекватный, несмотря на то что существует много ходов 
                    // с одинаковой оценкой.

                    id_moves = 1;
                    for (int id_rate0 = 1; id_rate0 < rates.n; ++id_rate0)
                    {
                        // Считаем количество ходов отличающихся от лучшего на одну сантипешку.
                        if (fabs(rates.moves[id_rate0].rate - rates.moves[0].rate) < 1.0) {
                            ++id_moves;
                        }
                    }
                    inumber_equivalent_moves = id_moves; // Число эквивалентных ответов на ход (отличие менее одной сантипешки).
                    if (ilanguage == Russian) {
                        std::cout << "всего ходов из данной позиции = " << rates.n << " число ходов в окрестности сантипешка от лучшего = " << id_moves;
                    }
                    else {
                        std::cout << "all moves = " << rates.n << " equivalent moves = " << id_moves;
                    }

                    max_rate = rates.moves[0].rate;
                    xy_from = rates.moves[0].xy_from;
                    xy_to = rates.moves[0].xy_to;
                }
                

                if (ilanguage == Russian) {
                    std::cout << "Ход чёрных по первой линии движка:" << std::endl;
                }
                else {
                    std::cout << "best move for black:" << std::endl;
                }
                for (int id_rate = 0; id_rate < std::min(4+std::max(inumber_equivalent_moves, std::min(5, rates.n)), rates.n); ++id_rate)
                {
                    std::cout << rates.moves[id_rate].rate << " " << static_cast<char>('a' + rates.moves[id_rate].xy_from.x);
                    std::cout << 8 - rates.moves[id_rate].xy_from.y << static_cast<char>('a' + rates.moves[id_rate].xy_to.x);
                    std::cout << 8 - rates.moves[id_rate].xy_to.y << std::endl;
                }
                std::cout << std::endl;

                if (ilanguage == Russian) {
                    std::cout << "количество шахматных позиций просмотренных при анализе ЭВМ : " << number_chess_positions << std::endl;
                }
                else {
                    std::cout << "number chess positions in tree : " << number_chess_positions << std::endl;
                }

                // Поиск в цикле наилучшего хода из списка сгенерированных.
                /* for (__int8 id_rate = 0; id_rate < rates.n; ++id_rate)
                {

                    //std::cout << "current rate = " << rates.moves[id_rate].rate << std::endl;

                    if (rates.moves[id_rate].rate < max_rate) {
                        continue;
                    }
                    // запоминаем кандидата на наилучший ход.
                    if (max_rate < rates.moves[id_rate].rate)
                    {
                        max_rate = rates.moves[id_rate].rate;
                        xy_from = rates.moves[id_rate].xy_from;
                        xy_to = rates.moves[id_rate].xy_to;

                    }
                }
                */

                //chess_board.move(xy_from, xy_to,43); // Двигаем фигуру по лучшему найденному ходу.

                 // смена цвета.
                //color = Color_invert(color);

                //chess_board.print();// Печатаем состояние доски после



                int xfrom1, yfrom1, xto1, yto1;

                xfrom1 = xy_from.x;
                yfrom1 = xy_from.y;
                xto1 = xy_to.x;
                yto1 = xy_to.y;

                //std::cout << xfrom1 << " " << yfrom1 << " " << xto1 << " " << yto1 << std::endl;


                 // Предыдущий ход на этой старой  доске.
                //chess_board.previos_moves.xy_from = chess_board.previos_moves.xy_from;
                //chess_board.previos_moves.xy_to = chess_board.previos_moves.xy_to;
                //chess_board.previos_moves.figure = chess_board.previos_moves.figure;
                {
                    if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::B_KING) &&
                        (xy_from.y == 0) && (xy_to.y == 0) && (xy_from.x == 4) && (xy_to.x == 2)) {
                        // Рокировка чёрных в длинную сторону



                        // Завершающий ход ладьи так как рокировка это двойной ход.
                         // from
                        chess_board.bList.delete_move(0, 0);



                        chess_board.bList.append(3, 0);
                        chess_board.board[0][3] = ChessPiece::B_ROOK;
                        chess_board.board[0][0] = ChessPiece::EMPTY;

                        chess_board.BLACK_O_O = true;
                        chess_board.BLACK_O_O_O = true;

                    }

                    if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::B_KING) &&
                        (xy_from.y == 0) && (xy_to.y == 0) && (xy_from.x == 4) && (xy_to.x == 6)) {
                        // Рокировка чёрных в короткую сторону

                         // Завершающий ход ладьи так как рокировка это двойной ход.
                         // from
                        chess_board.bList.delete_move(7, 0);



                        chess_board.bList.append(5, 0);
                        chess_board.board[0][5] = ChessPiece::B_ROOK;
                        chess_board.board[0][7] = ChessPiece::EMPTY;

                        chess_board.BLACK_O_O = true;
                        chess_board.BLACK_O_O_O = true;

                    }

                    if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::W_KING) &&
                        (xy_from.y == 7) && (xy_to.y == 7) && (xy_from.x == 4) && (xy_to.x == 2)) {
                        // Рокировка белых в длинную сторону

                          // Завершающий ход ладьи так как рокировка это двойной ход.
                                 // from
                        chess_board.wList.delete_move(0, 7);



                        chess_board.wList.append(3, 7);
                        chess_board.board[7][3] = ChessPiece::W_ROOK;
                        chess_board.board[7][0] = ChessPiece::EMPTY;

                        chess_board.WHITE_O_O = true;
                        chess_board.WHITE_O_O_O = true;
                    }

                    if ((chess_board.board[xy_from.y][xy_from.x] == ChessPiece::W_KING) &&
                        (xy_from.y == 7) && (xy_to.y == 7) && (xy_from.x == 4) && (xy_to.x == 6)) {
                        // Рокировка белых в короткую сторону

                          // Завершающий ход ладьи так как рокировка это двойной ход.
                                 // from
                        chess_board.wList.delete_move(7, 7);


                        chess_board.wList.append(5, 7);
                        chess_board.board[7][5] = ChessPiece::W_ROOK;
                        chess_board.board[7][7] = ChessPiece::EMPTY;

                        chess_board.WHITE_O_O = true;
                        chess_board.WHITE_O_O_O = true;

                    }
                }
                chess_board.move(xy_from, xy_to); // Двигаем фигуру по каждому сгенерированному ходу.

                 // Предыдущий ход на этой новой  доске после хода.
                chess_board.previos_moves.xy_from = xy_from;
                chess_board.previos_moves.xy_to = xy_to;
                if ((xy_to.y >= 0) && (xy_to.y <= 7) && (xy_to.x >= 0) && (xy_to.x <= 7)) {
                    chess_board.previos_moves.figure = chess_board.board[xy_to.y][xy_to.x];
                }
                else {
                    chess_board.previos_moves.figure = ChessPiece::EMPTY;
                }

            }
        }
        
        chess_board.print();// Печатаем состояние доски после
        // смена цвета.
        color = Color_invert(color);

#ifndef CODE_FOR_DLL
        if ((1 && ((i + 1) % 2 != 0))&&(!bfound)) {
            // Только в том случае если была очередь ЭВМ, иначе оценка просто не вычислена будет.

            if (ilanguage == Russian) {
                //if (bfound) {
                  //  printf("\n библиотечный ход. число фигур на доске = %d", number_figures(chess_board.board, chess_board.wList, chess_board.bList));
                //}
                //else {
                printf("\n оценка = %.1f  число фигур на доске = %d (%2.1f)", max_rate, number_figures(chess_board.board, chess_board.wList, chess_board.bList), calculation_phase_for_conic_rate(chess_board.board, chess_board.wList, chess_board.bList));
                //}
            }
            else {
                //if (bfound) {
                  //  printf("\n library move number figures = %d", number_figures(chess_board.board, chess_board.wList, chess_board.bList));
                //}
                //else {
                printf("\n maximum rate = %.1f number figures = %d (%2.1f)", max_rate, number_figures(chess_board.board, chess_board.wList, chess_board.bList), calculation_phase_for_conic_rate(chess_board.board, chess_board.wList, chess_board.bList));
                //}
            }
        }
        if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
            // Здесь может не быть доступных ходов и поэтому это может быть мат.
            int ilevel_Game_memo = ilevel_Game;
            int THINKING_DEPTH_memo = THINKING_DEPTH;

            ilevel_Game = 2; // Включён форсированный вариант.
            THINKING_DEPTH = 2;

            max_rate = -9999.0f;
            xy_from = init(None);
            xy_to = init(None);

            // Компьютер играет только за чёрных. 
            LIST_MOVES rates1 = minimax(Color::WHITE, THINKING_DEPTH, chess_board);

            ilevel_Game = ilevel_Game_memo;
            THINKING_DEPTH = THINKING_DEPTH_memo;

            int id_moves = 0;

            if ((rates1.n == 0) || ((rates1.n == 1) &&
                (rates1.moves[id_moves].xy_from.x == None) &&
                (rates1.moves[id_moves].xy_from.y == None) &&
                (rates1.moves[id_moves].xy_to.x == None) &&
                (rates1.moves[id_moves].xy_to.y == None))) {

                if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                    if (ilanguage == Russian) {
                        printf("  Мат.\n");
                    }
                    else {
                        printf("  Mat\n");
                    }
                }
                else {
                    if (ilanguage == Russian) {
                        printf("  Пат.\n");
                    }
                    else {
                        printf(" Stalemate\n");
                    }
                }
                if (ilanguage == Russian) {
                    printf("Игра окончена...\n");
                }
                else {
                    printf("Game over...\n");
                }
                if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                    if (color == Color::BLACK) {
                        if (ilanguage == Russian) {
                            printf("  Победа белых...\n");
                        }
                        else {
                            printf("  White won...\n");
                        }
                    }
                    else {
                        if (ilanguage == Russian) {
                            printf("  Победа черных...\n");
                        }
                        else {
                            printf("  Black won...\n");
                        }
                    }
                }
                else {
                    if (ilanguage == Russian) {
                        printf(" Зафиксирована ничья.\n");
                    }
                    else {
                        printf(" Fixed a draw.\n");
                    }
                }
                i = number_of_half_passes; // конец компьютерной партии
            }
            else {



                if (ilanguage == Russian) {


                    printf("  Шах\n");
                }
                else {
                    printf("  Check\n");
                }
            }
        }
        if (i == iprint) {
            //std::cout << "from =" << xy_from.x << " " << xy_from.y << " to=" << xy_to.x << " " << xy_to.y << std::endl;
           // printf("from =%d %d to= %d %d\n", xy_from.x, xy_from.y, xy_to.x, xy_to.y);
        }
#endif
        clock_t end = clock();
        double seconds = (double)((1.0*end - 1.0*start)) / CLOCKS_PER_SEC;
        printf("\n "); 
        if (ilanguage == Russian) {
            if ((i + 1) % 2 != 0) {
                printf(" чёрные ");
            }
            else {
                printf(" белые ");
            }
        }
        else {
            if ((i + 1) % 2 != 0) {
                printf(" black ");
            }
            else {
                printf(" white ");
            }
        }
        if (ilanguage == Russian) {
            printf("%d Время анализа: %f секунд\n", (i + 1) / 2, seconds);
        }
        else {
            printf("%d The time: %f seconds\n", (i + 1) / 2, seconds);
        }

        // Режим Коуча (режим подсказки).
        if (1 && ((i + 1) % 2 != 0)) {
            // Вызываем только в ответ чёрных
            if (bKouch_rechime>0)
            {
                if (ilanguage == Russian) {
                    if (bKouch_rechime == 2) {
                        std::cout << " Электронный помошник готовит подсказку лучшего хода для Вас...\n У Вас есть примерно две минуты чтобы подумать над своим ходом.\n";
                    }
                    else {
                        std::cout << " Электронный помошник готовит подсказку лучшего хода для Вас...\n У Вас есть примерно одинадцать секунд чтобы подумать над своим ходом.\n";
                    }
                }
                else {
                    std::cout << " Kouch is thinking... Please thinking your move.\n";
                }

                const float conic_rate = calculation_phase_for_conic_rate(chess_board.board, chess_board.wList, chess_board.bList);
                //if (conic_rate > KING_CENTER_PRIORITY_WEIGHT) {
                    // перешли в эндшпиль
                  //  THINKING_DEPTH = 8;// 12;
                //}
                //if (conic_rate > ENDSHPIL_INCREASE_DEPTH_POROUG) {
                    // перешли в глубокий эндшпиль
                  //  THINKING_DEPTH = 8;// 12;
               // }

                int ilevel_Game_memo = ilevel_Game;
                int THINKING_DEPTH_memo = THINKING_DEPTH;

                ilevel_Game = 2; // Включён форсированный вариант.
                THINKING_DEPTH = 4;

                max_rate = -9999.0f;
                xy_from = init(None);
                xy_to = init(None);

                // Компьютер играет только за чёрных. 
                LIST_MOVES rates1 = minimax(Color::WHITE, THINKING_DEPTH, chess_board);
                bool b1 = false, b2 = false, b3 = false, b4 = false, b5 = false;

                //printf("rates.n=%d\n", rates.n);

                int id_moves = 0;

                if ((rates1.n == 0) || ((rates1.n == 1) &&
                    (rates1.moves[id_moves].xy_from.x == None) &&
                    (rates1.moves[id_moves].xy_from.y == None) &&
                    (rates1.moves[id_moves].xy_to.x == None) &&
                    (rates1.moves[id_moves].xy_to.y == None))) {

                    if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                        if (ilanguage == Russian) {
                            printf("  Мат.\n");
                        }
                        else {
                            printf("  Mat\n");
                        }
                    }
                    else {
                        if (ilanguage == Russian) {
                            printf("  Пат.\n");
                        }
                        else {
                            printf(" Stalemate\n");
                        }
                    }
                    if (ilanguage == Russian) {
                        printf("Игра окончена...\n");
                    }
                    else {
                        printf("Game over...\n");
                    }
                    if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                        if (color == Color::BLACK) {
                            if (ilanguage == Russian) {
                                printf("  Победа белых...\n");
                            }
                            else {
                                printf("  White won...\n");
                            }
                        }
                        else {
                            if (ilanguage == Russian) {
                                printf("  Победа черных...\n");
                            }
                            else {
                                printf("  Black won...\n");
                            }
                        }
                    }
                    else {
                        if (ilanguage == Russian) {
                            printf(" Зафиксирована ничья.\n");
                        }
                        else {
                            printf(" Fixed a draw.\n");
                        }
                    }
                    i = number_of_half_passes; // конец компьютерной партии
                }
                else {

                    // Сортировка по убыванию рейтинга
                    for (int id_rate = 1; id_rate < rates1.n; ++id_rate)
                    {
                        for (int id_rate0 = 0; id_rate0 < rates1.n - id_rate; ++id_rate0)
                        {
                            if (rates1.moves[id_rate0].rate < rates1.moves[id_rate0 + 1].rate) {

                                // swap
                                LIST_NODE_MOVES tmp;
                                tmp = rates1.moves[id_rate0];
                                rates1.moves[id_rate0] = rates1.moves[id_rate0 + 1];
                                rates1.moves[id_rate0 + 1] = tmp;


                            }
                        }
                    }

                    b1 = true;



                }

                ilevel_Game = 2; // Включён форсированный вариант.
                THINKING_DEPTH = 3;

                max_rate = -9999.0f;
                xy_from = init(None);
                xy_to = init(None);

                // Компьютер играет только за чёрных. 
                LIST_MOVES rates4 = minimax(Color::WHITE, THINKING_DEPTH, chess_board);

                // на тройке
                {


                    //printf("rates.n=%d\n", rates.n);

                    int id_moves = 0;

                    if ((rates4.n == 0) || ((rates4.n == 1) &&
                        (rates4.moves[id_moves].xy_from.x == None) &&
                        (rates4.moves[id_moves].xy_from.y == None) &&
                        (rates4.moves[id_moves].xy_to.x == None) &&
                        (rates4.moves[id_moves].xy_to.y == None))) {

                        if (0) {

                            if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                                if (ilanguage == Russian) {
                                    printf("  Мат.\n");
                                }
                                else {
                                    printf("  Mat\n");
                                }
                            }
                            else {
                                if (ilanguage == Russian) {
                                    printf("  Пат.\n");
                                }
                                else {
                                    printf(" Stalemate\n");
                                }
                            }
                            if (ilanguage == Russian) {
                                printf("Игра окончена...\n");
                            }
                            else {
                                printf("Game over...\n");
                            }
                            if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                                if (color == Color::BLACK) {
                                    if (ilanguage == Russian) {
                                        printf("  Победа белых...\n");
                                    }
                                    else {
                                        printf("  White won...\n");
                                    }
                                }
                                else {
                                    if (ilanguage == Russian) {
                                        printf("  Победа черных...\n");
                                    }
                                    else {
                                        printf("  Black won...\n");
                                    }
                                }
                            }
                            else {
                                if (ilanguage == Russian) {
                                    printf(" Зафиксирована ничья.\n");
                                }
                                else {
                                    printf(" Fixed a draw.\n");
                                }
                            }
                            i = number_of_half_passes; // конец компьютерной партии

                        }
                    }
                    else {

                        // Сортировка по убыванию рейтинга
                        for (int id_rate = 1; id_rate < rates4.n; ++id_rate)
                        {
                            for (int id_rate0 = 0; id_rate0 < rates4.n - id_rate; ++id_rate0)
                            {
                                if (rates4.moves[id_rate0].rate < rates4.moves[id_rate0 + 1].rate) {

                                    // swap
                                    LIST_NODE_MOVES tmp;
                                    tmp = rates4.moves[id_rate0];
                                    rates4.moves[id_rate0] = rates4.moves[id_rate0 + 1];
                                    rates4.moves[id_rate0 + 1] = tmp;


                                }
                            }
                        }

                        b4 = true;




                    }
                }

                ilevel_Game = 2; // Включён форсированный вариант.
                THINKING_DEPTH = 5;

                max_rate = -9999.0f;
                xy_from = init(None);
                xy_to = init(None);

                // Компьютер играет только за чёрных. 
                LIST_MOVES rates5 = minimax(Color::WHITE, THINKING_DEPTH, chess_board);

                // на пятёрке
                {


                    //printf("rates.n=%d\n", rates.n);

                    int id_moves = 0;

                    if ((rates5.n == 0) || ((rates5.n == 1) &&
                        (rates5.moves[id_moves].xy_from.x == None) &&
                        (rates5.moves[id_moves].xy_from.y == None) &&
                        (rates5.moves[id_moves].xy_to.x == None) &&
                        (rates5.moves[id_moves].xy_to.y == None))) {

                        if (0) {

                            if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                                if (ilanguage == Russian) {
                                    printf("  Мат.\n");
                                }
                                else {
                                    printf("  Mat\n");
                                }
                            }
                            else {
                                if (ilanguage == Russian) {
                                    printf("  Пат.\n");
                                }
                                else {
                                    printf(" Stalemate\n");
                                }
                            }
                            if (ilanguage == Russian) {
                                printf("Игра окончена...\n");
                            }
                            else {
                                printf("Game over...\n");
                            }
                            if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                                if (color == Color::BLACK) {
                                    if (ilanguage == Russian) {
                                        printf("  Победа белых...\n");
                                    }
                                    else {
                                        printf("  White won...\n");
                                    }
                                }
                                else {
                                    if (ilanguage == Russian) {
                                        printf("  Победа черных...\n");
                                    }
                                    else {
                                        printf("  Black won...\n");
                                    }
                                }
                            }
                            else {
                                if (ilanguage == Russian) {
                                    printf(" Зафиксирована ничья.\n");
                                }
                                else {
                                    printf(" Fixed a draw.\n");
                                }
                            }
                            i = number_of_half_passes; // конец компьютерной партии

                        }
                    }
                    else {

                        // Сортировка по убыванию рейтинга
                        for (int id_rate = 1; id_rate < rates5.n; ++id_rate)
                        {
                            for (int id_rate0 = 0; id_rate0 < rates5.n - id_rate; ++id_rate0)
                            {
                                if (rates5.moves[id_rate0].rate < rates5.moves[id_rate0 + 1].rate) {

                                    // swap
                                    LIST_NODE_MOVES tmp;
                                    tmp = rates5.moves[id_rate0];
                                    rates5.moves[id_rate0] = rates5.moves[id_rate0 + 1];
                                    rates5.moves[id_rate0 + 1] = tmp;


                                }
                            }
                        }

                        b5 = true;




                    }
                }

                ilevel_Game = 2; // Включён форсированный вариант.
                THINKING_DEPTH = 6;

                max_rate = -9999.0f;
                xy_from = init(None);
                xy_to = init(None);

                // Компьютер играет только за чёрных. 
                LIST_MOVES rates2;

                // на шестёрке
                if ((bKouch_rechime == 2) || (conic_rate > KING_CENTER_PRIORITY_WEIGHT)) 
                {
                    rates2 = minimax(Color::WHITE, THINKING_DEPTH, chess_board);

                    //printf("rates.n=%d\n", rates.n);

                    int id_moves = 0;

                    if ((rates2.n == 0) || ((rates2.n == 1) &&
                        (rates2.moves[id_moves].xy_from.x == None) &&
                        (rates2.moves[id_moves].xy_from.y == None) &&
                        (rates2.moves[id_moves].xy_to.x == None) &&
                        (rates2.moves[id_moves].xy_to.y == None))) {

                        if (0) {

                            if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                                if (ilanguage == Russian) {
                                    printf("  Мат.\n");
                                }
                                else {
                                    printf("  Mat\n");
                                }
                            }
                            else {
                                if (ilanguage == Russian) {
                                    printf("  Пат.\n");
                                }
                                else {
                                    printf(" Stalemate\n");
                                }
                            }
                            if (ilanguage == Russian) {
                                printf("Игра окончена...\n");
                            }
                            else {
                                printf("Game over...\n");
                            }
                            if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                                if (color == Color::BLACK) {
                                    if (ilanguage == Russian) {
                                        printf("  Победа белых...\n");
                                    }
                                    else {
                                        printf("  White won...\n");
                                    }
                                }
                                else {
                                    if (ilanguage == Russian) {
                                        printf("  Победа черных...\n");
                                    }
                                    else {
                                        printf("  Black won...\n");
                                    }
                                }
                            }
                            else {
                                if (ilanguage == Russian) {
                                    printf(" Зафиксирована ничья.\n");
                                }
                                else {
                                    printf(" Fixed a draw.\n");
                                }
                            }
                            i = number_of_half_passes; // конец компьютерной партии

                        }
                    }
                    else {

                        // Сортировка по убыванию рейтинга
                        for (int id_rate = 1; id_rate < rates2.n; ++id_rate)
                        {
                            for (int id_rate0 = 0; id_rate0 < rates2.n - id_rate; ++id_rate0)
                            {
                                if (rates2.moves[id_rate0].rate < rates2.moves[id_rate0 + 1].rate) {

                                    // swap
                                    LIST_NODE_MOVES tmp;
                                    tmp = rates2.moves[id_rate0];
                                    rates2.moves[id_rate0] = rates2.moves[id_rate0 + 1];
                                    rates2.moves[id_rate0 + 1] = tmp;


                                }
                            }
                        }

                        b2 = true;




                    }
                }


                ilevel_Game = 2; // Включён форсированный вариант.
                THINKING_DEPTH = 8;

                max_rate = -9999.0f;
                xy_from = init(None);
                xy_to = init(None);

                // Компьютер играет только за чёрных. 
                LIST_MOVES rates3;
               

                // на восьмёрке
                if ((bKouch_rechime == 2)|| (conic_rate > KING_CENTER_PRIORITY_WEIGHT)) {

                    // перешли в эндшпиль

                    rates3 = minimax(Color::WHITE, THINKING_DEPTH, chess_board);

                    //printf("rates.n=%d\n", rates.n);

                    int id_moves = 0;

                    if ((rates3.n == 0) || ((rates3.n == 1) &&
                        (rates3.moves[id_moves].xy_from.x == None) &&
                        (rates3.moves[id_moves].xy_from.y == None) &&
                        (rates3.moves[id_moves].xy_to.x == None) &&
                        (rates3.moves[id_moves].xy_to.y == None))) {

                        if (0) {

                            if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                                if (ilanguage == Russian) {
                                    printf("  Мат.\n");
                                }
                                else {
                                    printf("  Mat\n");
                                }
                            }
                            else {
                                if (ilanguage == Russian) {
                                    printf("  Пат.\n");
                                }
                                else {
                                    printf(" Stalemate\n");
                                }
                            }
                            if (ilanguage == Russian) {
                                printf("Игра окончена...\n");
                            }
                            else {
                                printf("Game over...\n");
                            }
                            if (InCheck(chess_board.board, chess_board.wList, chess_board.bList, chess_board.previos_moves, color, chess_board.WHITE_O_O, chess_board.WHITE_O_O_O, chess_board.BLACK_O_O, chess_board.BLACK_O_O_O)) {
                                if (color == Color::BLACK) {
                                    if (ilanguage == Russian) {
                                        printf("  Победа белых...\n");
                                    }
                                    else {
                                        printf("  White won...\n");
                                    }
                                }
                                else {
                                    if (ilanguage == Russian) {
                                        printf("  Победа черных...\n");
                                    }
                                    else {
                                        printf("  Black won...\n");
                                    }
                                }
                            }
                            else {
                                if (ilanguage == Russian) {
                                    printf(" Зафиксирована ничья.\n");
                                }
                                else {
                                    printf(" Fixed a draw.\n");
                                }
                            }
                            i = number_of_half_passes; // конец компьютерной партии

                        }
                    }
                    else {

                        // Сортировка по убыванию рейтинга
                        for (int id_rate = 1; id_rate < rates3.n; ++id_rate)
                        {
                            for (int id_rate0 = 0; id_rate0 < rates3.n - id_rate; ++id_rate0)
                            {
                                if (rates3.moves[id_rate0].rate < rates3.moves[id_rate0 + 1].rate) {

                                    // swap
                                    LIST_NODE_MOVES tmp;
                                    tmp = rates3.moves[id_rate0];
                                    rates3.moves[id_rate0] = rates3.moves[id_rate0 + 1];
                                    rates3.moves[id_rate0 + 1] = tmp;


                                }
                            }
                        }

                        b3 = true;




                    }
                }


                if (b4) {
                    if (ilanguage == Russian) {
                        std::cout << "лучший ход электронного помошника за белых при анализе на глубину 3 полухода:\n";
                    }
                    else {
                        std::cout << "best move for white (Teacher 3):" << std::endl;
                    }
                    // Печатаем три первых линии.
                    for (int id_rate = 0; id_rate < std::min(rates4.n,3); ++id_rate)
                    {
                        std::cout << rates4.moves[id_rate].rate << " " << static_cast<char>('a' + rates4.moves[id_rate].xy_from.x);
                        std::cout << 8 - rates4.moves[id_rate].xy_from.y << static_cast<char>('a' + rates4.moves[id_rate].xy_to.x);
                        std::cout << 8 - rates4.moves[id_rate].xy_to.y << std::endl;
                    }
                    //std::cout << std::endl;
                }

                if (b1) {
                    if (ilanguage == Russian) {
                        std::cout << "лучший ход электронного помошника за белых при анализе на глубину 4 полухода:\n";
                    }
                    else {
                        std::cout << "best move for white (Teacher 4):" << std::endl;
                    }
                    for (int id_rate = 0; id_rate < std::min(rates1.n, 3); ++id_rate)
                    {
                        std::cout << rates1.moves[id_rate].rate << " " << static_cast<char>('a' + rates1.moves[id_rate].xy_from.x);
                        std::cout << 8 - rates1.moves[id_rate].xy_from.y << static_cast<char>('a' + rates1.moves[id_rate].xy_to.x);
                        std::cout << 8 - rates1.moves[id_rate].xy_to.y << std::endl;
                    }
                    // std::cout << std::endl;
                }

                if (b5) {
                    if (ilanguage == Russian) {
                        std::cout << "лучший ход электронного помошника за белых при анализе на глубину 5 полуходов:\n";
                    }
                    else {
                        std::cout << "best move for white (Teacher 5):" << std::endl;
                    }
                    // Печатаем три первых линии.
                    for (int id_rate = 0; id_rate < std::min(rates5.n, 3); ++id_rate)
                    {
                        std::cout << rates5.moves[id_rate].rate << " " << static_cast<char>('a' + rates5.moves[id_rate].xy_from.x);
                        std::cout << 8 - rates5.moves[id_rate].xy_from.y << static_cast<char>('a' + rates5.moves[id_rate].xy_to.x);
                        std::cout << 8 - rates5.moves[id_rate].xy_to.y << std::endl;
                    }
                    //std::cout << std::endl;
                }

                if ((bKouch_rechime == 2) || (conic_rate > KING_CENTER_PRIORITY_WEIGHT)) {
                    if (b2) {
                        if (ilanguage == Russian) {
                            std::cout << "лучший ход электронного помошника за белых при анализе на глубину 6 полуходов:\n";
                        }
                        else {
                            std::cout << "best move for white (Teacher 6):" << std::endl;
                        }
                        // Печатаем три первых линии.
                        for (int id_rate = 0; id_rate < std::min(rates2.n, 3); ++id_rate)
                        {
                            std::cout << rates2.moves[id_rate].rate << " " << static_cast<char>('a' + rates2.moves[id_rate].xy_from.x);
                            std::cout << 8 - rates2.moves[id_rate].xy_from.y << static_cast<char>('a' + rates2.moves[id_rate].xy_to.x);
                            std::cout << 8 - rates2.moves[id_rate].xy_to.y << std::endl;
                        }
                        //std::cout << std::endl;
                    }
                }

                if ((bKouch_rechime == 2) || (conic_rate > KING_CENTER_PRIORITY_WEIGHT))  {
                    // перешли в эндшпиль

                    if (b3) {
                        if (ilanguage == Russian) {
                            std::cout << "лучший ход электронного помошника за белых при анализе на глубину 8 полуходов:\n";
                        }
                        else {
                            std::cout << "best move for white (Teacher 8):" << std::endl;
                        }
                        // Печатаем три первых линии.
                        for (int id_rate = 0; id_rate < std::min(rates3.n, 3); ++id_rate)
                        {
                            std::cout << rates3.moves[id_rate].rate << " " << static_cast<char>('a' + rates3.moves[id_rate].xy_from.x);
                            std::cout << 8 - rates3.moves[id_rate].xy_from.y << static_cast<char>('a' + rates3.moves[id_rate].xy_to.x);
                            std::cout << 8 - rates3.moves[id_rate].xy_to.y << std::endl;
                        }
                        std::cout << std::endl;
                    }
                }

                ilevel_Game = ilevel_Game_memo;
                THINKING_DEPTH = THINKING_DEPTH_memo;

            }
        }

        //system("PAUSE");
    
    }
    /* xfrom1 = 4;
     yfrom1 = 1;
     xto1 = 4;
     yto1 = 2;*/

     /* move1[0] = xfrom1;
      move1[1] = yfrom1;
      move1[2] = xto1;
      move1[3] = yto1;

      if (rates.n == 0) {
          // Конец партии
          int stop = 1;
          move1[9] = stop;
      }
      else {
          // Партия не окончена.
          move1[9] = 0;
      }*/

   
    // Верен только SEE вариант взятий.
    //              minimax                               alpha-beta
    //       debug            realese
    // DEPTH time    ratio   time
    //   3    0.056                                      0.009  (3,3)
    //   4    2.195  (39.2)   0.334                      0.06   (4,4)
    //   5   52.383  (23.86)  12.7                       1.189  (4,5)  0.355
    //   6                   276.244  (21.75) (436kb)    5.853  (4,6)  0.739
    //   7                                              55.259  (4,7)  2.171  (471 kb)  
    //   8                                                      (4,8)  3.15
    //   9                                                      (4,9)  3.921
    //  30  форсированный вариант, взятия просматриваем на 30  (4,30)  6.693  (541 кб) 

    // 16.294s; 2.452; 4.916; 2.62; 15.466; Быстродействие без списка фигур.
    // 39.07s  5.417; 15.769; 3.289; 81.053; 13.877; 123.199; Со списком фигур без сортировки.
    // 38.249s; 9.461; 25.165; 11.468; 130.511; 14.887; 209.534; 18.866;   
    // 46.22s; 4.307; 28.857; 12.702; 60.77; 12.256; 85.019;
    // 45.655s; 4.315; 29.393; 13.034; 62.104; 12.122; с сортировкой
    // 34.229s; 3.274; 20.406; 9.427; 40.984; 9.153; 56.257;
    // 0.184s; 0.133; 0.038; 0.048; 0.029; 0.037; 0.033; // передаю данные по ссылке !!! 4 полухода
    // 3.327s; 10.36; 3.527; 11.611; 2.233; 5.686; 2.772; 3.756; 2.877; // 6 полуходов
    // 2.23s; 8.171; 3.104; 9.123; 1.907; 5.538; 4.305; 3.3; 2.026; 3.732; // 6 полуходов
    // 2.302s; 2.637; 1.034; 2.728; 0.45; 1.392; 1.353; 1.018; 0.879; 0.971; // 6 полуходов от акумулятора.
    // 2.21s; 1.715; 0.768; 1.86; 0.321; 0.975; 0.925; 0.688; 0.605; 0.661; // 6 полуходов 500Kb ОЗУ.

    // 27.08.2022


    system("PAUSE");

    return 0;
}

#endif

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
