#pragma once
#include "Buffer.h"

namespace Alpha
{
	class ConstantBuffer : public Buffer
	{
	private:

	public:
		ConstantBuffer() = default;
		~ConstantBuffer();

		struct Args
		{
			Buffer::Args base{};
		};

		void Initialize(const Args& args);

		void WriteData(void* data);

	};

}