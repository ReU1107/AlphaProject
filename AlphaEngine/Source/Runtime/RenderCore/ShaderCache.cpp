#include "ShaderCache.h"
#include "Shader.h"

#include "RenderCore.h"

#include "Core/Math/HashUtil.h"

namespace Alpha
{
	ShaderCache::~ShaderCache() = default;

	void ShaderCache::Initialize(const Args& args)
	{
        m_shader_palette.Initialize(100);
	}

    Shader* ShaderCache::Find(StringView file_path, ERHIShaderType type, StringView entry_point)
    {
        UInt64 handle_id = 0;

        HashCombine(handle_id, file_path);

        UInt32 palette_index = 0;

        if (m_shader_palette.TryGetIndex(handle_id, &palette_index))
        {
            return m_shader_palette[palette_index].get();
        }

        return nullptr;
    }

    Shader* ShaderCache::Find(StringView file_path, std::span<const StringView> entry_points)
    {
        UInt64 handle_id = 0;

        HashCombine(handle_id, file_path);

        UInt32 palette_index = 0;

        if (m_shader_palette.TryGetIndex(handle_id, &palette_index))
        {
            return m_shader_palette[palette_index].get();
        }

        return nullptr;
    }

    Shader* ShaderCache::Add(StringView file_path, TUnique<Shader>&& shader)
    {
        UInt64 handle_id = 0;

        HashCombine(handle_id, file_path);

        UInt32 palette_index = 0;

        auto& resource = m_shader_palette.Allocate(handle_id, palette_index);
        resource = std::move(shader);

        return resource.get();
    }

}