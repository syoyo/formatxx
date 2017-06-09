// formatxx - C++ string formatting library.
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non - commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>
//
// Authors:
//   Sean Middleditch <sean@middleditch.us>

#include <formatxx/format.h>
#include <formatxx/wide.h>

#include <formatxx/_detail/format_traits.h>
#include <formatxx/_detail/parse_unsigned.h>
#include <formatxx/_detail/parse_format.h>
#include <formatxx/_detail/write_integer.h>
#include <formatxx/_detail/format_impl.h>
#include <formatxx/_detail/printf_impl.h>

#include <cwchar>
#include <cstdlib>
#include <climits>

namespace formatxx {

#pragma warning(push)
#pragma warning(disable: 4996)

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, char ch, wstring_view)
{
	std::mbstate_t state{};
	wchar_t wc;
	std::size_t const rs = std::mbrtowc(&wc, &ch, 1, &state);
	if (rs > 0 && rs < static_cast<std::size_t>(-2))
	{
		out.write({&wc, 1});
	}
}

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, char* zstr, wstring_view spec)
{
	format_value(out, const_cast<char const*>(zstr), spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, char const* zstr, wstring_view)
{
	if (zstr != nullptr)
	{
		std::mbstate_t state{};
		while (*zstr != '\0')
		{
			wchar_t wc;
			std::size_t const rs = std::mbrtowc(&wc, zstr++, 1, &state);
			if (rs != static_cast<std::size_t>(-2))
			{
				out.write({&wc, 1});
			}
		}
	}
}

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, string_view str, wstring_view)
{
	std::mbstate_t state{};
	for (auto const ch : str)
	{
		wchar_t wc;
		std::size_t const rs = std::mbrtowc(&wc, &ch, 1, &state);
		if (rs != static_cast<std::size_t>(-2))
		{
			out.write({&wc, 1});
		}
	}
}

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wchar_t ch, wstring_view)
{
	out.write(wstring_view(&ch, 1));
}

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wchar_t* zstr, wstring_view spec)
{
	format_value(out, wstring_view(zstr), spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wchar_t const* zstr, wstring_view spec)
{
	format_value(out, wstring_view(zstr), spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, wstring_view str, wstring_view)
{
	out.write(str);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, wchar_t ch, string_view)
{
	std::mbstate_t state{};
	char mb[MB_LEN_MAX];
	std::size_t const rs = std::wcrtomb(mb, ch, &state);
	if (rs != static_cast<std::size_t>(-1))
	{
		out.write({mb, rs});
	}
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, wchar_t* zstr, string_view spec)
{
	format_value(out, const_cast<wchar_t const*>(zstr), spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, wchar_t const* zstr, string_view)
{
	if (zstr != nullptr)
	{
		std::mbstate_t state{};
		char mb[MB_LEN_MAX];
		while (*zstr != '\0')
		{
			std::size_t const rs = std::wcrtomb(mb, *zstr++, &state);
			if (rs != static_cast<std::size_t>(-1))
			{
				out.write({mb, rs});
			}
		}
	}
}

FORMATXX_PUBLIC void FORMATXX_API format_value(format_writer& out, wstring_view str, string_view)
{
	std::mbstate_t state{};
	char mb[MB_LEN_MAX];
	for (auto const ch : str)
	{
		std::size_t const rs = std::wcrtomb(mb, ch, &state);
		if (rs != static_cast<std::size_t>(-1))
		{
			out.write({mb, rs});
		}
	}
}

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, bool value, wstring_view spec)
{
	format_value(out, value ? _detail::FormatTraits<wchar_t>::sTrue : _detail::FormatTraits<wchar_t>::sFalse, spec);
}

#pragma warning(pop)

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed int value, wstring_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed char value, wstring_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed long value, wstring_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed short value, wstring_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, signed long long value, wstring_view spec) { _detail::write_integer(out, value, spec); }

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned int value, wstring_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned char value, wstring_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned long value, wstring_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned short value, wstring_view spec) { _detail::write_integer(out, value, spec); }
FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, unsigned long long value, wstring_view spec) { _detail::write_integer(out, value, spec); }

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, float value, wstring_view spec)
{
	format_value(out, static_cast<double>(value), spec);
}

FORMATXX_PUBLIC void FORMATXX_API format_value(wformat_writer& out, double value, wstring_view spec_string)
{
	wchar_t fmt[3] = L"%f";

	wformat_spec const spec = parse_format_spec(spec_string);

	switch (spec.code)
	{
	case L'a':
	case L'A':
	case L'e':
	case L'E':
	case L'f':
	case L'F':
	case L'g':
	case L'G':
		fmt[1] = spec.code;
		break;
	default:
		// leave default
		break;
	}

	constexpr std::size_t buf_size = 1024;
	wchar_t buf[buf_size]; // not actually enough for every float, but...
	int const len = std::swprintf(buf, buf_size, fmt, value);
	if (len > 0)
		out.write({buf, std::size_t(len)});
}

namespace _detail {

template basic_format_writer<wchar_t>& FORMATXX_API format_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);

template basic_format_writer<wchar_t>& FORMATXX_API printf_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);

} // namespace _detail

template basic_format_spec<wchar_t> FORMATXX_API parse_format_spec(basic_string_view<wchar_t>);

} // namespace formatxx
