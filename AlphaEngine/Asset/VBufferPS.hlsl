struct PixelInput
{
    float4 position : SV_Position;
    uint primitive_id : SV_PrimitiveID;
    
    nointerpolation uint instance_id : CV_Instance_ID;
    nointerpolation uint meshlet_id : CV_Meshlet_ID;

};

struct PSOutput
{
    uint2 visibility : SV_Target0;
    
};

PSOutput main(PixelInput input)
{
    PSOutput output = (PSOutput) 0;
    output.visibility.r = input.instance_id;
    output.visibility.g = (input.meshlet_id << 16) | input.primitive_id;
    //output.visibility.a = 1.0f;
    
    
    
    return output;
}