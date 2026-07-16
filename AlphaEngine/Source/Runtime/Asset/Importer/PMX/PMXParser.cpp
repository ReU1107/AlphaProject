#include "PMXParser.h"

#include "Animation/AnimationResource.h"

#include "Asset/AssetManager.h"
#include "Asset/MaterialHandle.h"
#include "Asset/MaterialTableHandle.h"
#include "Asset/TextureHandle.h"

#include "Core/Math/MathF.h"
#include "Core/Math/MathUtil.h"
#include "Core/String/StringUtil.h"

#include <ranges>

namespace
{
	using namespace Alpha;

	uint8_t GetIndexSize(const PMXHeader& header, PMXIndexType type)
	{
		return header.indexSizeTable[(uint8_t)type];
	}
}

namespace Alpha
{
	using namespace Animation;
	using namespace Renderer;

	PMXParser::PMXParser(StringView file_name)
	{
		m_reader.Open(file_name);

		m_reader.Read(m_header.signature, sizeof(m_header.signature));
		m_reader.Read(&m_header.version, sizeof(m_header.version));
		m_reader.Read(&m_header.indexSizeTableCount, sizeof(m_header.indexSizeTableCount));
		m_reader.Read(&m_header.encoding, sizeof(m_header.encoding));
		m_reader.Read(&m_header.additionalUVCount, sizeof(m_header.additionalUVCount));
		m_reader.Read(m_header.indexSizeTable, sizeof(m_header.indexSizeTable));

		SetEncoding(m_header.encoding == 0 ? PMXEncoding::UTF16 : PMXEncoding::UTF8);

		m_info.name = ReadString();
		m_info.nameEnglish = ReadString();
		m_info.comment = ReadString();
		m_info.commentEnglish = ReadString();

		m_file_path = file_name;

	}

	void PMXParser::SetEncoding(PMXEncoding encoding)
	{
		m_encoding = encoding;
	}

	String PMXParser::ReadString()
	{
		uint32_t length = m_reader.ReadUInt32();
		String result = {};
		if (m_encoding == PMXEncoding::UTF16)
		{
			std::u16string utf16String(length / 2, u'\0');
			m_reader.Read(&utf16String[0], length);

			result = StringUtil::ConvertU16ToString(utf16String);
		}
		else if (m_encoding == PMXEncoding::UTF8)
		{
			std::string utf8String(length, u'\0');
			m_reader.Read(&utf8String[0], length);
		}
		return result;
	}

	void PMXParser::Parse()
	{
		ParseVertices();
		ParseIndices();
		ParseTextureTable();
		ParseMaterials();

		ParseBones();

	}

	TArray<uint32_t> PMXParser::CreateIndices() const
	{
		TArray<uint32_t> indices(m_indices.size() * 3);

		for (const auto& [i, pmx] : m_indices | std::views::enumerate)
		{
			uint64_t index = i * 3;
			indices[index] = pmx.index[0];
			indices[index + 1] = pmx.index[1];
			indices[index + 2] = pmx.index[2];

		}

		return indices;
	}

	TShared<MaterialTableHandle> PMXParser::CreateMaterials() const
	{
		TShared<MaterialTableHandle> table = std::make_shared<MaterialTableHandle>();
		table->SetName(GetName() + L"MaterialTable");

		for (const auto& [i, pmx] : m_materials | std::views::enumerate)
		{
			auto material = std::make_shared<MaterialHandle>();
			material->SetName(GetName() + pmx.name);

			auto& data = material->GetProperties();
			data.base_color = pmx.diffuseCoeff.XYZ();
			data.opacity = pmx.diffuseCoeff.W();
			data.metallic = (pmx.sphereFlag == PMXSphereMode::Sph) ? 0.8f : 0.0f;
			data.roughness = 1.0f - Clamp(pmx.specularity / 100.0f, 0.0f, 1.0f);
			data.emissive_color = pmx.ambientCoeff.XYZ();
			data.emissive_intensity = (data.emissive_color.X() > 0.0f || data.emissive_color.Y() > 0.0f || data.emissive_color.Z() > 0.0f) ? 1.0f : 0.0f;
			data.specular_tint = pmx.specularCoeff.XYZ();
			data.occlusion = 1.0f;
			data.refraction_index = 1.5f;

			auto folder_path = StringUtil::GetFolderPath(m_file_path);

			if (pmx.decalIndex >= 0 && pmx.decalIndex < m_textures.size())
			{
				String texture_path = folder_path + m_textures[pmx.decalIndex].textureFileName;
				material->AddTexturePath(ETextureAttribute::BaseColor, texture_path);
			}

			// トーンは一旦なし
			//if (pmx.toonIndex > 0 && pmx.toonIndex < 255)
			//{
			//	if (pmx.toonIndex < m_textures.size())
			//	{
			//		auto toon_ref = AssetManager::Get().Load(folder_path + m_textures[pmx.decalIndex].textureFileName);

			//		material->GetTexture().push_back(std::static_pointer_cast<TextureHandle>(toon_ref));
			//	}
			//}
			table->AddMaterial(material);
		}

		return table;
	}

	TArray<SubMeshHandle> PMXParser::CreateSubMeshes() const
	{
		uint32_t location = 0;
		TArray<SubMeshHandle> sub_meshes(m_materials.size());
		for (const auto& [i, pmx] : m_materials | std::views::enumerate)
		{
			SubMeshHandle mesh = {};
			mesh.material_index = (uint32_t)i;
			mesh.index_offset = location;
			mesh.index_count = pmx.indexCount;
			location += pmx.indexCount;
			sub_meshes[i] = mesh;
		}

		return sub_meshes;
	}

	TUnique<Animation::Skeleton> PMXParser::CreateSkeleton() const
	{
		auto skeleton = std::make_unique<Skeleton>();
		// ジョイント作成
		for (auto& pmx : m_bones)
		{
			skeleton->AddJoint(pmx.name, std::make_unique<Joint>());
		}

		// 親子関係作成
		for (const auto& [i, pmx] : m_bones | std::views::enumerate)
		{

			Vector3 position = pmx.position;

			const int32_t parent_index = (int32_t)pmx.parentIndex;

			auto joint = skeleton->GetJoint(pmx.name);

			float3 absolute_position = pmx.position;
			float3 relative_position = absolute_position;
			// 親がいる
			if (parent_index != -1)
			{
				auto& parent_bone = m_bones[parent_index];

				auto parent_joint = skeleton->GetJoint(parent_bone.name);

				parent_joint->AddChild(joint);
				relative_position = absolute_position - parent_bone.position;
			}
			Transform transform = {};
			transform.position = relative_position;
			
			joint->SetBindPose(transform);
			joint->SetOffsetMatrix(Matrix4x4Inverse(Matrix4x4Translation(absolute_position)));

			if (EnumHasFlag(pmx.boneFlag, PMXBoneFlag::IK))
			{
				auto chain = std::make_unique<IKChain>();
				chain->SetTargetIndex(i);
				chain->SetEffectorIndex(pmx.ikTargetIndex);

				chain->SetMaxIterationCount(pmx.ikLoopCount);

				for (const auto& [j, pmx_link] : pmx.ikLink | std::views::enumerate)
				{
					IKLink link = {};
					link.index = pmx_link.linkIndex;
					uint32_t child_index = (j == 0) ? pmx.ikTargetIndex : pmx.ikLink[j - 1].linkIndex;
					link.next_index = child_index;
					link.dir = VectorNormalize(m_bones[child_index].position - m_bones[link.index].position);
					link.length = VectorLength(m_bones[child_index].position - m_bones[link.index].position);

					if (EnumHasFlag(pmx_link.limitFlag, PMXLimitFlag::ON))
					{
						auto& link_bone = m_bones[pmx_link.linkIndex];

						float y_range = std::abs(pmx_link.upperLimitAngle.Y() - pmx_link.lowerLimitAngle.Y());
						float z_range = std::abs(pmx_link.upperLimitAngle.Z() - pmx_link.lowerLimitAngle.Z());

						if (y_range < 0.001f && z_range < 0.001f)
						{
							link.constraint.mode = EConstraintMode::Hinge;
							link.constraint.rotation_axis = Vector3(1, 0, 0); // X軸固定
							link.constraint.min_angle = pmx_link.lowerLimitAngle.X();
							link.constraint.max_angle = pmx_link.upperLimitAngle.X();
						}

					}

					chain->AddLink(link);
				}

				skeleton->AddIKChain(std::move(chain));
			}

		}


		return skeleton;
	}

	void PMXParser::ParseVertices()
	{
		// 頂点数
		uint32_t vertex_num = 0;
		m_reader.Read(&vertex_num, sizeof(vertex_num));
		m_vertices.resize(vertex_num);

		for (auto& vertex : m_vertices)
		{
			m_reader.Read(&vertex.position, sizeof(vertex.position));
			m_reader.Read(&vertex.normal, sizeof(vertex.normal));
			m_reader.Read(&vertex.texcoord, sizeof(vertex.texcoord));

			for (uint8_t i = 0; i < m_header.additionalUVCount; ++i)
			{
				m_reader.Read(&vertex.addTexcoord[i], sizeof(vertex.addTexcoord[i]));
			}
			m_reader.Read(&vertex.type, sizeof(vertex.type));

			// ボーン情報
			switch (vertex.type)
			{
			case PMXWeightType::BDEF1:
				// ボーン1つのみ
				m_reader.Read(&vertex.boneIndex[0], GetIndexSize(m_header, PMXIndexType::Bone));
				vertex.boneWeight[0] = 1.0f;
				break;
			case PMXWeightType::BDEF2:
				// ボーン2つ,ボーン1のウェイト値
				m_reader.Read(&vertex.boneIndex[0], GetIndexSize(m_header, PMXIndexType::Bone));
				m_reader.Read(&vertex.boneIndex[1], GetIndexSize(m_header, PMXIndexType::Bone));

				m_reader.Read(&vertex.boneWeight[0], sizeof(vertex.boneWeight[0]));
				vertex.boneWeight[1] = 1.0f - vertex.boneWeight[0];
				break;
			case PMXWeightType::BDEF4:
				// ボーン4つ、ボーンそれぞれのウェイト値
				m_reader.Read(&vertex.boneIndex[0], GetIndexSize(m_header, PMXIndexType::Bone));
				m_reader.Read(&vertex.boneIndex[1], GetIndexSize(m_header, PMXIndexType::Bone));
				m_reader.Read(&vertex.boneIndex[2], GetIndexSize(m_header, PMXIndexType::Bone));
				m_reader.Read(&vertex.boneIndex[3], GetIndexSize(m_header, PMXIndexType::Bone));

				m_reader.Read(&vertex.boneWeight[0], sizeof(vertex.boneWeight[0]));
				m_reader.Read(&vertex.boneWeight[1], sizeof(vertex.boneWeight[1]));
				m_reader.Read(&vertex.boneWeight[2], sizeof(vertex.boneWeight[2]));
				m_reader.Read(&vertex.boneWeight[3], sizeof(vertex.boneWeight[3]));
				break;
			case PMXWeightType::SDEF:
				// BDEF2,SDEF用のfloat3(Vector3)が3つ
				m_reader.Read(&vertex.boneIndex[0], GetIndexSize(m_header, PMXIndexType::Bone));
				m_reader.Read(&vertex.boneIndex[1], GetIndexSize(m_header, PMXIndexType::Bone));

				m_reader.Read(&vertex.boneWeight[0], sizeof(vertex.boneWeight[0]));
				vertex.boneWeight[1] = 1.0f - vertex.boneWeight[0];

				m_reader.Read(&vertex.sdefC, sizeof(vertex.sdefC));
				m_reader.Read(&vertex.sdefR0, sizeof(vertex.sdefR0));
				m_reader.Read(&vertex.sdefR1, sizeof(vertex.sdefR1));
				break;
			default:
				break;
			}

			// エッジ倍率
			m_reader.Read(&vertex.edge, sizeof(vertex.edge));
		}


	}

	void PMXParser::ParseIndices()
	{
		uint32_t index_num = 0;
		m_reader.Read(&index_num, sizeof(index_num));
		m_indices.resize(index_num / 3);

		for (auto& index : m_indices)
		{
			m_reader.Read(&index.index[0], GetIndexSize(m_header, PMXIndexType::Vertex));
			m_reader.Read(&index.index[1], GetIndexSize(m_header, PMXIndexType::Vertex));
			m_reader.Read(&index.index[2], GetIndexSize(m_header, PMXIndexType::Vertex));
		}

	}

	void PMXParser::ParseTextureTable()
	{
		uint32_t texture_num = 0;
		m_reader.Read(&texture_num, sizeof(texture_num));
		m_textures.resize(texture_num);

		for (auto& texture : m_textures)
		{
			texture.textureFileName = ReadString();
		}
	}

	void PMXParser::ParseMaterials()
	{
		uint32_t material_num = 0;
		m_reader.Read(&material_num, sizeof(material_num));
		m_materials.resize(material_num);

		for (auto& material : m_materials)
		{
			material.name = ReadString();
			material.nameEnglish = ReadString();

			m_reader.Read(&material.diffuseCoeff, sizeof(material.diffuseCoeff));
			m_reader.Read(&material.specularCoeff, sizeof(material.specularCoeff));
			m_reader.Read(&material.specularity, sizeof(material.specularity));
			m_reader.Read(&material.ambientCoeff, sizeof(material.ambientCoeff));

			m_reader.Read(&material.renderFlag, sizeof(material.renderFlag));
			m_reader.Read(&material.edge, sizeof(material.edge));
			m_reader.Read(&material.edgeSize, sizeof(material.edgeSize));

			m_reader.Read(&material.decalIndex, GetIndexSize(m_header, PMXIndexType::Texture));
			m_reader.Read(&material.sphereIndex, GetIndexSize(m_header, PMXIndexType::Texture));

			m_reader.Read(&material.sphereFlag, sizeof(material.sphereFlag));
			m_reader.Read(&material.shareToonFlag, sizeof(material.shareToonFlag));

			if (material.shareToonFlag == PMXShareToonFlag::Individual)
			{
				m_reader.Read(&material.toonIndex, GetIndexSize(m_header, PMXIndexType::Texture));
			}
			else if (material.shareToonFlag == PMXShareToonFlag::Share)
			{
				m_reader.Read(&material.shareToonTexture, GetIndexSize(m_header, PMXIndexType::Texture));
			}
			material.memo = ReadString();

			m_reader.Read(&material.indexCount, sizeof(material.indexCount));
		}

	}

	void PMXParser::ParseBones()
	{
		uint32_t bone_num = 0;
		m_reader.Read(&bone_num, sizeof(bone_num));
		m_bones.resize(bone_num);

		for (auto& bone : m_bones)
		{
			bone.name = ReadString();
			bone.nameEnglish = ReadString();

			m_reader.Read(&bone.position, sizeof(bone.position));

			bone.parentIndex = ReadIndex(PMXIndexType::Bone);

			m_reader.Read(&bone.transformHierarchy, sizeof(bone.transformHierarchy));

			m_reader.Read(&bone.boneFlag, sizeof(bone.boneFlag));
			// 接続先1
			if (((uint16_t)bone.boneFlag & (uint16_t)PMXBoneFlag::AccessShowMode) == 0)
			{
				m_reader.Read(&bone.offsetPosition, sizeof(bone.offsetPosition));
			}
			else
			{
				bone.connectIndex = ReadIndex(PMXIndexType::Bone);
			}

			// 回転付与, 移動付与1
			if (((uint16_t)bone.boneFlag & (uint16_t)PMXBoneFlag::TranlateAppend) ||
				((uint16_t)bone.boneFlag & (uint16_t)PMXBoneFlag::RotateAppend))
			{
				bone.influenceTargetIndex = ReadIndex(PMXIndexType::Bone);
				m_reader.Read(&bone.influenceCoeff, sizeof(bone.influenceCoeff));
			}
			// 軸固定1
			if (((uint16_t)bone.boneFlag & (uint16_t)PMXBoneFlag::FixedAxis))
			{
				m_reader.Read(&bone.axisVector, sizeof(bone.axisVector));
			}

			// ローカル軸1
			if (((uint16_t)bone.boneFlag & (uint16_t)PMXBoneFlag::LocalAxis))
			{
				m_reader.Read(&bone.xAxis, sizeof(bone.xAxis));
				m_reader.Read(&bone.zAxis, sizeof(bone.zAxis));
			}

			// 外部親変形1
			if (((uint16_t)bone.boneFlag & (uint16_t)PMXBoneFlag::DeformOuterParent))
			{
				m_reader.Read(&bone.key, sizeof(bone.key));
			}

			// IK1
			if (((uint16_t)bone.boneFlag & (uint16_t)PMXBoneFlag::IK))
			{
				bone.ikTargetIndex = ReadIndex(PMXIndexType::Bone);

				m_reader.Read(&bone.ikLoopCount, sizeof(bone.ikLoopCount));
				m_reader.Read(&bone.ikLimitAngle, sizeof(bone.ikLimitAngle));
				m_reader.Read(&bone.ikLinkCount, sizeof(bone.ikLinkCount));

				bone.ikLink.resize(bone.ikLinkCount);

				for (auto& ikLink : bone.ikLink)
				{
					ikLink.linkIndex = ReadIndex(PMXIndexType::Bone);

					m_reader.Read(&ikLink.limitFlag, sizeof(ikLink.limitFlag));

					// IKリンク1
					if (((uint8_t)ikLink.limitFlag & (uint8_t)PMXLimitFlag::ON))
					{
						m_reader.Read(&ikLink.lowerLimitAngle, sizeof(ikLink.lowerLimitAngle));
						m_reader.Read(&ikLink.upperLimitAngle, sizeof(ikLink.upperLimitAngle));
					}
				}
			}
		}
	}

	int32_t PMXParser::ReadIndex(PMXIndexType type)
	{
		// ヘッダーから対応するIndexサイズを取得 (1, 2, 4)
		uint8_t size = GetIndexSize(m_header, type);

		if (size == 1)
		{
			int8_t val = 0;
			m_reader.Read(&val, 1);
			return (val == -1) ? -1 : (int32_t)(uint8_t)val;
		}
		if (size == 2)
		{
			int16_t val = 0;
			m_reader.Read(&val, 2);
			return (val == -1) ? -1 : (int32_t)(uint16_t)val;
		}
		if (size == 4) 
		{
			int32_t val = 0;
			m_reader.Read(&val, 4);
			return val;
		}
		return -1;
	}


	//std::vector<PMXMorph> PMXParseer::ParseMorphs(PMXBinaryReader& reader)
	//{
	//	uint32_t morphCount;
	//	reader.Read(&morphCount, sizeof(morphCount));
	//	std::vector<PMXMorph> morphs(morphCount);

	//	for (auto& morph : morphs)
	//	{
	//		morph.name = std::move(reader.ReadString());
	//		morph.nameEnglish = std::move(reader.ReadString());

	//		reader.Read(&morph.operation, sizeof(morph.operation));
	//		reader.Read(&morph.type, sizeof(morph.type));
	//		reader.Read(&morph.offsetCount, sizeof(morph.offsetCount));

	//		if (morph.type == PMXMorphType::Vertex)
	//		{
	//			morph.vertexData.resize(morph.offsetCount);
	//			for (auto& vertex : morph.vertexData)
	//			{
	//				vertex.index = reader.ReadIndex(PMXIndexType::Vertex);
	//				reader.Read(&vertex.offset, sizeof(vertex.offset));
	//			}
	//		}
	//		else if (morph.type == PMXMorphType::Texcoord || morph.type == PMXMorphType::AddTexcoord1 ||
	//			morph.type == PMXMorphType::AddTexcoord2 || morph.type == PMXMorphType::AddTexcoord3 ||
	//			morph.type == PMXMorphType::AddTexcoord4) {
	//			morph.texcoordData.resize(morph.offsetCount);
	//			for (auto& texcoord : morph.texcoordData)
	//			{
	//				texcoord.index = reader.ReadIndex(PMXIndexType::Vertex);
	//				reader.Read(&texcoord.offset, sizeof(texcoord.offset));
	//			}
	//		}
	//		else if (morph.type == PMXMorphType::Bone)
	//		{
	//			morph.boneData.resize(morph.offsetCount);
	//			for (auto& bone : morph.boneData)
	//			{
	//				bone.index = reader.ReadIndex(PMXIndexType::Bone);

	//				reader.Read(&bone.displacement, sizeof(bone.displacement));
	//				reader.Read(&bone.rotation, sizeof(bone.rotation));
	//			}
	//		}
	//		else if (morph.type == PMXMorphType::Material)
	//		{
	//			morph.materialData.resize(morph.offsetCount);
	//			for (auto& material : morph.materialData)
	//			{
	//				material.index = reader.ReadIndex(PMXIndexType::Material);
	//				reader.Read(&material.operation, sizeof(material.operation));
	//				reader.Read(&material.diffuse, sizeof(material.diffuse));
	//				reader.Read(&material.specular, sizeof(material.specular));
	//				reader.Read(&material.specularity, sizeof(material.specularity));
	//				reader.Read(&material.ambient, sizeof(material.ambient));
	//				reader.Read(&material.edge, sizeof(material.edge));
	//				reader.Read(&material.edgeSize, sizeof(material.edgeSize));
	//				reader.Read(&material.textureCoeff, sizeof(material.textureCoeff));
	//				reader.Read(&material.sphereTexCoeff, sizeof(material.sphereTexCoeff));
	//				reader.Read(&material.toonTexCoeff, sizeof(material.toonTexCoeff));
	//			}
	//		}
	//		else if (morph.type == PMXMorphType::Group)
	//		{
	//			morph.groupData.resize(morph.offsetCount);
	//			for (auto& group : morph.groupData)
	//			{
	//				group.index = reader.ReadIndex(PMXIndexType::Morph);
	//				reader.Read(&group.coeff, sizeof(group.coeff));
	//			}
	//		}
	//		else {
	//			assert(!"");
	//		}
	//	}
	//	return std::move(morphs);
	//}
}