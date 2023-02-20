/*
Author: github.com/annadostoevskaya
File: genbundle.cpp
Date: 20/02/23 09:13:13

Description: Simple assets.bundle generator for bill.
*/
#define _CRT_SECURE_NO_WARNINGS 1
#include "../core/base.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

U32 getfilesize(FILE *fp)
{
    fseek(fp, 0L, SEEK_END);
    U32 fsz = (U32)ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return fsz;
}

BMP loadbmp(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    U32 fsz = getfilesize(fp);
    void *buf = malloc(fsz);
    fread(buf, fsz, 1, fp);
    fclose(fp);

    BMP bmp = {};
    Assert(strlen(filename) < BMP_DEFAULT_FILENAME_SIZE);
    strcpy((char*)bmp.filename, filename);
    bmp.header = (BMPHeader*)buf;
    bmp.info = (BMPInfo*)((U8*)buf + sizeof(BMPHeader));
    bmp.bitmap = (U32*)((U8*)buf + bmp.header->offset);
    Assert(bmp.header->type == 'BM');
    return bmp;
}

void unloadbmp(BMP *bmp)
{
    free((void*)bmp->header);
}

void displaybmp(BMP *bmp)
{
    printf("name: %s\n\
size: %d bytes\n\
width: %d\n\
height: %d\n\
imgsize: %d bytes\n\
pixels: %d\n", bmp->filename, 
        bmp->header->size, 
        bmp->info->width, bmp->info->height,
        bmp->info->imgsize, bmp->info->imgsize/(bmp->info->bitcount / 8));
}

int main(int argc, char **argv)
{
    BMP bmp = loadbmp("./bmp/test.bmp");
    displaybmp(&bmp);
    unloadbmp(&bmp);
    return 0;
}

