#include <windows.h>
#include <gl/gl.h>
#include <math.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")


#define MAP_W 20
#define MAP_H 20

typedef struct _CELL
{
    BOOL mine;
    BOOL flag;
    BOOL open;
    int minesCount;
}CELL;

CELL map[MAP_W][MAP_H];
int minesCount;
int closedCells;
BOOL gameLose;

BOOL isCellInsideMap(int x, int y)
{
    return (x >= 0) && (y >= 0) && (x < MAP_W) && (y < MAP_H);
}

POINTFLOAT screenToOpenGL(HWND hwnd, int x, int y)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    POINTFLOAT pf = { x / (float)rect.right * MAP_W, MAP_H - y / (float)rect.bottom * MAP_H };
    return pf;
}

void initGame()
{
    srand(GetTickCount());
    memset(map, 0, sizeof(map));

    minesCount = 5;
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

    PlaySoundW(L"sounds/sound_2.wav", NULL, SND_FILENAME | SND_ASYNC);
}

void openFields_r(int x, int y)
{
    if (!isCellInsideMap(x, y) || map[x][y].open) return;
    map[x][y].open = TRUE;
    closedCells--;

    if (map[x][y].minesCount == 0)
    {
        for (int rx = -1; rx < 2; rx++)
            for (int ry = -1; ry < 2; ry++)
                openFields_r(x + rx, y + ry);
    }
}

void digitLine(float x1, float y1, float x2, float y2)
{
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
}
void showMinesCount(int d)
{
    glLineWidth(3);
    glColor3f(0, 0.3, 0.7);
    
    glBegin(GL_LINES);
        if ((d != 1) && (d != 4)) digitLine(0.3, 0.85, 0.7, 0.85);
        if ((d != 0) && (d != 1) && (d != 7)) digitLine(0.3, 0.5, 0.7, 0.5);
        if ((d != 1) && (d != 4) && (d != 7)) digitLine(0.3, 0.15, 0.7, 0.15);
        if ((d != 5) && (d != 6)) digitLine(0.7, 0.5, 0.7, 0.85);
        if ((d != 2)) digitLine(0.7, 0.5, 0.7, 0.15);
        if ((d != 1) && (d != 2) && (d != 3) && (d != 7)) digitLine(0.3, 0.5, 0.3, 0.85);
        if ((d == 0) || (d == 2) || (d == 6) || (d == 8)) digitLine(0.3, 0.5, 0.3, 0.15);
    glEnd();
}

void showMine()
{
    glColor3f(0, 0, 0);
    int n = 6;
    float x, y;
    float d = 3.1415 * 2 / n;
    glBegin(GL_POLYGON);
        for (int i = 0; i < n; i++)
        {
            x = 0.5 + cos(d * i + 3.1415 / 6) * 0.35;
            y = 0.5 + sin(d * i + 3.1415 / 6) * 0.35;
            glVertex2f(x, y);
        }
    glEnd();
}

void showField()
{
   
    glBegin(GL_QUADS);
        glColor3f(0.7, 0.7, 0.7); glVertex2f(0, 0);
        glColor3f(0.8, 0.8, 0.8); glVertex2f(0, 1);
        glColor3f(0.7, 0.7, 0.7); glVertex2f(1, 1);
        glColor3f(0.6, 0.6, 0.6); glVertex2f(1, 0);
    glEnd();
}

void showOpenedField()
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

void gameShow()
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
                showOpenedField();
                if (map[x][y].minesCount > 0) showMinesCount(map[x][y].minesCount);
            }
            else
            {
                showField();
                if (gameLose && map[x][y].mine) showMine();
                if (map[x][y].flag) showFlag();
            }

            glPopMatrix();
        }

        
    }
}

// протитипы функций
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{           
    WNDCLASSEX wc;          // WNDCLASSEX – window class
    HWND hwnd;              // HWND – handle to the window
    HDC hdc;                // РВС – handle to the DC (document context)
    HGLRC hrc;              // HGLRC – handle to the GL rendering context
    MSG msg;                // MSG – message
    BOOL quit = FALSE;      // BOOL – boolean


    // регестрируем класс окна
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"ClassName";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    RegisterClassEx(&wc);


    // создаем главное окно
    hwnd = CreateWindowEx(
        0,
        L"ClassName",
        L"MinesWeeper",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        640,
        640,
        NULL,
        NULL,
        hInstance,
        NULL
    );


    // показываем окно
    ShowWindow(hwnd, nCmdShow);

    // подключаем OpenGL к окну
    EnableOpenGL(hwnd, &hdc, &hrc);


    // основной цикл программы
    initGame();

    while (!quit)
    {
        // проверка сообщения
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // обрабатывать или отправлять сообщения
            if (msg.message == WM_QUIT)
            {
                quit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* отрисовка OpenGL начинается здесь */
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            gameShow();

        
            SwapBuffers(hdc);

            if (minesCount == closedCells)
            {
                PlaySoundW(L"sounds/sound_4.wav", NULL, SND_FILENAME | SND_ASYNC);

                MessageBox(hwnd, L"You Are Win!                    ", L"MinesWeeper", MB_OK);
                initGame();
            }
            if (gameLose)
            {
                PlaySoundW(L"sounds/sound_1.wav", NULL, SND_FILENAME | SND_ASYNC);

                MessageBox(hwnd, L"You Are Lose!                    ", L"MinesWeeper", MB_OK);
                initGame();
            }

            Sleep(100);
        }
    }


    // отключаем OpenGL
    DisableOpenGL(hwnd, hdc, hrc);

    // уничтожаем окно
    DestroyWindow(hwnd);


    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_LBUTTONDOWN && !gameLose)
    {
        POINTFLOAT pf = screenToOpenGL(hwnd, LOWORD(lParam), HIWORD(lParam));
        
        int x = (int)pf.x;
        int y = (int)pf.y;

        if (isCellInsideMap(x, y) && !map[x][y].flag)
        {
            if (map[x][y].mine) 
            {
                gameLose = TRUE;
            }
            else
            {
                PlaySoundW(L"sounds/sound_3.wav", NULL, SND_FILENAME | SND_ASYNC);

                openFields_r(x, y);
            }
            
        }
    }
    else if (message == WM_RBUTTONDOWN && !gameLose)
    {
        POINTFLOAT pf = screenToOpenGL(hwnd, LOWORD(lParam), HIWORD(lParam));

        int x = (int)pf.x;
        int y = (int)pf.y;

        if (isCellInsideMap(x, y) && !map[x][y].open) map[x][y].flag = !map[x][y].flag;
    }
    else if (message == WM_CLOSE) PostQuitMessage(0);
    else if (message == WM_KEYDOWN)
    {
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void EnableOpenGL(HWND hwnd, HDC* hdc, HGLRC* hrc)
{
    PIXELFORMATDESCRIPTOR pfd; // pixel format descriptor – дескриптор формата пикселей
    int fmt;

    // получаем контекст устройства
    *hdc = GetDC(hwnd);

    // установливаем формат пикселей для контекста
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    fmt = ChoosePixelFormat(*hdc, &pfd);
    SetPixelFormat(*hdc, fmt, &pfd);


    // создаем и включаем контекст отрисовки
    *hrc = wglCreateContext(*hdc);
    wglMakeCurrent(*hdc, *hrc);
}

void DisableOpenGL(HWND hwnd, HDC hdc, HGLRC hrc)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
    ReleaseDC(hwnd, hdc);
}