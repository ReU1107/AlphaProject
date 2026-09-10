#pragma once
#include "Core/Common.h"
#include "Core/Container/SlotPalette.h"

#include "RHI/Types.h"

#include <span>

namespace Alpha
{
	class Shader;

    class ShaderCache
    {
    private:
        TSlotPalette<TUnique<Shader>> m_shader_palette{};

    public:
        ~ShaderCache();

        struct Args
        {
        };

        void Initialize(const Args& args);

        Shader* Find(StringView file_path, ERHIShaderType type, StringView entry_point);
        Shader* Find(StringView file_path, std::span<const StringView> entry_points);

        Shader* Add(StringView file_path, TUnique<Shader>&& shader);

	};
}