# **RansomAware**

## Overview
RansomAware is an educational C++ program designed to simulate a real ransomware attack. This project is part of my engineering thesis '*Ransomware for educational purposes*'. It aims to demonstrate the mechanics and dangers of ransomware attacks in a controlled environment.

## Technologies and Tools
- **Programming Language**: Entirely written in C/C++20.
- **Development Environment**: Developed using Microsoft Visual Studio.
- **Simulation Environment**: Utilizes a secure VirtualBox environment running Windows 11.

## Features
- **Ransomware Simulation**: Mimics a real ransomware attack sequence.
- **Encryption Libraries**: Incorporates OpenSSL for *AES128* and *RSA2048* encryption.
- **String Encryption**: Uses XOR techniques for encrypting strings within the application, enhancing the overall security and obfuscation.
- **Disguise Mechanism**: Masquerades as a 'Virtual Intranet Access' application to encourage execution. It also uses misleading strings and information to obscure its true purpose and mislead static analysis efforts.
- **Digital Signature Spoofing**: Utilizes '*stolen*' digital signatures (more here: [SigThief](https://github.com/secretsquirrel/SigThief)) from Microsoft and Virtual Intranet Access .dll and .exe files.
- **Low System Resource Utilization**: Designed to consume minimal system resources, ensuring that the encryption process remains undetected by users and system monitoring tools until it's too late...
- **File Encryption**: Encrypts files with extensions such as *.xlsl*, *.png*, *.mp3*, while avoiding system-critical extensions like *.dll*, *.exe*, *.dat* to maintain system stability.
- **User Interface**: Changes desktop wallpaper and displays a **GUI** with decryption instructions upon encryption completion.
- **VirusTotal Detection Score**: Demonstrates a low detection rate with a score of ***7/72*** on VirusTotal, highlighting its stealth and sophistication. ![VirusTotal Result](ss/virustotal.png)

## Screenshots
### Before running Virtual Intranet Access.exe
![Before](ss/before.png)
### During encryption process
![During](ss/during.png)
### After encryption process
![After](ss/after.png)
![After2](ss/result2.png)
### After 'payment'
![After3](ss/after2.png)

## Installation and Usage
1. **Environment**: Recommended to run in a virtual, isolated environment like VirtualBox.
2. **Dependencies**: Requires Microsoft Visual C++ Redistributable.
3. **Supported Systems**: Compatible only with Windows architecture x64.
4. **Visual Studio Project**: The project includes a Visual Studio (2022) .sln file, which automatically signs the binaries during compilation using '*stolen*' certificates. However, Visual Studio sometimes bugs out and triggers the signing script faster than the compilation finishes—despite using the script in the Post-Build Event (the 2019 version works correctly). If such an error occurs, you'll need to manually sign the binaries using the script utilized in the Post-Build Event properties of each subproject.

## TODO:
- **Ensure Persistence**: Develop functionality to add registry entries and run the program as a service to ensure its persistence in the system.
- **Enhanced Security Against Analysis**: Expand methods to protect against both static and dynamic analysis. This includes implementing debugger detection checks, code packing (packing), and virtualization techniques to further obfuscate the malware.

## Educational Purpose note
This project is strictly for educational purposes to understand and demonstrate the risks associated with ransomware attacks. It is crucial to use this software responsibly and ethically.