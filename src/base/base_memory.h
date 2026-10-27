#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H




void* reserve_memory(U64 size_bytes);
B32 commit_memory(void* ptr, U64 size_bytes);
void decommit_memory(void* ptr, U64 size_bytes);
void release_memory(void* ptr, U64 size_bytes);


void* reserve_memory_large(U64 size_bytes);
B32 commit_memory_large(void* ptr, U64 size_bytes);

#endif // BASE_MEMORY_H
