#include <windows.h>
#include <string>

class NamedPipeServer 
{
    HANDLE m_hPipe;
    std::string m_pipeName;

public:
    explicit NamedPipeServer(const std::string& name) : m_pipeName("\\\\.\\pipe\\" + name), m_hPipe(INVALID_HANDLE_VALUE) {}

    bool create() 
    {
        m_hPipe = CreateNamedPipeA(
            m_pipeName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1,
            1024 * 16,
            1024 * 16,
            NMPWAIT_USE_DEFAULT_WAIT,
            NULL);

        return m_hPipe != INVALID_HANDLE_VALUE;
    }

    bool connect() 
    {
        return ConnectNamedPipe(m_hPipe, NULL) ? true : (GetLastError() == ERROR_PIPE_CONNECTED);
    }

    bool readData(void* buffer, DWORD bufferSize, DWORD& bytesRead) 
    {
        return ReadFile(m_hPipe, buffer, bufferSize, &bytesRead, NULL) != FALSE;
    }

    bool writeData(const void* buffer, DWORD bufferSize, DWORD& bytesWritten) 
    {
        return WriteFile(m_hPipe, buffer, bufferSize, &bytesWritten, NULL) != FALSE;
    }

    void close() 
    {
        if (m_hPipe != INVALID_HANDLE_VALUE)
        {
            DisconnectNamedPipe(m_hPipe);
            CloseHandle(m_hPipe);
            m_hPipe = INVALID_HANDLE_VALUE;
        }
    }

    ~NamedPipeServer() 
    {
        close();
    }
};