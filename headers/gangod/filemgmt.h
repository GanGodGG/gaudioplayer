#pragma once

#include <windows.h>
#include <wincrypt.h>
#include <filesystem>

#include <iostream>
#include <string>
#include <sys/stat.h>

// Reading and checking files

namespace fileRead{
    static bool isFileExist(const std::string& path){ // 103ms per file
        struct stat buff; // buffer
        return (stat (path.c_str(), &buff) == 0); // windows and linux perfomance. No "trash"
    }

    static void Createfile(const std::string& path, const std::string& file, const void* content){
        std::filesystem::create_directories(std::filesystem::u8path(path));

        std::ofstream stream(path + "\\" + file);
        if(stream.is_open()){
            stream << content;
        }
        stream.close();
    }

    static void Createfile(const std::string& path, const std::string& file, const char* content, int size){
        std::filesystem::create_directories(std::filesystem::u8path(path));

        std::ofstream stream(path + "\\" + file, std::ios::binary);
        stream.write(content, size);
        stream.close();
    }

    static std::string OpenFILE(const std::string& path){
        std::ifstream stream(path);
        std::string resp;
        std::string buffer;

        while(std::getline(stream, buffer)){
            resp += buffer + "\n";
        }
        stream.close();
        return resp; 
    }

    static std::vector<BYTE> binOpenFILE(const std::string& path){
        std::ifstream stream(path, std::ios::binary | std::ios::ate);
        if(!stream.is_open()) return {};
        std::streamsize size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::vector<BYTE> buff(size);
        if(!stream.read(reinterpret_cast<char*>(buff.data()), size)){
            return {};
        }
        stream.close();
        return buff; 
    }

    // DPAPI
    static std::vector<BYTE> encrypt_token(const std::string& token) {
        DATA_BLOB data_in;
        DATA_BLOB data_out;

        data_in.pbData = (BYTE*)token.c_str();
        data_in.cbData = token.length();
        //wacha lookin here????
        if (CryptProtectData(&data_in, L"UserToken", NULL, NULL, NULL, 0, &data_out)) {
            std::vector<BYTE> result(data_out.pbData, data_out.pbData + data_out.cbData);
            LocalFree(data_out.pbData);
            return result; 
        }
    return {};
    }

    static std::string decrypt_token(const std::vector<BYTE>& content) {
        if(content.empty()) return "";

        DATA_BLOB data_in;
        DATA_BLOB data_out;
        data_in.pbData = const_cast<BYTE*>(content.data());
        data_in.cbData = static_cast<DWORD>(content.size());
        if(CryptUnprotectData(&data_in, NULL, NULL, NULL, NULL, 0, &data_out)){
            std::string result(reinterpret_cast<char*>(data_out.pbData), data_out.cbData);
            LocalFree(data_out.pbData);
            return result; 
        }
        return {};
    }
}