#pragma once
#define WIN32_LEAN_AND_MEAN
#include <iostream>
#include <string>
#include <Windows.h>
#include <Lmcons.h>
#include <cstdio>
#include <random>
#include <memory>
#include "..\skCrypter\files\skCrypter.h"

namespace utils 
{
    static std::unique_ptr<FILE, decltype(&fclose)> g_logFile(nullptr, &fclose);

    inline void logToFile(const char* prefix, const char* fmt, ...) 
    {
        va_list args;
        va_start(args, fmt);
        printf("%s(): ", prefix);
        vprintf(fmt, args);
        printf("\n");
        va_end(args);

        if (g_logFile != nullptr) 
        {
            va_start(args, fmt);
            fprintf(g_logFile.get(), "%s(): ", prefix);
            vfprintf(g_logFile.get(), fmt, args);
            fprintf(g_logFile.get(), "\n");
            va_end(args);
        }
    }
} // namespace utils

#define _(str) std::string(skCrypt(str))

#define ENABLE_PRINT_TO_FILE

#ifdef ENABLE_PRINT_TO_FILE
    #define DBG_PRINT(fmt, ...) utils::logToFile(__FUNCTION__, skCrypt(fmt), __VA_ARGS__)
#else
    #ifdef _DEBUG
        #define DBG_PRINT(fmt, ...) printf("%s(): " skCrypt(fmt) "\n", __FUNCTION__, __VA_ARGS__)
    #else
        #define DBG_PRINT(fmt, ...) ((void)0)
    #endif
#endif

namespace utils
{
    inline bool createDir(const std::string& path, bool hidden)
    {
        if (hidden)
        {
            if (CreateDirectoryA(path.c_str(), NULL) or GetLastError() == ERROR_ALREADY_EXISTS)
            {
                SetFileAttributesA(path.c_str(), FILE_ATTRIBUTE_HIDDEN);
                return true;
            }
            return false;
        }

        if (CreateDirectoryA(path.c_str(), NULL) or GetLastError() == ERROR_ALREADY_EXISTS)
            return true;

        return false;
    }

    inline void stopAndExit(int exitCode)
    {
        exitCode == EXIT_SUCCESS ? DBG_PRINT("SUCCESS") : DBG_PRINT("FAILURE");

#ifdef _DEBUG
#pragma warning(push)
#pragma warning(disable: 6031) // return value ignored

        std::getchar();

#pragma warning(pop)
#endif // DEBUG
        exit(exitCode);
    }

    inline std::string getSystemUserName()
    {
        char username[UNLEN + 1];
        DWORD username_len = UNLEN + 1;
        GetUserNameA(username, &username_len);
        return std::string(username);
    }

    inline std::string genRandomString(const size_t len)
    {
        static const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, int(chars.size() - 1));

        std::string randomString;
        for (int i = 0; i < len; ++i)
            randomString += chars[distribution(generator)];

        return randomString;
    }

    inline const std::string& getCurrentWorkingDir()
    {
        static std::string cachedPath{};

        if (cachedPath.empty())
        {
            char buffer[MAX_PATH];
            DWORD length = GetCurrentDirectoryA(MAX_PATH, buffer);
            if (length == 0)  cachedPath = "";
            else cachedPath = std::string(buffer, length);
        }

        return cachedPath;
    }

} // namespace utils

namespace dbg
{
    inline bool initLogs()
    {
#ifdef _DEBUG
        if (GetConsoleWindow())
            return true;

        if (!AllocConsole())
            return false;

        FILE* fDummy;
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);

        SetConsoleTitleA(utils::genRandomString(0x10).c_str());

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

#ifdef ENABLE_PRINT_TO_FILE
        utils::g_logFile = std::unique_ptr<FILE, decltype(&fclose)>(fopen("log.cfg", "a"), &fclose);
        if (utils::g_logFile)
            fprintf(utils::g_logFile.get(), "\n\n=== LOG SESSION START ===\n\n");
#endif

        return true;
    }

} // namespace dbg

static const char* g_legitStrings[] =
{
    "Initiating encrypted protocol for transaction safety...",
    "Securing channel for financial operations...",
    "Activating SSL layers for data protection...",
    "Verifying digital certificate for online banking...",
    "Establishing TLS handshake for account access...",
    "Authenticating secure gateway for fund transfer...",
    "Encrypting payload for secure data transmission...",
    "Validating SSL certificate for secure connectivity...",
    "Generating cryptographic keys for transaction security...",
    "Applying advanced encryption for account safety...",
    "Setting up secure tunnel for banking communication...",
    "Implementing HTTPS protocol for data confidentiality...",
    "Initiating secure socket layer for online payment...",
    "Establishing VPN tunnel for encrypted banking access...",
    "Validating trust chain for secure financial exchange...",
    "Activating end-to-end encryption for sensitive data...",
    "Ensuring data integrity in secure online transactions...",
    "Encrypting client-server communication for financial safety...",
    "Authenticating secure connection for online trading...",
    "Enforcing cryptographic standards for banking privacy...",
};

#define INSERT_RND_LEGIT_STR std::cout << g_legitStrings[__LINE__ % _countof(g_legitStrings)] << std::endl;