#include "Util.h"
#include "RenderCore.h"
#include "PersistentBufferAllocator.h"
#include "TransientBufferAllocator.h"
#include "Buffer.h"

namespace Alpha
{
    TUnique<PersistentBufferAllocator> CreatePersistentAllocator(RenderCore* render_core, size_t alignment, size_t max_size, EBufferPresetType preset, StringView name)
    {
        auto allocator = std::make_unique<PersistentBufferAllocator>();

        PersistentBufferAllocator::Args desc = {
            .render_core = render_core,
            .alignment = alignment,
            .max_size = max_size,
            .preset = preset,
        };

        allocator->Initialize(desc);

        allocator->SetName(name);

        return allocator;
    }

    TUnique<TransientBufferAllocator> CreateTransientAllocator(RenderCore* render_core, size_t alignment, size_t max_size, EBufferPresetType preset, StringView name)
    {
        auto allocator = std::make_unique<TransientBufferAllocator>();
        TransientBufferAllocator::Args desc = {
            .render_core = render_core,
            .block_num = 3,
            .stride = alignment,
            .size = max_size,
            .alignment = alignment,
            .type = EBufferPresetType::UploadStructured,
        };

        allocator->Initialize(desc);

        allocator->SetName(name);

        return TUnique<TransientBufferAllocator>();
    }

}
