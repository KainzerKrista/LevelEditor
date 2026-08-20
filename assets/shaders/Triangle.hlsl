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

VertextOutput VSMain(VertextInput input)
{
    VertextOutput output;
    
    output.position = float4(input.position, 1.0f);
    output.color = input.color;
    return output;
}

float4 PSMain(VertextOutput input) : SV_Target
{
    return input.color;
}