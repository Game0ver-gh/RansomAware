#include "CryptoKeyManager.hpp"
#include "..\include\include.hpp"

#include <openssl/rsa.h>
#include <openssl/aes.h>
#include "openssl/applink.c" 
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/param_build.h>

#include <fstream>
#include <filesystem>
#include <vector>

#ifdef _DEBUG
void printKeys(const EVP_PKEY* pKey)
{
    if (pKey == nullptr)
    {
        std::cout << "Key is null" << std::endl;
        return;
    }

    BIO* bio = BIO_new(BIO_s_mem());

    if (PEM_write_bio_PrivateKey(bio, pKey, nullptr, nullptr, 0, nullptr, nullptr))
    {
        char* buffer;
        long length = BIO_get_mem_data(bio, &buffer);
        std::cout << "Private Key:" << std::endl;
        std::cout.write(buffer, length);
        std::cout << std::endl;
        BIO_reset(bio);
    }
    else std::cout << "Private key is not available or exportable." << std::endl;

    if (PEM_write_bio_PUBKEY(bio, pKey))
    {
        char* buffer;
        long length = BIO_get_mem_data(bio, &buffer);
        std::cout << "Public Key:" << std::endl;
        std::cout.write(buffer, length);
        std::cout << std::endl;
    }
    else std::cout << "Public key is not available." << std::endl;

    BIO_free(bio);
}
#define PRINT_KEYS(pkey) printKeys(pkey)
#else
#define PRINT_KEYS(pkey) (void(0))
#endif // _DEBUG

#define         RSA_KEY_SIZE 2048
EVP_PKEY*       m_rsaKeys{ nullptr };	///< Pointer to RSA keys.
std::string	    m_lastError;            ///< String to store the last error message.

CryptoKeyManager::~CryptoKeyManager() 
{
    EVP_PKEY_free(m_rsaKeys);
    EVP_cleanup();
    ERR_free_strings();
}

CryptoKeyManager::CryptoKeyManager()
{
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    ERR_print_errors_fp(stderr);
}

bool CryptoKeyManager::generateRSAKey()
{
    if (m_rsaKeys)
    {
        EVP_PKEY_free(m_rsaKeys);
    }

    auto* pkey_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (not pkey_ctx)
    {
        collectLastError();
        return false;  
    }

    if (EVP_PKEY_keygen_init(pkey_ctx) <= 0)
    {
        collectLastError();
        EVP_PKEY_CTX_free(pkey_ctx);
        return false;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(pkey_ctx, RSA_KEY_SIZE) <= 0)
    {
        collectLastError();
        EVP_PKEY_CTX_free(pkey_ctx);
        return false;
    }

    if (EVP_PKEY_keygen(pkey_ctx, &m_rsaKeys) <= 0)
    {
        collectLastError();
        EVP_PKEY_CTX_free(pkey_ctx);
        return false;
    }

    EVP_PKEY_CTX_free(pkey_ctx);
    return true;
}

bool CryptoKeyManager::sendKeysTo(const char* path)
{
    //remove file extension
    auto p = std::filesystem::path(path);
    p.replace_extension();

    auto* file = fopen((p.string() + ".pem").c_str(), "wb");
    if (not file)
    {
        collectLastError();
        return false;
    }

    if (PEM_write_PrivateKey(file, m_rsaKeys, nullptr, nullptr, 0, nullptr, nullptr) <= 0)
    {
        fclose(file);
        collectLastError();
        return false;
    }
    fclose(file);

    file = fopen((p.string() + ".pub").c_str(), "wb");
    if (not file)
    {
		collectLastError();
		return false;
    }

    if (PEM_write_PUBKEY(file, m_rsaKeys) <= 0)
    {
        fclose(file);
        collectLastError();
        return false;
    }
    fclose(file);

    return true;
}

bool CryptoKeyManager::destroyPrivateKey()
{
    if (not m_rsaKeys)
    {
        collectLastError();
        return false;
    }

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_from_pkey(nullptr, m_rsaKeys, nullptr);
    if (not ctx)
    {
        collectLastError();
        return false;
    }

    if (EVP_PKEY_fromdata_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return false;
    }

    OSSL_PARAM_BLD* param_bld = OSSL_PARAM_BLD_new();
    if (!param_bld)
    {
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return false;
    }

    BIGNUM* n = nullptr;
    BIGNUM* e = nullptr;
    EVP_PKEY_get_bn_param(m_rsaKeys, "n", &n);
    EVP_PKEY_get_bn_param(m_rsaKeys, "e", &e);

    if (not OSSL_PARAM_BLD_push_BN(param_bld, "n", n) or
        not OSSL_PARAM_BLD_push_BN(param_bld, "e", e))
    {
        OSSL_PARAM_BLD_free(param_bld);
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return false;
    }

    OSSL_PARAM* params = OSSL_PARAM_BLD_to_param(param_bld);
    if (not params)
    {
        OSSL_PARAM_BLD_free(param_bld);
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return false;
    }

    EVP_PKEY* public_key = nullptr;
    if (EVP_PKEY_fromdata(ctx, &public_key, EVP_PKEY_PUBLIC_KEY, params) <= 0)
    {
        OSSL_PARAM_free(params);
        OSSL_PARAM_BLD_free(param_bld);
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return false;
    }

    OSSL_PARAM_free(params);
    OSSL_PARAM_BLD_free(param_bld);
    EVP_PKEY_CTX_free(ctx);

    EVP_PKEY_free(m_rsaKeys);
    m_rsaKeys = public_key;

    return true;
}

bool CryptoKeyManager::receivePrivateKeyFrom(const char* path)
{
    if (not m_rsaKeys)
    {
        collectLastError();
        return false;
    }

    if (not std::filesystem::exists(path))
    {
        collectLastError();
        return false;
    }

    FILE* privateKeyFile = fopen(path, "rb");
    if (not privateKeyFile)
    {
        collectLastError();
        return false;
    }

    EVP_PKEY* privatePkey = PEM_read_PrivateKey(privateKeyFile, nullptr, nullptr, nullptr);
    fclose(privateKeyFile);

    if (not privatePkey)
    {
        collectLastError();
        return false;
    }

    if (m_rsaKeys && EVP_PKEY_eq(privatePkey, m_rsaKeys) != 1)
    {
        collectLastError();
        EVP_PKEY_free(privatePkey);
        return false;
    }

    EVP_PKEY_free(m_rsaKeys);
    m_rsaKeys = privatePkey;

    return true;
}

bool CryptoKeyManager::generateAESKey(CryptoKeyManager::AESKey* aesKey)
{
    if (not aesKey)
    {
        collectLastError();
        return false;
    }

    aesKey->key.resize(AESKey::KEY_SIZE);
    aesKey->iv.resize(AESKey::IV_SIZE);

    if (RAND_bytes(aesKey->key.data(), AESKey::KEY_SIZE) != 1 or
        RAND_bytes(aesKey->iv.data(), AESKey::IV_SIZE) != 1)
    {
        collectLastError();
        return false;
    }

    return true;
}

bool CryptoKeyManager::restorePublicKeyFrom(const char* path)
{
    if (m_rsaKeys)
    {
        EVP_PKEY_free(m_rsaKeys);
        m_rsaKeys = nullptr;
    }

    FILE* file = fopen(path, "r");
    if (not file) 
    {
        collectLastError();
        return false;
    }

    m_rsaKeys = PEM_read_PUBKEY(file, nullptr, nullptr, nullptr);
    fclose(file);

    if (not m_rsaKeys) 
    {
        collectLastError();
        return false;
    }

    return true;
}

bool CryptoKeyManager::encryptAESKey(AESKey* aesKey)
{
    if (not m_rsaKeys or not aesKey)
    {
        collectLastError();
        return aesKey;
    }

    auto* ctx = EVP_PKEY_CTX_new(m_rsaKeys, nullptr);
    if (not ctx)
    {
        collectLastError();
        return aesKey;
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return aesKey;
    }

    size_t outlen;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen, aesKey->key.data(), aesKey->key.size()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return aesKey;
    }

    std::vector<unsigned char> encryptedData;
    encryptedData.resize(outlen);
    if (EVP_PKEY_encrypt(ctx, encryptedData.data(), &outlen, aesKey->key.data(), aesKey->key.size()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return aesKey;
    }

    EVP_PKEY_CTX_free(ctx);

    aesKey->key = std::move(encryptedData);

    return true;
}

bool CryptoKeyManager::decryptAESKey(AESKey* aesKey)
{
    if (not m_rsaKeys or not hasPrivateKey())
    {
        collectLastError();
        return false;
    }

    if (not aesKey or not aesKey->isEncrypted())
    {
        collectLastError();
        return false;
    }

    auto* ctx = EVP_PKEY_CTX_new(m_rsaKeys, nullptr);
    if (not ctx)
    {
        collectLastError();
        return false;
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return false;
    }

    size_t outlen;
    std::vector<unsigned char> decryptedData;
    decryptedData.resize(AESKey::KEY_SIZE);
    if (EVP_PKEY_decrypt(ctx, decryptedData.data(), &outlen, aesKey->key.data(), aesKey->key.size()) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        collectLastError();
        return false;
    }

    EVP_PKEY_CTX_free(ctx);

    aesKey->key = std::move(decryptedData);

    return true;
}

const char* CryptoKeyManager::getLastError()
{
    return m_lastError.c_str();
}

void CryptoKeyManager::collectLastError()
{
    unsigned long errCode = ERR_get_error();
    char errMsg[256];
    ERR_error_string_n(errCode, errMsg, sizeof(errMsg));
    m_lastError = std::string("Error ").append(std::to_string(errCode)).append(": ").append(errMsg);
}

bool CryptoKeyManager::hasPrivateKey()
{
    if (not m_rsaKeys) 
    {
        collectLastError();
        return false;
    }

    BIO* bio = BIO_new(BIO_s_mem());
    if (not bio)
    {
        collectLastError();
        return false;
    }

    if (PEM_write_bio_PrivateKey(bio, m_rsaKeys, nullptr, nullptr, 0, nullptr, nullptr)) 
    {
        collectLastError();
        BIO_free(bio);
        return true;
    }

    BIO_free(bio);
    collectLastError();
    return false;
}