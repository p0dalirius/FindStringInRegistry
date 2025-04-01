#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

void SearchRegistry(HKEY hKey, const std::string& path, const std::string& searchString);

int main(int argc, char* argv[])
{
    std::string searchString;

    if (argc < 2)
    {
        std::cout << "[?] Enter the string to search in registry keys: ";
        std::getline(std::cin, searchString);
        if (searchString.empty())
        {
            std::cout << "[!] No search string provided. Exiting..." << std::endl;
            return 1;
        }
    }
    else
    {
        searchString = argv[1];
    }

    std::cout << "\n[>] Searching all registry hives for keys containing '" << searchString << "'..." << std::endl;

    SearchRegistry(HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE", searchString);
    SearchRegistry(HKEY_CURRENT_USER, "HKEY_CURRENT_USER", searchString);
    SearchRegistry(HKEY_CLASSES_ROOT, "HKEY_CLASSES_ROOT", searchString);
    SearchRegistry(HKEY_USERS, "HKEY_USERS", searchString);

    std::cout << "Search complete. Press Enter to close the program." << std::endl;
    std::cin.get();
    return 0;
}

void SearchRegistry(HKEY hKey, const std::string& path, const std::string& searchString)
{
    DWORD index = 0;
    CHAR valueName[MAX_PATH];
    BYTE data[MAX_PATH];
    DWORD valueNameSize, dataSize, type;

    while (true)
    {
        valueNameSize = MAX_PATH;
        dataSize = MAX_PATH;
        LONG result = RegEnumValueA(hKey, index, valueName, &valueNameSize, NULL, &type, data, &dataSize);

        if (result == ERROR_NO_MORE_ITEMS)
            break;
        if (result == ERROR_SUCCESS)
        {
            std::string valueStr(reinterpret_cast<char*>(data), dataSize);
            if (valueStr.find(searchString) != std::string::npos)
            {
                std::cout << "[+] Path: " << path << "\\" << valueName << std::endl;
                std::cout << "  | Value: " << valueStr << std::endl;
                std::cout << "" << std::endl;
            }
        }
        ++index;
    }

    index = 0;
    CHAR subKeyName[MAX_PATH];
    while (RegEnumKeyA(hKey, index, subKeyName, MAX_PATH) == ERROR_SUCCESS)
    {
        HKEY subKey;
        if (RegOpenKeyA(hKey, subKeyName, &subKey) == ERROR_SUCCESS)
        {
            SearchRegistry(subKey, path + "\\" + subKeyName, searchString);
            RegCloseKey(subKey);
        }
        ++index;
    }
}
