#include "HLSLLoader.h"
#include "Asset/ShaderHandle.h"

namespace Alpha
{
	TShared<AssetHandle> HLSLLoader::AsyncLoad(StringView file_path)
	{
		auto handle = std::make_shared<ShaderHandle>();

		//TArray<uint8_t> file_data;
		//std::ifstream inFile(file_path);


		//if (!FileSystem::ReadFile(path, &file_data))
		//{
		//	return false;
		//}

		//// パース用に、プレーンなテキストのビュー（StringView）を作成
		//StringView content(reinterpret_cast<const char*>(file_data.Data()), file_data.Size());

		//const StringView tag_prefix = "// [Engine:";
		//size_t current_pos = 0;

		//while (true)
		//{
		//	// "// [Engine:" を検索
		//	size_t tag_start = content.Find(tag_prefix, current_pos);
		//	if (tag_start == StringView::npos)
		//	{
		//		break; // これ以上タグが見つからなければ終了
		//	}

		//	// 閉じブラケット "]" を検索してタグの範囲を確定させる
		//	size_t tag_end = content.Find("]", tag_start);
		//	if (tag_end == StringView::npos)
		//	{
		//		break; // タグの閉じ方が不正なら中断
		//	}

		//	// タグの中身（"Stage, EntryPoint" の部分）を抽出
		//	size_t data_start = tag_start + tag_prefix.Size();
		//	size_t data_length = tag_end - data_start;
		//	StringView tag_inner = content.SubString(data_start, data_length);

		//	// カンマでステージとエントリーポイントに分割
		//	size_t comma_pos = tag_inner.Find(",");
		//	if (comma_pos != StringView::npos)
		//	{
		//		// 前後の不要な空白をトリミングして純粋な文字列を抽出
		//		StringView stage_view = tag_inner.SubString(0, comma_pos).Trim();
		//		StringView entry_view = tag_inner.SubString(comma_pos + 1).Trim();

		//		// ステージ名から、D3D12のコンパイラが理解できるプロファイル文字列にマッピング
		//		String profile = "lib_6_8"; // デフォルト
		//		if (stage_view == "Vertex")    profile = "vs_6_8";
		//		else if (stage_view == "Pixel")     profile = "ps_6_8";
		//		else if (stage_view == "Compute")   profile = "cs_6_8";
		//		else if (stage_view == "WorkGraph") profile = "lib_6_8";
		//		else if (stage_view == "RayTracing")profile = "lib_6_8";

		//		// 3. 確定した情報を ShaderHandle（コンテナ）に小分けにして登録
		//		// file_data（ファイル全体の生テキスト）をそのまま渡します
		//		out_handle->AddShaderBlock(String(entry_view), profile, file_data);
		//	}

		//	// 次の検索位置を更新
		//	current_pos = tag_end + 1;
		//}

		//// 1つもメタタグが見つからなかった場合のフォールバック（親切設計）
		//if (out_handle->GetBlocks().IsEmpty())
		//{
		//	// タグがなければ、一般的な頂点シェーダーのデフォルトとして丸ごと登録しておく
		//	out_handle->AddShaderBlock("main", "vs_6_8", file_data);
		//}

		//return true;


		return handle;
	}
}