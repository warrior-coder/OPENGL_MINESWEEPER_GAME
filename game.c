// game.с
// в данном файле реализована логика игры "—апер"
#include "game.h"


// определение глобальных переменных
extern CELL map[MAP_W][MAP_H];
extern int minesCount;
extern int closedCells;
extern BOOL gameLose;


// реализаци€ функций
BOOL isCellInsideMap(int x, int y)
{
    return (x >= 0) && (y >= 0) && (x < MAP_W) && (y < MAP_H);
}

POINTFLOAT getOpenGLCoords(HWND hwnd, int x, int y)
{
    RECT rect; // RECT (rectangle) Ц определ€ет пр€моугольник координатами его левого верхнего и правого нижнего углов
    GetClientRect(hwnd, &rect);

    POINTFLOAT pf = { x / (float)rect.right * MAP_W, MAP_H - y / (float)rect.bottom * MAP_H }; // POINTFLOAT (point of float) Ц содержит вещественные координаты точки на плоскости
    
    return pf;
}

void initGame()
{
    srand(GetTickCount());
    memset(map, 0, sizeof(map));

    minesCount = 40;
    closedCells = MAP_W * MAP_H;
    gameLose = FALSE;

    int x, y;
    for (int i = 0; i < minesCount; i++)
    {
        x = rand() % MAP_W;
        y = rand() % MAP_H;

        if (map[x][y].mine) i--;
        else
        {
            map[x][y].mine = TRUE;

            for (int rx = -1; rx < 2; rx++)
                for (int ry = -1; ry < 2; ry++)
                    if (isCellInsideMap(x + rx, y + ry)) map[x + rx][y + ry].minesCount++;
        }
    }

    PlaySoundW(L"sounds/game_start.wav", NULL, SND_FILENAME | SND_ASYNC);
}

void openCells_r(int x, int y)
{
    if (!isCellInsideMap(x, y) || map[x][y].open || map[x][y].mine) return;

    map[x][y].open = TRUE;
    closedCells--;

    if (map[x][y].minesCount == 0)
    {
        for (int rx = -1; rx < 2; rx++)
            for (int ry = -1; ry < 2; ry++)
                openCells_r(x + rx, y + ry);
    }
}

void showDigit(int d)
{
    glLineWidth(3);
    glColor3f(0, 0.3, 0.7);

    glBegin(GL_LINES);
        if ((d != 1) && (d != 4))                         { glVertex2f(0.3, 0.85); glVertex2f(0.7, 0.85); }
        if ((d != 0) && (d != 1) && (d != 7))             { glVertex2f(0.3, 0.5); glVertex2f(0.7, 0.5); }
        if ((d != 1) && (d != 4) && (d != 7))             { glVertex2f(0.3, 0.15); glVertex2f(0.7, 0.15); }
        if ((d != 5) && (d != 6))                         { glVertex2f(0.7, 0.5); glVertex2f(0.7, 0.85); }
        if ((d != 2))                                     { glVertex2f(0.7, 0.5); glVertex2f(0.7, 0.15); }
        if ((d != 1) && (d != 2) && (d != 3) && (d != 7)) { glVertex2f(0.3, 0.5); glVertex2f(0.3, 0.85); }
        if ((d == 0) || (d == 2) || (d == 6) || (d == 8)) { glVertex2f(0.3, 0.5); glVertex2f(0.3, 0.15); }
    glEnd();
}

void showMine()
{
    const int n = 6;
    float x, y;
    
    glColor3f(0, 0, 0);
    
    glBegin(GL_POLYGON);
        for (int i = 0; i < n; i++)
        {
            y = 0.5 + cos(M_PI * 2.0 / n * i) * 0.4;
            x = 0.5 + sin(M_PI * 2.0 / n * i) * 0.4;
            glVertex2f(x, y);
        }
    glEnd();
}

void showClosedCell()
{
    glBegin(GL_QUADS);
        glColor3f(0.7, 0.7, 0.7); glVertex2f(0, 0);
        glColor3f(0.8, 0.8, 0.8); glVertex2f(0, 1);
        glColor3f(0.7, 0.7, 0.7); glVertex2f(1, 1);
        glColor3f(0.6, 0.6, 0.6); glVertex2f(1, 0);
    glEnd();
}

void showOpenedCell()
{
    glBegin(GL_QUADS);
        glColor3f(0.3, 0.6, 0.3); glVertex2f(0, 0);
        glColor3f(0.3, 0.7, 0.3); glVertex2f(0, 1);
        glColor3f(0.3, 0.6, 0.3); glVertex2f(1, 1);
        glColor3f(0.3, 0.5, 0.3); glVertex2f(1, 0);
    glEnd();
}

void showFlag()
{
    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
        glVertex2f(0.22, 0.95);
        glVertex2f(0.87, 0.65);
        glVertex2f(0.22, 0.35);
    glEnd();

    glColor3f(0.36, 0.25, 0.22);
    glLineWidth(4);
    glBegin(GL_LINES);
        glVertex2f(0.2, 0.95);
        glVertex2f(0.2, 0.05);
    glEnd();
}

void showGame()
{
    glLoadIdentity();

    glScalef(2.0 / MAP_W, 2.0 / MAP_H, 1.0);
    glTranslatef(-MAP_W / 2.0, -MAP_H / 2.0, 0.0);

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            glPushMatrix();

            glTranslatef(x, y, 0);

            if (map[x][y].open)
            {
                showOpenedCell();
                if (map[x][y].minesCount > 0) showDigit(map[x][y].minesCount);
            }
            else
            {
                showClosedCell();
                if (gameLose && map[x][y].mine) showMine();
                if (map[x][y].flag) showFlag();
            }

            glPopMatrix();
        }
    }
}