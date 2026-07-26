#pragma once
#include "Core/SmartPtr.h"
#include "Core/Assertion.h"

#include "Declarations.h"


namespace Alpha
{
	class AHIDynamic
	{
	public:
		virtual ~AHIDynamic() {}

		virtual TUnique<AHIMasterBus> CreateMasterBus(const AHIMasterBusDesc& desc) = 0;

		virtual TUnique<AHISubmixBus> CreateSubmixBus(const AHISubmixBusDesc& desc) = 0;

		virtual TUnique<AHISourceBus> CreateSourceBus(const AHISourceBusDesc& desc) = 0;

	};

	// AHIObjectクラスに関数を追加したらこっちにも追加すること
	template<typename AHIObj, typename AHIDesc>
	concept IsAHIObject = requires(AHIObj * obj, const AHIDesc & desc)
	{
		{ obj->Initialize(desc) }->std::convertible_to<bool>;
	};


	// AHIオブジェクト生成共通関数
	template<typename AHIObject, typename AHIDevice, typename AHIDesc>
	requires IsAHIObject<AHIObject, AHIDesc>
	TUnique<AHIObject> CreateAHIObject(AHIDevice* device_, const AHIDesc& desc_)
	{
		if (ALPHA_CHECK_FATAL(device_, L"CreateAHIObject: デバイスが有効ではありません"))
			return nullptr;

		auto object = std::make_unique<AHIObject>(device_);
		if (object->Initialize(desc_) != true)
		{
			// Initializeの内部でログはあるけど一応ね
			//ALPHA_LOG_ERROR(L"AHIリソースの生成に失敗しました: {}", Text(typeid(AHIObject).name()));
			object.reset();
			return nullptr;
		}

		return object;
	}

}