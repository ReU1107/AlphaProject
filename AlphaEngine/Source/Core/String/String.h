#pragma once
#include <string>
#include <string_view>
#include <filesystem>
#include <format>

namespace Alpha
{
	using tchar_t = wchar_t;
	static const tchar_t* ErrorMessage = L"エラーが発生しました";

	using TString = ::std::basic_string<tchar_t, std::char_traits<tchar_t>, std::allocator<tchar_t>>;
	using TStringView = std::basic_string_view<tchar_t>;

	class String;

	class StringView
	{
	private:
		TStringView m_view{};

	public:
		constexpr StringView() = default;
		constexpr StringView(const tchar_t* s)
			: m_view(s)
		{
		}
		constexpr StringView(TStringView v)
			: m_view(v)
		{
		}
		template<std::size_t N>
		constexpr StringView(const tchar_t(&str)[N]) noexcept
			: m_view(str, N - 1)
		{
		}

		constexpr StringView(const String& s);
		constexpr const tchar_t* data() const noexcept { return m_view.data(); }
		constexpr std::size_t size() const noexcept { return m_view.size(); }
		constexpr bool empty() const noexcept { return m_view.empty(); }

		constexpr const wchar_t* c_str() const noexcept { return m_view.data(); }

		std::size_t Hash() const
		{
			return std::hash<TStringView>{}(m_view);
		}

		constexpr bool operator==(const StringView& other) const noexcept
		{
			return m_view == other.m_view;
		}

		constexpr StringView Substring(std::size_t offset, std::size_t count = TStringView::npos) const
		{
			return StringView(m_view.substr(offset, count));
		}
	};

	class String
	{
	private:
		TString m_data{};

	public:
		constexpr String() = default;
		constexpr String(const tchar_t* s)
			: m_data(s)
		{
		}

		constexpr String(std::u16string_view s)
			: m_data(reinterpret_cast<const wchar_t*>(s.data()))
		{
		}

		String(const TString& s)
			: m_data(s)
		{
		}
		String(TString&& s) 
			: m_data(std::move(s)) 
		{
		}

		constexpr String(StringView s);

		std::size_t Hash() const { return std::hash<TString>{}(m_data); }

		bool operator==(const String& other) const 
		{
			return (Hash() == other.Hash()) && (m_data == other.m_data);
		}

		String& operator+=(const String& other)
		{
			m_data += other.m_data;
			return *this;
		}

		constexpr size_t length() const noexcept { return m_data.length(); }

		const wchar_t* c_str() const { return m_data.c_str(); }

		std::string ToUtf8() const
		{
			return std::filesystem::path(m_data).string();
		}

		static String FromUtf8(std::string_view utf8)
		{
			auto u16 = std::filesystem::path(reinterpret_cast<const char8_t*>(utf8.data())).u16string();
			return String(u16);
		}
		bool IsEmpty() const { return m_data.empty(); }

	};

	constexpr StringView::StringView(const String& s)
		: m_view(s.c_str(), s.length())
	{
	}

	constexpr String::String(StringView s)
		: m_data(s.c_str())
	{
	}

	inline bool operator==(const String& lhs, const StringView& rhs) noexcept
	{
		return StringView(lhs) == rhs;
	}
	inline bool operator==(const StringView& lhs, const String& rhs) noexcept
	{
		return lhs == StringView(rhs);
	}

	inline String operator+(const String& lhs, const String& rhs)
	{
		String res = lhs;
		res += rhs;
		return res;
	}

	// StringMap用
	struct StringHash
	{
		using is_transparent = void;
		size_t operator()(StringView sv) const
		{
			return sv.Hash();
		}
		size_t operator()(const String& s) const
		{
			return s.Hash();
		}
		size_t operator()(const tchar_t* p) const
		{
			return StringView(p).Hash();
		}
	};

	// 文字セット吸収用
	inline String Text(const std::string& text)
	{
		return String(std::filesystem::path(text).wstring().c_str());
	}

	template<typename... Args>
	inline String Format(std::wformat_string<Args...> fmt, Args&&... args)
	{
		return String(std::format(fmt, std::forward<Args>(args)...));
	}
}
namespace std 
{
	// std::hashの特殊化
	template <>
	struct hash<Alpha::String>
	{
		size_t operator()(const Alpha::String& s) const noexcept {
			return s.Hash();
		}
	};
	template <>
	struct hash<Alpha::StringView>
	{
		size_t operator()(const Alpha::StringView& sv) const noexcept 
		{
			return sv.Hash();
		}
	};
}
