struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : UV;
};

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = txDiffuse.Sample(samLinear, input.Tex);
    
    // 배경이 회색일 경우에 픽셀을 버린다.
    // 플루트 값은 정확히 측정이 안되서 0.01오차 내로 줄였습니다. 
    // 더 나은 방법 있으면 수정좀...
    if (textureColor.r >= 0.49f && textureColor.r <= 0.51f && textureColor.g >= 0.49f && textureColor.g <= 0.51f &&textureColor.b >= 0.49f && textureColor.b <= 0.51f)
    {
        discard;
    }
    
    return textureColor;

}