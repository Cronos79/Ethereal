cbuffer alphaBuffer : register(b0)
{
    float alpha;
};

cbuffer lightBuffer : register(b1)
{
    float3 lightDirection; // Should be normalized
    float padding; // Padding to align float4
    float3 lightColor;
    float ambientStrength;
};

Texture2D objTexture : register(t0); // Diffuse map
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
    // Sample normal map and convert to [-1,1]
    float3 sampledNormal = normalMap.Sample(objSamplerState, input.inTexCoord).rgb;
    sampledNormal = normalize(sampledNormal * 2.0f - 1.0f);

    // For now assume tangent is orthogonal and cross product gives binormal
    float3 N = normalize(input.inNormal);
    float3 T = normalize(input.inTangent);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);
    float3 worldNormal = normalize(mul(sampledNormal, TBN)); // Convert to world space

    // Diffuse lighting
    float NdotL = saturate(dot(worldNormal, -lightDirection)); // Negative because lightDirection points *at* light
    float3 diffuse = NdotL * lightColor;

    // Ambient + Diffuse lighting
    float3 ambient = ambientStrength * lightColor;
    float3 lighting = ambient + diffuse;

    float3 baseColor = objTexture.Sample(objSamplerState, input.inTexCoord).rgb;
    float3 finalColor = baseColor * lighting;

    return float4(finalColor, alpha);
}