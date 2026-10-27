#ifndef MEM_STATIC
# define MEM_STATIC
# define MEM_API ASAN_NO_ADDR static inline
# include "third_party/martins_memfun/memfun.h"
#endif

internal B32
char_is_space(U8 c)
{
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}
internal B32
char_is_upper(U8 c)
{
	return ('A' <= c && c <= 'Z');
}


internal B32
char_is_lower(U8 c)
{
	return ('a' <= c && c <= 'z');
}


internal B32
char_is_alpha(U8 c)
{
	return (char_is_upper(c) || char_is_lower(c));
}

internal B32
char_is_slash(U8 c)
{
	return (c == '/' || c == '\\');
}

internal B32
char_is_digit(U8 c, U32 base)
{
	B32 result = 0;
	if (0 < base && base < 16)
	{
		U8 val = integer_symbol_reverse[c];
		if (val < base)
		{
			result = 1;
		}
	}
	return result;
}

internal U8
lower_from_char(U8 c)
{
	if (char_is_upper(c))
	{
		c += ('a' - 'A');
	}
	return c;
}

internal U8 upper_from_char(U8 c)
{
	if (char_is_lower(c))
	{
		c += ('A' - 'a');
	}
	return c;
}


internal U8
correct_slash_from_char(U8 c)
{
	if (char_is_slash(c))
	{
		c = '/';
	}
	return c;
}

// C-String Measurement

internal U64
cstring8_length(U8* cstr)
{
	U64 length = 0;

	if (cstr)
	{
		U8 *ptr = cstr;
		for (;*ptr != 0; ptr += 1);
		length = (U64)(ptr - cstr);
	}
	return length;
}

internal U64
cstring16_length(U16* cstr)
{
	U64 length = 0;

	if (cstr)
	{
		U16* ptr = cstr;
		for (;*ptr != 0; ptr += 1);
		length = (U64)(ptr - cstr);
	}

	return length;
}

internal U64
cstring32_length(U32* cstr)
{
	U64 length = 0;

	if (cstr)
	{
		U32* ptr = cstr;
		for (;*ptr != 0; ptr += 1);
		length = (U64)(ptr - cstr);
	}

	return length;
}

internal String8
str8(U8* str, U64 size)
{
	String8 result = {str, size};
	return result;
}

internal String8
str8_range(U8 *first, U8 *one_past_last)
{
	String8 result = {first, (U64)(one_past_last - first)};
	return result;
}

internal String8
str8_zero(void)
{
	String8 result = {0};
	return result;
}


internal String8
str8_cstring(const char* c)
{
	String8 result = {(U8*)c, cstring8_length((U8*)c)};
	return result;
}

internal String8
str8_cstring_capped(void *cstr, void *cap)
{
	U64 cap_size = (U64)((U8 *)cap - (U8 *)cstr);
	U64 size = MemFind(cstr, cap_size, 0);
	return str8(cstr, size);
}

internal String8
upper_from_str8(Arena* arena, String8 string)
{
	string = push_str8_copy(arena, string);
	for (U64 i = 0; i < string.size; i++)
	{
		string.str[i] = upper_from_char(string.str[i]);
	}
	return string;
}

internal String8
lower_from_str8(Arena* arena, String8 string)
{
	string = push_str8_copy(arena, string);
	for (U64 i = 0; i < string.size; i++)
	{
		string.str[i] = lower_from_char(string.str[i]);
	}
	return string;
}


// String Matching

internal B32
str8_match(String8 a, String8 b, StringMatchFlags flags)
{
	B32 result = 0;

	if (a.size == b.size && flags == 0)
	{
		result = MemoryMatch(a.str, b.str, b.size);
	}
	else if (a.size == b.size && flags == StringMatchFlag_CaseInsensitive)
	{
		// TODO(Insomniac): Implement when needed
	}
	else if (a.size == b.size || (flags & StringMatchFlag_RightSideSloppy))
	{
		// TODO(Insomniac): Implement when needed
	}

	return result;
}


internal B32
str8_char_match(U8 a, U8 b, StringMatchFlags flags)
{
	U8 at = a;
	U8 bt = b;

	if (flags & StringMatchFlag_CaseInsensitive)
	{
		at = upper_from_char(at);
		bt = upper_from_char(bt);
	}
	if (flags & StringMatchFlag_SlashInsensitive)
	{
		if (at == '\\') { at = '/'; }
		if (bt == '\\') { bt = '/'; }
	}

	return (at == bt);
}

internal B32
str8_match_wildcard(String8 string, String8 pattern, StringMatchFlags flags)
{
	B32 matched = 0;

	U64 pattern_cursor = 0;
	U64 string_cursor = 0;

	U64 pattern_start = max_U64;
	U64 string_start = 0;

	for (;;)
	{
		if (pattern_cursor == pattern.size)
		{
			if (string_cursor == string.size || (flags & StringMatchFlag_RightSideSloppy))
			{
				matched = 1;
				break;
			}
		}

		if (string_cursor == string.size)
		{
			while (pattern_cursor < pattern.size && pattern.str[pattern_cursor] == '*')
			{
				pattern_cursor += 1;
			}
			matched = (pattern_cursor == pattern.size);
			break;
		}

		if (pattern_cursor < pattern.size && pattern.str[pattern_cursor] == '*')
		{
			pattern_start = pattern_cursor;
			string_start = string_cursor;
			pattern_cursor += 1;
			continue;
		}

		if (pattern_cursor < pattern.size && (pattern.str[pattern_cursor] == '?' || str8_char_match(string.str[string_cursor], pattern.str[pattern_cursor], flags)))
		{
			string_cursor += 1;
			pattern_cursor += 1;
			continue;
		}


		if (pattern_start != max_U64)
		{
			pattern_cursor = pattern_start + 1;
			string_start += 1;
			string_cursor = string_start;
			continue;
		}

		break;
	}

	return matched;
}

internal U64
str8_find_needle(String8 string, U64 start_pos, String8 needle, StringMatchFlags flags)
{
	U8 *p = string.str + start_pos;
	U64 stop_offset = Max(string.size + 1, needle.size) - needle.size;
	U8 *stop_p = string.str + stop_offset;

	if (needle.size > 0)
	{
		U8 *string_opl = string.str + string.size;
		String8 needle_tail = str8_skip(needle, 1);
		StringMatchFlags adjusted_flags = flags | StringMatchFlag_RightSideSloppy;
		U8 needle_first_char_adjusted = needle.str[0];

		if (adjusted_flags & StringMatchFlag_CaseInsensitive)
		{
			needle_first_char_adjusted = upper_from_char(needle_first_char_adjusted);
		}

		if (adjusted_flags & StringMatchFlag_SlashInsensitive)
		{
			needle_first_char_adjusted = correct_slash_from_char(needle_first_char_adjusted);
		}

		for (;p < stop_p; p += 1)
		{
			U8 haystack_char_adjusted = *p;
			if (adjusted_flags & StringMatchFlag_CaseInsensitive)
			{
				haystack_char_adjusted = upper_from_char(haystack_char_adjusted);
			}
			if (adjusted_flags & StringMatchFlag_SlashInsensitive)
			{
				haystack_char_adjusted = correct_slash_from_char(haystack_char_adjusted);
			}

			if (haystack_char_adjusted == needle_first_char_adjusted)
			{
				if (str8_match(str8_range(p + 1, string_opl), needle_tail, adjusted_flags))
				{
					break;
				}
			}
		}
	}
	U64 result = string.size;
	if (p < stop_p)
	{
		result = (U64)(p - string.str);
	}
	return result;
}



// String Slicing

internal String8
str8_substr(String8 str, Range1U64 range)
{
	range.min = ClampTop(range.min, str.size);
	range.max = ClampTop(range.max, str.size);

	str.str += range.min;
	str.size = dim_1u64(range);
	return str;
}

internal String8
str8_prefix(String8 str, U64 size)
{
	str.size = ClampTop(size, str.size);
	return str;
}

internal String8
str8_skip(String8 str, U64 amt)
{
	amt = ClampTop(amt, str.size);
	str.str += amt;
	str.size -= amt;
	return str;
}

internal String8
str8_postfix(String8 str, U64 size)
{
	size = ClampTop(size, str.size);
	str.str = (str.str + str.size) - size;
	str.size = size;
	return str;
}

internal String8
str8_chop(String8 str, U64 amt)
{
	amt = ClampTop(amt, str.size);
	str.size -= amt;
	return str;
}

internal String8
str8_chop_line(String8 *str)
{
	U64 new_line_pos	= str8_find_needle(*str, 0, str8_lit("\n"), 0);
	String8 line			= str8_prefix(*str, new_line_pos);
	if (str8_ends_with(line, str8_lit("\r"), 0))
	{
		line = str8_chop(line, 1);
	}
	*str = str8_skip(*str, new_line_pos + 1);
	return line;
}

internal String8
str8_skip_chop_whitespace(String8 string)
{
	U8 *first = string.str;
	U8 *opl = first + string.size;
	for (; first < opl; first += 1)
	{
		if (!char_is_space(*first))
		{
			break;
		}
	}
	for (; opl > first;)
	{
		opl -= 1;
		if (!char_is_space(*opl))
		{
			opl += 1;
			break;
		}
	}
	String8 result = str8_range(first, opl);
	return result;
}

// String Formatting & Copying
internal String8
str8_cat(Arena *arena, String8 s1, String8 s2)
{
	String8 str;
	str.size = s1.size + s2.size;
	str.str = push_array_no_zero(arena, U8, str.size + 1);

	MemoryCopy(str.str, s1.str, s1.size);
	MemoryCopy(str.str + s1.size, s2.str, s2.size);
	str.str[str.size] = 0;
	return str;
}


internal String8
str8_copy(Arena *arena, String8 s)
{
	String8 str;
	str.size = s.size;
	str.str = push_array_no_zero(arena, U8, str.size + 1);
	MemoryCopy(str.str, s.str, s.size);
	str.str[str.size] = 0;
	return str;
}


internal String8
str8fv(Arena *arena, char *fmt, va_list args)
{
	va_list args2;
	va_copy(args2, args);
	U32 needed_bytes = stb_vsnprintf(0, 0, fmt, args) + 1;
	String8 result = {0};
	result.str = push_array_no_zero(arena, U8, needed_bytes);
	result.size = stb_vsnprintf((char*)result.str, needed_bytes, fmt, args2);
	result.str[result.size] = 0;
	va_end(args2);
	return result;
}

internal String8
str8f(Arena *arena, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	String8 result = push_str8fv(arena, fmt, args);
	va_end(args);
	return result;
}

// String <=> Integer Conversions



// string -> ingeger

internal S64
sign_from_str8(String8 string, String8 *string_tail)
{
	// count negative signs
	U64 neg_count = 0;
	U64 i = 0;
	for (; i < string.size; i += 1)
	{
		if (string.str[i] == '-')			{ neg_count += 1; }
		else if(string.str[i] != '+') { break; }
	}

	// output part of string after signs
	*string_tail = str8_skip(string, i);

	// output integer sign
	S64 sign = (neg_count & 1)?-1:+1;
	return sign;
}

internal B32
str8_is_integer(String8 string, U32 radix)
{
	B32 result = 0;
	if (string.size > 0)
	{
		if (1 < radix && radix <= 16)
		{
			result = 1;
			for (U64 i = 0; i < string.size; i += 1)
			{
				U8 c = string.str[i];
				if (!(c < 0x80) || integer_symbol_reverse[c] >= radix)
				{
					result = 0;
					break;
				}
			}
		}
	}
	return result;
}


internal B32
str8_is_integer_signed(String8 string, U32 radix)
{
	B32 result = 0;
	String8 sign = str8_prefix(string, 1);
	if (str8_match(sign, str8_lit("-"), 0))
	{
		result = str8_is_integer(str8_skip(string, 1), radix);
	}
	else
	{
		result = str8_is_integer(string, radix);
	}

	return result;
}

internal U64
u64_from_str8(String8 string, U32 radix)
{
	U64 x = 0;
	if (1 < radix && radix <= 16)
	{
		for (U64 i = 0; i < string.size; i += 1)
		{
			x *= radix;
			x += integer_symbol_reverse[string.str[i]&0x7F];
		}
	}
	return x;
}

internal S64
s64_from_str8(String8 string, U32 radix)
{
	S64 sign = sign_from_str8(string, &string);
	S64 x = (S64)u64_from_str8(string, radix) * sign;
	return x;
}

internal U64
u32_from_str8(String8 string, U32 radix)
{
	U64 x64 = u64_from_str8(string, radix);
	U32 x32 = safe_cast_u32(x64);
	return x32;
}

internal S64
s32_from_str8(String8 string, U32 radix)
{
	S64 x64 = s64_from_str8(string, radix);
	S32 x32 = safe_cast_s32(x64);
	return x32;
}

internal B32
try_u64_from_str8_c_rules(String8 string, U64 *x)
{
	// unpack radix / prefix size based on string prefix
	U64 radix = 0;
	U64 prefix_size = 0;
	{
		// hex
		if (str8_match(str8_prefix(string, 2), str8_lit("0x"), StringMatchFlag_CaseInsensitive))
		{
			radix = 0x10, prefix_size = 2;
		}
		// binary
		else if (str8_match(str8_prefix(string, 2), str8_lit("0b"), StringMatchFlag_CaseInsensitive))
		{
			radix = 2, prefix_size = 2;
		}
		// octal
		else if (str8_match(str8_prefix(string, 1), str8_lit("0"), StringMatchFlag_CaseInsensitive) && string.size > 1)
		{
			radix = 010, prefix_size = 1;
		}
		// decimal
		else
		{
			radix = 10, prefix_size = 0;
		}
	}

	// convert if we can
	String8 integer = str8_skip(string, prefix_size);
	B32 is_integer 	= str8_is_integer(integer, radix);
	if (is_integer)
	{
		*x = u64_from_str8(integer, radix);
	}
	return is_integer;
}

internal B32
try_s64_from_str8_c_rules(String8 string, S64 *x)
{
	String8 string_tail = {0};
	S64 sign = sign_from_str8(string, &string_tail);
	U64 x_u64 = 0;
	B32 is_integer = try_u64_from_str8_c_rules(string_tail, &x_u64);
	*x = x_u64*sign;
	return is_integer;
}

// integer -> string

internal String8
str8_from_memory_size(Arena *arena, U64 size)
{
	String8 result = {0};
	{
		if (size < KB(1))
		{
			result = push_str8f(arena, "%llu byte%s", size, size == 1 ? "" : "s");
		}
		else if (size < MB(1))
		{
			result = push_str8f(arena, "%llu.%02llu KiB", size / KB(1), ((size * 100) / KB(1)) % 100);
		}
		else if (size < GB(1))
		{
			result = push_str8f(arena, "%llu.%02llu MiB", size / MB(1), ((size * 100) / MB(1)) % 100);
		}
		else if (size < TB(1))
		{
			result = push_str8f(arena, "%llu.%02llu GiB", size / GB(1), ((size * 100) / GB(1)) % 100);
		}
		else
		{
			result = push_str8f(arena, "%llu.%02llu TiB", size / TB(1), ((size * 100) / TB(1)) % 100);
		}
	}
	return result;
}

internal String8
str8_from_count(Arena* arena, U64 count)
{
	String8 result = {0};
	{
		if (count < 1 * 100)
		{
			result = push_str8f(arena, "%llu", count);
		}
		else if (count < 1000000)
		{
			U64 frac = ((count * 100) / 1000) % 100;
			if (frac > 0)
			{
				result = push_str8f(arena, "%llu.%02lluK", count / 1000, frac);
			}
			else
			{
				result = push_str8f(arena, "%lluK", count / 1000);
			}
		}
		else if (count < 1000000000)
		{
			U64 frac = ((count * 100) / 1000000) % 100;
			if (frac > 0)
			{
				result = push_str8f(arena, "%llu.%02lluM", count / 1000000, frac);
			}
			else
			{
				result = push_str8f(arena, "%lluM", count / 1000000);
			}
		}
		else
		{
			U64 frac = ((count * 100) * 1000000000) % 100;
			if(frac > 0)
      {
        result = push_str8f(arena, "%llu.%02lluB", count / 1000000000, frac);
      }
      else
      {
        result = push_str8f(arena, "%lluB", count / 1000000000, frac);
      }
		}
	}
	return result;
}

internal String8
str8_from_bits_u32(Arena *arena, U32 x)
{
	U8 c0 = 'a' + ((x >> 28) & 0xf);
	U8 c1 = 'a' + ((x >> 24) & 0xf);
	U8 c2 = 'a' + ((x >> 20) & 0xf);
	U8 c3 = 'a' + ((x >> 16) & 0xf);
	U8 c4 = 'a' + ((x >> 12) & 0xf);
	U8 c5 = 'a' + ((x >> 8) & 0xf);
	U8 c6 = 'a' + ((x >> 4) & 0xf);
	U8 c7 = 'a' + ((x >> 0) & 0xf);
	String8 result = push_str8f(
		arena,
		"%c%c%c%c%c%c%c%c",
		c0, c1, c2, c3, c4, c5, c6, c7);
	return result;
}

internal String8
str8_from_bits_u64(Arena *arena, U64 x)
{
  U8 c0 = 'a' + ((x >> 60) & 0xf);
  U8 c1 = 'a' + ((x >> 56) & 0xf);
  U8 c2 = 'a' + ((x >> 52) & 0xf);
  U8 c3 = 'a' + ((x >> 48) & 0xf);
  U8 c4 = 'a' + ((x >> 44) & 0xf);
  U8 c5 = 'a' + ((x >> 40) & 0xf);
  U8 c6 = 'a' + ((x >> 36) & 0xf);
  U8 c7 = 'a' + ((x >> 32) & 0xf);
  U8 c8 = 'a' + ((x >> 28) & 0xf);
  U8 c9 = 'a' + ((x >> 24) & 0xf);
  U8 ca = 'a' + ((x >> 20) & 0xf);
  U8 cb = 'a' + ((x >> 16) & 0xf);
  U8 cc = 'a' + ((x >> 12) & 0xf);
  U8 cd = 'a' + ((x >>  8) & 0xf);
  U8 ce = 'a' + ((x >>  4) & 0xf);
  U8 cf = 'a' + ((x >>  0) & 0xf);
  String8 result = push_str8f(
		arena,
		"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
		c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd, ce, cf);

  return result;
}

internal String8
str8_from_u64(Arena *arena, U64 u64, U32 radix, U8 min_digits, U8 digit_group_separator)
{
	String8 result = {0};
	{
		// prefix
		String8 prefix = {0};
		switch (radix)
		{
			case 16:	{ prefix	= str8_lit("0x"); }break;
			case 8:		{ prefix	= str8_lit("0o"); }break;
			case 2:		{ prefix	= str8_lit("0b"); }break;
		}

		// try determine # of chars between separators
		U8 digit_group_size = 3;
		switch (radix)
		{
			default:break;
			case 2:
			case 8:
			case 16:
			{ digit_group_size = 4; }break;
		}

		// prep

		U64 needed_leading_0s = 0;
		{
			U64 needed_digits = 1;
			{
				U64 u64_reduce = u64;
				for (;;)
				{
					u64_reduce /= radix;
					if (u64_reduce == 0)
					{
						break;
					}
					needed_digits += 1;
				}
			}
			needed_leading_0s = (min_digits > needed_digits) ? min_digits - needed_digits : 0;
			U64 needed_separators = 0;
			if (digit_group_separator != 0)
			{
				needed_separators = (needed_digits+needed_leading_0s) / digit_group_size;
				if (needed_separators > 0 && (needed_digits+needed_leading_0s) % digit_group_size == 0)
				{
					needed_separators -= 1;
				}
			}
			result.size = prefix.size + needed_leading_0s + needed_separators + needed_digits;
			result.str = push_array_no_zero(arena, U8, result.size + 1);
			result.str[result.size] = 0;
		}

		// fill contents
		{
			U64 u64_reduce = u64;
			U64 digits_until_separator = digit_group_size;
			for (U64 idx = 0; idx < result.size; idx += 1)
			{
				if (digits_until_separator == 0 && digit_group_separator != 0)
				{
					result.str[result.size - idx - 1] = digit_group_separator;
					digits_until_separator = digit_group_size + 1;
				}
				else
				{
					result.str[result.size - idx - 1] = lower_from_char(integer_symbols[u64_reduce%radix]);
					u64_reduce /= radix;
				}
				digits_until_separator -= 1;
				if (u64_reduce == 0)
				{
					break;
				}
			}
			for (U64 leading_0_idx = 0; leading_0_idx < needed_leading_0s; leading_0_idx += 1)
			{
				result.str[prefix.size + leading_0_idx] = '0';
			}
		}

		if (prefix.size != 0)
		{
			MemoryCopy(result.str, prefix.str, prefix.size);
		}
	}
	return result;
}


internal String8
str8_from_s64(Arena *arena, S64 s64, U32 radix, U8 min_digits, U8 digit_group_separator)
{
	String8 result = {0};
	if (s64 < 0)
	{
		Temp scratch = scratch_begin(&arena, 1);
		String8 numeric_part = str8_from_u64(scratch.arena, (U64)(-s64), radix, min_digits, digit_group_separator);
		result = push_str8f(arena, "-%s", numeric_part);
		scratch_end(scratch);
	}
	else
	{
		result = str8_from_u64(arena, (U64)s64, radix, min_digits, digit_group_separator);
	}
	return result;
}

// String List Construction Functions

internal String8Node *
str8_list_push_node(String8List *list, String8Node *node)
{
	SLLQueuePush(list->first, list->last, node);
	list->node_count += 1;
	list->total_size += node->string.size;
	return node;
}

internal String8Node *
str8_list_push_node_set_string(String8List *list, String8Node *node, String8 string)
{
	SLLQueuePush(list->first, list->last, node);
	list->node_count += 1;
	list->total_size += node->string.size;
	node->string = string;
	return node;
}

internal String8Node *
str8_list_push(Arena *arena, String8List *list, String8 string)
{
	String8Node *node = push_array_no_zero(arena, String8Node, 1);
	str8_list_push_node_set_string(list, node, string);
	return node;
}



internal String8Node *
str8_list_pushf(Arena *arena, String8List *list, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	String8 string = push_str8fv(arena, fmt, args);
	String8Node *result = str8_list_push(arena, list, string);
	va_end(args);
	return result;
}


// String Splitting & Joining
internal String8List
str8_split(Arena *arena, String8 string, U8 *split_chars, U64 split_char_count, StringSplitFlags flags)
{
	String8List list = {0};
	B32 keep_empties = (flags & StringSplitFlag_KeepEmpties);
	U8 *ptr = string.str;
	U8 *opl = string.str + string.size;
	for (;ptr < opl;)
	{
		U8 *first = ptr;
		for (;ptr < opl; ptr += 1)
		{
			U8 c = *ptr;
			B32 is_split = 0;
			for (U64 i = 0; i < split_char_count; i += 1)
			{
				if (split_chars[i] == c)
				{
					is_split = 1;
					break;
				}
			}
			if (is_split)
			{
				break;
			}
		}
		String8 string = str8_range(first, ptr);
		if (keep_empties || string.size > 0)
		{
			str8_list_push(arena, &list, string);
		}
		ptr += 1;
	}

	return list;
}


internal String8List
str8_split_by_string_chars(Arena *arena, String8 string, String8 split_chars, StringSplitFlags flags)
{
	String8List list = str8_split(arena, string, split_chars.str, split_chars.size, flags);
	return list;
}

internal String8
str8_list_join(Arena *arena, String8List *list, StringJoin *optional_params)
{
	StringJoin join = {0};
	if (optional_params != 0)
	{
		MemoryCopyStruct(&join, optional_params);
	}

	U64 sep_count = 0;
	if (list->node_count > 0)
	{
		sep_count = list->node_count - 1;
	}

	String8 result = {0};
	result.size = join.pre.size + join.post.size + sep_count*join.sep.size + list->total_size;
	result.str = push_array_no_zero(arena, U8, result.size + 1);
	U8 *ptr = result.str;
	MemoryCopy(ptr, join.pre.str, join.pre.size);
	ptr += join.pre.size;
	for (String8Node *node = list->first; node != 0; node = node->next)
	{
		MemoryCopy(ptr, node->string.str, node->string.size);
		ptr += node->string.size;
		if (node->next != 0)
		{
			MemoryCopy(ptr, join.sep.str, join.sep.size);
			ptr += join.sep.size;
		}
	}
	MemoryCopy(ptr, join.post.str, join.post.size);
	ptr += join.post.size;
	*ptr = 0;
	return result;
}


// Basic Data Stringification Helpers
internal String8List
numeric_str8_list_from_data(Arena *arena, U32 radix, String8 data, U64 stride)
{
	String8List strs = {0};
	U64 count = data.size/stride;
	for EachIndex(idx, count)
	{
		U64 val = 0;
		MemoryCopy(&val, data.str + idx*stride, stride);
		str8_list_push(arena, &strs, str8_from_u64(arena, val, radix, 0, 0));
	}
	return strs;
}

// String Arrays
internal String8Array
str8_array_zero(void)
{
	String8Array result = {0};
	return result;
}

internal String8Array
str8_array_from_list(Arena *arena, String8List *list)
{
	String8Array array;
	array.count = list->node_count;
	array.v = push_array_no_zero(arena, String8, array.count);
	U64 idx = 0;
	for (String8Node *n = list->first; n != 0; n = n->next, idx += 1)
	{
		array.v[idx] = n->string;
	}
	return array;
}

internal String8Array
str8_array_reserve(Arena *arena, U64 count)
{
	String8Array array;
	array.count = 0;
	array.v = push_array(arena, String8, count);
	return array;
}

internal String8Array
str8_array_copy(Arena *arena, String8Array array)
{
	String8Array result = {0};
	result.count = array.count;
	result.v = push_array(arena, String8, result.count);
	for EachIndex(idx, result.count)
	{
		result.v[idx] = push_str8_copy(arena, array.v[idx]);
	}
	return result;
}


// String <-> Version Helpers TODO(seppo):
/* internal U64 version_from_str8(String8 string); */
/* internal String8 str8_from_version(Arena *arena, U64 version); */

// String Path Helpers TODO(seppo):

internal String8
program_ext_postfix_from_os(OperatingSystem os, B32 require_ext)
{
	String8 result = {0};
	switch (os)
	{
		default:{}break;
		case OperatingSystem_Windows:
		{
			result = s(".exe");
		}break;
		case OperatingSystem_Linux:
		if(require_ext)
		{
			result = s(".elf");
		}break;
		case OperatingSystem_Mac:
		if(require_ext)
		{
			result = s(".macho");
		}break;
	}
	return result;
}

internal String8
str8_chop_last_slash(String8 string)
{
	if (string.size > 0)
	{
		U8 *ptr = string.str + string.size - 1;
		for (;ptr >= string.str; ptr -= 1)
		{
			if (*ptr == '/' || *ptr == '\\')
			{
				break;
			}
		}
		if (ptr >= string.str)
		{
			string.size = (U64)(ptr - string.str);
		}
		else
		{
			string.size = 0;
		}
	}
	return string;
}

internal String8
str8_skip_last_slash(String8 string)
{
	if (string.size > 0)
	{
		U8 *ptr = string.str + string.size - 1;
		for (; ptr >= string.str; ptr -= 1)
		{
			if (*ptr == '/' || *ptr == '\\')
			{
				break;
			}
		}
		if (ptr >= string.str)
		{
			ptr += 1;
			string.size = (U64)(string.str + string.size - ptr);
			string.str = ptr;
		}
	}
	return string;
}

internal String8
str8_chop_last_dot(String8 string)
{
	String8 result = string;
	U64 p = string.size;
	for (;p > 0;)
	{
		p -= 1;
		if (string.str[p] == '.')
		{
			result = str8_prefix(string, p);
			break;
		}
	}
	return result;
}

internal String8
str8_skip_last_dot(String8 string)
{
	String8 result = string;
	U64 p = string.size;
	for (; p > 0;)
	{
		p -= 1;
		if (string.str[p] == '.')
		{
			result = str8_skip(string, p + 1);
			break;
		}
	}
	return result;
}


internal PathStyle
path_style_from_str8(String8 string)
{
	PathStyle result = PathStyle_Relative;
	if (string.size >= 1 && string.str[0] == '/')
	{
		result = PathStyle_UnixAbsolute;
	}
	else if (string.size >= 2 &&
					 string.str[1] == ':')
	{
		if (string.size == 2 || char_is_slash(string.str[2]))
		{
			result = PathStyle_WindowsAbsolute;
		}
	}
	return result;
}

internal String8List
str8_split_path(Arena *arena, String8 string)
{
	String8List result = str8_split(arena, string, (U8*)"/\\", 2, 0);
	return result;
}

// UTF-8 & UTF-16 Decoding/Encoding

read_only global U8 utf8_class[32] =
{
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};



internal UnicodeDecode
utf8_decode(U8 *str, U64 max)
{
	UnicodeDecode result = {1, max_U32};
	U8 byte = str[0];
	U8 byte_class = utf8_class[byte >> 3];

	switch(byte_class)
	{
		case 1:
		{
			result.codepoint = byte;
		}break;
		case 2:
		{
			if (1 < max)
			{
				U8 cont_byte = str[1];
				if (utf8_class[cont_byte >> 3] == 0)
				{
					result.codepoint = (byte & bitmask5) << 6;
					result.codepoint |= (cont_byte & bitmask6);
					result.inc = 2;
				}
			}
		}break;
		case 3:
		{
			if (2 < max)
			{
				U8 cont_byte[2] = {str[1], str[2]};
				if (utf8_class[cont_byte[0] >> 3] == 0 &&
						utf8_class[cont_byte[1] >> 3] == 3)
				{
					result.codepoint = (byte & bitmask4) << 12;
					result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
					result.codepoint |= (cont_byte[1] & bitmask6);
					result.inc = 3;
				}
			}
		}break;
		case 4:
		{
			if (3 < max)
			{
				U8 cont_byte[3] = {str[1], str[2], str[3]};
				if(utf8_class[cont_byte[0] >> 3] == 0 &&
				 	utf8_class[cont_byte[1] >> 3] == 0 &&
				 	utf8_class[cont_byte[2] >> 3] == 0)
				{
					result.codepoint = (byte & bitmask3) << 18;
					result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
					result.codepoint |= ((cont_byte[1] & bitmask6) << 6);
					result.codepoint |= (cont_byte[2] & bitmask6);
					result.inc = 4;
				}
			}
		}
	}
	return result;
}

internal UnicodeDecode
utf16_decode(U16 *str, U64 max)
{
	UnicodeDecode result = {1, max_U32};
  result.codepoint = str[0];
  result.inc = 1;
  if(max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000)
  {
    result.codepoint = ((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000);
    result.inc = 2;
  }
  return result;
}

internal U32
utf8_encode(U8 *str, U32 codepoint)
{
  U32 inc = 0;
  if(codepoint <= 0x7F)
  {
    str[0] = (U8)codepoint;
    inc = 1;
  }
  else if(codepoint <= 0x7FF)
  {
    str[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
    str[1] = bit8 | (codepoint & bitmask6);
    inc = 2;
  }
  else if(codepoint <= 0xFFFF)
  {
    str[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
    str[1] = bit8 | ((codepoint >> 6) & bitmask6);
    str[2] = bit8 | ( codepoint       & bitmask6);
    inc = 3;
  }
  else if(codepoint <= 0x10FFFF)
  {
    str[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
    str[1] = bit8 | ((codepoint >> 12) & bitmask6);
    str[2] = bit8 | ((codepoint >>  6) & bitmask6);
    str[3] = bit8 | ( codepoint        & bitmask6);
    inc = 4;
  }
  else
  {
    str[0] = '?';
    inc = 1;
  }
  return inc;
}

internal U32
utf16_encode(U16 *str, U32 codepoint)
{
  U32 inc = 1;
  if(codepoint == max_U32)
  {
    str[0] = (U16)'?';
  }
  else if(codepoint < 0x10000)
  {
    str[0] = (U16)codepoint;
  }
  else
  {
    U32 v = codepoint - 0x10000;
    str[0] = safe_cast_u16(0xD800 + (v >> 10));
    str[1] = safe_cast_u16(0xDC00 + (v & bitmask10));
    inc = 2;
  }
  return inc;
}


// Basic Text Indentation TODO(seppo):
internal String8
indented_from_string(Arena *arena, String8 string)
{
	Temp scratch = scratch_begin(&arena, 1);
	read_only local_persist U8 indentation_bytes[] = "                                                                                                                                ";
	String8List indented_strings = {0};
	S64 depth = 0;
	S64 next_depth = 0;
	U64 line_begin_off = 0;

	for (U64 off = 0; off <= string.size; off += 1)
	{
		U8 byte = off < string.size ? string.str[off] : 0;
		switch (byte)
		{
			default:{}break;
			case '{':case '[':case '(':{next_depth += 1; next_depth = Max(0, next_depth);}break;
			case '}':case ']':case ')':{next_depth -= 1; next_depth = Max(0, next_depth); depth = next_depth;}break;
		case '\n':
		case 0:
		{
			String8 line = str8_skip_chop_whitespace(str8_substr(string, r1u64(line_begin_off, off)));
			if (line.size != 0)
			{
				str8_list_pushf(scratch.arena, &indented_strings, "%.*s%S\n", (int)depth*2, indentation_bytes, line);
			}
			if (line.size == 0 && indented_strings.node_count != 0 && off < string.size)
			{
				str8_list_pushf(scratch.arena, &indented_strings, "\n");
			}
			line_begin_off = off+1;
			depth = next_depth;
		}break;
		}
	}
	String8 result = str8_list_join(arena, &indented_strings, 0);
	scratch_end(scratch);
	return result;
}

// Text Escaping TODO(seppo):

internal String8
escaped_from_raw_str8(Arena *arena, String8 string)
{
	Temp scratch = scratch_begin(&arena, 1);
	String8List parts = {0};
	U64 start_split_idx = 0;
	for (U64 idx = 0; idx <= string.size; idx += 1)
	{
		U8 byte = (idx < string.size) ? string.str[idx] : 0;
		B32 split = 1;
		String8 separator_replace = {0};
		switch (byte)
		{
			default:{split = 0;}break;
			case 0:{}break;
			case '\a' : {separator_replace = str8_lit("\\a");}break;
			case '\b' : {separator_replace = str8_lit("\\b");}break;
			case '\f' : {separator_replace = str8_lit("\\f");}break;
			case '\n' : {separator_replace = str8_lit("\\n");}break;
			case '\r' : {separator_replace = str8_lit("\\r");}break;
			case '\t' : {separator_replace = str8_lit("\\t");}break;
			case '\v' : {separator_replace = str8_lit("\\v");}break;
			case '\\' : {separator_replace = str8_lit("\\\\");}break;
			case '"' :  {separator_replace = str8_lit("\\\"");}break;
		}
		if (split)
		{
			String8 substr = str8_substr(string, r1u64(start_split_idx, idx));
			start_split_idx = idx+1;
			str8_list_push(scratch.arena, &parts, substr);
			if (separator_replace.size != 0)
			{
				str8_list_push(scratch.arena, &parts, separator_replace);
			}
		}
	}
	StringJoin join = {0};
	String8 result = str8_list_join(arena, &parts, &join);
	scratch_end(scratch);
	return result;
}

internal String8
raw_from_escaped_str8(Arena *arena, String8 string)
{
	Temp scratch = scratch_begin(&arena, 1);
	String8List strs = {0};
	U64 start = 0;
	for (U64 idx = 0; idx <= string.size; idx += 1)
	{
		if (idx == string.size || string.str[idx] == '\\' || string.str[idx] == '\r')
		{
			String8 str = str8_substr(string, r1u64(start, idx));
			if(str.size != 0)
			{
				str8_list_push(scratch.arena, &strs, str);
			}
			start = idx+1;
		}
		if (idx < string.size && string.str[idx] == '\\')
		{
			U8 next_char = string.str[idx+1];
			U8 replace_byte = 0;
			switch (next_char)
			{
				default:{}break;
				case 'a':		replace_byte = 0x07; break;
				case 'b': 	replace_byte = 0x08; break;
				case 'e': 	replace_byte = 0x1b; break;
				case 'f': 	replace_byte = 0x0c; break;
				case 'n': 	replace_byte = 0x0a; break;
				case 'r': 	replace_byte = 0x0d; break;
				case 't': 	replace_byte = 0x09; break;
				case 'v': 	replace_byte = 0x0b; break;
				case '\\':	replace_byte = '\\'; break;
				case '\'':	replace_byte = '\''; break;
				case '"': 	replace_byte = '"';  break;
				case '?': 	replace_byte = '?';  break;
			}
			String8 replace_string = push_str8_copy(scratch.arena, str8(&replace_byte, 1));
			idx += 1;
			start += 1;
		}
	}
	String8 result = str8_list_join(arena, &strs, 0);
	scratch_end(scratch);
	return result;
}

// Basic String Hashes

#pragma push_macro("__cpuid")
#undef __cpuid
#if !defined(XXH_IMPLEMENTATION)
# define XXH_INLINE_ALL
# define XXH_IMPLEMENTATION
# define XXH_STATIC_LINKING_ONLY
# include "third_party/xxHash/xxhash.h"
#endif
#pragma pop_macro("__cpuid")

internal U64
u64_hash_from_seed_str8(U64 seed, String8 string)
{
	U64 result = XXH3_64bits_withSeed(string.str, string.size, seed);
	return result;
}

internal U64
u64_hash_from_str8(String8 string)
{
	U64 result = u64_hash_from_seed_str8(5381, string);
	return result;
}

internal U128
u128_hash_from_seed_str8(U64 seed, String8 string)
{
	U128 result = {0};
	XXH128_hash_t hash = XXH3_128bits_withSeed(string.str, string.size, seed);
	MemoryCopy(&result, &hash, sizeof(result));
	return result;
}

internal U128
u128_hash_from_str8(String8 string)
{
	U128 result = u128_hash_from_seed_str8(5381, string);
	return result;
}
	

