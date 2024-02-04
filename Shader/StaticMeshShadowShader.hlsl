
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : UV; // 버텍스 쉐이더의 시멘틱은 의미있는 이름으로 짓는 게 좋다.
    float3 Norm : NORMAL;
    float3 TangentWorld : TANGENT;
};

struct PS_INPUT
{
    float4 ProjPos : SV_POSITION;
    float4 WorldPos : POSITION;
    float2 Tex : UV;
    float3 Norm : NORMAL;
    float3 TangentWorld : TANGENT;
    
    float3 mDiffuse : TEXCOORD1;
    float3 mViewDir : TEXCOORD2;
    float3 mReflection : TEXCOORD3;
};

cbuffer TransformW : register(b2)
{
    matrix World;
}

cbuffer TransformVP : register(b3)
{
    matrix View;
    matrix Projection;
    matrix ShadowView;
    matrix ShadowProjection;
}

cbuffer MatrixPalette : register(b5)
{
    matrix MatrixPaletteArray[128];
}

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    // 오브젝트 월드 변환
    float4 pos = input.Pos;
    pos = mul(pos, World);
    output.WorldPos = pos;
    
    // ndc 공간 내의 위치
    pos = mul(pos, ShadowView);
    pos = mul(pos, ShadowProjection);
    output.ProjPos = pos;
    
    return output;
}

// Pixel Shader(PS) 프로그래밍
float4 PS(PS_INPUT input) : SV_Target1
{
    return float4(1.f, 1.f, 1.f, 1.f);
}