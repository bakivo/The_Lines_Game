#include <stdio.h>
#include <windows.h>
#include "lines.h"
LRESULT CALLBACK my_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK field_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void game_over(void);
void alarm(int type);
void PaintCell( HDC hdc, int type, int x, int y, int selected, int cell_size );
void paint_score(HDC hdc, RECT * rc );

int view_mode = 1; // mode view : 0 -colors, 1-bitmaps
COLORREF colors[NUM_CELL_TYPE] = {
        RGB(192,192,192),
        RGB(255,0,0),
        RGB(255,255,0),
        RGB(0,255,0),
        RGB(0,0,255)};
HBITMAP bitmaps[NUM_CELL_TYPE];

int field_size;
int cell_size;
HWND field;
HWND reset;
HWND win_main;

int WINAPI WinMain
        (HINSTANCE hInstance, HINSTANCE hPrevInstance, char * pCmdLine, int nCmdShow)
{

    for(int i=0; i<NUM_CELL_TYPE; i++ )
    {
    char name [256];
    sprintf( name, "%d.bmp", i );
    HBITMAP hb = (HBITMAP)LoadImage(NULL, name, IMAGE_BITMAP,0,0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);
    if (hb==NULL) printf("not load");
     bitmaps[i]=hb;
    }

    WNDCLASS wc;
    memset(&wc, 0, sizeof(wc));
    wc.lpszClassName = (const char *) "my_app";
    wc.lpfnWndProc = my_proc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));//(HBRUSH)COLOR_WINDOWFRAME;
    RegisterClass(&wc);

    wc.lpszClassName = (const char *) "field";
    wc.lpfnWndProc = field_proc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));//(HBRUSH)COLOR_WINDOWFRAME;
    RegisterClass(&wc);


    win_main = CreateWindow ("my_app", "lines",
                             WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
                             WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                             50, 50, 400, 400, NULL, NULL, hInstance, NULL);
    ShowWindow(win_main, SW_SHOW);



    MSG msg;
    while (GetMessage(&msg, win_main, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK field_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);

            for (int y = 0; y < SIZE; y++) {
                for (int x = 0; x < SIZE; x++) {
                    int cell =  NODES[y*SIZE+x];
                    PaintCell( hdc, cell, x, y, (y*SIZE+x)==selected, cell_size ); //main paint function
                }
            }
            EndPaint(hwnd,&ps);
        }
            return 0;

        case WM_LBUTTONUP: {
            int xpix = (int)LOWORD(lParam); int ypix = (int)HIWORD(lParam);
            int x = xpix/cell_size; int y = ypix/cell_size;
            int num = y*SIZE+x;

            int r = select(num);   //Main action

            if( r == GAME_OVER ) game_over();
            else alarm(r);

            InvalidateRect(hwnd, 0, true);
            InvalidateRect(GetParent(hwnd), 0, true);
        }
            return 0;
        case WM_RBUTTONUP: {
            game_over();
            InvalidateRect(hwnd, 0, true);
            InvalidateRect(GetParent(hwnd), 0, true);
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
LRESULT CALLBACK my_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            field=CreateWindow( "field", "",
                                WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
                                       50, 50, 400, 400, hwnd, NULL, GetModuleHandle(0), NULL);
            reset=CreateWindow( "button", "NEW",
                                WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER,
                                50, 50, 400, 400, hwnd, (HMENU)121, GetModuleHandle(0), NULL);
        }
            return 0;

        case WM_SIZE:
        {
            int w = (int)LOWORD(lParam);
            int h = (int)HIWORD(lParam);
            //printf ("field size after min=%d\n",field_size);
            if ( w != field_size )
            {
                cell_size = h / (SIZE+1);
                field_size = cell_size * SIZE;
                //field_size = cell_size * SIZE;
                RECT rc;
                SetRect(&rc, 0, 0, field_size, field_size+cell_size);
                AdjustWindowRect( &rc, GetWindowLong(hwnd,GWL_STYLE), true);
                //printf ("1w = %d h = %d wc=%d hc=%d   ww=%d wh=%d\n", w, h, field_size, field_size+cell_size, (int)(rc.right - rc.left), (int)(rc.bottom - rc.top) );
                SetWindowPos(hwnd, 0, 0, 0, rc.right - rc.left,rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
            }
            //else
            //return DefWindowProc(hwnd, uMsg, wParam, lParam);
            //printf ("2w = %d h = %d wc=%d hc=%d\n", w, h, field_size, field_size+cell_size );

            MoveWindow( field, 0 , cell_size, field_size, field_size, true );
            MoveWindow( reset, cell_size*3+8, 0, cell_size, (3*cell_size)/4, true );

            InvalidateRect( field, 0, true); InvalidateRect( hwnd, 0, true);
        }
        return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            //

            //NEXT
            for (int x = 0; x < 3; x++) {
                int cell =  NEXT[x];
                PaintCell( hdc, cell, x, 0, 0, (3*cell_size)/4);
            }
            //SCORE
            RECT rc = {cell_size*4, 0, cell_size*4+cell_size*4, (3*cell_size)/4 };
            paint_score( hdc, &rc );
            //
            EndPaint(hwnd,&ps);
            printf("score=%d\n",score);
        }
            return 0;

        case WM_COMMAND:
            if (wParam==121)
            {
                new_game();
                InvalidateRect(hwnd, 0, true);
                InvalidateRect(field, 0, true);
            }
            return 0;

        case WM_KEYDOWN: {
            if (wParam == VK_SPACE) ShowWindow(hwnd,SW_MINIMIZE);
            if (wParam == VK_ESCAPE)DestroyWindow(hwnd);
        }
            return 0;
 }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void PaintCell( HDC hdc, int type, int x, int y, int selected, int cell_size ) {
    // SIMPLE COLORS
    if ( view_mode == 0) {
        HPEN hp, hpprev;
        HBRUSH hb = CreateSolidBrush(colors[type]);
        HBRUSH hbprev = (HBRUSH) SelectObject(hdc, hb);
        if (selected) // выделенная ячейка ?
        {
            hp = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            hpprev = (HPEN) SelectObject(hdc, hp);
        }
        RoundRect(hdc, x * cell_size, y * cell_size, x * cell_size + cell_size, y * cell_size + cell_size, 16, 16);

        if (selected) {
            SelectObject(hdc, hpprev); // помещаем старый карандаш
            DeleteObject(hp); // освобождаем наш карандаш
        }

        SelectObject(hdc, hbprev); // восстанавливаем кисть
        DeleteObject(hb);// освобождаем нашу кисть
    }
    // PICTURES
    if ( view_mode == 1 )
    {
        HBITMAP hb=bitmaps[type];
        BITMAPINFO bi;
        int res = GetObject( hb, sizeof(bi), &bi );

        HDC hdcmem = CreateCompatibleDC( hdc );
        HGDIOBJ hbold = SelectObject( hdcmem, hb );

        SetStretchBltMode(hdc,COLORONCOLOR);
        StretchBlt( hdc, x*cell_size, y*cell_size, cell_size, cell_size, hdcmem, 0, 0, bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, SRCCOPY );

        SelectObject( hdcmem, hbold );
        DeleteDC(hdcmem);
        if(selected){
            HPEN hp =CreatePen(PS_SOLID,2,RGB(0,0,0));
            HPEN hpprev = (HPEN)SelectObject(hdc, hp);
            HBRUSH f = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
            SelectObject(hdc,f);

            //MoveToEx(hdc,x*cell_size, y*cell_size, );
            Rectangle( hdc, x*cell_size, y*cell_size, x*cell_size + cell_size , y*cell_size + cell_size );
            SelectObject(hdc, hpprev); // помещаем старый карандаш
            DeleteObject(hp); // освобождаем наш карандаш
        }
    }
}
void paint_score(HDC hdc, RECT * rc ) {
    // create font
    LOGFONT font;
    memset(&font,0,sizeof(font));
    strcpy( font.lfFaceName, "Arial" );
    font.lfHeight = rc->bottom- rc->top;
    HFONT hf = CreateFontIndirect(&font);
    // select font to hdc
    HFONT hfold = (HFONT)SelectObject(hdc,hf);
    //draw text to hdc
    char buf[64];
    sprintf(buf,"%d",score);
    DrawText( hdc, buf, strlen(buf), rc, DT_CENTER | DT_VCENTER );
    //select old font and delete my font
    SelectObject(hdc, hfold);
    DeleteObject(hf);
}
void alarm(int type) {
    if (type==0)
        return;
    MessageBeep(1);
}
void game_over(void) {
    MessageBox(win_main,"good game\n rtyui","game over",MB_OK|MB_ICONSTOP);
}
