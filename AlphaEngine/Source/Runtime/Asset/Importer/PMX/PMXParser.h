#pragma once
#include "Core/IO/BinaryReader.h"
#include "PMXData.h"
#include "Renderer/Definitions.h"
#include "Asset/ModelHandle.h"

namespace Alpha
{
	// エンコード方式
	enum class PMXEncoding : uint8_t
	{
		UTF16 = 0,
		UTF8 = 1
	};

	enum class PMXIndexType
	{
		Vertex,
		Texture,
		Material,
		Bone,
		Morph,
		RigidBody
	};

	namespace Animation
	{
		class Skeleton;
	}

	class MaterialHandle;


	class PMXParser
	{
	private:
		BinaryReader m_reader{};
		String m_file_path{};
		PMXHeader m_header{};
		PMXModelInformation m_info{};
		PMXEncoding m_encoding{ PMXEncoding::UTF16 };			// 現在のモデルのエンコード方式

		std::vector<PMXVertex> m_vertices{};
		std::vector<PMXIndex> m_indices{};
		std::vector<PMXTexture> m_textures{};
		std::vector<PMXMaterial> m_materials{};
		std::vector<PMXBone> m_bones{};


	public:
		PMXParser(StringView file_name);
		// 
		void SetEncoding(PMXEncoding encoding);

		String ReadString();

		void Parse();

		bool IsSkeletal() const { return (m_bones.size() > 0); };

		String GetName() const { return m_info.name; }
		
		TArray<Renderer::StaticVertex> CreateStaticVertices() const;
		TArray<Renderer::SkeletalVertex> CreateSkeletalVertices() const;

		TArray<uint32_t> CreateIndices() const;

		TSharedArray<MaterialHandle> CreateMaterials() const;

		TArray<SubMeshHandle> CreateSubMeshes() const;

		TUnique<Animation::Skeleton> CreateSkeleton() const;

	private:

		void ParseVertices();

		void ParseIndices();

		void ParseTextureTable();

		void ParseMaterials();

		void ParseBones();

		int32_t ReadIndex(PMXIndexType type);

	};

}