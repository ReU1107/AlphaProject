#pragma once
#include "Core/Common.h"
#include "Core/Allocator/FreeListAllocator.h"
#include "RenderCore/Types.h"

namespace Alpha
{
	class Buffer;

    class PersistentBufferAllocator
    {
    private:
        TUnique<Buffer> m_buffer{ nullptr };
        FreeListAllocator m_allocator{};

    public:
        bool Initialize(UInt64 stride, UInt64 total_size, EBufferPresetType preset);

        Buffer* GetBuffer() const { return m_buffer.get(); }
    };
}


//#pragma once
//
//namespace Alpha
//{
//    // メッシュが持つ、プール内の住所録
//    struct SkeletalMeshAllocation
//    {
//        PooledBuffer::Allocation position;
//        PooledBuffer::Allocation normal;
//        PooledBuffer::Allocation texcoord;
//        PooledBuffer::Allocation bone_indices;
//        PooledBuffer::Allocation bone_weights;
//        PooledBuffer::Allocation index;
//    };
//
//    class ResourceManager : public Subsystem<ResourceManager>
//    {
//    private:
//        TMap<UInt64, TShared<SkeletalMesh>> m_mesh_cache{};
//        TMap<UInt64, TShared<Material>>     m_material_cache{};
//        TMap<UInt64, TShared<Texture>>      m_texture_cache{};
//
//        // 各データ専用の管理プール
//        TUnique<PooledBuffer> m_base_position_pool{ nullptr };
//        TUnique<PooledBuffer> m_base_normal_pool{ nullptr };
//        TUnique<PooledBuffer> m_texcoord_pool{ nullptr };
//        TUnique<PooledBuffer> m_bone_indices_pool{ nullptr };
//        TUnique<PooledBuffer> m_bone_weights_pool{ nullptr };
//        TUnique<PooledBuffer> m_index_pool{ nullptr };
//
//        // スキニング用、DXR用のプール
//        TUnique<PooledBuffer> m_bone_matrices_pool{ nullptr };
//        TUnique<PooledBuffer> m_deformed_position_pool{ nullptr };
//        TUnique<PooledBuffer> m_deformed_normal_pool{ nullptr };
//
//    public:
//        struct Args
//        {
//            UInt64 base_position_pool_size{ 48_MB };
//            UInt64 base_normal_pool_size{ 48_MB };
//            UInt64 texcoord_pool_size{ 32_MB };
//            UInt64 bone_indices_pool_size{ 32_MB };
//            UInt64 bone_weights_pool_size{ 64_MB };
//            UInt64 bone_matrices_pool_size{ 2_MB };
//            UInt64 deformed_position_pool_size{ 48_MB };
//            UInt64 deformed_normal_pool_size{ 48_MB };
//            UInt64 index_pool_size{ 48_MB };
//        };
//
//        bool Initialize(const Args& args)
//        {
//            // すべてのプールを初期化
//            m_base_position_pool     = MakeUnique<PooledBuffer>();
//            m_base_normal_pool       = MakeUnique<PooledBuffer>();
//            m_texcoord_pool          = MakeUnique<PooledBuffer>();
//            m_bone_indices_pool      = MakeUnique<PooledBuffer>();
//            m_bone_weights_pool      = MakeUnique<PooledBuffer>();
//            m_index_pool             = MakeUnique<PooledBuffer>();
//            m_bone_matrices_pool     = MakeUnique<PooledBuffer>();
//            m_deformed_position_pool = MakeUnique<PooledBuffer>();
//            m_deformed_normal_pool   = MakeUnique<PooledBuffer>();
//
//            bool success = true;
//            success &= m_base_position_pool->Initialize(args.base_position_pool_size, EBufferPresetType::Default);
//            success &= m_base_normal_pool->Initialize(args.base_normal_pool_size, EBufferPresetType::Default);
//            success &= m_texcoord_pool->Initialize(args.texcoord_pool_size, EBufferPresetType::Default);
//            
//            // インデックスやボーン等、用途に応じてUAV等のプリセットがあれば変えてください
//            success &= m_bone_indices_pool->Initialize(args.bone_indices_pool_size, EBufferPresetType::Default);
//            success &= m_bone_weights_pool->Initialize(args.bone_weights_pool_size, EBufferPresetType::Default);
//            success &= m_index_pool->Initialize(args.index_pool_size, EBufferPresetType::Default);
//            
//            success &= m_bone_matrices_pool->Initialize(args.bone_matrices_pool_size, EBufferPresetType::Default);
//            
//            // deformed は ComputeShader で書き込むので UAV が必要
//            success &= m_deformed_position_pool->Initialize(args.deformed_position_pool_size, EBufferPresetType::RWStructured);
//            success &= m_deformed_normal_pool->Initialize(args.deformed_normal_pool_size, EBufferPresetType::RWStructured);
//
//            return success;
//        }
//
//        void Finalize()
//        {
//            m_mesh_cache.Clear();
//            m_material_cache.Clear();
//            m_texture_cache.Clear();
//        }
//
//        TShared<SkeletalMesh> GetSkeletalMesh(TShared<ModelHandle> handle)
//        {
//            UInt64 asset_id = handle->GetID();
//            if (m_mesh_cache.Contains(asset_id)) return m_mesh_cache[asset_id];
//
//            // 1. 各種データのサイズを計算 (PMXデータのゲッターは仮です、環境に合わせて調整してください)
//            UInt64 pos_bytes    = handle->GetPositions().size() * sizeof(float3);
//            UInt64 norm_bytes   = handle->GetNormals().size() * sizeof(float3);
//            UInt64 tex_bytes    = handle->GetTexCoords().size() * sizeof(float2);
//            UInt64 idx_bytes    = handle->GetIndices().size() * sizeof(uint32_t);
//            UInt64 b_idx_bytes  = handle->GetBoneIndices().size() * sizeof(uint16_t) * 4;
//            UInt64 b_wgt_bytes  = handle->GetBoneWeights().size() * sizeof(float4);
//
//            // 2. プールから領域を確保
//            SkeletalMeshAllocation alloc;
//            alloc.position     = m_base_position_pool->Allocate(pos_bytes, 16);
//            alloc.normal       = m_base_normal_pool->Allocate(norm_bytes, 16);
//            alloc.texcoord     = m_texcoord_pool->Allocate(tex_bytes, 8);
//            alloc.bone_indices = m_bone_indices_pool->Allocate(b_idx_bytes, 4);
//            alloc.bone_weights = m_bone_weights_pool->Allocate(b_wgt_bytes, 16);
//            alloc.index        = m_index_pool->Allocate(idx_bytes, 4);
//
//            // 確保失敗チェック
//            if (!alloc.position.is_valid || !alloc.index.is_valid /* ...他のチェックも必要なら */) {
//            // 失敗したら確保した分を返す
//            m_base_position_pool->Free(alloc.position);
//            m_index_pool->Free(alloc.index);
//            return nullptr;
//            }
//
//            // 3. 一時バッファ（TransientBuffer）等を使ってGPUプールへCopyBufferRegionする
//            auto cmd = RenderCore::Get().GetCommandList();
//            cmd->Begin(0);
//
//            auto native_list = static_cast<D3D12CommandBuffer*>(cmd->GetRHI())->GetNative();
//            auto transient = RenderCore::Get().GetTransientBuffer();
//
//            // 例: Position の転送
//            UInt64 t_pos_offset = transient->WriteData(handle->GetPositions().data(), pos_bytes);
//            native_list->CopyBufferRegion(m_base_position_pool->GetNative(), alloc.position.offset, transient->GetNative(), t_pos_offset, pos_bytes);
//
//            // 例: Index の転送
//            UInt64 t_idx_offset = transient->WriteData(handle->GetIndices().data(), idx_bytes);
//            native_list->CopyBufferRegion(m_index_pool->GetNative(), alloc.index.offset, transient->GetNative(), t_idx_offset, idx_bytes);
//
//            // ...他のデータ（Normal, TexCoord等）も同様にCopyBufferRegionする...
//
//            cmd->End();
//            RenderCore::Get().GetCommandQueue()->Execute(cmd);
//
//            // 4. メッシュを生成して住所録（Allocation）を渡す
//            auto mesh = MakeShared<SkeletalMesh>();
//            mesh->m_allocation = alloc;
//            mesh->m_index_count = handle->GetIndices().size();
//
//            m_mesh_cache[asset_id] = mesh;
//            return mesh;
//        }
//
//        // メッシュが不要になったときにプールを空ける関数
//        void FreeSkeletalMesh(const SkeletalMeshAllocation& alloc)
//        {
//            m_base_position_pool->Free(alloc.position);
//            m_base_normal_pool->Free(alloc.normal);
//            m_texcoord_pool->Free(alloc.texcoord);
//            m_bone_indices_pool->Free(alloc.bone_indices);
//            m_bone_weights_pool->Free(alloc.bone_weights);
//            m_index_pool->Free(alloc.index);
//        }
//    };
//}
