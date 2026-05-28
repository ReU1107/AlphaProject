#pragma once
#include "RHIResource.h"
#include "Definitions.h"

namespace Alpha
{
	class RHIBuffer : public RHIResource
	{
	private:
		uint64_t m_stride{ 0 };
		uint64_t m_size{ 0 };
		ERHIBufferAccess m_access{ ERHIBufferAccess::Static };
		ERHIBufferBindFlags m_flags{ ERHIBufferBindFlags::ShaderResource };
	public:
		virtual ~RHIBuffer() {}

		virtual bool Initialize(const RHIBufferDesc& desc) = 0;

		void SetStride(uint64_t stride) { m_stride = stride; }

		void SetSize(uint64_t size) { m_size = size; }
		void SetAccess(ERHIBufferAccess access) { m_access = access; }
		void SetFlags(ERHIBufferBindFlags flags) { m_flags = flags; }

		// バッファストライド取得
		uint64_t GetStride() const { return m_stride; }
		// サイズ取得
		uint64_t GetSize() const { return m_size; }
		// バッファ使用法取得
		ERHIBufferBindFlags GetFlags() const { return m_flags; }

		virtual void Write(void* transfer_data) = 0;

		virtual uint8_t* Map(uint64_t read_begin, uint64_t read_end) = 0;

		virtual void Unmap(uint64_t written_begin, uint64_t written_end) = 0;

		virtual uint64_t GetGPUAddress() const = 0;

		virtual void Upload(void* map_ptr, void* data, uint64_t size, uint64_t offset = 0) = 0;

	};
}

/*
* UploadとTransferの違い
* Write:
* CPUから直接アクセス可能なメモリ（D3D12_HEAP_TYPE_UPLOAD）へ即座に書き込みます。
* 主に、毎フレーム書き換わる頂点バッファや定数バッファに使用します。
*
* Upload:
* GPU専用の高速メモリ（D3D12_HEAP_TYPE_DEFAULT）へデータを転送するための「前処理」を行います。
* GPUDataTransfer（Copy Queue）を介して、ステージングバッファ経由でVRAMへコピーする際に呼び出されます。
*/