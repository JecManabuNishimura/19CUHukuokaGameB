// 宣言
// float2 InTex;
// float2 InUV;
// float2 TexelSize;

// 定義
#define FXAA_EDGE_THRESHOLD             1.f / 8.f
#define FXAA_EDGE_THRESHOLD_MIN         1.f / 16.f
#define FXAA_SUBPIX                     1
#define FXAA_SUBPIX_TRIM                1.f / 4.f
#define FXAA_SUBPIX_CAP                 7.f / 8.f
#define FXAA_SUBPIX_TRIM_SCALE          1.f
#define FXAA_SUBPIX_QUALITY             3.f / 4.f
#define FXAA_QUALITY                    1.5
#define FXAA_SEARCH_STEPS_INT           12

// 関数の構造体
struct FxaaFunc
{
    // 輝度修正
    float FxaaLuma(float3 rgb)
    {
        // return 0.299 * rgb.x + rgb.y * 0.587f  + rgb.z * 0.114;
        return rgb.y * (0.587f / 0.299f) + rgb.x;
    }

    // ピクセルシフト
    float3 FxaaTextureOffset(Texture2D tex, float2 pos, int2 offset)
    {
        float2 offsetTexel = offset * TexelSize;
        return Texture2DSample(tex, InTexSampler, pos + offsetTexel).rgb;
    }
};

FxaaFunc f;

// Local Contrast Check
float3 rgbN = f.FxaaTextureOffset(InTex, InUV, int2(0, -1));
float3 rgbW = f.FxaaTextureOffset(InTex, InUV, int2(-1, 0));
float3 rgbM = f.FxaaTextureOffset(InTex, InUV, int2(0, 0));
float3 rgbE = f.FxaaTextureOffset(InTex, InUV, int2(1, 0));
float3 rgbS = f.FxaaTextureOffset(InTex, InUV, int2(0, 1));

float lumaN = f.FxaaLuma(rgbN);
float lumaW = f.FxaaLuma(rgbW);
float lumaM = f.FxaaLuma(rgbM);
float lumaE = f.FxaaLuma(rgbE);
float lumaS = f.FxaaLuma(rgbS);
float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25f;

float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
float range = rangeMax - rangeMin;
if (range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD))
{
    // return FxaaFilterReturn(rgbM);
    return rgbM;
}

// Sub-pixel Aliasing Test
float rangeL = abs(lumaL - lumaM);
float blendL = max(0.0, (rangeL / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE;
blendL = min(FXAA_SUBPIX_CAP, blendL);

float3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;
float3 rgbNW = f.FxaaTextureOffset(InTex, InUV, int2(-1, -1));
float3 rgbNE = f.FxaaTextureOffset(InTex, InUV, int2(1, -1));
float3 rgbSW = f.FxaaTextureOffset(InTex, InUV, int2(-1, 1));
float3 rgbSE = f.FxaaTextureOffset(InTex, InUV, int2(1, 1));

float lumaNW = f.FxaaLuma(rgbNW);
float lumaNE = f.FxaaLuma(rgbNE);
float lumaSW = f.FxaaLuma(rgbSW);
float lumaSE = f.FxaaLuma(rgbSE);
float lumaC = (lumaNW + lumaNE + lumaSW + lumaSE) * 0.25f;

rgbL += rgbNW + rgbNE + rgbSW + rgbSE;
float ODN = 1.f / 9.f;
rgbL *= float3(ODN, ODN, ODN);

// Vertical/Horizontal Edge Test
float edgeVert =
abs((0.25f * lumaNW) + (-0.5f * lumaN) + (0.25f * lumaNE)) +
abs((0.25f * lumaW) + (-0.5f * lumaM) + (0.25f * lumaE)) +
abs((0.25f * lumaSW) + (-0.5f * lumaS) + (0.25f * lumaSE));

float edgeHorz =
abs((0.25f * lumaNW) + (-0.5f * lumaW) + (0.25f * lumaSW)) +
abs((0.25f * lumaN) + (-0.5f * lumaM) + (0.25f * lumaS)) +
abs((0.25f * lumaNE) + (-0.5f * lumaE) + (0.25f * lumaSE));

bool horzSpan = edgeHorz >= edgeVert;

float stepLength = horzSpan ? TexelSize.y : TexelSize.x;
//
//float luma1 = horzSpan ? lumaS : lumaW;
//float luma2 = horzSpan ? lumaN : lumaE;
//float gradient1 = luma1 - lumaM;
//float gradient2 = luma2 - lumaM;
//
//bool is1Steepest = abs(gradient1) >= abs(gradient2);
//
//float gradientScaled = 0.25f * max(abs(gradient1), abs(gradient2));
//
//float lumaLocalAverage = 0.f;
//if (is1Steepest)
//{
//    stepLength = -stepLength;
//    lumaLocalAverage = (luma1 + lumaM) / 2.f;
//}
//else
//{
//    lumaLocalAverage = (luma2 + lumaM) / 2.f;
//}

float2 currentUV = InUV;
if (horzSpan)
{
    currentUV.y += stepLength * 0.5f;
}
else
{
    currentUV.x += stepLength * 0.5f;
}

// End-of-edge Search
float2 offset = horzSpan ? float2(TexelSize.x, 0) : float2(0, TexelSize.y);
float2 uvN = currentUV - offset * FXAA_QUALITY;
float2 uvP = currentUV + offset * FXAA_QUALITY;
float3 rgbEndN = f.FxaaTextureOffset(InTex, InUV, -offset);
float3 rgbEndP = f.FxaaTextureOffset(InTex, InUV, offset);
float lumaEndN = f.FxaaLuma(rgbEndN);
float lumaEndP = f.FxaaLuma(rgbEndP);
//lumaEndN -= lumaLocalAverage;
//lumaEndP -= lumaLocalAverage;

bool doneN = false, doneP = false;
float gradientN = lumaL;

for (int i = 0; i < FXAA_SEARCH_STEPS_INT; i++)
{
    if (!doneN) lumaEndN = f.FxaaLuma(Texture2DSample(InTex, InTexSampler, uvN).rgb);
    if (!doneP) lumaEndP = f.FxaaLuma(Texture2DSample(InTex, InTexSampler, uvP).rgb);
    //if (!doneN) lumaEndN = f.FxaaLuma(Texture2DSample(InTex, InTexSampler, uvN).rgb) - lumaLocalAverage;
    //if (!doneP) lumaEndP = f.FxaaLuma(Texture2DSample(InTex, InTexSampler, uvP).rgb) - lumaLocalAverage;

    doneN = doneN || abs(lumaEndN - lumaN) >= gradientN;
    doneP = doneP || abs(lumaEndP - lumaN) >= gradientN;

    if (doneN && doneP) break;
    if (!doneN)  uvN -= offset * FXAA_QUALITY;
    if (!doneP)  uvP += offset * FXAA_QUALITY;
}

float3 finalColor = (Texture2DSample(InTex, InTexSampler, uvN).rgb + Texture2DSample(InTex, InTexSampler, uvP).rgb) * 0.5f;

// reduce the aliasing
//float distance1 = horzSpan ? (InUV.x - uvN.x) : (InUV.y - uvN.y);
//float distance2 = horzSpan ? (uvN.x - InUV.x) : (uvN.y - InUV.y);
//
//bool isDirection1 = distance1 < distance2;
//float distanceFinal = min(distance1, distance2);
//
//float edgeThickness = distance1 + distance2;
//
//bool isLumaMSmaller = lumaM < lumaLocalAverage;
//bool correctVariationN = (lumaEndN < 0.0) != isLumaMSmaller;
//bool correctVariationP = (lumaEndP < 0.0) != isLumaMSmaller;
//bool correctVariation = isDirection1 ? correctVariationN : correctVariationP;
//
//float pixelOffset = -distanceFinal / edgeThickness + 0.5f;
//float finalOffset = correctVariation ? pixelOffset : 0.f;
//
//float lumaAverage = ODN * (lumaL + lumaC) * 4.f;
//float subPixelOffset1 = clamp(abs(lumaAverage - lumaM) / range, 0.f, 1.f);
//float subPixelOffset2 = (-2.f * subPixelOffset1 + 3.f) * subPixelOffset1 * subPixelOffset1;
//float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * FXAA_SUBPIX_QUALITY;
//
//finalOffset = max(finalOffset, subPixelOffsetFinal);
//
//float2 finalUV = InUV;
//if (horzSpan)
//{
//    finalUV.y += finalOffset * TexelSize;
//}
//else
//{
//    finalUV.x += finalOffset * TexelSize;
//}
//
//float3 finalColor = Texture2DSample(InTex, InTexSampler, finalUV).rgb;

return finalColor;