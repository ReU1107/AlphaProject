#include "Object.h"
#include "Math/HashUtil.h"

namespace Alpha
{
	void Alpha::Object::SetName(StringView name)
	{
		m_name = name;

		USize seed = 0;

		hash_combine(seed, GetName());
		m_id = seed;
	}
}