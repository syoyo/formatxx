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

#if !defined(_guard_FORMATXX_H)
#define _guard_FORMATXX_H
#pragma once

#include <type_traits>
#include <cstring>
#include <memory>
#include <string>

namespace formatxx
{
	class string_view;
	struct format_spec;

	class format_writer;
	template <typename = std::string> class string_writer;
	template <std::size_t> class fixed_writer;
	template <std::size_t, typename = std::allocator<char>> class buffered_writer;

	template <typename... Args> void format(format_writer& out, string_view format, Args&&... args);
	template <typename StringT, typename... Args> StringT format(string_view format, Args&&... args);

	//template <typename T> void format_value(format_writer&, T const&, format_spec const&);
}

/// Describes a format string.
class formatxx::string_view
{
public:
	string_view(char const* first, char const* last) : _begin(first), _end(last) {}
	string_view(char const* nstr, std::size_t length) : string_view(nstr, nstr + length) {}
	string_view(char const* zstr) : string_view(zstr, std::strlen(zstr)) {}
	string_view(std::string const& str) : string_view(str.c_str(), str.size()) {}

	// hmm, this may be a bad idea, it'll bind to over-long character buffers
	template <std::size_t N> string_view(char const (&str)[N]) : string_view(str, N) {}

	char const* data() const { return _begin; }
	std::size_t size() const { return _end - _begin; }
	bool empty() const { return _begin != _end; }

private:
	char const* _begin = nullptr;
	char const* _end = nullptr;
};

/// Interface for any buffer that the format library can write into.
class formatxx::format_writer
{
public:
	virtual ~format_writer() = default;

	/// Write a string slice.
	/// @param str The string to write.
	virtual void write(string_view str) = 0;
};

/// A writer that generates a buffer (intended for std::string).
template <typename StringT>
class formatxx::string_writer : public format_writer
{
public:
	void write(string_view str) override { _string.append(str._begin, str._end - str._begin); }

	StringT const& str() const { return _string; }
	StringT& str() { return _string; }

	std::size_t size() const { return _string.size(); }
	char const* c_str() const { return _string.c_str(); }

private:
	StringT _string;
};

/// A writer with a fixed buffer that will never allocate.
template <std::size_t SizeN = 512>
class formatxx::fixed_writer : public format_writer
{
public:
	void write(string_view str) override;

	std::size_t size() const { return _last - _buffer; }
	char const* c_str() const { return _buffer; }

private:
	char* _last = _buffer;
	char _buffer[SizeN] = {'\0',};
};

/// A writer with a fixed buffer that will allocate when the buffer is exhausted.
template <std::size_t SizeN, typename AllocatorT>
class formatxx::buffered_writer : public format_writer, private AllocatorT
{
public:
	buffered_writer() = default;
	~buffered_writer();

	buffered_writer(buffered_writer const&) = delete;
	buffered_writer& operator=(buffered_writer const&) = delete;

	void write(string_view str) override;

	std::size_t size() const { return _last - _first; }
	char const* c_str() const { return _first; }

private:
	void _grow(std::size_t amount);

	char* _first = _buffer;
	char* _last = _buffer;
	char* _sentinel = _buffer + SizeN;
	char _buffer[SizeN] = {'\0',};
};

/// Extra formatting specifications.
struct formatxx::format_spec
{
	unsigned width = 0;
	unsigned precision = 0;
	// #FIXME: alignment
	// #FIXME: flags
	// #FIXME: custom string part
};

namespace formatxx
{
	/// Default format helpers.
	void format_value(format_writer& out, char* zstr, format_spec const& spec);
	void format_value(format_writer& out, char const* zstr, format_spec const& spec);
	void format_value(format_writer& out, string_view str, format_spec const& spec);
	void format_value(format_writer& out, char ch, format_spec const& spec);
	void format_value(format_writer& out, bool value, format_spec const& spec);
	void format_value(format_writer& out, float value, format_spec const& spec);
	void format_value(format_writer& out, double value, format_spec const& spec);
	void format_value(format_writer& out, signed int value, format_spec const& spec);
	void format_value(format_writer& out, signed long value, format_spec const& spec);
	void format_value(format_writer& out, signed short value, format_spec const& spec);
	void format_value(format_writer& out, signed long long value, format_spec const& spec);
	void format_value(format_writer& out, unsigned int value, format_spec const& spec);
	void format_value(format_writer& out, unsigned long value, format_spec const& spec);
	void format_value(format_writer& out, unsigned short value, format_spec const& spec);
	void format_value(format_writer& out, unsigned long long value, format_spec const& spec);
	void format_value(format_writer& out, void* value, format_spec const& spec);
	void format_value(format_writer& out, void const* value, format_spec const& spec);

	template <typename TraitsT, typename AllocatorT>
	void format_value(format_writer& out, std::basic_string<char, TraitsT, AllocatorT> const& string, format_spec const& spec)
	{
		format_value(out, string_view(string), spec);
	}

	/// Formatting for enumerations, using their numeric value.
	template <typename EnumT>
	auto format_value(format_writer& out, EnumT value, format_spec const& spec) -> std::enable_if_t<std::is_enum<EnumT>::value>
	{
		format_value(out, std::underlying_type_t<EnumT>(value), spec);
	}

	template <typename PointerT>
	auto format_value(format_writer& out, PointerT value, format_spec const& spec) -> std::enable_if_t<std::is_pointer<PointerT>::value>
	{
		format_value(out, static_cast<void const*>(value), spec);
	}

	/// Cause a friendlier error message on unknown type.
	template <typename T>
	auto format_value(format_writer& writer, T const& value, format_spec const& spec) -> std::enable_if_t<!std::is_enum<T>::value && !std::is_pointer<T>::value> = delete;

	/// @internal
	namespace _detail
	{
		using FormatFunc = void(*)(format_writer&, void const*, format_spec const&);

		template <typename T> struct wrap { static void fwd(format_writer& out, void const* ptr, format_spec const& spec) { format_value(out, *static_cast<T const*>(ptr), spec); } };
		// #FIXME: char[N] types will horribly do the wrong thing here.

		void format_impl(format_writer& out, string_view format, std::size_t count, FormatFunc const* funcs, void const** values);
	}
}

/// Write the string format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
template <typename... Args>
void formatxx::format(format_writer& out, string_view format, Args&&... args)
{
	void const* values[] = {std::addressof(args)..., nullptr};
	constexpr _detail::FormatFunc funcs[] = {&_detail::wrap<std::decay_t<Args>>::fwd..., nullptr};

	_detail::format_impl(out, format, sizeof...(Args), funcs, values);
}

/// Write the string format using the given parameters into a buffer.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
/// @returns a formatted string.
template <typename StringT = std::string, typename... Args>
StringT formatxx::format(string_view format, Args&&... args)
{
	string_writer<StringT> tmp;
	formatxx::format(tmp, format, args...);
	return static_cast<StringT&&>(tmp.str());
}

template <std::size_t SizeN>
void formatxx::fixed_writer<SizeN>::write(string_view str)
{
	std::size_t const remaining = SizeN - size() - 1;
	std::size_t const length = remaining < str.size() ? remaining : str.size();
	std::memcpy(_last, str.data(), length);
	_last += length;
	*_last = '\0';
}

template <std::size_t SizeN, typename AllocatorT>
formatxx::buffered_writer<SizeN, AllocatorT>::~buffered_writer()
{
	if (_first != _buffer)
		this->deallocate(_first, _sentinel - _first);
}

template <std::size_t SizeN, typename AllocatorT>
void formatxx::buffered_writer<SizeN, AllocatorT>::_grow(std::size_t amount)
{
	std::size_t const size = _last - _first;
	std::size_t const capacity = _sentinel - _first;
	std::size_t const required = size + amount + 1;

	if (required > capacity) // need space for NUL byte
	{
		std::size_t newCapacity = capacity;
		newCapacity += newCapacity >> 1; // grow by 50%
		if (newCapacity < required) // ensure we get the space we asked for
			newCapacity = required;

		char* newBuffer = this->allocate(newCapacity);
		std::memcpy(newBuffer, _first, size + 1);

		if (_first != _buffer)
			this->deallocate(_first, capacity);

		_first = newBuffer;
		_last = _first + size;
		_sentinel = _first + newCapacity;
	}
}

template <std::size_t SizeN, typename AllocatorT>
void formatxx::buffered_writer<SizeN, AllocatorT>::write(string_view str)
{
	_grow(str.size());
	std::memcpy(_last, str.data(), str.size());
	_last += str.size();
	*_last = '\0';
}

#endif // !defined(_guard_FORMATXX_H)
