#pragma once
#include "Core/Common.h"
#include "Types.h"

namespace Alpha
{
    class RenderCore;
    class PersistentBufferAllocator;
    class TransientBufferAllocator;

    TUnique<PersistentBufferAllocator> CreatePersistentAllocator(RenderCore* render_core, size_t alignment, size_t max_size, EBufferPresetType preset, StringView name);
    TUnique<TransientBufferAllocator> CreateTransientAllocator(RenderCore* render_core, size_t alignment, size_t max_size, EBufferPresetType preset, StringView name);


}