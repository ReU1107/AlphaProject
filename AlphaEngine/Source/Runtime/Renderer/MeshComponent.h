#pragma once
#include "Core/Common.h"
#include "Core/Component.h"

namespace Alpha::Renderer
{
	struct MeshComponent
	{
		UInt32 mesh_index{ 0 };	// メッシュ配列の直接指定用(ModelHandleをRenderResourceManagerに登録した後の戻り値)

		bool enabled{ true };	// RenderSceneへの参加
		bool visible{ true };	// モデルの描画

		UInt32 render_instance_index{ 0xFFFFFFFF };	// システム側で割り当てられるインスタンスid
	};

}
/*
* enabled: RenderSceneにインスタンス登録をするってだけ
* なんか追加するかも: 登録はするけど更新しない的ないるかな?
* visible: そのうえで描画する
*/