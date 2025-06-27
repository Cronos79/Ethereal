cbuffer lightBuffer : register(b0)
{
    float3 lightDirection; // Should be normalized and point *toward* the surface
    float padding; // Padding to align float4 boundary
    float3 ambientLightColor;
    float ambientLightStrength;
};

cbuffer CameraCB : register(b1)
{
    matrix viewProj;
    float3 cameraPosition;
    float padding0;
    float3 viewDirection;
    float padding1;
};

cbuffer alphaBuffer : register(b2)
{
    float alpha;
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

static const float PI = 3.14159265f;

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampledNormal = normalMap.Sample(objSamplerState, input.inTexCoord).rgb;
    sampledNormal = normalize(sampledNormal * 2.0f - 1.0f); // Convert from [0,1] to [-1,1]
    
    float3 N = normalize(input.inNormal);
    float3 T = normalize(input.inTangent);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);
    float3 worldNormal = normalize(mul(sampledNormal, TBN));
    
    // === Sample maps ===
    float3 baseColor = objTexture.Sample(objSamplerState, input.inTexCoord).rgb;
    float3 emissive = emissiveMap.Sample(objSamplerState, input.inTexCoord).rgb;
    float3 ao = aoMap.Sample(objSamplerState, input.inTexCoord).rgb;
    float metallic = metallicMap.Sample(objSamplerState, input.inTexCoord).r;
    metallic += 0.1f; // Ensure metallic is not zero to avoid black specular highlights
    metallic = clamp(metallic, 0.1f, 1.0f); // Ensure metallic is in [0, 1]
    float roughness = roughnessMap.Sample(objSamplerState, input.inTexCoord).r;
    float specularFactor = specularMap.Sample(objSamplerState, input.inTexCoord).r;

    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);

    float3 ambientLight = ambientLightColor * ambientLightStrength;

    float3 appliedLight = ambientLight;

    float3 vectorToLight = normalize(lightDirection - input.inWorldPos);

    float3 diffuseLightIntensity = max(dot(vectorToLight, input.inNormal), 0);

    //float NdotL = saturate(dot(worldNormal, -lightDirection));
    float3 diffuseLight = diffuseLightIntensity * (1.0f - metallic); // * dynamicLightStrength * dynamicLightColor;

    appliedLight += diffuseLight;

    float3 finalColor = sampleColor * appliedLight + emissive;

    return float4(finalColor, 1.0f);
}