#pragma once
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Core/EnumClassUtil.h"

namespace Alpha
{
	enum class ERHICommandType
	{
		Direct,
		Bundle,
		Compute,
		Copy,
	};

	enum class ERHIDescriptorType
	{
		SRV_UAV_CBV,	 
		RTV,			 
		DSV,			 
		Sampler,
	};

	enum class ERHIQueryType
	{
		Timestamp,          // GPUの時刻計測用 (D3D12_QUERY_TYPE_TIMESTAMP)
		Occlusion,          // 描画されたピクセル数のカウント用 (D3D12_QUERY_TYPE_OCCLUSION)
		BinaryOcclusion,    // ピクセルが描画されたか否かの2値判定用 (D3D12_QUERY_TYPE_BINARY_OCCLUSION)
		PipelineStatistics, // 頂点数やプリミティブ数などの統計用 (D3D12_QUERY_TYPE_PIPELINE_STATISTICS)
	};

	enum class ERHIResourceViewType
	{
		Unknown,
		ShaderResourceView,			// SRV
		UnorderedAccessView,		// UAV
		ConstantBufferView,			// CBV	// 廃止するかも理由定数バッファを送る時はrootconstantsだけで統一するのもありかなって
		RenderTargetView,			// RTV
		DepthStencilView,			// DSV
		SamplerView,				// Sampler
	};

	enum class ERHIPipelineLayoutFlags
	{
		InputAssembler	= 1 << 0,	//
		Local			= 1 << 1,	// レイトレ用(今どき全部グローバルでよくてもいいらしいけど)
		Bindless		= 1 << 2,	// バインドレスリソースを使用するか
	};

	EnumClassFlags(ERHIPipelineLayoutFlags);

	enum class ERHIIndirectArgType
	{
		Draw,
		DrawIndexed,
		Dispatch,
		DispatchMesh,
		Constant,
		VertexBuffer,
		IndexBuffer,
	};

	enum class ERHIAddressMode 
	{ 
		Wrap, 
		Mirror, 
		Clamp,
		Border 
	};

	enum class ERHIFilter 
	{
		Point, 
		Linear, 
		Anisotropic,
		ComparisonLinear 
	};

	enum class ERHIComparisonFunc
	{ 
		Never,
		Less, 
		Equal, 
		LessEqual, 
		Always
	};

	enum class ERHIParameterType
	{
		RootConstants,
		RootDescriptors,
		DescriptorTable,
	};

	enum class ERHIPipelineType
	{
		Graphics,
		Compute,
		RayTracing,
	};

	enum class ERHIPrimitiveTopology
	{
		Umdefined = 0,
		PointList = 1,
		LineList = 2,
		LineStrip = 3,
		TriangleList = 4,
		TriangleStrip = 5,
	};

	enum class ERHICompareOp
	{
		Never,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always,
	};

	enum class ERHIResourceDimension
	{
		Unknown,	// テクスチャタイプが初期化されていないか、不明です。
		None,		// テクスチャが指定されていません。
		Any,		// すべてのテクスチャタイプ。
		Tex1D,		// 1Dテクスチャ
		Tex2D,		// 2Dテクスチャ(Texture2D)。
		Tex3D,		// 3Dボリュームテクスチャ(Texture3D)。
		Cube,		// キューブマップ テクスチャ。
		Tex2DArray,	// 2D配列テクスチャ(Texture2DArray)。
		CubeArray,	// Cubemap array texture(CubemapArray).
	};

	enum class ERHIResourceState
	{
		Common = 0,							// なし
		VertexConstantBuffer	 = 1 << 0,	// 頂点,定数バッファ
		IndexBuffer				 = 1 << 1,	// インデックスバッファ
		RenderTarget			 = 1 << 2,	// レンダーターゲット
		UnorderedAccess			 = 1 << 3,	// アンオーダードアクセス
		DepthWrite				 = 1 << 4,	// 深度書き込み
		DepthRead				 = 1 << 5,	// 深度読み込み
		NonPixelShaderResource	 = 1 << 6,	// ピクセルシェーダー内でのリソースとして扱わない
		PixelShaderResource		 = 1 << 7,	// ピクセルシェーダー内でリソースとして扱う
		StreamOut				 = 1 << 8,	// ストリームアウト
		IndirectArgument		 = 1 << 9,	// 即時
		CopyDestination			 = 1 << 10,	// コピー先
		CopySource				 = 1 << 11,	// コピー元
		ResolveDestination		 = 1 << 12,	// 
		ResolveSource			 = 1 << 13,	// 
		RaytracingStructure		 = 1 << 14,	// レイトレーシング構造体
		GenericRead = ((((					// 一般的な読み込み用のステート
			(VertexConstantBuffer | IndexBuffer)
			| NonPixelShaderResource)
			| PixelShaderResource)
			| IndirectArgument) | CopySource),
		Present					 = 1 << 15,	// プレゼント
		AllShaderResource		= 1 << 16,
	};

	EnumClassFlags(ERHIResourceState);

	enum class ERHIResourceAccessType
	{
		Static,   // GPU専用 (DEFAULT heap)
		Dynamic,  // CPUからアクセス可能 (UPLOAD heap / ConstantBuffer的)
		Staging,  // 読み戻し用 (READBACK heap)
	};

	enum class ERHIResourceBindFlags
	{
		None = 0,
		ShaderResource	= 1 << 0, // 基本これ
		UnorderedAccess = 1 << 1, // RWならこれ
		Raw				= 1 << 2, // ByteAddressBuffer
		RenderTarget	= 1 << 3, // 描画先にするならこれ
		DepthStencil	= 1 << 4, // 深度、ステンシルにするならこれ
		RayTracingAS	= 1 << 5, // レイトレーシングの加速構造体にするならこれ
	};

	EnumClassFlags(ERHIResourceBindFlags);

	enum class ERHIResourceFormat
	{
		Unknown,

		// RGBA 32-bit (8-bit per channel)
		RGBAUnorm,
		RGBAUnormSRGB,       // sRGBカラー用
		RGBASnorm,           // 
		RGBA32UInt,          // 各チャンネル8bit(計32bit)のUINTとしてマッピング
		RGBA32SInt,          // 各チャンネル8bit(計32bit)のSINTとしてマッピング

		// RGBA 64-bit (16-bit per channel)
		RGBA16Float,         // RGBAHalf と同一視可能
		RGBAHalf,
		RGBA16Unorm,         // 
		RGBA16UInt,          // 
		RGBA16SInt,          // 

		// RGBA 128-bit (32-bit per channel)
		RGBAFloat,
		RGBA32Float,         // 明示的な命名用
		RGBA32UInt128,       // 各チャンネル32bitの真の128bitマッピング用
		RGBA32SInt128,       // 各チャンネル32bitの真の128bitマッピング用

		// RGB 系統
		RGBFloat,            // R32G32B32_FLOAT
		RGB10A2Unorm,
		R11G11B10Float,      // HDRレンダーターゲットや特殊バッファで頻出

		// RG
		RG8Unorm,            // 
		RG8UInt,             // 
		RGFloat,             // R32G32_FLOAT
		RGHalf,              // R16G16_FLOAT
		RG16Unorm,           // 
		RG16UInt,            // 
		RGUInt,              // R32G32_UINT

		R8Unorm,             // 
		R8UInt,              // 
		RHalf,               // R16_FLOAT
		R16Unorm,            // 
		R16UInt,             // 
		RFloat,              // R32_FLOAT
		RUInt,               // R32_UINT

		BGRA8Unorm,          // 
		BGRA8UnormSRGB,      // 

		// デプス / ステンシル
		Depth32,
		Depth16,
		Depth24Stencil8,
		Depth32Stencil8,     // D32_FLOAT_S8X24_UINT 用

		BC1Unorm,            // DXT1 不透明 / 1bitアルファ
		BC1UnormSRGB,        // 
		BC2Unorm,            // DXT3 明示的アルファ
		BC2UnormSRGB,        // 
		BC3Unorm,            // DXT5 補間アルファ
		BC3UnormSRGB,        // 
		BC4Unorm,            // ATI1 / BC4 単チャンネル圧縮 (法線マップなど)
		BC4Snorm,            // 
		BC5Unorm,            // 3DC / BC5 2チャンネル圧縮 (法線マップ標準)
		BC5Snorm,            // 

		BC6HUFloat,
		BC6HSFloat,

		BC7Unorm,            // 高画質RGBA圧縮 (近代的な標準)
		BC7UnormSRGB,        // 
	};

	constexpr StringView ToString(ERHIResourceFormat format)
	{
		switch (format)
		{
		case ERHIResourceFormat::Unknown:             return L"Unknown";

		case ERHIResourceFormat::RGBAUnorm:           return L"RGBAUnorm";
		case ERHIResourceFormat::RGBAUnormSRGB:       return L"RGBAUnormSRGB";
		case ERHIResourceFormat::RGBASnorm:           return L"RGBASnorm";
		case ERHIResourceFormat::RGBA32UInt:          return L"RGBA32UInt";
		case ERHIResourceFormat::RGBA32SInt:          return L"RGBA32SInt";

		case ERHIResourceFormat::RGBA16Float:         return L"RGBA16Float";
		case ERHIResourceFormat::RGBAHalf:            return L"RGBAHalf";
		case ERHIResourceFormat::RGBA16Unorm:         return L"RGBA16Unorm";
		case ERHIResourceFormat::RGBA16UInt:          return L"RGBA16UInt";
		case ERHIResourceFormat::RGBA16SInt:          return L"RGBA16SInt";

		case ERHIResourceFormat::RGBAFloat:           return L"RGBAFloat";
		case ERHIResourceFormat::RGBA32Float:         return L"RGBA32Float";
		case ERHIResourceFormat::RGBA32UInt128:       return L"RGBA32UInt128";
		case ERHIResourceFormat::RGBA32SInt128:       return L"RGBA32SInt128";

		case ERHIResourceFormat::RGBFloat:            return L"RGBFloat";
		case ERHIResourceFormat::RGB10A2Unorm:        return L"RGB10A2Unorm";
		case ERHIResourceFormat::R11G11B10Float:      return L"R11G11B10Float";

		case ERHIResourceFormat::RG8Unorm:            return L"RG8Unorm";
		case ERHIResourceFormat::RG8UInt:             return L"RG8UInt";
		case ERHIResourceFormat::RGFloat:             return L"RGFloat";
		case ERHIResourceFormat::RGHalf:              return L"RGHalf";
		case ERHIResourceFormat::RG16Unorm:           return L"RG16Unorm";
		case ERHIResourceFormat::RG16UInt:            return L"RG16UInt";
		case ERHIResourceFormat::RGUInt:              return L"RGUInt";

		case ERHIResourceFormat::R8Unorm:             return L"R8Unorm";
		case ERHIResourceFormat::R8UInt:              return L"R8UInt";
		case ERHIResourceFormat::RHalf:               return L"RHalf";
		case ERHIResourceFormat::R16Unorm:            return L"R16Unorm";
		case ERHIResourceFormat::R16UInt:             return L"R16UInt";
		case ERHIResourceFormat::RFloat:              return L"RFloat";
		case ERHIResourceFormat::RUInt:               return L"RUInt";

		case ERHIResourceFormat::BGRA8Unorm:          return L"BGRA8Unorm";
		case ERHIResourceFormat::BGRA8UnormSRGB:      return L"BGRA8UnormSRGB";

		case ERHIResourceFormat::Depth32:             return L"Depth32";
		case ERHIResourceFormat::Depth16:             return L"Depth16";
		case ERHIResourceFormat::Depth24Stencil8:     return L"Depth24Stencil8";
		case ERHIResourceFormat::Depth32Stencil8:     return L"Depth32Stencil8";

		case ERHIResourceFormat::BC1Unorm:            return L"BC1Unorm";
		case ERHIResourceFormat::BC1UnormSRGB:        return L"BC1UnormSRGB";
		case ERHIResourceFormat::BC2Unorm:            return L"BC2Unorm";
		case ERHIResourceFormat::BC2UnormSRGB:        return L"BC2UnormSRGB";
		case ERHIResourceFormat::BC3Unorm:            return L"BC3Unorm";
		case ERHIResourceFormat::BC3UnormSRGB:        return L"BC3UnormSRGB";
		case ERHIResourceFormat::BC4Unorm:            return L"BC4Unorm";
		case ERHIResourceFormat::BC4Snorm:            return L"BC4Snorm";
		case ERHIResourceFormat::BC5Unorm:            return L"BC5Unorm";
		case ERHIResourceFormat::BC5Snorm:            return L"BC5Snorm";

		case ERHIResourceFormat::BC6HUFloat:          return L"BC6HUFloat";
		case ERHIResourceFormat::BC6HSFloat:          return L"BC6HSFloat";

		case ERHIResourceFormat::BC7Unorm:            return L"BC7Unorm";
		case ERHIResourceFormat::BC7UnormSRGB:        return L"BC7UnormSRGB";
		}

		return L"Unknown";
	}

	constexpr bool IsSRGBFormat(ERHIResourceFormat format)
	{
		switch (format)
		{
		case ERHIResourceFormat::RGBAUnormSRGB:
		case ERHIResourceFormat::BGRA8UnormSRGB:
		case ERHIResourceFormat::BC1UnormSRGB:
		case ERHIResourceFormat::BC2UnormSRGB:
		case ERHIResourceFormat::BC3UnormSRGB:
		case ERHIResourceFormat::BC7UnormSRGB:
			return true;

		default:
			return false;
		}
	}

	constexpr bool IsBCFormat(ERHIResourceFormat format)
	{
		switch (format)
		{
		case ERHIResourceFormat::BC1Unorm:
		case ERHIResourceFormat::BC1UnormSRGB:
		case ERHIResourceFormat::BC2Unorm:
		case ERHIResourceFormat::BC2UnormSRGB:
		case ERHIResourceFormat::BC3Unorm:
		case ERHIResourceFormat::BC3UnormSRGB:
		case ERHIResourceFormat::BC4Unorm:
		case ERHIResourceFormat::BC4Snorm:
		case ERHIResourceFormat::BC5Unorm:
		case ERHIResourceFormat::BC5Snorm:
		case ERHIResourceFormat::BC6HUFloat:
		case ERHIResourceFormat::BC6HSFloat:
		case ERHIResourceFormat::BC7Unorm:
		case ERHIResourceFormat::BC7UnormSRGB:
			return true;

		default:
			return false;
		}
	}

	constexpr UInt32 GetFormatBlockSize(ERHIResourceFormat format)
	{
		switch (format)
		{
			// 4x4 pixels = 8 bytes
		case ERHIResourceFormat::BC1Unorm:
		case ERHIResourceFormat::BC1UnormSRGB:
		case ERHIResourceFormat::BC4Unorm:
		case ERHIResourceFormat::BC4Snorm:
			return 8;

			// 4x4 pixels = 16 bytes
		case ERHIResourceFormat::BC2Unorm:
		case ERHIResourceFormat::BC2UnormSRGB:
		case ERHIResourceFormat::BC3Unorm:
		case ERHIResourceFormat::BC3UnormSRGB:
		case ERHIResourceFormat::BC5Unorm:
		case ERHIResourceFormat::BC5Snorm:
		case ERHIResourceFormat::BC6HUFloat:
		case ERHIResourceFormat::BC6HSFloat:
		case ERHIResourceFormat::BC7Unorm:
		case ERHIResourceFormat::BC7UnormSRGB:
			return 16;

		default:
			return 0;
		}
	}

	constexpr ERHIResourceFormat ToSRGBFormat(ERHIResourceFormat format)
	{
		switch (format)
		{
		case ERHIResourceFormat::RGBAUnorm:
			return ERHIResourceFormat::RGBAUnormSRGB;

		case ERHIResourceFormat::BGRA8Unorm:
			return ERHIResourceFormat::BGRA8UnormSRGB;

		case ERHIResourceFormat::BC1Unorm:
			return ERHIResourceFormat::BC1UnormSRGB;

		case ERHIResourceFormat::BC2Unorm:
			return ERHIResourceFormat::BC2UnormSRGB;

		case ERHIResourceFormat::BC3Unorm:
			return ERHIResourceFormat::BC3UnormSRGB;

		case ERHIResourceFormat::BC7Unorm:
			return ERHIResourceFormat::BC7UnormSRGB;

		default:
			return format;
		}
	}

	constexpr ERHIResourceFormat ToLinearFormat(ERHIResourceFormat format)
	{
		switch (format)
		{
		case ERHIResourceFormat::RGBAUnormSRGB:
			return ERHIResourceFormat::RGBAUnorm;

		case ERHIResourceFormat::BGRA8UnormSRGB:
			return ERHIResourceFormat::BGRA8Unorm;

		case ERHIResourceFormat::BC1UnormSRGB:
			return ERHIResourceFormat::BC1Unorm;

		case ERHIResourceFormat::BC2UnormSRGB:
			return ERHIResourceFormat::BC2Unorm;

		case ERHIResourceFormat::BC3UnormSRGB:
			return ERHIResourceFormat::BC3Unorm;

		case ERHIResourceFormat::BC7UnormSRGB:
			return ERHIResourceFormat::BC7Unorm;

		default:
			return format;
		}
	}

	enum class ERHIIndexFormat
	{
		UInt16,		// 16bit 2byte
		UInt32,		// 32bit 4byte
	};

	enum class ERHIShaderType
	{
		Vertex,	// 
		Pixel,	// 
		Geometry,	// 
		Hull,	// 
		Domain,	//
		Amplification,	// 
		Mesh,	// 
		Compute,	// 
		Library,	// RayTraceもこれ
	};

	enum class ERHIShaderFlags
	{
		All					= 0,		// 最適化するなら↓を活用して
		Vertex				= 1 << 0,	// 
		Pixel				= 1 << 1,	// 
		Geometry			= 1 << 2,	// 
		Hull				= 1 << 3,	// 
		Domain				= 1 << 4,	// 
		Amplification		= 1 << 5,	// 
		Mesh				= 1 << 6,	// 
		Compute				= 1 << 7,	// 
		RayTracing			= 1 << 8,	// 
		Library				= 1 << 9,
	};

	EnumClassFlags(ERHIShaderFlags);
	EnumFlagsFromType(ERHIShaderFlags, ERHIShaderType);

	enum class ERHICullMode
	{
		None,
		Front,
		Back,
		FrontAndBack,
	};

	enum class ERHIFillMode
	{
		Solid,
		Wireframe,
	};

	enum class ERHIBlend
	{
		Zero,
		One,
		SrcColor,
		InvSrcColor,
		SrcAlpha,
		InvSrcAlpha,
		DestAlpha,
		InvDestAlpha,
		DestColor,
		InvDestColor,
		SrcAlphaSat,
		BlendFactor,
		InvBlendFactor,
	};

	enum class ERHIBlendOp
	{
		Add,
		Subtract,
		RevSubtract,
		Min,
		Max,
	};

	enum class ERHIWorkGraphNodeType
	{
		Compute,
		Mesh,
	};

	enum class ERHIHitGroupType
	{
		Triangles,
		Procedural,
	};
}