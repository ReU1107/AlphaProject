#pragma once
#include "RHIResource.h"
#include "Definitions.h"

namespace Alpha
{
	class RHIBuffer : public RHIResource
	{
	private:
		UInt64 m_stride{ 0 };
		UInt64 m_size{ 0 };

	public:
		virtual ~RHIBuffer() {}

		virtual bool Initialize(const RHIBufferDesc& desc) = 0;

		void SetStride(UInt64 stride) { m_stride = stride; }

		void SetSize(UInt64 size) { m_size = size; }


		// バッファストライド取得
		UInt64 GetStride() const { return m_stride; }
		// サイズ取得
		UInt64 GetSize() const { return m_size; }

		virtual void Write(void* transfer_data) = 0;

		virtual UInt8* Map(UInt64 read_begin, UInt64 read_end) = 0;

		virtual void Unmap(UInt64 written_begin, UInt64 written_end) = 0;

		virtual UInt64 GetGPUAddress() const = 0;

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