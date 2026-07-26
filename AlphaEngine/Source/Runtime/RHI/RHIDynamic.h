#pragma once
#include "Core/SmartPtr.h"
#include "Core/Assertion.h"
#include "Declarations.h"
#include <typeinfo>
#include <concepts>

namespace Alpha
{
	class RHIDynamic
	{
	public:
		virtual ~RHIDynamic() {}

		virtual TUnique<RHISwapChain> CreateSwapChain(const RHISwapChainDesc& desc) = 0;

		virtual TUnique<RHIBuffer> CreateBuffer(const RHIBufferDesc& desc) = 0;

		virtual TUnique<RHITexture> CreateTexture(const RHITextureDesc& desc) = 0;

		virtual TUnique<RHISamplerState> CreateSamplerState(const RHISamplerStateDesc& desc) = 0;

		virtual TUnique<RHICommandBuffer> CreateCommandBuffer(const RHICommandBufferDesc& desc) = 0;

		virtual TUnique<RHICommandQueue> CreateCommandQueue(const RHICommandQueueDesc& desc) = 0;

		virtual TUnique<RHICommandSignature> CreateCommandSignature(const RHICommandSignatureDesc& desc) = 0;

		virtual TUnique<RHITimeline> CreateTimeline(const RHITimelineDesc& desc) = 0;

		virtual TUnique<RHIShader> CreateShader(const RHIShaderDesc& desc) = 0;

		virtual TUnique<RHIDescriptorHeap> CreateDescriptorHeap(const RHIDescriptorHeapDesc& desc) = 0;

		virtual TUnique<RHIPipelineLayout> CreatePipelineLayout(const RHIPipelineLayoutDesc& desc) = 0;

		//virtual TUnique<RHIDescriptorView> CreateDescriptorView(const RHIDescriptorViewDesc& desc) = 0;

		virtual TUnique<RHIGraphicsPipeline> CreateGraphicsPipeline(const RHIGraphicsPipelineDesc& desc) = 0;

		virtual TUnique<RHIComputePipeline> CreateComputePipeline(const RHIComputePipelineDesc& desc) = 0;

		virtual void RequestTLASSize(const RHIBuildTLASDesc& desc, RHIASMemoryInfo* info) = 0;

		virtual void RequestBLASSize(const RHIBuildBLASDesc& desc, RHIASMemoryInfo* info) = 0;

	};

	// RHIオブジェクトで必ず実装すべき関数群コンセプト requres を使ってみた
	// RHIObjectクラスに関数を追加したらこっちにも追加すること
	template<typename RHIObj, typename RHIDesc>
	concept IsRHIObject = requires(RHIObj* obj, const RHIDesc& desc)
	{
		{ obj->Initialize(desc) }->std::convertible_to<bool>;
	};


	// RHIオブジェクト生成共通関数
	template<typename RHIObject,typename RHIDevice,typename RHIDesc>
	requires IsRHIObject<RHIObject, RHIDesc>
	TUnique<RHIObject> CreateRHIObject(RHIDevice* device_, const RHIDesc& desc_)
	{
		if (ALPHA_CHECK_FATAL(device_, L"CreateRHIObject: デバイスが有効ではありません"))
			return nullptr;

		auto object = std::make_unique<RHIObject>(device_);
		if (object->Initialize(desc_) != true)
		{
			// Initializeの内部でログはあるけど一応ね
			//ALPHA_LOG_ERROR(L"RHIリソースの生成に失敗しました: {}", Text(typeid(RHIObject).name()));
			object.reset();
			return nullptr;
		}

		return object;
	}

}
