/*
Author: github.com/annadostoevskaya
File: ./bmp.h
Date: 20/02/23 09:16:45

Description: <empty>
*/

#pragma pack(push, 1)
struct BMPHeader
{
    uint16_t type; // 2
    uint32_t size; // 6
    uint32_t RESERVED; // 10
    uint32_t offset; // 14 bytes
};

struct BMPInfo
{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes; // STUB
    uint16_t bitcount; // STUB
    uint32_t compression; // STUB
    uint32_t imgsize;
    int32_t xpxperMeter; // STUB
    int32_t ypxperMeter; // STUB
    uint32_t colorsUsed; // STUB
    uint32_t colorsImportant; // STUB // 40 
    uint32_t BGRA[1]; // STUB // 44 bytes
};
#pragma pack(pop)

#define BMP_DEFAULT_FILENAME_SIZE 128

struct BMP 
{
    char filename[BMP_DEFAULT_FILENAME_SIZE];
    BMPHeader   *header;
    BMPInfo     *info;
    uint32_t    *bitmap;
};
