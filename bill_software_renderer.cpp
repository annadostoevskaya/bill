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
            
            V2DF32 vecdenorm = (UV - pos) * vscale * whTexture;
            U32 out = textureGetPixel(texture, V2DU32{
                (U32)vecdenorm.x, (U32)vecdenorm.y
            });

            screen->buf[y*screen->w+x] = out;
        }
    }
}


