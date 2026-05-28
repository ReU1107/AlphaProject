#pragma once
#include "Buffer.h"
#include "RenderCore/Types.h"

namespace Alpha
{
	/*
	*  AoS (Array of Structures)
	* の時用
	* struct MyStruct
	* {
	* float4 a;
	* float3 b;
	* float c;
	* }
	* みたいな1の構造体を配列として持ちたいとき
	*/

	/*
	* hlsl側
	* StructuredBuffer<MyStruct> read_only_buffer : register(t0);	EResourceAccessFlags::ReadOnly
	* RWStructuredBuffer<MyStruct> read_write_buffer : register(u0);	EResourceAccessFlags::ReadWrite
	*/
	class RHIResourceView;

	class StructuredBuffer : public Buffer
	{
	public:
		StructuredBuffer();
		~StructuredBuffer();

		struct Args
		{
			Buffer::Args base{};
			EResourceAccessFlags access_flags{ EResourceAccessFlags::ReadOnly };
		};

		void Initialize(const Args& args);

	};
}