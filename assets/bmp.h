/*
Author: github.com/annadostoevskaya
File: ./bmp.h
Date: 20/02/23 09:16:45

Description: <empty>
*/

#pragma pack(push, 1)
struct BMPHeader
{
    U16 type; // 2
    U32 size; // 6
    U32 RESERVED; // 10
    U32 offset; // 14 bytes
};

struct BMPInfo
{
    U32 size;
    S32 width;
    S32 height;
    U16 planes; // STUB
    U16 bitcount; // STUB
    U32 compression; // STUB
    U32 imgsize;
    S32 xpxperMeter; // STUB
    S32 ypxperMeter; // STUB
    U32 colorsUsed; // STUB
    U32 colorsImportant; // STUB // 40 
    U32 BGRA[1]; // STUB // 44 bytes
};
#pragma pack(pop)

#define BMP_DEFAULT_FILENAME_SIZE 128

struct BMP 
{
    U8 filename[BMP_DEFAULT_FILENAME_SIZE];
    BMPHeader   *header;
    BMPInfo     *info;
    U32         *bitmap;
};
