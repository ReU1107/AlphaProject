#pragma once
#include "Types.h"
#include "String/String.h"
#include "IVisitor.h"

namespace Alpha
{
	class IVisitor;

	class Object
	{
	private:
		UInt64 m_id{ 0 };

		String m_name{};

	public:
		virtual ~Object() = default;

		void SetName(StringView name);

		UInt64 GetID() const { return m_id; }

		String GetName() const { return m_name; }

		virtual void Accept(IVisitor* visitor) = 0;
	};


}