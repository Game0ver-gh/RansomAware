#pragma once
#include "..\include\singleton.hpp"
#include <vector>
#include <string>

class Localization final : public Singleton<Localization>
{
	friend class Singleton<Localization>;
	Localization();

public:
	~Localization();

	enum class Language
	{
		ENGLISH = 1,
		POLISH,
		SPANISH,
		FRENCH,
		GERMAN,
		RUSSIAN,
		COUNT //dont use, only for counting
	};

	enum class Text
	{
		TITLE = 0,
		HEADER,
		HEADER2,
		HEADER3,
		PLACEHOLDER,
		PLACEHOLDER2,
		PLACEHOLDER3,
		RANSOM,
		RANSOM_TIMER_1,
		RANSOM_TIMER_2,
		RANSOM_TIMER_3,
		BUTTON_FREE,
		BUTTON_CHECK,
		BUTTON_COPY,
		HYPERLINK_1,
		HYPERLINK_2,
		LINK_1,
		LINK_2,
		BTC_WALLET,
		COUNT //dont use, only for counting
	};

	bool setLanguage(const Language& lang);
	std::wstring getTextStr(const Text& text);
	const wchar_t* getText(const Text& text);

private:
	Language m_language;
};