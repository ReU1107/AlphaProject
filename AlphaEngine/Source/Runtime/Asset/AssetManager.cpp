#include "AssetManager.h"


#include "Importer/PMX/PMXLoader.h"
#include "Importer/VMD/VMDLoader.h"
#include "Importer/WIC/WICLoader.h"

#include "TextureHandle.h"
#include "MaterialHandle.h"
#include "MaterialTableHandle.h"
#include "ModelHandle.h"

#include "Animation/Resource/Skeleton.h"

#include "Core/Math/GeometryUtil.h"


namespace Alpha
{
	static String BuildInWhiteTexture = L"Alpha://White";
	static String BuildInBlackTexture = L"Alpha://Black";
	static String BuildInNormalTexture = L"Alpha://Normal";
	static String BuildInMaterial = L"Alpha://Material";
	static String BuildInCube = L"Alpha://Cube";
	static String BuildInSphere = L"Alpha://Sphere";

	bool AssetManager::Initialize(const Args& args)
	{
		// 新しくローダーを作成したら追加してください↓

		RegisterLoader(L".pmx", new PMXLoader());
		RegisterLoader(L".vmd", new VMDLoader());

		auto wic = new WICLoader();

		RegisterLoader(L".png", wic);
		RegisterLoader(L".tga", wic);
		RegisterLoader(L".jpg", wic);
		RegisterLoader(L".bmp", wic);
		RegisterLoader(L".jpeg", wic);

		// BuildIn用のハンドル生成
		{
			auto texture = std::make_shared<TextureHandle>();
			texture->SetName(BuildInWhiteTexture);
			texture->SetWidth(1);
			texture->SetHeight(1);
			uint32_t white = 0xFFFFFFFF;
			texture->SetPixels(reinterpret_cast<uint8_t*>(&white));
			RegisterAsset(BuildInWhiteTexture, texture);
		}

		{
			auto texture = std::make_shared<TextureHandle>();
			texture->SetName(BuildInNormalTexture);
			texture->SetWidth(1);
			texture->SetHeight(1);
			uint32_t normal = 0xFFFF8080;
			texture->SetPixels(reinterpret_cast<uint8_t*>(&normal));
			RegisterAsset(BuildInNormalTexture, texture);
		}

		{
			auto texture = std::make_shared<TextureHandle>();
			texture->SetName(BuildInBlackTexture);
			texture->SetWidth(1);
			texture->SetHeight(1);
			uint32_t black = 0;
			texture->SetPixels(reinterpret_cast<uint8_t*>(&black));
			RegisterAsset(BuildInBlackTexture, texture);
		}

		{
			auto material = std::make_shared<MaterialHandle>();
			material->SetName(BuildInMaterial);

			RegisterAsset(BuildInMaterial, material);
		}

		{
			auto cube = std::make_shared<ModelHandle>();
			cube->SetName(BuildInCube);
			TArray<Vector3> positions;
			TArray<Vector3> normals;
			TArray<Vector2> texcoords;
			TArray<UInt32> indices;
			CreateUnitCube(&positions, &normals, &texcoords, &indices);

			cube->SetBaseVeretices(std::move(positions), std::move(normals), std::move(texcoords));

			cube->SetIndices(std::move(indices));

			RegisterAsset(BuildInCube, cube);
		}
		{

		}

		return true;
	}

	void AssetManager::Finalize()
	{
		for (auto& loader : m_loaders)
		{

		}
	}

	void AssetManager::RegisterLoader(StringView extension, AssetLoader* loader)
	{
		if (m_loaders.count(extension))
			return;

		m_loaders.insert({ extension,loader });
	}

	void AssetManager::RegisterAsset(StringView name, TShared<AssetHandle> handle)
	{
		auto found = m_assets.find(name);
		if (found != m_assets.cend())
			return;

		m_assets.insert({ name,handle });
	}

	String AssetManager::GetWhiteTexturePath() const
	{
		return BuildInWhiteTexture;
	}

	TShared<TextureHandle> AssetManager::GetWhiteTexture() const
	{
		return std::static_pointer_cast<TextureHandle>(m_assets.at(BuildInWhiteTexture));
	}

	TShared<TextureHandle> AssetManager::GetBlackTexture() const
	{
		return std::static_pointer_cast<TextureHandle>(m_assets.at(BuildInBlackTexture));
	}

	TShared<TextureHandle> AssetManager::GetNormalTexture() const
	{
		return std::static_pointer_cast<TextureHandle>(m_assets.at(BuildInNormalTexture));
	}

	TShared<AssetHandle> AssetManager::Load(StringView file_path)
	{
		auto found = m_assets.find(file_path);
		if (found != m_assets.cend())
			return found->second;

		std::filesystem::path path(file_path.c_str());

		auto loader = m_loaders.find(path.extension().wstring().c_str());
		if (loader != m_loaders.cend())
		{
			auto handle = loader->second->AsyncLoad(file_path);
			m_assets.insert({ file_path,handle });
			return handle;
		}

		return nullptr;
	}
}