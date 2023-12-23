#include <windows.h>
#include <string>

class NamedPipeClient 
{
    HANDLE hPipe;
    std::string pipeName;

public:
    explicit NamedPipeClient(const std::string& name) : pipeName("\\\\.\\pipe\\" + name), hPipe(INVALID_HANDLE_VALUE) {}

    bool connect() 
    {
        hPipe = CreateFileA(
            pipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        return hPipe != INVALID_HANDLE_VALUE;
    }

    bool readData(void* buffer, DWORD bufferSize, DWORD& bytesRead) 
    {
        return ReadFile(hPipe, buffer, bufferSize, &bytesRead, NULL) != FALSE;
    }

    bool writeData(const void* buffer, DWORD bufferSize, DWORD& bytesWritten) 
    {
        return WriteFile(hPipe, buffer, bufferSize, &bytesWritten, NULL) != FALSE;
    }

    void close() 
    {
        if (hPipe != INVALID_HANDLE_VALUE) 
        {
            CloseHandle(hPipe);
            hPipe = INVALID_HANDLE_VALUE;
        }
    }

    ~NamedPipeClient() 
    {
        close();
    }
};