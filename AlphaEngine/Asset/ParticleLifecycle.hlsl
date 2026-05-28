#include "Particle.hlsli"

RWStructuredBuffer<Particle> particles : register(u0);
RWByteAddressBuffer dead_list : register(u1);

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint idx = DTid.x;
    // 一旦定数にしておく後で渡せるようにする
    if (idx >= 2048)
        return;

    Particle p = particles[idx];

    // 生きている粒子のみ処理
    if (p.life_time > 0.0f)
    {
        p.life_time -= 1.0f / 60;   // 後でdeltatimeを渡すように

        // 死んだ瞬間
        if (p.life_time <= 0.0f)
        {
            p.life_time = -1.0f; // 無効化
            
            // DeadListにインデックスを返却
            uint write_pos;
            dead_list.InterlockedAdd(OFFSET_COUNT, 1, write_pos);
            dead_list.Store(OFFSET_DATA_START + ELEMENT_SIZE * write_pos, idx);
        }
        particles[idx] = p;
    }
}