/*
Author: github.com/annadostoevskaya
File: bill_software_renderer.cpp
Date: 04/04/23 23:27:25

Description: <empty>
*/

// TODO(annad): ??...
#include <immintrin.h>

U32 __pxlerp(U32 A, U32 B, F32 t)
{
    U8 alpha = (U8)((F32)(A >> 24 & 0xff) * t + (F32)(B >> 24 & 0xff) * (1.0f - t));
    U8 red = (U8)((F32)(A >> 16 & 0xff) * t + (F32)(B >> 16 & 0xff) * (1.0f - t));
    U8 green = (U8)((F32)(A >> 8 & 0xff) * t + (F32)(B >> 8 & 0xff) * (1.0f - t));
    U8 blue = (U8)((F32)(A & 0xff) * t + (F32)(B & 0xff) * (1.0f - t));
    return alpha << 24 | red << 16 | green << 8 | blue;
}

U32 textureGetPixel(HTexture *texture, V2DF32 pos)
{
    if ((U32)pos.x < texture->w && (U32)pos.y < texture->h)
    {
        // IMPORTANT(annad): Source code http://fastcpp.blogspot.com/2011/06/bilinear-pixel-interpolation-using-sse.html
        // NOTE(annad): pointer to first pixel
        U32 *p0 = texture->bitmap + (U32)pos.x + (U32)pos.y * texture->w;
        // NOTE(annad): load the data, 2px in one load
        __m128i p12 = _mm_loadl_epi64((const __m128i*)&p0[0*texture->w]);
        __m128i p34 = _mm_loadl_epi64((const __m128i*)&p0[1*texture->w]);
        // NOTE(annad): calc weight
        __m128 ssx = _mm_set_ss(pos.x);
        __m128 ssy = _mm_set_ss(pos.y);
        __m128 psXY = _mm_unpacklo_ps(ssx, ssy); // x y 0 0
        __m128 psXYfloor = _mm_floor_ps(psXY); // SSE4! x y 0 0
        __m128 psXYfrac = _mm_sub_ps(psXY, psXYfloor); // x y 0 0 
        __m128 psXYfrac1 = _mm_sub_ps(_mm_set1_ps(1), psXYfrac); // 1-x 1-y 0 0
        __m128 w_x = _mm_unpacklo_ps(psXYfrac1, psXYfrac); // x1 x y1 y
               w_x = _mm_movelh_ps(w_x, w_x); // x1 x x1 x
        __m128 w_y = _mm_shuffle_ps(psXYfrac1, psXYfrac, _MM_SHUFFLE(1, 1, 1, 1)); // y1 y y1 y
        __m128 weight = _mm_mul_ps(w_x, w_y); // x1*y1 x*y x1*y1 x*y
        // NOTE(annad): convert RGBA RGBA RGBA RGAB to RRRR GGGG BBBB AAAA (AoS to SoA)
        __m128i p1234 = _mm_unpacklo_epi8(p12, p34);
        __m128i p34xx = _mm_unpackhi_epi64(p1234, _mm_setzero_si128());
        __m128i p1234_8bit = _mm_unpacklo_epi8(p1234, p34xx);
        // NOTE(annad): extend to 16bit 
        __m128i pRG = _mm_unpacklo_epi8(p1234_8bit, _mm_setzero_si128());
        __m128i pBA = _mm_unpackhi_epi8(p1234_8bit, _mm_setzero_si128());
        // NOTE(annad): convert weights to integer
        weight = _mm_mul_ps(weight, _mm_set1_ps(256)); 
        __m128i weighti = _mm_cvtps_epi32(weight); // w4 w3 w2 w1
                weighti = _mm_packs_epi32(weighti, weighti); // 32->2x16bit
        // NOTE(annad): outRG = [w1*R1 + w2*R2 | w3*R3 + w4*R4 | w1*G1 + w2*G2 | w3*G3 + w4*G4]
        __m128i outRG = _mm_madd_epi16(pRG, weighti);
        // NOTE(annad): outBA = [w1*B1 + w2*B2 | w3*B3 + w4*B4 | w1*A1 + w2*A2 | w3*A3 + w4*A4]
        __m128i outBA = _mm_madd_epi16(pBA, weighti);
        // NOTE(annad): horizontal add that will produce the output values (in 32bit)
        __m128i out = _mm_hadd_epi32(outRG, outBA);
        // NOTE(annad): divide by 256
        out = _mm_srli_epi32(out, 8); 
        // NOTE(annad): convert 32bit->8bit
        out = _mm_packus_epi32(out, _mm_setzero_si128());
        out = _mm_packus_epi16(out, _mm_setzero_si128());
        return _mm_cvtsi128_si32(out);
    }
        
    return 0x0;
}

void renderTextureFast(Screen *screen, HTexture *texture, V2DF32 pos, V2DF32 vscale)
{
    V2DF32 whTexture = {(F32)texture->w, (F32)texture->h};
    for (U32 y = 0; y < screen->h; y += 1)
    {
        for (U32 x = 0; x < screen->w; x += 1)
        {
            V2DF32 UV = {(F32)x / (F32)screen->w, (F32)y / (F32)screen->h};
            V2DF32 vdenorm = (UV + pos) * vscale * whTexture - V2DF32{1.0f, 1.0f};
            U32 pixelA = textureGetPixel(texture, vdenorm);
            U32 pixelB = screen->buf[y*screen->w+x];
            // NOTE(annad): Blending x=a*t+b(1.0f-t);
            __m128i alpha = _mm_set1_epi32(pixelA >> 24 & 0xff);
            __m128i alpha1 = _mm_sub_epi32(_mm_set1_epi32(0xff), alpha);
            __m128i a = _mm_unpacklo_epi16(
                _mm_unpacklo_epi8(
                    _mm_set1_epi32(pixelA),
                    _mm_setzero_si128()
                ),

                _mm_setzero_si128()
            );

            __m128i b = _mm_unpacklo_epi16(
                _mm_unpacklo_epi8(
                    _mm_set1_epi32(pixelB),
                    _mm_setzero_si128()
                ),

                _mm_setzero_si128()
            );
            
            U32 out = _mm_cvtsi128_si32(
                _mm_packus_epi16(
                    _mm_packus_epi32(
                        _mm_srli_epi32(
                            _mm_add_epi32(
                                _mm_mul_epi32(a, alpha), 
                                _mm_mul_epi32(b, alpha1)
                            ),

                            8
                        ),
                        
                        _mm_setzero_si128()
                    ),
                    
                    _mm_setzero_si128()
                )
            );
            
            screen->buf[y*screen->w+x] = out;
        }
    }
}

