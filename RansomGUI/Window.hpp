#pragma once
#include <Windows.h>
#include <string>
#include <map>
#include <vector>
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
		TIMER = 1,
		LANGUAGE_LIST_BOX,
		FREE_DECRYPT_BUTTON,
		CHECK_PAYMENT_BUTTON,
		TEXT_BOX,
		COUNT
	};

	enum class Fonts
	{
		TEXT_BOX = 1,
		LABEL,
		TIMER,
		BUTTON,
		COUNT
	};

	enum class Bitmap
	{
		LOGO = 1,
		BITCOIN,
		COUNT
	};

	void						createWindow();
	static LRESULT CALLBACK		WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	HFONT						createFont(int height, int width, std::vector<BYTE>* fontData = nullptr);
	void						updateTimer();
	void						drawLabel(const std::wstring& text);
	void						drawRichText();
	void						drawListBox();
	void						createImages();

	std::map<Widget, HWND>		m_widgets;
	std::map<Fonts, HFONT>		m_fonts;
	std::map<Bitmap, HBITMAP>	m_bitmaps;
	const std::wstring			m_label { L"RansomAware" };
	const uint32_t				m_windowSize[2] { 900, 500 };
	HWND						m_window;
	WNDCLASSEX					m_windowClass;
	COLORREF					m_bgColor { RGB(135, 15, 10) };
	COLORREF					m_textColor { RGB(255, 255, 255) };
	HBRUSH						m_bgBrush;
	TimePoint					m_endTime;
};

} // namespace gui


