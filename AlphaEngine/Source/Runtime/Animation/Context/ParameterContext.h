#pragma once
#include "Core/Types.h"

namespace Alpha::Animation
{
	struct ParameterContext
	{
		TStringMap<float> parameter_set{};

		void Set(StringView name, float value)
		{
			parameter_set[name] = value;
		}

		float Get(StringView name) const
		{
			const auto& found = parameter_set.find(name);
			if (found == parameter_set.cend())
				return 0.0f;

			return found->second;
		}

		uint32_t Size() const
		{
			return static_cast<uint32_t>(parameter_set.size());
		}
	};
}