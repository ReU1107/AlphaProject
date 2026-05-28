
struct Particle
{
    float3 position;
    float life_time;
    
    float3 direction;
    float max_life_time;

    float4 color;
        
};

struct EmitInfo
{
    float4 color;

    float life_time;
    uint max_num;
};

static const uint OFFSET_COUNT = 0; // 最初のカウンタの位置
static const uint OFFSET_DATA_START = 4; // データが始まる位置
static const uint ELEMENT_SIZE = 4; // それぞれのデータのサイズ
