#include "include.hpp"

namespace dbg
{
    bool initConsole()
    {
#ifdef _DEBUG
        if (!AllocConsole())
            return false;

        FILE* fDummy;
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);

        SetConsoleTitle(L"RansomAware");

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        {
            COORD bufferSize;
            bufferSize.X = csbi.dwSize.X;
            bufferSize.Y = 500;
            SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
        }

        DBG_PRINT("%s", "Console initialized");
#endif

        return true;
    }

} // namespace dbg