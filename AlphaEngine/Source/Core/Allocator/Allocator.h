#pragma once
#include "Core/Types.h"
/*
* 現在のallocatorはあくまでも割り当てを計算して返すクラス群でメモリを内包するものじゃありません
*/
namespace Alpha
{
    struct Allocation
    {
        UInt64 offset{ 0 };     // バイト単位
        UInt64 size{ 0 };       // バイト単位
        bool is_valid{ false };
    };

}