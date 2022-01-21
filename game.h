// game.h
// � ������ ����� ������� ������ ���� "�����"
#ifndef _GAME_H_
#define _GAME_H_


#include <windows.h>
#include <gl/gl.h>
#define _USE_MATH_DEFINES
#include <math.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")


// ���������� ������� �����
#define MAP_W 20
#define MAP_H 20


// ��������� CELL ������ ���������� � ������ ����
typedef struct _CELL
{
    BOOL mine;
    BOOL flag;
    BOOL open;
    int minesCount;
}CELL;


// ���������� ���������� ���������� (��������� ������)
CELL map[MAP_W][MAP_H];
int minesCount;
int closedCells;
BOOL gameLose;


// ��������� �������
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