#include "ResourceManager.h"
#include "Asset/ModelHandle.h"
#include "Asset/MaterialHandle.h"
#include "Asset/TextureHandle.h"
#include "Asset/AssetManager.h"

#include "Resource/StaticMesh.h"
#include "Resource/SkeletalMesh.h"
#include "Resource/Material.h"
#include "Resource/Texture2D.h"

#include "RenderCore/Module.h"

namespace Alpha
{
	using namespace Renderer;

	Renderer::ResourceManager::~ResourceManager() = default;

	bool ResourceManager::Initialize(const Args& args)
	{
		return true;
	}

	void ResourceManager::Finalize()
	{
	}

	TShared<Renderer::SkeletalMesh> ResourceManager::GetSkeletalMesh(TShared<ModelHandle> handle)
	{
		auto found = m_mesh_cache.find(handle->GetID());
		if (found != m_mesh_cache.cend())
			return found->second;

		if (!handle->IsSkeletal())
		{
			// エラーを伝える
			return nullptr;
		}

		auto mesh = std::make_shared<SkeletalMesh>();

		auto render_core = RenderCore::GetPtr();

		auto& vertices = handle->GetSkeletalVeretices();
		mesh->SetVertexBuffer(render_core->CreateVertexBuffer(sizeof(SkeletalVertex), vertices.size(), vertices.data()));

		auto& indices = handle->GetIndices();

		mesh->SetIndexBuffer(render_core->CreateIndexBuffer(sizeof(uint32_t), indices.size(), indices.data()));

		auto& materials = handle->GetMaterialHandles();
		for (auto& sub_handle : handle->GetSubMeshHandles())
		{
			SubMesh sub_mesh = {};
			sub_mesh.SetIndexOffset(sub_handle.index_offset);
			sub_mesh.SetIndexCount(sub_handle.index_count);

			auto material_index = sub_handle.material_index;

			sub_mesh.SetMaterial(GetMaterial(materials[material_index]));

			mesh->AddSubMesh(sub_mesh);
		}

		mesh->SetVertexTotalCount((uint32_t)vertices.size());
		mesh->SetIndexTotalCount((uint32_t)indices.size());

		m_mesh_cache[handle->GetID()] = mesh;

		return mesh;
	}

	TShared<Material> Renderer::ResourceManager::GetMaterial(TShared<MaterialHandle> handle)
	{
		auto found = m_material_cache.find(handle->GetID());
		if (found != m_material_cache.cend())
			return found->second;

		auto material = std::make_shared<Material>();

		auto& data = handle->GetData();
		material->SetData(data);

		material->SetConstantBuffer(RenderCore::GetPtr()->CreateConstantBuffer(sizeof(MaterialData), 1, &data));

		for (auto& texture_handle : handle->GetTexture())
		{
			TextureBinding bind = {
				.usage = ETextureUsage::Albedo,
				.asset = GetTexture(texture_handle)
			};
			material->AddTextureBinding(bind);
		}

		auto table = RenderCore::GetPtr()->CreateDescriptorTable(
			RenderCore::GetPtr()->GetBindlessHeap(),
			[&](DescriptorTable& layout)
			{
				for (auto& bind : material->GetTextureBindings())
				{
					layout.AddSRV(bind.asset.get()->GetResource());
				}
			}
		);

		material->SetDescriptorTable(std::move(table));

		m_material_cache[handle->GetID()] = material;

		return material;
	}
	
	TShared<Texture2D> Renderer::ResourceManager::GetTexture(TShared<TextureHandle> handle)
	{
		auto found = m_texture_cache.find(handle->GetID());
		if (found != m_texture_cache.cend())
			return found->second;

		auto texture = std::make_shared<Texture2D>();
		auto pixels = handle->GetPixels();

		texture->SetResource(RenderCore::GetPtr()->CreateTexture(handle->GetWidth(), handle->GetHeight(), handle->GetFormat(), pixels));

		m_texture_cache[handle->GetID()] = texture;

		return texture;
	}
}