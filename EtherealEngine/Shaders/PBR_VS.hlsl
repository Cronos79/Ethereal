cbuffer PerObjectCB : register(b0)
{
    matrix worldViewProj;
};

cbuffer CameraCB : register(b1)
{
    matrix view;
    matrix projection;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNormal : NORMAL;
    float2 inTexCoord : TEXCOORD;
    float3 inTangent : TANGENT; // for normal mapping
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outTangent : TANGENT;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), worldViewProj);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = input.inNormal;
    output.outTangent = input.inTangent;
    return output;
}