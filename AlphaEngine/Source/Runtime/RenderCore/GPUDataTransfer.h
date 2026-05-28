#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderCore;

	class CommandList;
	class CommandQueue;
	class Timeline;
	class RingBuffer;

	class GPUResource;
	class Buffer;
	class VertexBuffer;
	class IndexBuffer;
	class StructuredBuffer;
	class Texture;

	enum class UploadType
	{
		Structured,
		Texture,
	};

	struct UploadRequest
	{
		UploadType type{ UploadType::Structured };
		GPUResource* resource{ nullptr };				// 転送先バッファ
		void* data{ nullptr };					// 転送元データ
		
		UInt64 size{ 0 };
	};

	// CPUとGPUのデータの受け渡しを担う
	class GPUDataTransfer
	{
	private:
		TArray<UploadRequest> m_requests{};		// Uploadするバッファ群
		TUnique<CommandQueue> m_copy_queue{};	// コピー用のコマンドキュー
		TUnique<CommandList> m_copy_list{};		// コピー用のコマンドリスト
		TUnique<Timeline> m_copy_fence{};		// コピー完了を待つためのフェンス
		TUnique<RingBuffer> m_staging_buffer{};	// ステージングバッファ

	public:
		GPUDataTransfer();
		~GPUDataTransfer();

		struct Args
		{
			RenderCore* render_core{ nullptr };
			uint64_t staging_buffer_size{ 10_MB };
		};

		void Initialize(const Args& args);

		void AddStructuredBuffer(StructuredBuffer* buffer, void* data);
		void AddTexture(Texture* texture, void* data);

		bool HasUpload() const { return !m_requests.empty(); }

		// 非同期サブミット：コピーキューにサブミットして fence value を返す
		uint64_t UploadAsync();

		// 同期(ブロッキング)：UploadAsync() の後 fence 到達まで待つ
		void Upload();

		// Upload がシグナルする Timeline を返す（レンダー側が待つために使用）
		Timeline* GetTimeline() const { return m_copy_fence.get(); }

	};

}

