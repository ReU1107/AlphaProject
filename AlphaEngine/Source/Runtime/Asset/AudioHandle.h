#pragma once
#include "AssetHandle.h"

namespace Alpha
{
	class TextureHandle;
	// 読み込み方法
	enum class AudioLoadType
	{
		DecompressOnLoad,	// 解凍状態で一括でメモリに格納
		CompressedInMemory,	// 圧縮状態
		Streaming,			// ストリーミング
	};

	class AudioHandle : public AssetHandle
	{
	private:
		UInt32 m_sampleRate{ 44100 };
		UInt16 m_channels{ 2 };
		UInt16 m_bitsPerSample{ 16 };
		AudioLoadType m_type{ AudioLoadType::DecompressOnLoad };

		// オンメモリ再生用（効果音など）の全バイナリデータ
		TArray<UInt8> m_pcmData{};

	public:
		~AudioHandle();

		static AssetType Type() { return AssetType::Audio; }

		AssetType GetType() const final { return AssetType::Audio; }

		void Accept(IVisitor* visitor) override {}

	};

}