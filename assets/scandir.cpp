/*
Author: github.com/annadostoevskaya
File: scandir.cpp
Date: 21/02/23 09:19:41

Description: <empty>
*/

#define _CRT_SECURE_NO_WARNINGS 1

#include <filesystem>
#include <iostream>
#include <string>
#include <string.h>

char *scandir(const char *dir, const char *targetExt)
{
    std::string result;
    int fileCounter = 0;
    for (const auto &entry : std::filesystem::directory_iterator(dir))
    {
        std::filesystem::path filepath = entry.path();
        std::filesystem::path filename = entry.path().filename();
        if (filename.extension() == targetExt)
        {
            if (fileCounter > 0) result += std::string("\n");
            result += filename.string();
        }

        fileCounter += 1;
    }
    
    if (fileCounter > 0)
    {
        char *listOfFiles = (char*)malloc(result.length());
        strcpy(listOfFiles, result.c_str());
        return listOfFiles;
    }
    
    return NULL;
}

