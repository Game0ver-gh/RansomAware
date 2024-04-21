#include <windows.h>
#include <string>

class NamedPipeClient 
{
    HANDLE m_hPipe;
    std::string m_pipeName;

public:
    explicit NamedPipeClient(const std::string& name) : m_pipeName("\\\\.\\pipe\\" + name), m_hPipe(INVALID_HANDLE_VALUE) {}

    bool connect() 
    {
        m_hPipe = CreateFileA(
            m_pipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);

        return m_hPipe != INVALID_HANDLE_VALUE;
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
            CloseHandle(m_hPipe);
            m_hPipe = INVALID_HANDLE_VALUE;
        }
    }

    ~NamedPipeClient() 
    {
        close();
    }
};