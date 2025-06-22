cbuffer mycBuffer : register(b0)
{
    float4x4 mat;
};

struct VS_INPUT
{
    float3 inPos : POSITION;
    float3 inNormal : NORMAL;
    float2 inTexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    // Optionally pass normal to pixel shader if needed:
    // float3 outNormal : NORMAL;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.outPosition = mul(float4(input.inPos, 1.0f), mat);
    output.outTexCoord = input.inTexCoord;
    // output.outNormal = input.inNormal; // If needed in pixel shader
    return output;
}

//cbuffer mycBuffer : register(b0)
//{
//    float4x4 mat;
//};

//struct VS_INPUT
//{
//    float3 inPos : POSITION;
//    float2 inTexCoord : TEXCOORD;
//};

//struct VS_OUTPUT
//{
//    float4 outPosition : SV_POSITION;
//    float2 outTexCoord : TEXCOORD;
//};

//VS_OUTPUT main( VS_INPUT input ) 
//{
//    VS_OUTPUT output;
//    output.outPosition = mul(float4(input.inPos, 1.0f), mat);
//    output.outTexCoord = input.inTexCoord;
//    return output;
//}