#include <windows.h>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <conio.h>
#include <algorithm>
using namespace std;

HANDLE hConsole;
bool started;

BOOL WINAPI consoleHandler(DWORD);

void consoleInit()
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &info);
    SetConsoleCtrlHandler(consoleHandler, TRUE);
}

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x * 2;
    coord.Y = y;
    SetConsoleCursorPosition(hConsole, coord);
}

const int WIDTH = 10;
const int HEIGHT = 24;

BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO info;
        info.dwSize = 100;
        info.bVisible = TRUE;
        SetConsoleCursorInfo(hConsole, &info);
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        if (started) gotoxy(0, HEIGHT + 2);
        cout << "GAME QUIT" << endl;
        exit(0);
    }
    return TRUE;
}

const int shape[7][4][4] =
    {
        // I
        {{0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 0, 0}},
        // J
        {{0, 0, 1, 0},
         {0, 0, 1, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        // L
        {{0, 1, 0, 0},
         {0, 1, 0, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        // O
        {{0, 0, 0, 0},
         {0, 1, 1, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        // S
        {{0, 0, 0, 0},
         {0, 1, 1, 0},
         {1, 1, 0, 0},
         {0, 0, 0, 0}},
        // Z
        {{0, 0, 0, 0},
         {1, 1, 0, 0},
         {0, 1, 1, 0},
         {0, 0, 0, 0}},
        // T
        {{0, 0, 0, 0},
         {0, 1, 1, 1},
         {0, 0, 1, 0},
         {0, 0, 0, 0}},
};

int getShapeNo(char c)
{
    switch (c)
    {
    case 'I':
        return 0;
    case 'J':
        return 1;
    case 'L':
        return 2;
    case 'O':
        return 3;
    case 'S':
        return 4;
    case 'Z':
        return 5;
    case 'T':
        return 6;
    }
}

char ctable[] = {'I', 'J', 'L', 'O', 'S', 'Z', 'T'};

char getShapeChar(int no)
{
    return ctable[no];
}

void copyShape(const int a[4][4], int b[4][4])
{
    for (int x = 0; x < 4; x++)
        for (int y = 0; y < 4; y++)
            b[y][x] = a[y][x];
}

void rotateShape(int a[4][4], int n = 1)
{
    int b[4][4];
    for (int i = 1; i <= n; i++)
    {
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                b[x][3 - y] = a[y][x];
        copyShape(b, a);
    }
}

void drawTetrion()
{
    SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
    for (int x = 0; x <= WIDTH + 1; x++)
    {
        gotoxy(x, 0);
        cout << "  ";
    }
    for (int x = 0; x <= WIDTH + 1; x++)
    {
        gotoxy(x, HEIGHT + 1);
        cout << "  ";
    }
    for (int y = 1; y <= HEIGHT + 1; y++)
    {
        gotoxy(0, y);
        cout << "  ";
    }
    for (int y = 1; y <= HEIGHT + 1; y++)
    {
        gotoxy(WIDTH + 1, y);
        cout << "  ";
    }
}

void drawShape(int x, int y, const int sh[4][4], bool erase = false)
{
    if (erase)
    {
        SetConsoleTextAttribute(hConsole, 0);
    }
    else
    {
        SetConsoleTextAttribute(hConsole, BACKGROUND_RED);
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (sh[j][i] && (1 <= x + i && x + i <= WIDTH && 1 <= y + j && y + j <= HEIGHT))
            {
                gotoxy(x + i, y + j);
                cout << "  ";
            }
        }
    }
}

bool board[HEIGHT + 9][WIDTH + 9], temp[HEIGHT + 9][WIDTH + 9];
int tx, ty, tNo, tNextNo, tShape[4][4], lineCnt;

void drawLimit()
{
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    for (int x = 1; x <= WIDTH; x++)
    {
        if (board[4][x])
        {
            continue;
        }
        gotoxy(x, 4);
        cout << "__";
    }
}

bool valid(int x, int y, const int sh[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (sh[j][i])
            {
                if (y + j > HEIGHT)
                    return false;
                if (x + i < 1 || x + i > WIDTH)
                    return false;
                if (board[y + j][x + i])
                    return false;
            }
        }
    }
    return true;
}

void toBoard(int x, int y, const int sh[4][4])
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (sh[j][i])
                board[y + j][x + i] = true;
}

void printBoard()
{
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED);
    for (int x = 1; x <= WIDTH; x++)
    {
        for (int y = 1; y <= HEIGHT; y++)
        {
            if (board[y][x])
            {
                SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE);
            }
            else
            {
                SetConsoleTextAttribute(hConsole, 0);
            }
            gotoxy(x, y);
            cout << "  ";
        }
    }
}

void elimLines(bool anim = true)
{
    vector<int> fullLines;
    for (int row = 1; row <= HEIGHT; row++)
    {
        bool full = true;
        for (int i = 1; i <= WIDTH; i++)
        {
            if (!board[row][i])
            {
                full = false;
                break;
            }
        }
        if (full)
        {
            fullLines.push_back(row);
        }
    }
    int nLines = fullLines.size();
    if (anim)
    {
        lineCnt += nLines;
        for (int i = 1; i <= WIDTH / 2; i++)
        {
            for (int iR = 0; iR < nLines; iR++)
            {
                SetConsoleTextAttribute(hConsole, 0);
                gotoxy(WIDTH / 2 - i + 1, fullLines[iR]);
                cout << "  ";
                gotoxy(WIDTH / 2 + i, fullLines[iR]);
                cout << "  ";
            }
            Sleep(70);
        }
    }
    for (int iR = 0; iR < nLines; iR++)
    {
        for (int i = fullLines[iR]; i >= 1; i--)
            for (int j = 1; j <= WIDTH; j++)
                board[i][j] = board[i - 1][j];
    }
    if (anim)
    {
        printBoard();
    }
}

void init()
{
    memset(board, 0, sizeof(board));
    drawTetrion();
    printBoard();
    srand(time(0));
}

int findMinHeight(int no)
{
    for (int x = 1; x <= WIDTH; x++)
        for (int y = 1; y <= HEIGHT; y++)
            temp[y][x] = board[y][x];
    int sh[4][4];
    copyShape(shape[no], sh);
    int h = HEIGHT;
    for (int i = 1; i <= 4; i++)
    {
        for (int x = -4; x <= WIDTH; x++)
        {
            if (!valid(x, 1, sh))
            {
                continue;
            }
            int y = 1;
            for (;; y++)
            {
                if (!valid(x, y, sh))
                {
                    break;
                }
            }
            y--;
            toBoard(x, y, sh);
            elimLines(false);
            int row = 1;
            for (;; row++)
            {
                bool empty = true;
                for (int i = 1; i <= WIDTH; i++)
                {
                    if (board[row][i])
                    {
                        empty = false;
                        break;
                    }
                }
                if (!empty)
                {
                    break;
                }
            }
            h = min(h, HEIGHT - row + 1);
            for (int x = 1; x <= WIDTH; x++)
                for (int y = 1; y <= HEIGHT; y++)
                    board[y][x] = temp[y][x];
        }
        rotateShape(sh);
    }
    return h;
}

int findHardShape()
{
    vector<int> h;
    for (int no = 0; no < 7; no++)
    {
        h.push_back(findMinHeight(no));
    }
    int mxh = *max_element(h.begin(), h.end());
    int tno = 0;
    for (int no = 0; no < 7; no++)
    {
        if (h[no] == mxh)
        {
            tno = no;
            if (getShapeChar(no) == 'S' || getShapeChar(no) == 'Z')
            {
                return no;
            }
        }
    }
    return tno;
}

void spawn()
{
    tNo = findHardShape();
    copyShape(shape[tNo], tShape);
    tx = WIDTH / 2 - 1;
    ty = 1;
}

void left()
{
    if (valid(tx - 1, ty, tShape))
    {
        drawShape(tx, ty, tShape, 1);
        drawLimit();
        tx--;
        drawShape(tx, ty, tShape);
    }
}

void right()
{
    if (valid(tx + 1, ty, tShape))
    {
        drawShape(tx, ty, tShape, 1);
        drawLimit();
        tx++;
        drawShape(tx, ty, tShape);
    }
}

void rotate()
{
    drawShape(tx, ty, tShape, 1);
    rotateShape(tShape);
    if (valid(tx, ty, tShape))
        ;
    else if (valid(tx - 1, ty, tShape))
        tx--;
    else if (valid(tx + 1, ty, tShape))
        tx++;
    else
        rotateShape(tShape, 3);
    drawLimit();
    drawShape(tx, ty, tShape);
}

void info()
{
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
    gotoxy(WIDTH + 4, 4);
    cout << "SCORE: " << lineCnt << "              ";
}

void game()
{
    init();
    drawLimit();
    spawn();
    drawShape(tx, ty, tShape);
    info();
    bool down;
    while (1)
    {
        down = false;
        if (kbhit() && getch() == 224)
        {
            switch (getch())
            {
            case 72:
                rotate();
                break;
            case 75:
                left();
                break;
            case 77:
                right();
                break;
            case 80:
                down = true;
            }
        }
        if (down)
        {
            if (!valid(tx, ty + 1, tShape))
            {
                bool fail = false;
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        if (tShape[j][i] && ty + j <= 4)
                        {
                            fail = true;
                        }
                    }
                }
                toBoard(tx, ty, tShape);
                printBoard();
                drawLimit();
                if (fail)
                {
                    break;
                }
                elimLines();
                drawLimit();
                info();
                spawn();
                drawShape(tx, ty, tShape);
                continue;
            }
            drawLimit();
            drawShape(tx, ty, tShape, 1);
            ty++;
            drawShape(tx, ty, tShape);
        }
    }
}

int main()
{
    system("title HATETETRIS = HATE + TETRIS");
    consoleInit();
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    cout << "GUIDE:" << endl;
    cout << "press left/right arrow to move" << endl;
    cout << "press up arrow to rotate clockwise" << endl;
    cout << "press down arrow to drop, the tetrinos are not going to drop automatically" << endl;
    cout << "TIP: The tetris hates you, so just survive as long as you can :)" << endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    cout << "PRESS ANY KEY TO START HATETRIS" << endl;
    while (true)
        if (kbhit())
        {
            getch();
            break;
        }
    started = true;
    while (1)
    {
        system("cls");
        game();
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        gotoxy(0, HEIGHT + 2);
        cout << "GAME OVER" << endl;
        cout << "SCORE: " << lineCnt << endl;
        cout << "PRESS R TO RESTART" << endl;
        while (true)
            if (kbhit() && getch() == 'r')
                break;
        gotoxy(0, HEIGHT + 2);
        cout << "                             " << endl;
        cout << "                             " << endl;
        cout << "                             " << endl;
    }
    return 0;
}
