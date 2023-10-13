#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <iostream>
#include <ctime>
#include <chrono>

/*
    ScriptHookV.dll
    std::vector<unsigned char> searchPattern = { 0x83, 0x38, 0x00, 0x74, 0x3A }; // jz
    std::vector<unsigned char> replacementBytes = { 0x83, 0x38, 0x00, 0xEB, 0x3A }; // jmp
*/

std::string GetCurrentLocalTime() {
    std::time_t currentTime = std::time(nullptr);
    std::tm localTime = *std::localtime(&currentTime);

    char timeStr[20];
    std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &localTime);

    return std::string(timeStr);
}

void LogMessage(const std::string& message) {
    std::string timestamp = GetCurrentLocalTime();
    std::cout << "[Byte Patcher] [" << timestamp << "] " << message << std::endl;
}

bool ReplacementBytesExist(const std::vector<unsigned char>& buffer, const std::vector<unsigned char>& replacementBytes) {
    auto it = std::search(buffer.begin(), buffer.end(), replacementBytes.begin(), replacementBytes.end());
    return it != buffer.end();
}

void PatchBytes() {
    LogMessage("Preparing To Open File");
    std::fstream file("path to file you want to patch", std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        LogMessage("Failed to open the file.");
        return;
    }

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});

    //Binary you want to change
    std::vector<unsigned char> searchPattern = { 0x48, 0xFF, 0xCB, 0x75, 0xE8 };
    std::vector<unsigned char> replacementBytes = { 0x48, 0xFF, 0xCB, 0xEB, 0xE8 };

    std::string hexSearchPattern = "Search Pattern: ";
    for (unsigned char byte : searchPattern) {
        char hexByte[6];
        snprintf(hexByte, sizeof(hexByte), "%02X ", byte);
        hexSearchPattern += hexByte;
    }
    LogMessage(hexSearchPattern);

    if (ReplacementBytesExist(buffer, replacementBytes)) {
        LogMessage("Replacement bytes already exist in the file.");
        return;
    }

    auto it = std::search(buffer.begin(), buffer.end(), searchPattern.begin(), searchPattern.end());

    if (it != buffer.end()) {
        LogMessage("Search pattern found in the file. Patching in progress...");

        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::copy(replacementBytes.begin(), replacementBytes.end(), it);

        file.seekp(0);
        file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());

        if (file.good()) {
            std::string patchedBytesMessage = "PATCHED BYTES: ";
            for (unsigned char byte : replacementBytes) {
                char hexByte[6];
                snprintf(hexByte, sizeof(hexByte), "%02X ", byte);
                patchedBytesMessage += hexByte;
            }
            LogMessage(patchedBytesMessage);
            LogMessage("Byte Patching Complete");
        }
        else {
            LogMessage("Write operation failed.");
        }
    }
    else {
        LogMessage("Search pattern not found in the file. No patching performed.");
    }

    file.close();
}

int main() {
    PatchBytes();
    while (true) {

    }
}
