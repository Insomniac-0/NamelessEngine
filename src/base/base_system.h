#ifndef SYSTEM_H
#define SYSTEM_H


typedef struct SystemInfo SystemInfo;
struct SystemInfo
{
	U32 logical_processor_count;
	U64 page_size;
	U64 large_page_size;
	U64 allocation_granularity;
	U64 physical_memory_size;
	String8 machine_name;
};


internal SystemInfo *get_system_info(void);

#endif // SYSTEM_H
