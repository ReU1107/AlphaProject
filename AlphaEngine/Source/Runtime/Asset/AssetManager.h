#pragma once
#include "Core/Types.h"
#include "Core/Subsystem.h"
#include "AssetHandle.h"

namespace Alpha
{
	class AssetLoader;
	class TextureHandle;

	class AssetManager : public Subsystem<AssetManager>
	{
	private:
		TStringMap<AssetLoader*> m_loaders{};
		TStringMap<TShared<AssetHandle>> m_assets{};

	public:
		struct Args
		{
		};

		bool Initialize(const Args& args);
		void Finalize();

		void RegisterLoader(StringView extension, AssetLoader* loader);
		void RegisterAsset(StringView name, TShared<AssetHandle> handle);

		TShared<TextureHandle> GetWhiteTexture() const;
		TShared<TextureHandle> GetBlackTexture() const;
		TShared<TextureHandle> GetNormalTexture() const;

		TShared<AssetHandle> Load(StringView file_path);

		template<IsAsset T>
		TShared<T> Load(StringView file_path)
		{
			auto base_handle = Load(file_path);

			if (base_handle && base_handle->GetType() == T::Type())
				return std::static_pointer_cast<T>(base_handle);
			return nullptr;
		}

	};


}