#pragma once
#include "Core/Common.h"
#include "Core/Object.h"
#include <atomic>
#include <concepts>

namespace Alpha
{
	enum class AssetType
	{
		Audio,
		Model,
		Motion,
		Texture,
		Material,
		Particle,
	};

	class AssetHandle : public Object
	{
	private:
		std::atomic_bool m_is_loaded{ false };
	public:
		virtual ~AssetHandle() = default;
		virtual AssetType GetType() const = 0;

		bool IsLoaded() const { return m_is_loaded.load(); }

		template<typename T>
		T* Get()
		{
			if (this->GetType() != T::Type())
				return nullptr;

			return dynamic_cast<T*>(this);
		}

		void Accept(IVisitor* visitor) override {}

	protected:
		void MarkAsLoaded() { m_is_loaded.store(true); }

	};

	template<typename T>
	concept IsAsset = std::derived_from<T, AssetHandle>;

}