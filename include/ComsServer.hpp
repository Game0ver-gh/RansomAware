#include <windows.h>
#include <string>

class NamedPipeServer 
{
    HANDLE hPipe;
    std::string pipeName;

public:
    explicit NamedPipeServer(const std::string& name) : pipeName("\\\\.\\pipe\\" + name), hPipe(INVALID_HANDLE_VALUE) {}

    bool create() 
    {
        hPipe = CreateNamedPipeA(
            pipeName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1,
            1024 * 16,
            1024 * 16,
            NMPWAIT_USE_DEFAULT_WAIT,
            NULL);

        return hPipe != INVALID_HANDLE_VALUE;
    }

    bool connect() 
    {
        return ConnectNamedPipe(hPipe, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
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
            DisconnectNamedPipe(hPipe);
            CloseHandle(hPipe);
            hPipe = INVALID_HANDLE_VALUE;
        }
    }

    ~NamedPipeServer() 
    {
        close();
    }
};