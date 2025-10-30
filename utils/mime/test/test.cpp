#include "../include/EmailBuilder.h"
#include "CryptoManager.h"
#include <iostream>
#include <fstream>

std::vector<uint8_t> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open file: " + path);

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
        throw std::runtime_error("Failed to read file: " + path);

    return buffer;
}


int main(){
    auto data = readFile("../test/file/diagram.png");

    EmailBuilder email;
    email.from("taras@gmail.com")
        .to("volodymyr@gmail.com")
        .to("Valik@gmail.com")
        .subject("Hello")
        .body("This is a test email.")
        .attachment("diagram.png", "image/png", "attachment", data);

    std::vector<uint8_t> mimeBinary = email.buildBinary();
    
    std::cout << "MIME size: " << mimeBinary.size() << " bytes" << std::endl;
    std::cout << "================================================================================================" << std::endl;
    
    std::vector<unsigned char> sessionKey(32, 0x01); 
    smtp::ssl::CryptoManager crypto(sessionKey);
   
    std::string mimeString(reinterpret_cast<const char*>(mimeBinary.data()), mimeBinary.size());
    
    std::vector<unsigned char> encrypted = crypto.encrypt(mimeString);
    std::cout << "Encrypted size: " << encrypted.size() << " bytes" << std::endl;

    std::cout << "================================================================================================" << std::endl;
    
    try {
        std::string decryptedMime = crypto.decrypt(encrypted);
        std::cout << "Decrypted size: " << decryptedMime.size() << " bytes" << std::endl;
        
        
        bool same = (decryptedMime.size() == mimeBinary.size() && 
                     std::memcmp(decryptedMime.data(), mimeBinary.data(), mimeBinary.size()) == 0);
        std::cout << "Data integrity: " << (same ? "OK ✓" : "FAILED ✗") << std::endl;

        
    } catch (const std::exception& e) {
        std::cerr << "Decryption failed: " << e.what() << "\n";
        return 1;
    }

    std::cout << "================================================================================================" << std::endl;

    return 0;
}