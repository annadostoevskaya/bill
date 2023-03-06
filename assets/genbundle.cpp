/*
Author: github.com/annadostoevskaya
File: genbundle.cpp
Date: 20/02/23 09:13:13

Description: Simple assets.bundle generator for bill.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cassert>
#include <cctype>

void to_c_style_const(std::string &s)
{
    for (char& c : s)
    {
        if (c == '.') c = '_';
        c = std::toupper(c);
    }
}

int main(int argc, char **argv)
{
    std::ofstream bundle("./../build/assets.bundle", std::ios::out | std::ios::binary | std::ios::ate);
    std::ofstream assets_h("./../assets.h", std::ios::out);
    assert(bundle.is_open() && assets_h.is_open());
    
    assets_h << "/*\n";
    assets_h << "    This file generated with 'genbundle' program!\n";
    assets_h << "    See ./assets/README.txt for more information.\n";
    assets_h << "*/\n";
    assets_h << '\n';
    assets_h << "enum ASSETS_BUNDLE\n";
    assets_h << "{\n";
    std::uintmax_t file_begin_from = 0;
    for (const auto &entry : std::filesystem::directory_iterator("./bmp"))
    {
        std::filesystem::path file_path = entry.path();
        std::ifstream bmp_file(file_path.string(), std::ios::in | std::ios::binary);
        
        std::string filename = file_path.filename().string();
        to_c_style_const(filename);
        std::uintmax_t file_size = entry.file_size();
        
        assets_h << "   ASSETS_BUNDLE_" << filename << " = " << file_begin_from 
            << ", // sizeof: 0x" << std::hex << file_size << std::dec << '\n';
        
        assert(bmp_file.is_open()); // NOTE(annad): Can't open this file.
        file_begin_from += file_size;
        
        bundle << bmp_file.rdbuf();
        std::cout << "[GENBUNDLE] (" << file_size / 1024 << "KB) " << file_path.filename().string() << '\n';

        bmp_file.close();
    }

    assets_h << "};\n\n";

    return 0;
}

