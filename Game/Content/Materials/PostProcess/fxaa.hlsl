// 宣言
float2 TexColor;
float2 TexCoord;
flaot2 TexSize;

// 定義
#define FXAA_EDGE_THRESHOLD         1.f / 4.f
#define FXAA_EDGE_THRESHOLD_MIN     1.f / 16.f
#define FXAA_SUBPIX                 1
#define FXAA_SUBPIX_TRIM            1.f / 4.f
#define FXAA_SUBPIX_CAP             7.f / 8.f

// 関数の構造体
struct FxaaFunc
{
    // 輝度修正
    float FxaaLuma(float3 rgb)
    {
        return rgb.y * (0.587f / 0.299f) + rgb.x;
    }

    // ピクセルシフト
    float3 FxaaTextureOffset(float3 color, float2 pos, int2 offset)
    {
        return Texture2DSample(color, TexColorSampler, TexCoord + offset);
    }
};

FxaaFunc f;

// Local Contrast Check
float3 rgbN = f.FxaaTextureOffset(TexColor, TexCoord, int2( 0, -1));
float3 rgbW = f.FxaaTextureOffset(TexColor, TexCoord, int2(-1,  0)); 
float3 rgbM = f.FxaaTextureOffset(TexColor, TexCoord, int2( 0,  0));
float3 rgbE = f.FxaaTextureOffset(TexColor, TexCoord, int2( 1,  0));
float3 rgbS = f.FxaaTextureOffset(TexColor, TexCoord, int2( 0,  1));

float lumaN = f.FxaaLuma(rgbN);
float lumaW = f.FxaaLuma(rgbW);
float lumaM = f.FxaaLuma(rgbM);
float lumaE = f.FxaaLuma(rgbE);
float lumaS = f.FxaaLuma(rgbS);

float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
float range = rangeMax - rangeMin;
if ( range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD))
{
    // return FxaaFilterReturn(rgbM);
    return rgbM;
}

// Sub-pixel Aliasing Test
float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;
float rangeL = abs(lumaL - lumaM);
float blendL = max(0.0, (rangeL / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE;
blendL = min(FXAA_SUBPIX_CAP, blendL);

float3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;
float3 rgbNW = f.FxaaTextureOffset(TexColor, TexCoord, int2(-1, -1));
float3 rgbNE = f.FxaaTextureOffset(TexColor, TexCoord, int2( 1, -1));
float3 rgbSW = f.FxaaTextureOffset(TexColor, TexCoord, int2(-1,  1));
float3 rgbSE = f.FxaaTextureOffset(TexColor, TexCoord, int2( 1,  1));

float lumaNW = f.FxaaLuma(rgbNW);
float lumaNE = f.FxaaLuma(rgbNE);
float lumaSW = f.FxaaLuma(rgbSW);
float lumaSE = f.FxaaLuma(rgbSE);

rgbL += rgbNW + rgbNE + rgbSW + rgbSE;
float ODN = 1.f / 9.f;
rgbL *= float3(ODN, ODN, ODN);

// Vertical/Horizontal Edge Test
float edgeVert = 
        abs((0.25f * lumaNW) + (-0.5f * lumaN) + (0.25f * lumaNE)) +
        abs((0.25f * lumaW ) + (-0.5f * lumaM) + (0.25f * lumaE )) +
        abs((0.25f * lumaSW) + (-0.5f * lumaS) + (0.25f * lumaSE));

float edgeHorz = 
        abs((0.25f * lumaNW) + (-0.5f * lumaW) + (0.25f * lumaSW)) +
        abs((0.25f * lumaN ) + (-0.5f * lumaM) + (0.25f * lumaS )) +
        abs((0.25f * lumaNE) + (-0.5f * lumaE) + (0.25f * lumaSE));

bool horzSpan = edgeHorz >= edgeVert;

// End-of-edge Search
for(int i = 0; i < FXAA_SEARCH_STEPS; i++)
{
    #if FXAA_SEARCH_ACCELERATION == 1
}


float3 result = float3(Pos.x, Pos.y, 0);

return result;