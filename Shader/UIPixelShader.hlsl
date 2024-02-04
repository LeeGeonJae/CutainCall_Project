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
    
    // ����� ȸ���� ��쿡 �ȼ��� ������.
    // �÷�Ʈ ���� ��Ȯ�� ������ �ȵǼ� 0.01���� ���� �ٿ����ϴ�. 
    // �� ���� ��� ������ ������...
    if (textureColor.r >= 0.49f && textureColor.r <= 0.51f && textureColor.g >= 0.49f && textureColor.g <= 0.51f &&textureColor.b >= 0.49f && textureColor.b <= 0.51f)
    {
        discard;
    }
    
    return textureColor;

}