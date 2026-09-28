#pragma once
#include "Core/Common.h"
#include "Core/Math/AxisAlignedBoundingBox.h"
#include "Renderer/Definitions.h"

namespace Alpha
{
    class RenderCore;
    class TransientBufferAllocator;

}

namespace Alpha::Renderer
{
    class RenderScene
    {
    private:

        TUnique<TransientBufferAllocator> m_instance_allocator{ nullptr };
        TUnique<TransientBufferAllocator> m_vert_to_inst_allocator{ nullptr };

        TUnique<TransientBufferAllocator> m_instance_tlas_allocator{ nullptr };
        TUnique<TransientBufferAllocator> m_scratch_tlas_allocator{ nullptr };

        TUnique<TransientBufferAllocator> m_result_tlas_allocator{ nullptr };
        TUnique<TransientBufferAllocator> m_result_blas_allocator{ nullptr };

        TUnique<TransientBufferAllocator> m_dynamic_vertex_allocator{ nullptr };
        TUnique<TransientBufferAllocator> m_override_material_allocator{ nullptr };

        TUnique<TransientBufferAllocator> m_light_allocator{ nullptr };
        TUnique<TransientBufferAllocator> m_volume_allocator{ nullptr };
        TUnique<TransientBufferAllocator> m_procedural_primitive_allocator{ nullptr };

        TUnique<TransientBufferAllocator> m_matrix_palette_allocator{ nullptr };


        UInt32 m_instance_count{ 0 };
        UInt32 m_light_count{ 0 };
        UInt32 m_volume_count{ 0 };

    public:

        ~RenderScene();

        struct Args
        {
            RenderCore* render_core{ nullptr };
        };

        bool Initialize(const Args& args);
        void Finalize();

        void Reset(UInt32 frame_index);

        void WriteInstances(const InstanceData* data, UInt32 count);

        void WriteLights(const LightData* data, UInt32 count);

        void WriteVolumes(const VolumeMaterial* data, UInt32 count);

        void WriteProceduralPrimitives(const AxisAlignedBoundingBox* data, UInt32 count);

        DynamicBufferIndices GetBufferIndices() const;

        TransientBufferAllocator* GetDynamicVertexAllocator();
        TransientBufferAllocator* GetOverrideMaterialAllocator();
        TransientBufferAllocator* GetBLASAllocator();
        TransientBufferAllocator* GetTLASAllocator();
        TransientBufferAllocator* GetTLASScratchAllocator();
        TransientBufferAllocator* GetTLASInstanceAllocator();
        TransientBufferAllocator* GetVertToInstanceAllocator();

        TransientBufferAllocator* GetMatrixPaletteAllocator();


        UInt32 GetInstanceCount() const { return m_instance_count; }
        UInt32 GetLightCount() const { return m_light_count; }
        UInt32 GetVolumeCount() const { return m_volume_count; }
    };
}