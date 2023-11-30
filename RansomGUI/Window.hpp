#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <chrono>
#include "..\include\singleton.hpp"

namespace gui
{

class Window final : public Singleton<Window>
{
	friend class Singleton<Window>;
	using TimePoint = std::chrono::steady_clock::time_point;

public:
	~Window();

	void show();

private:
	Window();

	enum class Widget
	{
		TIMER = 1001,
		TIMER_UPDATE,
		LANGUAGE_LIST_BOX,
		FREE_DECRYPT_BUTTON,
		CHECK_PAYMENT_BUTTON,
		COPY_BUTTON,
		TEXT_BOX,
		HYPERLINK_WHAT_IS_BTC,
		HYPERLINK_HOW_TO_BUY,
		LABEL
	};

	enum class Fonts
	{
		TEXT_BOX = 1,
		LABEL,
		TIMER,
		BUTTON,
		TEXT,
		HYPERLINK
	};

	enum class Bitmap
	{
		LOGO = 1,
		BITCOIN,
	};

	void						createWindow();
	static LRESULT CALLBACK		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	HFONT						createFont(const std::wstring& fontName, int height, int weight, bool italic = false, bool strikeout = false, bool underline = false);
	std::wstring				timeToString(const std::chrono::seconds& time);
	void						updateTimer();
	void						drawLabel();
	void						drawTimer();
	void						drawLinks();
	void						drawRichText();
	void						drawListBox();
	void						drawButtons();
	void						drawImage(HDC hdc, const Bitmap& bitmap, int x, int y, float scale = 1.0f, int w = -1, int h = -1);
	void						drawRect(int x, int y, int w, int h, COLORREF color, bool solid = false);
	void						drawText(const std::wstring& text, int x, int y, const Fonts& font, COLORREF color, bool center = false);
	void						createImages();
	RECT						getWidgetRect(const Widget& widget);
	void						redrawWidgets();

	const std::wstring			m_label { L"RansomAware" };
	const uint32_t				m_windowSize[2]{ 900, 520 };
	COLORREF					m_bgColor{ RGB(255, 38, 26) };
	COLORREF					m_textColor{ RGB(255, 255, 255) };
	std::chrono::seconds		m_expirationTime{ 7 * 24 * 60 * 60 };

	std::map<Widget, HWND>		m_widgets;
	std::map<Fonts, HFONT>		m_fonts;
	std::map<Bitmap, HBITMAP>	m_bitmaps;
	HWND						m_window;
	WNDCLASSEX					m_windowClass;
	HBRUSH						m_bgBrush;
	TimePoint					m_endTime;
};

} // namespace gui


