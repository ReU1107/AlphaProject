struct LineVertex
{
    float3 position : POSITION;
    float4 color    : COLOR;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct FrameRenderContext
{
    float4x4 view;
    float4x4 view_proj;
    float4x4 proj_view;
    float4 camera_pos;
    float4 frustum_planes;

    uint instance_num;
    uint light_count;

};

ConstantBuffer<FrameRenderContext> frame : register(b0);


VSOutput main(in LineVertex input)
{
    float4 position = mul(frame.view_proj, float4(input.position, 1.0f));
    
    VSOutput output;
    output.position = position;
    output.color = input.color;
    
    return output;
}