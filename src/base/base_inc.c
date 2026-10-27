#include "base_core.c"
#include "base_profile.c"
#include "base_memory.c"
#include "base_arena.c"
#include "base_math.c"
#include "base_strings.c"
#include "base_hash.c"
#include "base_system.c"
#include "base_threads.c"
#include "base_ring.c"
#include "base_thread_context.c"
#include "base_files.c"
#include "base_processes.c"
#include "base_command_line.c"
#include "base_entry_point.c"


#if OS_WINDOWS
# include "base_win32.c"
#elif OS_LINUX
# include "base_linux.c"
#else
# error Operating system backend not found for base layer
#endif
