// Safe Casts


internal U16
safe_cast_u16(U32 x)
{
	AssertAlways(x <= max_U16);
	U16 result = (U16)x;
	return result;
}


internal U32
safe_cast_u32(U64 x)
{
	AssertAlways(x <= max_U32);
	U32 result = (U32)x;
	return result;
}

internal S32
safe_cast_s32(S64 x)
{
	AssertAlways(x <= max_S32);
	S32 result = (S32)x;
	return result;
}


// Memory Functions

internal B32
memory_is_zero(void* ptr, U64 size)
{
	B32 result = 1;

	U64 extra = (size & 0x7);
	U64 count8 = (size >> 3);

	U64* p64 = (U64*)ptr;
	if (result)
	{
		for (U64 i = 0; i < count8; i+= 1, p64 += 1)
		{
			if (*p64 != 0)
			{
				result = 0;
				goto done;
			}
		}
	}

	if(result)
	{
		U8* p8 = (U8*)p64;
		for (U64 i = 0; i < extra; i+= 1, p8 += 1)
		{
			if (*p8 != 0)
			{
				result = 0;
				goto done;
			}
		}
	}

	done:;
	return result;
}


// Write
internal void UBSAN_NO_ALIGN
memory_write16(void *ptr, U16 v)
{
	MemoryCopy(ptr, &v, sizeof(v));
}

internal void UBSAN_NO_ALIGN
memory_write32(void *ptr, U32 v)
{
	MemoryCopy(ptr, &v, sizeof(v));
}

internal void UBSAN_NO_ALIGN
memory_write64(void *ptr, U64 v)
{
	MemoryCopy(ptr, &v, sizeof(v));
}

// Read
internal U8 UBSAN_NO_ALIGN
memory_read8(void *ptr)
{
	U8 result;
	MemoryCopy(&result, ptr, sizeof(result));
	return result;
}

internal U16 UBSAN_NO_ALIGN
memory_read16(void *ptr)
{
	U16 result;
	MemoryCopy(&result, ptr, sizeof(result));
	return result;
}

internal U32 UBSAN_NO_ALIGN
memory_read32(void *ptr)
{
	U32 result;
	MemoryCopy(&result, ptr, sizeof(result));
	return result;

}

internal U64 UBSAN_NO_ALIGN
memory_read64(void *ptr)
{
	U64 result;
	MemoryCopy(&result, ptr, sizeof(result));
	return result;
}


// Text 2D Coordinates/Range Functions TODO(seppo):

internal TxtPt
txt_pt(S64 line, S64 column)
{
	TxtPt p = {0};
	p.line = line;
	p.column = column;
	return p;
}

internal B32
txt_pt_match(TxtPt a, TxtPt b)
{
	return a.line == b.line && a.column == b.column;
}

internal B32
txt_pt_less_than(TxtPt a, TxtPt b)
{
	B32 result = 0;
	if(a.line < b.line)
	{
		result = 1;
	}
	else if (a.line == b.line)
	{
		result = a.column < b.column;
	}
	return result;
}

internal TxtPt
txt_pt_min(TxtPt a, TxtPt b)
{
	TxtPt result = b;
	if (txt_pt_less_than(a, b))
	{
		result = a;
	}
	return result;
}

internal TxtPt
txt_pt_max(TxtPt a, TxtPt b)
{
	TxtPt result = a;
	if (txt_pt_less_than(a, b))
	{
		result = b;
	}
	return result;
}

internal TxtRng
txt_rng(TxtPt min, TxtPt max)
{
	TxtRng range = {0};
	if (txt_pt_less_than(min, max))
	{
		range.min = min;
		range.max = max;
	}
	else
	{
		range.min = max;
		range.max = min;
	}
	return range;
}

internal TxtRng
txt_rng_intersect(TxtRng a, TxtRng b)
{
	TxtRng result = {0};
	result.min = txt_pt_max(a.min, b.min);
	result.max = txt_pt_min(a.max, b.max);
	if (txt_pt_less_than(result.max, result.min))
	{
		MemoryZeroStruct(&result);
	}
	return result;
}

internal TxtRng
txt_rng_union(TxtRng a, TxtRng b)
{
	TxtRng result = {0};
	result.min = txt_pt_min(a.min, b.min);
	result.max = txt_pt_max(a.max, b.max);
	return result;
}

internal B32
txt_rng_contains(TxtRng r, TxtPt pt)
{
	B32 result = ((txt_pt_less_than(r.min, pt) || txt_pt_match(r.min, pt)) &&
								txt_pt_less_than(pt, r.max));
	return result;
}


// Time Functions

internal DenseTime
dense_time_from_date_time(DateTime date_time)
{
	DenseTime result = 0;
	result += date_time.year;
	result *= 12;
	result += date_time.mon;
	result *= 31;
	result += date_time.day;
	result *= 24;
	result += date_time.hour;
	result *= 60;
	result += date_time.min;
	result *= 61;
	result += date_time.sec;
	result *= 1000;
	result += date_time.msec;
	return result;
}

internal DateTime
date_time_from_dense_time(DenseTime time)
{
  DateTime result = {0};
  result.msec = time%1000;
  time /= 1000;
  result.sec  = time%61;
  time /= 61;
  result.min  = time%60;
  time /= 60;
  result.hour = time%24;
  time /= 24;
  result.day  = time%31;
  time /= 31;
  result.mon  = time%12;
  time /= 12;
  Assert(time <= max_U32);
  result.year = (U32)time;
  return result;
}

internal DateTime
date_time_from_micro_seconds(U64 time)
{
  DateTime result = {0};
  result.micro_sec = time%1000;
  time /= 1000;
  result.msec = time%1000;
  time /= 1000;
  result.sec = time%60;
  time /= 60;
  result.min = time%60;
  time /= 60;
  result.hour = time%24;
  time /= 24;
  result.day = time%31;
  time /= 31;
  result.mon = time%12;
  time /= 12;
  Assert(time <= max_U32);
  result.year = (U32)time;
  return(result);
}

internal DateTime date_time_from_unix_time(U64 unix_time)
{
  DateTime date = {0};
  date.year     = 1970;
  date.day      = 1 + (unix_time / 86400);
  date.sec      = (U32)unix_time % 60;
  date.min      = (U32)(unix_time / 60) % 60;
  date.hour     = (U32)(unix_time / 3600) % 24;
  
  for(;;)
  {
    for(date.month = 0; date.month < 12; ++date.month)
    {
      U64 c = 0;
      switch(date.month)
      {
        case Month_Jan: c = 31; break;
        case Month_Feb:
        {
          if((date.year % 4 == 0) && ((date.year % 100) != 0 || (date.year % 400) == 0))
          {
            c = 29;
          }
          else
          {
            c = 28;
          }
        } break;
        case Month_Mar: c = 31; break;
        case Month_Apr: c = 30; break;
        case Month_May: c = 31; break;
        case Month_Jun: c = 30; break;
        case Month_Jul: c = 31; break;
        case Month_Aug: c = 31; break;
        case Month_Sep: c = 30; break;
        case Month_Oct: c = 31; break;
        case Month_Nov: c = 30; break;
        case Month_Dec: c = 31; break;
        default: InvalidPath;
      }
      if(date.day <= c)
      {
        goto exit;
      }
      date.day -= c;
    }
    ++date.year;
  }
  exit:;
  
  return date;
}


// Wrapped Ring Buffer Reads/Writes

internal U64
wrapped_write(U8 *ring_base, U64 ring_size, U64 ring_pos, void *src_data, U64 src_data_size)
{
	Assert(src_data_size <= ring_size);
	{
		U64 ring_off = ring_pos % ring_size;
		U64 bytes_before_split = ring_size - ring_off;
		U64 pre_split_bytes = Min(bytes_before_split, src_data_size);
		U64 pst_split_bytes = src_data_size-pre_split_bytes;
		void *pre_split_data = src_data;
		void *pst_split_data = ((U8 *)src_data + pre_split_bytes);
		MemoryCopy(ring_base+ring_off, pre_split_data, pre_split_bytes);
		MemoryCopy(ring_base+0, pst_split_data, pst_split_bytes);
	}
	return src_data_size;
}

internal U64
wrapped_read(U8 *ring_base, U64 ring_size, U64 ring_pos, void *dst_data, U64 read_size)
{
	Assert(read_size <= ring_size);
	{
		U64 ring_off = ring_pos % ring_size;
		U64 bytes_before_split = ring_size - ring_off;
		U64 pre_split_bytes = Min(bytes_before_split, read_size);
		U64 pst_split_bytes = read_size - pre_split_bytes;

		MemoryCopy(dst_data, ring_base + ring_off, pre_split_bytes);
		MemoryCopy((U8 *)dst_data + pre_split_bytes, ring_base+0, pst_split_bytes);
	}
	return read_size;
}

 

// Thrid Party Includes

#define STB_SPRINTF_IMPLEMENTATION
#define STB_SPRINTF_STATIC
#include "third_party/stb/stb_sprintf.h"

// String <-> Integer Table

read_only global U8 integer_symbols[16] =
{
	'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F',
};

read_only global U8 integer_symbol_reverse[128] =
{
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};
