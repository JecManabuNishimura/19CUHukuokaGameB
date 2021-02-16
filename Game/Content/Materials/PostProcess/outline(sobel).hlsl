// 定義
#define MAXDEPTH 5000
#define ID_CUSTOMDEPTH 13
#define ID_PPIN 14
#define Outline_Threhold
// 宣言
float2 InvSize = View.ViewSizeAndInvSize.zw;;
float2 UV = GetDefaultSceneTextureUV(Parameters, ID_CUSTOMDEPTH);

float3x3 Kernel_Martix =
(
-1, -1, -1,
-1,  8, -1,
-1, -1, -1
);

float3 OutlineColor = (1.f, 1.f, 1.f);

//float2 Depth_M  = mul(InvSize, int(0, 0));
float depthN  = clamp(SceneTextureLookup(UV + mul(InvSize, int( 0,  1)), ID_CUSTOMDEPTH, 0), 0, MAXDEPTH);
float depthS  = clamp(SceneTextureLookup(UV + mul(InvSize, int( 0, -1)), ID_CUSTOMDEPTH, 0), 0, MAXDEPTH);
float depthW  = clamp(SceneTextureLookup(UV + mul(InvSize, int(-1,  0)), ID_CUSTOMDEPTH, 0), 0, MAXDEPTH);
float depthE  = clamp(SceneTextureLookup(UV + mul(InvSize, int( 1,  0)), ID_CUSTOMDEPTH, 0), 0, MAXDEPTH);
float depthNW = clamp(SceneTextureLookup(UV + mul(InvSize, int(-1,  1)), ID_CUSTOMDEPTH, 0), 0, MAXDEPTH);
float depthNE = clamp(SceneTextureLookup(UV + mul(InvSize, int( 1,  1)), ID_CUSTOMDEPTH, 0), 0, MAXDEPTH);
float depthSW = clamp(SceneTextureLookup(UV + mul(InvSize, int(-1, -1)), ID_CUSTOMDEPTH, 0), 0, MAXDEPTH);
float depthSE = clamp(SceneTextureLookup(UV + mul(InvSize, int( 1, -1)), ID_CUSTOMDEPTH, 0), 0, MAXDEPTH);

float3 sobelN = float3(Depth_NW, Depth_N, Depth_NE);
float3 sobelM = float3(Depth_W,  Depth_M, Depth_E);
float3 sobelS = float3(Depth_SW, Depth_S, Depth_SE);

float weight = dot(sobelN, Kernel_Martix[0]) + dot(sobelM, Kernel_Martix[1]) + dot(sobelM, Kernel_Martix[2]);

