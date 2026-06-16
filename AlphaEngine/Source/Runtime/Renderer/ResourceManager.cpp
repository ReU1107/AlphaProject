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
		m_base_position_pool = RenderCore::Get().CreateBuffer(sizeof(Vector3), args.base_position_pool_size, EBufferPresetType::ROByteAddress);
		m_base_normal_pool = RenderCore::Get().CreateBuffer(sizeof(Vector3), args.base_normal_pool_size, EBufferPresetType::ROByteAddress);
		m_texcoord_pool = RenderCore::Get().CreateBuffer(sizeof(Vector2), args.texcoord_pool_size, EBufferPresetType::ROByteAddress);
		
		m_bone_indices_pool = RenderCore::Get().CreateBuffer(sizeof(UVector4), args.bone_indices_pool_size, EBufferPresetType::ROByteAddress);
		m_bone_weights_pool = RenderCore::Get().CreateBuffer(sizeof(Vector4), args.bone_weights_pool_size, EBufferPresetType::ROByteAddress);

		m_bone_matrices_pool = RenderCore::Get().CreateBuffer(sizeof(Matrix4x4), args.bone_matrices_pool_size, EBufferPresetType::ROByteAddress);

		m_deformed_position_pool = RenderCore::Get().CreateBuffer(sizeof(Vector3), args.deformed_position_pool_size, EBufferPresetType::RWByteAddress);
		m_deformed_normal_pool = RenderCore::Get().CreateBuffer(sizeof(Vector3), args.deformed_normal_pool_size, EBufferPresetType::RWByteAddress);

		m_index_pool = RenderCore::Get().CreateBuffer(sizeof(UInt32), args.index_pool_size, EBufferPresetType::ROByteAddress);

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
			ALPHA_LOG_WARN(L"モデルハンドル(ID: {})はスケルタルメッシュではありません。", handle->GetID());
			// エラーを伝える
			return nullptr;
		}

		auto mesh = std::make_shared<SkeletalMesh>();

		auto render_core = RenderCore::GetPtr();

		RenderCore::Get().UploadBuffer(m_base_position_pool.get(), handle->GetPositions().data(), );

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