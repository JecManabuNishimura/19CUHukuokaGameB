// 定義
#define MAXDEPTH 5000
#define ID_CUSTOMDEPTH 13
#define ID_PPIN0 14
#define OUTLINE_THREHOLD 0.1
// 宣言
float2 InvSize = View.ViewSizeAndInvSize.zw;
//float2 InvSize = CustomDepthInvSize;
float2 UV = GetDefaultSceneTextureUV(Parameters, ID_CUSTOMDEPTH);

//float3x3 Kernel_Martix =
//(
//-1, -1, -1,
//-1,  8, -1,
//-1, -1, -1
//);

float3 Kernel_Martix_Top = float3(-1, -1, -1);
float3 Kernel_Martix_Mid = float3(-1,  8, -1);
float3 Kernel_Martix_Bot = float3(-1, -1, -1);

float3 OutlineColor = float3(1.f, 1.f, 1.f);

// offset NW -1, -1 -> SE 1, 1
float depthM  = clamp(SceneTextureLookup(UV, ID_CUSTOMDEPTH, false), 0, MAXDEPTH);
float depthN  = clamp(SceneTextureLookup(UV + InvSize * float2( 0, -1), ID_CUSTOMDEPTH, false), 0, MAXDEPTH);
float depthS  = clamp(SceneTextureLookup(UV + InvSize * float2( 0,  1), ID_CUSTOMDEPTH, false), 0, MAXDEPTH);
float depthW  = clamp(SceneTextureLookup(UV + InvSize * float2(-1,  0), ID_CUSTOMDEPTH, false), 0, MAXDEPTH);
float depthE  = clamp(SceneTextureLookup(UV + InvSize * float2( 1,  0), ID_CUSTOMDEPTH, false), 0, MAXDEPTH);
float depthNW = clamp(SceneTextureLookup(UV + InvSize * float2(-1, -1), ID_CUSTOMDEPTH, false), 0, MAXDEPTH);
float depthNE = clamp(SceneTextureLookup(UV + InvSize * float2( 1, -1), ID_CUSTOMDEPTH, false), 0, MAXDEPTH);
float depthSW = clamp(SceneTextureLookup(UV + InvSize * float2(-1,  1), ID_CUSTOMDEPTH, false), 0, MAXDEPTH);
float depthSE = clamp(SceneTextureLookup(UV + InvSize * float2( 1,  1), ID_CUSTOMDEPTH, false), 0, MAXDEPTH);

float3 sobelN = float3(depthNW, depthN, depthNE);
float3 sobelM = float3(depthW,  depthM, depthE);
float3 sobelS = float3(depthSW, depthS, depthSE);

float weight = dot(sobelN, Kernel_Martix_Top) + dot(sobelM, Kernel_Martix_Mid) + dot(sobelS, Kernel_Martix_Bot);

if (weight < OUTLINE_THREHOLD)  return PPIN0_Color;
else                            return OutlineColor;

