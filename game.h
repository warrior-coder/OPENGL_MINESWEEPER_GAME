// game.h
// в данном файле описана логика игры "—апер"
#ifndef _GAME_H_
#define _GAME_H_


#include <windows.h>
#include <gl/gl.h>
#define _USE_MATH_DEFINES
#include <math.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")


// определ€ем размеры карты
#define MAP_W 20
#define MAP_H 20


// структура CELL хранит информацию о €чейке пол€
typedef struct _CELL
{
    BOOL mine;
    BOOL flag;
    BOOL open;
    int minesCount;
}CELL;


// объ€вление глобальных переменных (выделение пам€ти)
CELL map[MAP_W][MAP_H];
int minesCount;
int closedCells;
BOOL gameLose;


// прототипы функций
BOOL isCellInsideMap(int, int);

POINTFLOAT getOpenGLCoords(HWND, int, int);

void initGame();

void openCells_r(int, int);

void showDigit(int);

void showMine();

void showClosedCell();

void showOpenedCell();

void showFlag();

void showGame();


#endif // _GAME_H_