#pragma once
#include "Core/Common.h"
#include "Core/Container/SlotPalette.h"

#include "PipelineLayout.h"
#include "RayTracePipeline.h"
#include "GraphicsPipeline.h"
#include "ComputePipeline.h"

namespace Alpha
{
	class GraphicsPipeline;
	class ComputePipeline;
	class RayTracePipeline;
	class PipelineLayout;

    class PipelineCache
    {
    private:
        TSlotPalette<TUnique<PipelineLayout>> m_layout_cache{};

        TSlotPalette<TUnique<GraphicsPipeline>> m_gpso_cache{};
        TSlotPalette<TUnique<ComputePipeline>> m_cpso_cache{};
        TSlotPalette<TUnique<RayTracePipeline>> m_rtpso_cache{};

    public:
        ~PipelineCache();
        struct Args
        {
        };

        void Initialize(const Args& args);



        PipelineLayout* FindLayout(UInt64 handle_id) { return FindInternal(m_layout_cache, handle_id); }
        GraphicsPipeline* FindGraphics(UInt64 handle_id) { return FindInternal(m_gpso_cache, handle_id); }
        ComputePipeline* FindCompute(UInt64 handle_id) { return FindInternal(m_cpso_cache, handle_id); }
        RayTracePipeline* FindRayTrace(UInt64 handle_id) { return FindInternal(m_rtpso_cache, handle_id); }

        PipelineLayout* Add(UInt64 handle_id, TUnique<PipelineLayout>&& layout) { return AddInternal(m_layout_cache, handle_id, std::move(layout)); }
        GraphicsPipeline* Add(UInt64 handle_id, TUnique<GraphicsPipeline>&& pipeline) { return AddInternal(m_gpso_cache, handle_id, std::move(pipeline)); }
        ComputePipeline* Add(UInt64 handle_id, TUnique<ComputePipeline>&& pipeline) { return AddInternal(m_cpso_cache, handle_id, std::move(pipeline)); }
        RayTracePipeline* Add(UInt64 handle_id, TUnique<RayTracePipeline>&& pipeline) { return AddInternal(m_rtpso_cache, handle_id, std::move(pipeline)); }

    private:
        template<typename Type>
        Type* FindInternal(TSlotPalette<TUnique<Type>>& palette, UInt64 handle_id)
        {
            UInt32 palette_index = 0;
            if (palette.TryGetIndex(handle_id, &palette_index))
            {
                return palette[palette_index].get();
            }
            return nullptr;
        }

        template<typename Type>
        Type* AddInternal(TSlotPalette<TUnique<Type>>& palette, UInt64 handle_id, TUnique<Type>&& resource)
        {
            UInt32 palette_index = 0;
            auto& slot = palette.Allocate(handle_id, palette_index);
            slot = std::move(resource);
            return slot.get();
        }

    };

}