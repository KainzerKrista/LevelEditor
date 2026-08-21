struct VertextInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VertextOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer TransformBuffer : register(b0)
{
    column_major float4x4 model;
    column_major float4x4 view;
    column_major float4x4 projection;
};

VertextOutput VSMain(VertextInput input)
{
    VertextOutput output;
    
    float4 localPosition = float4(input.position, 1.0f);
    float4 worldPosition = mul(model, localPosition);
    float4 viewPosition = mul(view, worldPosition);
    
    output.position = mul(projection, viewPosition);
    output.color = input.color;
    return output;
}

float4 PSMain(VertextOutput input) : SV_Target
{
    return input.color;
}