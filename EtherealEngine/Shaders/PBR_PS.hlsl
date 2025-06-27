cbuffer alphaBuffer : register(b0)
{
    float alpha;
};

cbuffer lightBuffer : register(b1)
{
    float3 lightDirection; // Should be normalized and point *toward* the surface
    float padding; // Padding to align float4 boundary
    float3 lightColor;
    float ambientStrength;
};

// PBR Texture Inputs
Texture2D objTexture : register(t0); // Diffuse (Albedo)
Texture2D normalMap : register(t1); // Normal
Texture2D specularMap : register(t2); // Specular
Texture2D metallicMap : register(t3); // Metallic
Texture2D roughnessMap : register(t4); // Roughness
Texture2D aoMap : register(t5); // Ambient Occlusion
Texture2D emissiveMap : register(t6); // Emissive

SamplerState objSamplerState : register(s0);

// Vertex Shader Output
struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inWorldPos : WORLD_POSITION;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    // === Sample Normal Map and Reconstruct TBN Space ===
    float3 sampledNormal = normalMap.Sample(objSamplerState, input.inTexCoord).rgb;
    sampledNormal = normalize(sampledNormal * 2.0f - 1.0f); // Convert from [0,1] to [-1,1]

    float3 N = normalize(input.inNormal);
    float3 T = normalize(input.inTangent);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);
    float3 worldNormal = normalize(mul(sampledNormal, TBN));

    // === Lighting Calculation ===
    float3 baseColor = objTexture.Sample(objSamplerState, input.inTexCoord).rgb;
    float3 emissive = emissiveMap.Sample(objSamplerState, input.inTexCoord).rgb;
    float3 ao = aoMap.Sample(objSamplerState, input.inTexCoord).rgb;
    float metallic = metallicMap.Sample(objSamplerState, input.inTexCoord).r;
    float roughness = roughnessMap.Sample(objSamplerState, input.inTexCoord).r;
    float specularFactor = specularMap.Sample(objSamplerState, input.inTexCoord).r;

    float3 ambient = ambientStrength * lightColor * ao;
    
    float NdotL = saturate(dot(worldNormal, -lightDirection));
    float3 diffuse = NdotL * lightColor * (1.0f - metallic);

    // Very basic specular approximation
    float3 specular = lightColor * specularFactor * (1.0f - roughness);

    float3 lighting = ambient + diffuse + specular;
    float3 finalColor = baseColor * lighting + emissive;

    //return float4(finalColor, alpha);
    return float4(worldNormal * 0.5f + 0.5f, 1.0f);
}