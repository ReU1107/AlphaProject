#pragma once
#include "Core/Common.h"
#include "Core/Component.h"

namespace Alpha
{
	struct MeshComponent
	{
		UInt32 handle_id{ 0 };			// 描画に使用するModelHandleのUniqueID
		UInt32 mesh_id{ 0 };			// ↑のIDから直接アクセスするためのID
		UInt32 material_table_id{ 0 };	// 

	};


}