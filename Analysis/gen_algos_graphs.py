import os
import time
import matplotlib.pyplot as plt
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.PublicKey import RSA
from Crypto.Cipher import ARC4
from Crypto.Random import get_random_bytes

MAX_FILES = 50

def generate_files():
    for i in range(MAX_FILES):
        with open(f"file_{i}.bin", 'ab') as f:
            f.write(os.urandom(1024 * 1024 * 128))

def clear_files():
    for i in range(MAX_FILES):
        os.remove(f"file_{i}.bin")

# AES128
def encrypt_decrypt_aes(file_path):
    key = get_random_bytes(16)  # AES key size: 128 bits
    cipher = AES.new(key, AES.MODE_EAX)
    with open(file_path, 'rb') as f:
        data = f.read()
    
    start_time = time.time()
    ciphertext, tag = cipher.encrypt_and_digest(data)
    encryption_time = time.time() - start_time

    cipher = AES.new(key, AES.MODE_EAX, nonce=cipher.nonce)
    start_time = time.time()
    cipher.decrypt_and_verify(ciphertext, tag)
    decryption_time = time.time() - start_time

    return encryption_time, decryption_time

# RSA2048
def encrypt_decrypt_rsa(file_path):
    key = RSA.generate(2048)
    public_key = key.publickey()
    cipher = PKCS1_OAEP.new(public_key)

    with open(file_path, 'rb') as f:
        data = f.read()

    start_time = time.time()
    ciphertext = cipher.encrypt(data)
    encryption_time = time.time() - start_time

    cipher = PKCS1_OAEP.new(key)
    start_time = time.time()
    cipher.decrypt(ciphertext)
    decryption_time = time.time() - start_time

    return encryption_time, decryption_time

# RC4
def encrypt_decrypt_rc4(file_path):
    key = get_random_bytes(16)
    cipher = ARC4.new(key)
    
    with open(file_path, 'rb') as f:
        data = f.read()

    start_time = time.time()
    ciphertext = cipher.encrypt(data)
    encryption_time = time.time() - start_time

    cipher = ARC4.new(key)
    start_time = time.time()
    cipher.decrypt(ciphertext)
    decryption_time = time.time() - start_time

    return encryption_time, decryption_time

def main():
    print("Generating files...")
    # generate_files()

    aes_times = []
    rsa_times = []
    rc4_times = []

    for i in range(MAX_FILES):
        file_path = f"file_{i}.bin"

        aes_enc_time, aes_dec_time = encrypt_decrypt_aes(file_path)
        aes_times.append((aes_enc_time, aes_dec_time))

        rsa_enc_time, rsa_dec_time = encrypt_decrypt_rsa(file_path)
        rsa_times.append((rsa_enc_time, rsa_dec_time))

        rc4_enc_time, rc4_dec_time = encrypt_decrypt_rc4(file_path)
        rc4_times.append((rc4_enc_time, rc4_dec_time))

    # Tworzenie wykresów
    sizes = [i * 1024 * 1024 * 128 for i in range(1, MAX_FILES + 1)]
    plt.figure(figsize=(10, 5))

    # Wykres szyfrowania
    plt.subplot(1, 2, 1)
    plt.plot(sizes, [t[0] for t in aes_times], label='AES128')
    plt.plot(sizes, [t[0] for t in rsa_times], label='RSA2048')
    plt.plot(sizes, [t[0] for t in rc4_times], label='RC4')
    plt.xlabel('Rozmiar pliku (bajty)')
    plt.ylabel('Czas szyfrowania (sekundy)')
    plt.title('Czas szyfrowania')
    plt.legend()

    # Wykres deszyfrowania
    plt.subplot(1, 2, 2)
    plt.plot(sizes, [t[1] for t in aes_times], label='AES128')
    plt.plot(sizes, [t[1] for t in rsa_times], label='RSA2048')
    plt.plot(sizes, [t[1] for t in rc4_times], label='RC4')
    plt.xlabel('Rozmiar pliku (bajty)')
    plt.ylabel('Czas deszyfrowania (sekundy)')
    plt.title('Czas deszyfrowania')
    plt.legend()

    plt.tight_layout()
    plt.show()
    
    # clear_files()


if __name__ == "__main__":
    main()