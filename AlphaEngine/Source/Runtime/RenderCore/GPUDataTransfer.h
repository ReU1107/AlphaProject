#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;

	class CommandList;
	class CommandQueue;
	class Timeline;

	class GPUResource;
	class Buffer;
	class Texture;

	enum class UploadType
	{
		Buffer,
		Texture,
	};

	struct UploadRequest
	{
		UploadType type{ UploadType::Buffer };
		GPUResource* resource{ nullptr };				// 転送先バッファ
		UInt64 offset{ 0 };						// 転送先オフセット（Bufferの要素単位、Textureのサブリソース単位）
		void* data{ nullptr };					// 転送するデータ
		
		UInt64 size{ 0 };						// 転送するデータのサイズ（Bufferの場合はバイト単位、Textureの場合はサブリソース全体のサイズ）
	};

	// CPUとGPUのデータの受け渡しを担う
	class GPUDataTransfer
	{
	private:
		TArray<UploadRequest> m_requests{};		// Uploadするバッファ群
		TUnique<CommandQueue> m_copy_queue{};	// コピー用のコマンドキュー
		TUnique<CommandList> m_copy_list{};		// コピー用のコマンドリスト
		TUnique<Timeline> m_copy_fence{};		// コピー完了を待つためのフェンス
		TUnique<Buffer> m_staging_buffer{};		// ステージングバッファ
		LinearAllocator m_staging_allocator{};	// ステージングバッファ用のアロケータ
	public:
		GPUDataTransfer();
		~GPUDataTransfer();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			UInt64 staging_buffer_size{ 64_MB };
		};

		void Initialize(const Args& args);

		void AddBufferRegion(Buffer* buffer, UInt64 offset, void* data, UInt64 size);
		void AddTexture(Texture* texture, void* data);

		bool HasUpload() const { return !m_requests.empty(); }

		// 非同期サブミット：コピーキューにサブミットして fence value を返す
		UInt64 UploadAsync();

		// 同期(ブロッキング)：UploadAsync() の後 fence 到達まで待つ
		void Upload();

		// Upload がシグナルする Timeline を返す（レンダー側が待つために使用）
		Timeline* GetTimeline() const { return m_copy_fence.get(); }

	};

}

