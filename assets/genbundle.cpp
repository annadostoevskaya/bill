/*
Author: github.com/annadostoevskaya
File: genbundle.cpp
Date: 20/02/23 09:13:13

Description: Simple assets.bundle generator for bill.
*/

#include <iostream> // TODO(annad): Write in output log
#include <fstream>
#include <string>
#include <filesystem>
#include <cassert>
#include <cctype>

void to_c_style_const(std::string &s)
{
    for (auto iter = s.begin(); iter != s.end(); iter++)
    {
        if (*iter == '.')
        {
            *iter = '_';
        }
        
        *iter = std::toupper(static_cast<unsigned char>(*iter));
    }
}

int main(int argc, char **argv)
{
    std::ofstream bundle("./../build/assets.bundle", std::ios::out | std::ios::binary | std::ios::ate);
    std::ofstream assets_h("./../assets.h", std::ios::out);
    
    assets_h << "/*" << std::endl;
    assets_h << "    This file generated with 'genbundle' program!" << std::endl;
    assets_h << "    See ./assets/README.txt for more information." << std::endl;
    assets_h << "*/" << std::endl;
    assets_h << std::endl;
    assets_h << "enum ASSETS_BUNDLE" << std::endl;
    assets_h << "{" << std::endl;
    std::uintmax_t file_begin_from = 0;
    for (const auto &entry : std::filesystem::directory_iterator("./bmp"))
    {
        std::filesystem::path file_path = entry.path();
        std::ifstream bmp_file(file_path.string(), std::ios::in | std::ios::binary);
        
        std::string filename = file_path.filename().string();
        to_c_style_const(filename);
        std::uintmax_t file_size = entry.file_size();

        assets_h << "   ASSETS_BUNDLE_" 
            << filename << "_START = " << file_begin_from 
            << ", " << std::endl;
        assets_h << "   ASSETS_BUNDLE_" 
            << filename << "_END = " << file_begin_from + file_size 
            << ", ";
        assets_h << "// sizeof: 0x" << std::hex << file_size << std::dec << std::endl;
         
        assert(bmp_file.is_open()); // NOTE(annad): Can't open this file.
        file_begin_from += file_size;

        bundle << bmp_file.rdbuf();
        
        bmp_file.close();
    }

    assets_h << "}" << std::endl;
    assets_h << std::endl;

    return 0;
}

