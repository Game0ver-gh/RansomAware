#include "Window.hpp"
#include "..\include\include.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    dbg::initConsole();

    auto& window = gui::Window::getInstance();
    window.show();

    return 0;
}
