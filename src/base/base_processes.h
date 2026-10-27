#ifndef BASE_PROCESSES_H
#define BASE_PROCESSES_H

typedef struct ProcessInfo ProcessInfo;
struct ProcessInfo
{
	U32 pid;
	B32 large_pages_allowed;
	String8 binary_file_path;
	String8 binary_path;
	String8 initial_path;
	String8 user_program_config_data_path;
	String8 user_program_cache_data_path;
	String8 user_program_logs_data_path;
	String8List module_load_paths;
	String8List environment;
};


typedef struct Process Process;
struct Process
{
	U64 u64[1];
};


typedef struct ProcessGroup ProcessGroup;
struct ProcessGroup
{
	U64 u64[1];
};

typedef struct ProcessNode ProcessNode;
struct ProcessNode
{
	ProcessNode *next;
	Process v;
};

typedef struct ProcessList ProcessList;
struct ProcessList
{
	ProcessNode *first;
	ProcessNode *last;
	U64 count;
};

typedef struct ProcessLaunchParams ProcessLaunchParams;
struct ProcessLaunchParams
{
	String8List cmd_line;
	String8 path;
	String8List env;
	B32 inherit_env;
	B32 debug_subprocesses;
	B32 consoleless;
	B32 new_console;
	ProcessGroup process_group;
	File stdout_file;
	File Stderr_file;
	File stdin_file;
};


// @per_os_impl Process Info
internal ProcessInfo *get_process_info(void);
internal String8 get_current_path(Arena *arena);
internal U32 get_process_start_time_unix();

#endif // BASE_PROCESSES_H
