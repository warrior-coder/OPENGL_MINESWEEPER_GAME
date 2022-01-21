#include "game.h"


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


// функция входа приложения
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{           
    WNDCLASSEX wc;
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
    MSG msg;


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
    wc.lpszClassName = L"className";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    RegisterClassEx(&wc);


    // создаем главное окно
    hwnd = CreateWindowEx(
        0,
        L"className",
        L"MinesWeeper Game",
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

    if (!hwnd) return 1;


    // показываем окно
    ShowWindow(hwnd, nCmdShow);

    // подключаем OpenGL к окну
    EnableOpenGL(hwnd, &hdc, &hrc);


    // основной цикл программы
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    initGame();

    while (TRUE)
    {
        // проверка сообщения
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // отправка или обработка сообщения
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) break;
        }
        else
        {
            showGame();
            SwapBuffers(hdc);

            if (minesCount == closedCells)
            {
                PlaySoundW(L"sounds/game_win.wav", NULL, SND_FILENAME | SND_ASYNC);
                MessageBox(hwnd, L"You Are Win!\t\t", L"MinesWeeper Game", MB_OK);
                initGame();
            }
            if (gameLose)
            {
                PlaySoundW(L"sounds/game_lose.wav", NULL, SND_FILENAME | SND_ASYNC);
                MessageBox(hwnd, L"You Are Lose!\t\t", L"MinesWeeper Game", MB_OK);
                initGame();
            }

            Sleep(100);
        }
    }


    // отключаем OpenGL
    DisableOpenGL(hwnd, hdc, hrc);

    // уничтожаем окно
    DestroyWindow(hwnd);


    return 0;
}


// процедура обрабатки сообщений окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_LBUTTONDOWN)
    {
        POINTFLOAT pf = getOpenGLCoords(hwnd, LOWORD(lParam), HIWORD(lParam));
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
                PlaySoundW(L"sounds/cell_open.wav", NULL, SND_FILENAME | SND_ASYNC);
                openCells_r(x, y);
            }
        }
    }
    else if (message == WM_RBUTTONDOWN)
    {
        POINTFLOAT pf = getOpenGLCoords(hwnd, LOWORD(lParam), HIWORD(lParam));
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
    PIXELFORMATDESCRIPTOR pfd;
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