#include <iostream>
#include <chrono>
#include <queue>
#include <thread>
#include <sstream>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::queue;
using std::pair;
using std::make_pair;
using std::vector;
using std::string;
using std::thread;
using std::rand;
using std::chrono::seconds;
using std::this_thread::sleep_for;
using std::terminate;

// Макросы для удобства работы с координатами
#define x first
#define y second
#define mp make_pair

// Глобальные переменные
vector<vector<int>> field; // Игровое поле
pair<int, int> dir, apple; // Направление движения змейки и координаты яблока
std::atomic<bool> gameOn{ true }; // Флаг, указывающий, продолжается ли игра
std::atomic<bool> queried{ true }; // Флаг, указывающий, был ли получен ввод от пользователя
std::atomic<bool> playAgain = { true }; // Флаг, указывающий, хочет ли пользователь играть снова
int cursize = 1; // Текущий размер змейки
int x_size, y_size; // Размеры игрового поля
string curword = ""; // Текущее слово, которое нужно ввести для управления змейкой
int difficulty = 3; // Уровень сложности (длина слова)
int ease = 2; // Скорость игры (задержка между ходами)

// Структура, представляющая сегмент змейки
struct segment {
    segment* next = nullptr, * pred = nullptr; // Указатели на следующий и предыдущий сегменты
    queue<pair<int, int>> moves; // Очередь движений сегмента
    int x, y; // Координаты сегмента

    // Метод для добавления нового сегмента змейки
    void push() {
        if (next != nullptr) {
            next->push();
            return;
        }

        pair<int, int> ddir = dir;
        if (pred != nullptr and !pred->moves.empty()) ddir = pred->moves.front();

        next = new segment(x - ddir.x, y - ddir.y);
        next->moves = moves;
        next->moves.push(ddir);
        next->pred = this;
    }

    // Метод для перемещения сегмента
    int move(int X, int Y) {
        moves.push(make_pair(X, Y));
        x += moves.front().x;
        y += moves.front().y;
        moves.pop();

        // Проверка на выход за границы поля
        if (x < 0 || y < 0 || x >= x_size || y >= y_size) return 0;

        // Рекурсивное перемещение следующего сегмента
        if (next != nullptr) return next->move(X, Y);
        return 1;
    }

    // Метод для удаления сегмента (конец игры)
    void die() {
        if (pred != nullptr) pred->next = nullptr;
        else {
            cout << "GAME OVER" << endl;
        }
    }

    // Метод для отрисовки сегмента на поле
    int draw() {
        if (next != nullptr) next->draw();
        if (field[x][y] == 1) return 0; // Проверка на столкновение с собой
        field[x][y] = 1;
        return 1;
    }

    // Конструктор сегмента
    segment(int X, int Y) {
        x = X;
        y = Y;
    }
};

segment* head = nullptr; // Голова змейки

// Функция для отрисовки игрового поля
int draw() {
    field.clear();
    field.resize(y_size, vector<int>(x_size, 0));

    // Отрисовка змейки и яблока
    if (head->draw() == 0) {
        return 0;
    }
    field[head->x][head->y] = 2; // Голова змейки
    field[apple.x][apple.y] = 3; // Яблоко

    // Вывод игрового поля в консоль
    cout << "______________________________________" << endl;
    for (int i = 0; i < y_size; i++) {
        for (int j : field[i]) {
            if (j == 1) cout << "#"; // Тело змейки
            if (j == 2) cout << cursize; // Голова змейки
            if (j == 3) cout << "O"; // Яблоко
            if (!j) cout << "."; // Пустое пространство
        }
        cout << endl;
    }
    cout << curword << endl; // Вывод текущего слова для управления
    return 1;
}

// Функция, отвечающая за игровой цикл
void tick() {
    if (head == nullptr) return;


    // Перемещение змейки
    if (head->move(dir.x, dir.y) == 0) return;
    // Проверка на съедание яблока
    if (head->x == apple.x and head->y == apple.y) {
        head->push();
        cursize++;
        apple.x = rand() % x_size;
        apple.y = rand() % y_size;
    }
    // Отрисовка поля
    if (!draw()) {
        return;
    }
    queried = 1;
    sleep_for(seconds(ease)); // Задержка между ходами
    tick(); // Рекурсивный вызов для следующего хода
}

// Генерация нового слова для управления
void genWord() {
    curword = "";
    for (int i = 0; i < difficulty; i++) {
        char chh = 97 + (rand() % 25);
        curword += chh;
    }
}

// Функция для обработки ввода пользователя
void processInput() {
    int X, Y;
    string s, d;
    while (gameOn) {
        if (!gameOn) break;
        if (queried) {
            if (cin >> s) {
                queried = 0;
            }
        } else s = " ";
        bool ok = true;
        for (int i = 0;i < difficulty;i++) {
            if (curword[i] != s[i])ok = false;
        }
        if (!ok)continue;
        d = s[s.size() - 1];
        if (d == "l")dir = mp(0, -1); // Движение влево
        if (d == "r")dir = mp(0, 1); // Движение вправо
        if (d == "u")dir = mp(-1, 0); // Движение вверх
        if (d == "d")dir = mp(1, 0); // Движение вниз
        if (d == "+")head->push(); // Увеличение змейки
        genWord(); // Генерация нового слова
    }
}

// Функция для начала игры
void startGame() {
    genWord();
    x_size = 9;
    y_size = 9;
    field.resize(y_size, vector<int>(x_size, 0));

    // Инициализация змейки и яблока
    head = new segment(1, 1);
    apple.x = rand() % x_size;
    apple.y = rand() % y_size;
    dir = make_pair(0, 1);
    head->push();
    cursize = 1;

    gameOn = true;
    thread* t1 = new thread(processInput); // Запуск потока для обработки ввода

    tick(); // Запуск игрового цикла
    cout << "You lost! Your score was:" << cursize << " Type anything into the console to continue..." << endl;
    gameOn = false;

    // Очистка памяти
    if (head != nullptr) {
        delete head;
        head = nullptr;
    }
    t1->join();
    delete t1;

    // Предложение сыграть снова
    char choice;
    cout << "Do you want to play again? (y/n): ";
    cin >> choice;
    if (choice == 'y' || choice == 'Y') {
        startGame(); // Запуск игры снова
    } else {
        playAgain = false; // Выход из игры
    }
}

// Функция для отображения настроек сложности
void showDifficultySettings() {
    cout << "==============================" << endl;
    cout << "      Difficulty Settings     " << endl;
    cout << "==============================" << endl;
    cout << "1. Easy (slow speed)" << endl;
    cout << "2. Medium (normal speed)" << endl;
    cout << "3. Hard (fast speed)" << endl;
    cout << "==============================" << endl;
    cout << "Choose difficulty level: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        ease = 3; // Медленная скорость
        difficulty = 2; // Короткая последовательность
    } else if (choice == 2) {
        ease = 2; // Нормальная скорость
        difficulty = 3; // Средняя последовательность
    } else if (choice == 3) {
        ease = 1; // Быстрая скорость
        difficulty = 4; // Длинная последовательность
    } else {
        cout << "Invalid choice. Setting to Medium difficulty." << endl;
        ease = 2;
        difficulty = 3;
    }
}
