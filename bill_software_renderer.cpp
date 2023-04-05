/*
Author: github.com/annadostoevskaya
File: bill_software_renderer.cpp
Date: 04/04/23 23:27:25

Description: <empty>
*/

U32 textureGetPixel(HTexture *texture, V2DU32 p)
{
    U32 out = 0x0;
    if (p.x < texture->w && p.y < texture->h)
    {
        out = texture->bitmap[p.y * texture->w + p.x];
    }
    
    return out;
}
#if 0
BilinearSample 
textureGetBilinearSample(HTexture *texture, V2DS32 vcell)
{
    BilinearSample sample = textureGetBilinearSample;
    sample.topLeft = textureGetPixel(texture, vcell);
    sample.topRight = textureGetPixel(texture, vcell + V2DS32{1, 0});
    sample.bottomLeft = textureGetPixel(texture, vcell + V2DS32{0, 1});
    sample.bottomRight = textureGetPixel(texture, vcell + V2DS32{1, 1});
    return sample;
}
#endif

U32 __pxlerp(U32 A, U32 B, F32 t)
{
    U8 alpha = (U8)((F32)(A >> 24 & 0xff) * t + (F32)(B >> 24 & 0xff) * (1.0f - t));
    U8 red = (U8)((F32)(A >> 16 & 0xff) * t + (F32)(B >> 16 & 0xff) * (1.0f - t));
    U8 green = (U8)((F32)(A >> 8 & 0xff) * t + (F32)(B >> 8 & 0xff) * (1.0f - t));
    U8 blue = (U8)((F32)(A & 0xff) * t + (F32)(B & 0xff) * (1.0f - t));
    return alpha << 24 | red << 16 | green << 8 | blue;
}

void renderTextureFast(Screen *screen, HTexture *texture, V2DF32 pos, V2DF32 vscale)
{
    V2DF32 whTexture = {(F32)texture->w, (F32)texture->h};
    for (U32 y = 0; y < screen->h; y += 1)
    {
        for (U32 x = 0; x < screen->w; x += 1)
        {
            V2DF32 UV = {
                (F32)x / (F32)screen->w,
                (F32)y / (F32)screen->h
            };
            
            V2DF32 vdenorm = (UV + pos) * vscale * whTexture;
            U32 out = textureGetPixel(texture, V2DU32{(U32)vdenorm.x, (U32)vdenorm.y});

            screen->buf[y*screen->w+x] = out;
        }
    }
}

void renderTexture(Screen *screen, HTexture *texture, V2DF32 pos, V2DF32 vscale)
{
    V2DF32 whTexture = {(F32)texture->w, (F32)texture->h};
    for (U32 y = 0; y < screen->h; y += 1)
    {
        for (U32 x = 0; x < screen->w; x += 1)
        {
            V2DF32 UV = {
                (F32)x / (F32)screen->w,
                (F32)y / (F32)screen->h
            };
            
            V2DF32 vdenorm = (UV + pos) * vscale * whTexture;
            vdenorm -= V2DF32{0.5f, 0.5f};
            V2DU32 vcell = {(U32)f32Floor(vdenorm.x), (U32)f32Floor(vdenorm.y)};
            V2DF32 voffset = {vdenorm.x - (F32)vcell.x, vdenorm.y - (F32)vcell.y};
            BilinearSample sample;
            sample.topLeft = textureGetPixel(texture, vcell);
            sample.topRight = textureGetPixel(texture, vcell + V2DU32{1, 0});
            sample.bottomLeft = textureGetPixel(texture, vcell + V2DU32{0, 1});
            sample.bottomRight = textureGetPixel(texture, vcell + V2DU32{1, 1});
            U32 topX = __pxlerp(sample.topRight, sample.topLeft, voffset.x);
            U32 botX = __pxlerp(sample.bottomRight, sample.bottomLeft, voffset.x);
            U32 out = __pxlerp(botX, topX, voffset.y);

            screen->buf[y*screen->w+x] = out;
        }
    }
}

void renderTextureSlow(Screen *screen, HTexture *texture, V2DF32 pos, V2DF32 vscale)
{
    V2DF32 whTexture = {(F32)texture->w, (F32)texture->h};
    for (U32 y = 0; y < screen->h; y += 1)
    {
        for (U32 x = 0; x < screen->w; x += 1)
        {
            V2DF32 UV = {
                (F32)x / (F32)screen->w,
                (F32)y / (F32)screen->h
            };
            
            V2DF32 vdenorm = (UV + pos) * vscale * whTexture;
            vdenorm -= V2DF32{0.5f, 0.5f};
            V2DU32 vcell = {(U32)f32Floor(vdenorm.x), (U32)f32Floor(vdenorm.y)};
            V2DF32 voffset = {vdenorm.x - (F32)vcell.x, vdenorm.y - (F32)vcell.y};
            BilinearSample sample;
            sample.topLeft = textureGetPixel(texture, vcell);
            sample.topRight = textureGetPixel(texture, vcell + V2DU32{1, 0});
            sample.bottomLeft = textureGetPixel(texture, vcell + V2DU32{0, 1});
            sample.bottomRight = textureGetPixel(texture, vcell + V2DU32{1, 1});
            U32 topX = __pxlerp(sample.topRight, sample.topLeft, voffset.x);
            U32 botX = __pxlerp(sample.bottomRight, sample.bottomLeft, voffset.x);
            U32 out = __pxlerp(botX, topX, voffset.y);

            screen->buf[y*screen->w+x] = out;
        }
    }
}

