#include "RayTracePipeline.h"

#include "PipelineLayout.h"
#include "RenderHardwareInterface.h"
#include "Shader.h"
#include "Util.h"
#include "PersistentBufferAllocator.h"
#include "RenderCore.h"
#include "RHI/RHIRayTracePipeline.h"


namespace Alpha
{
	RayTracePipeline::RayTracePipeline()
	{
		


	}

	RayTracePipeline::~RayTracePipeline()
	{
	}

	void RayTracePipeline::Initialize(const RayTracePipelineArgs& args)
	{
        RHIRayTracePipelineDesc desc = {};
        desc.global_layout = args.pipeline_layout->GetRHI();

        for (Shader* lib : args.libraries)
        {
            if (lib)
                desc.libraries.push_back(lib->GetRHI());
        }

        UInt32 shader_record_count = 0;
        desc.ray_gen_entry_name = args.ray_gen_entry_name;
		m_ray_gen_table.export_names.push_back(args.ray_gen_entry_name);
        shader_record_count++;

		desc.miss_entry_names = args.miss_entry_names;

        m_miss_table.export_names = args.miss_entry_names;

		shader_record_count += args.miss_entry_names.size();

        desc.hit_groups = args.hit_groups;
        m_hit_group_table.export_names.reserve(args.hit_groups.size());

        for (const auto& hg : args.hit_groups)
        {
            m_hit_group_table.export_names.push_back(hg.name);
        }
        shader_record_count += args.hit_groups.size();

        desc.max_payload_size = args.max_payload_size;
        desc.max_attribute_size = args.max_attribute_size;
        desc.max_recursion_depth = args.max_recursion_depth;

        m_rhi = args.rhi->CreateRayTracePipeline(desc);
        m_global_layout = args.pipeline_layout;

        static const size_t ShaderTableSize = 64;
        static const size_t ShaderIdSize = 32; // D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES

        // ちょっと多めに作っとく
        m_shader_table_allocator = CreatePersistentAllocator(RenderCore::GetPtr(), ShaderTableSize, ShaderTableSize * shader_record_count, EBufferPresetType::ROStructured, L"ShaderTable");

        /*
        * D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT
        * D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT
        * D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES
        * Bindlessでいくからローカルルートシグネチャは無し
        */

        auto CreateShaderTable = [&](ShaderTable* table)
        {
            UInt32 num = table->export_names.size();
            UInt32 size = ShaderIdSize * num;
            Allocation alloc = {};
            m_shader_table_allocator->AllocateBySize(size, &alloc);

            for (auto& export_name : table->export_names)
            {
                auto shader_id = m_rhi->GetShaderIdentifier(export_name);

                ALPHA_CHECK_ERROR((shader_id != nullptr), L"shader_idがありませんでした");

                table->stream.insert(table->stream.end(), (const uint8_t*)shader_id, (const uint8_t*)shader_id + ShaderIdSize);

            }

            RenderCore::Get().UploadBuffer(m_shader_table_allocator->GetBuffer(), alloc.offset, table->stream.data(), table->stream.size());


            table->alloc = alloc;
			table->count = num;
        };

        CreateShaderTable(&m_ray_gen_table);
        CreateShaderTable(&m_miss_table);
        CreateShaderTable(&m_hit_group_table);


	}

    RHIRayTracePipeline* RayTracePipeline::GetRHI() const
    {
        return m_rhi.get();
    }

    Buffer* RayTracePipeline::GetShaderTableBuffer() const
    {
        return m_shader_table_allocator->GetBuffer();
    }


}