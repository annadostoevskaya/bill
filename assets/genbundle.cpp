/*
Author: github.com/annadostoevskaya
File: genbundle.cpp
Date: 20/02/23 09:13:13

Description: Simple assets.bundle generator for bill.
*/
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "bmp.h"
#include "scandir.h"

uint32_t getfilesize(FILE *fp)
{
    fseek(fp, 0L, SEEK_END);
    uint32_t fsz = (uint32_t)ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return fsz;
}

BMP loadbmp(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    uint32_t fsz = getfilesize(fp);
    void *buf = malloc(fsz);
    fread(buf, fsz, 1, fp);
    fclose(fp);

    BMP bmp = {};
    assert(strlen(filename) < BMP_DEFAULT_FILENAME_SIZE); // Error: Too long filename
    strcpy((char*)bmp.filename, filename);
    bmp.header = (BMPHeader*)buf;
    bmp.info = (BMPInfo*)((uint8_t*)buf + sizeof(BMPHeader));
    bmp.bitmap = (uint32_t*)((uint8_t*)buf + bmp.header->offset);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmultichar"
    assert(bmp.header->type == 'BM'); // Error: Filetype identificate failed.
#pragma clang diagnostic pop
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

#define PrintInfo(STR, ...) printf("[genbundle] " STR "\n" __VA_OPT__(,) __VA_ARGS__)

struct Chain
{
    struct Chain *next;
    void *data;
};

Chain *chainNext(Chain *c, size_t dataSize)
{
    Chain *last = c;
    for (;;)
    {
        if (last->next) 
        {
            last = last->next;
        }
    }
    
    last->next = (Chain*)malloc(sizeof(Chain));
    last->next->data = malloc(dataSize);
    return last;
}

Chain chainInit(size_t dataSize)
{
    Chain c;
    c.data = malloc(dataSize);
    return c;
}

int main(int argc, char **argv)
{
    // BMP bmp = loadbmp("./bmp/test.bmp");
    // displaybmp(&bmp);
    // unloadbmp(&bmp);
    Chain bmpChain = chainInit(32);
    
    return 0;
}

