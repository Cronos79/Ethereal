cbuffer MVPBuffer : register(b0)
{
    float4x4 u_MVP;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.pos = mul(u_MVP, float4(input.position, 1.0f));
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    return output;
}