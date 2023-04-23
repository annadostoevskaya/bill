/*
Author: github.com/annadostoevskaya
File: bill_software_renderer.h
Date: 04/04/23 23:28:52

Description: <empty>
*/

struct ARGB
{
    U8 a, r, g, b;
};

struct BilinearSample
{

    union {
        struct {
            U32 A, B, C, D;
        };
        

        struct {
            U32 topLeft;
            U32 topRight;
            U32 bottomLeft;
            U32 bottomRight;
        };
    };

};

struct HTexture
{
    U16 w;
    U16 h;
    U32 *bitmap;
};


U32 textureGetPixel(HTexture *texture, V2DF32 pos);
void textureRender(Screen *screen, HTexture *texture, V2DF32 pos, V2DF32 vscale);

