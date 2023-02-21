/*
Author: github.com/annadostoevskaya
File: scandir.h
Date: 21/02/23 09:59:37

Description: After use returned result of
    scandir, i mean filelist, you must 
    free memory with `free` from stdlib, ok?
*/
 
char *scandir(const char *dir, const char *targetExt);

