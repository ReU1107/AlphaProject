#pragma once
#include "Buffer.h"
#include "RenderCore/Types.h"

namespace Alpha
{
	/*
	* SoA (Structure of Arrays)

	*/

	class ByteAddressBuffer : public Buffer
	{
	private:

	public:
		ByteAddressBuffer();
		~ByteAddressBuffer();

		struct Args
		{
			Buffer::Args base{};
			EResourceAccessFlags access_flags{ EResourceAccessFlags::ReadOnly };
		};

		void Initialize(const Args& args);
	};
}
