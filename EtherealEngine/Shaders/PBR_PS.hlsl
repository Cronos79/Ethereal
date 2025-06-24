cbuffer alphaBuffer : register(b0)
{
    float alpha;
};

Texture2D objTexture : register(t0); // Diffuse
Texture2D normalMap : register(t1); // Normal map
SamplerState objSamplerState : register(s0);

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 normalTex = normalMap.Sample(objSamplerState, input.inTexCoord).rgb;
    normalTex = normalize(normalTex * 2.0f - 1.0f); // Convert from [0,1] to [-1,1]

    float3 diffuseColor = objTexture.Sample(objSamplerState, input.inTexCoord).rgb;
    return float4(diffuseColor, alpha);
}