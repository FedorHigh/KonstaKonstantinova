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
#define x first
#define y second
#define mp make_pair

vector<vector<int>> field;
pair<int, int> adddir = make_pair(0, 0);
pair<int, int> dir, apple;
std::atomic<bool> gameOn{ true };
std::atomic<bool> queried{ true };
std::atomic<bool> playAgain = { true };
int cursize = 1;
int x_size, y_size;
string curword = "";
int difficulty = 3;
int ease = 2;

struct segment {
    segment* next = nullptr, * pred = nullptr;
    queue<pair<int, int>> moves;
    int x, y;

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

    int move(int X, int Y) {
        moves.push(make_pair(X, Y));
        x += moves.front().x;
        y += moves.front().y;
        moves.pop();

        if (x < 0 || y < 0 || x >= x_size || y >= y_size) return 0;

        if (next != nullptr) return next->move(X, Y);
        return 1;
    }

    void die() {
        if (pred != nullptr) pred->next = nullptr;
        else {
            cout << "GAME OVER" << endl;
        }
    }

    int draw() {
        if (next != nullptr) next->draw();
        if (field[x][y] == 1) return 0;
        field[x][y] = 1;
        return 1;
    }

    segment(int X, int Y) {
        x = X;
        y = Y;
    }
};

segment* head = nullptr;

int draw() {
    field.clear();
    field.resize(y_size, vector<int>(x_size, 0));

    if (head->draw() == 0) {
        return 0;
    }
    field[head->x][head->y] = 2;
    field[apple.x][apple.y] = 3;

    cout << "______________________________________" << endl;
    for (int i = 0; i < y_size; i++) {
        for (int j : field[i]) {
            if (j == 1) cout << "#";
            if (j == 2) cout << cursize;
            if (j == 3) cout << "O";
            if (!j) cout << ".";
        }
        cout << endl;
    }
    cout << curword << endl;
    return 1;
}

void tick() {
    if (head == nullptr) return;

    if (head->move(dir.x, dir.y) == 0) return;
    if (head->x == apple.x and head->y == apple.y) {
        head->push();
        cursize++;
        apple.x = rand() % x_size;
        apple.y = rand() % y_size;
    }
    if (!draw()) {
        return;
    }
    queried = 1;
    sleep_for(seconds(ease));
    tick();
}

void genWord() {
    curword = "";
    for (int i = 0; i < difficulty; i++) {
        char chh = 97 + (rand() % 25);
        curword += chh;
    }
}

void processInput() {
    int X, Y;
    string s, d;
    while (gameOn) {
        //cin >> X >> Y;
        //dir = make_pair(X, Y);
        if (!gameOn) break;
        if (queried) {
            if (cin >> s) {
                //cout << "INPUT RECEIVED" << endl;
                queried = 0;
            }
        } else s = " ";
        bool ok = true;
        for (int i = 0;i < difficulty;i++) {
            if (curword[i] != s[i])ok = false;
        }
        if (!ok)continue;
        d = s[s.size() - 1];
        if (d == "l")dir = mp(0, -1);
        if (d == "r")dir = mp(0, 1);
        if (d == "u")dir = mp(-1, 0);
        if (d == "d")dir = mp(1, 0);
        if (d == "+")head->push();
        genWord();
    }
    //cout << "чтобы завершить/продолжить игру, введите любой символ" << endl;
    //cout << "Wanna play again? (y/n): ";
    //cout << "goodbye!";
}

void startGame() {
    genWord();
    x_size = 9;
    y_size = 9;
    field.resize(y_size, vector<int>(x_size, 0));


    head = new segment(1, 1);
    apple.x = rand() % x_size;
    apple.y = rand() % y_size;
    dir = make_pair(0, 1);
    head->push();
    cursize = 1;

    gameOn = true;
    thread* t1 = new thread(processInput);

    tick();
    //cout << "GAME OVER!!!" << endl;
    cout << "You lost! Your score was:" << cursize << " Type anything into the console to continue..." << endl;
    //cout << "Do you want to play again? (y/n): ";
    gameOn = false;



    if (head != nullptr) {
        delete head;
        head = nullptr;
    }
    //t1->die();
    //std::cin.clear();
    //std::stringstream ss;
    //ss << "0";
    //std::cin.rdbuf(ss.rdbuf());
    //std::cin.clear();
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
        ease = 3; // Slow speed
        difficulty = 2; // Short sequence
    } else if (choice == 2) {
        ease = 2; // Normal speed
        difficulty = 3; // Medium sequence
    } else if (choice == 3) {
        ease = 1; // Fast speed
        difficulty = 4; // Long sequence
    } else {
        cout << "Invalid choice. Setting to Medium difficulty." << endl;
        ease = 2;
        difficulty = 3;
    }
}

void showMainMenu() {
    cout << "==============================" << endl;
    cout << "         Snake v1.5          " << endl;
    cout << "==============================" << endl;
    cout << "1. Start Game" << endl;
    cout << "2. Difficulty Settings" << endl;
    cout << "3. Exit" << endl;
    cout << "==============================" << endl;
    cout << "Controls: below the game field, a sequence of letters will appear. To change the direction, enter this sequence, followed one of the direction letters: r, l, u, or d" << endl;
    cout << "==============================" << endl;
    cout << "Choose an option: ";
}

int main() {
    int choice;
    while (true) {
        showMainMenu();
        cin >> choice;

        if (choice == 1) {
            startGame();
            //break;
        } else if (choice == 2) {
            showDifficultySettings();
        } else if (choice == 3) {
            return 0; // Exit the game
        } else {
            cout << "Invalid choice. Please try again." << endl;
        }
    }
}