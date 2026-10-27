// Helpers

internal DateTime
lnx_date_time_from_tm(tm in, U32 msec)
{
	DateTime dt = {0};
	dt.sec	= in.tm_sec;
	dt.min	= in.tm_min;
	dt.hour = in.tm_hour;
	dt.day	= in.tm_mday-1;
	dt.mon	= in.tm_mon;
	dt.year = in.tm_year+1900;
	dt.msec = msec;
	return dt;
}
internal tm
lnx_tm_from_date_time(DateTime dt)
{
	tm result = {0};
	result.tm_sec		= dt.sec;
	result.tm_min		= dt.sec;
	result.tm_hour	= dt.sec;
	result.tm_mday	= dt.day+1;
	result.tm_mon		= dt.mon;
	result.tm_year	= dt.year-1900;
	return result;
}


internal timespec
lnx_timespec_from_date_time(DateTime dt)
{
	tm tm_val = lnx_tm_from_date_time(dt);
	time_t seconds = timegm(&tm_val);
	timespec result = {0};
	result.tv_sec = seconds;
	return result;
}

internal DenseTime
lnx_dense_time_from_timespec(timespec in)
{
	DenseTime result = 0;
	{
		struct tm tm_time = {0};
		gmtime_r(&in.tv_sec, &tm_time);
		DateTime date_time = lnx_date_time_from_tm(tm_time, in.tv_nsec/Million(1));
		result = dense_time_from_date_time(date_time);
	}
	return result;
}

internal FileProperties
lnx_file_properties_from_stat(struct stat *s)
{
	FileProperties props = {0};
	props.size			= s->st_size;
	props.created		= lnx_dense_time_from_timespec(s->st_ctim);
	props.modified	= lnx_dense_time_from_timespec(s->st_mtim);
	if (s->st_mode & S_IFDIR)
	{
		props.flags |= FilePropertyFlag_IsFolder;
	}
	return props;
}


internal LinuxEntity *
linux_entity_alloc(LinuxEntityKind kind)
{
	LinuxEntity* entity = 0;

	DeferLoop(pthread_mutex_lock(&linux_state.entity_mutex),
			  pthread_mutex_unlock(&linux_state.entity_mutex))
	{
		entity = linux_state.entity_free;
		if (entity)
		{
		    SLLStackPop(linux_state.entity_free);
		}
		else
		{
			entity = push_array_no_zero(linux_state.entity_arena, LinuxEntity, 1);
		}
	}
	MemoryZeroStruct(entity);
	entity->kind = kind;
	return entity;
}

internal void
linux_entity_release(LinuxEntity* entity)
{
	DeferLoop(pthread_mutex_lock(&linux_state.entity_mutex),
			  pthread_mutex_unlock(&linux_state.entity_mutex))
	{
		SLLStackPush(linux_state.entity_free, entity);
	}
}

internal void*
linux_thread_entry_point(void *ptr)
{
	LinuxEntity* entity = (LinuxEntity*)ptr;
	ThreadEntryPointFunctionType* func = entity->thread.func;
	void* thread_ptr = entity->thread.ptr;
	supplement_thread_base_entry_point(func, thread_ptr);
	return 0;
}


// @per_os_impl Platform Time Functions
internal U64
now_time_us(void)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	U64 result = t.tv_sec*Million(1) + (t.tv_nsec/Thousand(1));
	return result;
}


internal U32
now_time_unix(void)
{
	time_t t = time(0);
	return (U32)t;
}


/* internal DateTime now_time_universal(void); */
/* internal DateTime universal_from_local_time(DateTime *dt); */
/* internal DateTime local_from_universal_time(DateTime *dt); */

internal void
sleep_ms(U32 ms)
{
	usleep(ms*Thousand(1));
}

void* reserve_memory(U64 size_bytes)
{
	void* result = mmap(0, size_bytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (result == MAP_FAILED)
	{
		result = 0;
	}
	return result;
}

B32 commit_memory(void* ptr, U64 size_bytes)
{
	B32 result = (mprotect(ptr, size_bytes, PROT_READ|PROT_WRITE) != 0);
	return result;
}

void decommit_memory(void* ptr, U64 size_bytes)
{
	madvise(ptr, size_bytes, MADV_DONTNEED);
	mprotect(ptr, size_bytes, PROT_NONE);
}

void release_memory(void* ptr, U64 size_bytes)
{
	munmap(ptr, size_bytes);
}

void* reserve_memory_large(U64 size_bytes)
{
	void* result = mmap(0, size_bytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS| MAP_HUGETLB, -1, 0);
	if (result == MAP_FAILED)
	{
		result = 0;
	}
	return result;
}

B32 commit_memory_large(void* ptr, U64 size_bytes)
{
	B32 result = (mprotect(ptr, size_bytes, PROT_READ | PROT_WRITE) != 0);
	return result;
}


// @per_os_impl System Info
internal SystemInfo *
get_system_info(void)
{
	return &linux_state.system_info;
}


// @per_os_impl Process Info
internal ProcessInfo *
get_process_info(void)
{
	return &linux_state.process_info;
}

internal String8
get_current_path(Arena *arena)
{
	char *cwdir = getcwd(0, 0);
	String8 string = str8_copy(arena, str8_cstring(cwdir));
	free(cwdir);
	return string;
}


// @per_os_impl Current Thread Info

internal U32
tid(void)
{
	U32 result = gettid();
	return result;
}


internal void
set_platform_thread_name(String8 name)
{
	Temp scratch = scratch_begin(0, 0);
	String8 name_copy = str8_copy(scratch.arena, name);
	prctl(PR_SET_NAME, (char *)name_copy.str);
	scratch_end(scratch);
}

// @per_os_impl Thread Functions

internal Thread
thread_launch(ThreadEntryPointFunctionType *f, void* p)
{
	ProfBeginFunction();
	LinuxEntity* entity = linux_entity_alloc(LinuxEntityKind_Thread);
	entity->thread.func = f;
	entity->thread.ptr = p;
	{
		int pthread_result = pthread_create(&entity->thread.handle, 0, linux_thread_entry_point, entity);
		if (pthread_result != 0)
		{
			linux_entity_release(entity);
			entity = 0;
		}
	}
	Thread handle = {(U64)entity};
	ProfEnd();
	return handle;
}

internal B32
thread_join(Thread thread, U64 endt_us)
{
	if (MemoryIsZeroStruct(&thread)) { return 0; }
	LinuxEntity* entity = (LinuxEntity*)thread.u64[0];
	int join_result = pthread_join(entity->thread.handle, 0);
	B32 result = (join_result == 0);
	linux_entity_release(entity);
	return result;
}

internal void
thread_detach(Thread thread)
{
	if (MemoryIsZeroStruct(&thread)) { return; }
	LinuxEntity* entity = (LinuxEntity*)thread.u64[0];
	linux_entity_release(entity);
}


// Mutexes

internal Mutex
mutex_alloc(void)
{
	LinuxEntity* entity = linux_entity_alloc(LinuxEntityKind_Mutex);
	int init_result = pthread_mutex_init(&entity->mutex_handle, 0);
	if(init_result == -1)
	{
		linux_entity_release(entity);
		entity = 0;
	}
	Mutex handle = {(U64)entity};
	return handle;
}

internal void
mutex_release(Mutex mutex)
{
	if (MemoryIsZeroStruct(&mutex)) { return; }
	LinuxEntity* entity = (LinuxEntity*)mutex.u64[0];
	pthread_mutex_destroy(&entity->mutex_handle);
	linux_entity_release(entity);
}

internal void
mutex_take(Mutex mutex)
{
	if (MemoryIsZeroStruct(&mutex)) { return; }
	LinuxEntity* entity = (LinuxEntity*)mutex.u64[0];
	pthread_mutex_lock(&entity->mutex_handle);
}

internal void
mutex_drop(Mutex mutex)
{
	if (MemoryIsZeroStruct(&mutex)) { return; }
	LinuxEntity* entity = (LinuxEntity*)mutex.u64[0];
	pthread_mutex_unlock(&entity->mutex_handle);
}


// reader/writer mutexes

internal RWMutex
rw_mutex_alloc(void)
{
	LinuxEntity *entity =  linux_entity_alloc(LinuxEntityKind_RWMutex);
	S32 init_result = pthread_rwlock_init(&entity->rwmutex_handle, 0);
	if (init_result == -1)
	{
		linux_entity_release(entity);
		entity = 0;
	}

	RWMutex handle = {(U64)entity};
	return handle;
}

internal void
rw_mutex_release(RWMutex mutex)
{
	if (MemoryIsZeroStruct(&mutex)) { return; }
	LinuxEntity *entity = (LinuxEntity *)mutex.u64[0];
	pthread_rwlock_destroy(&entity->rwmutex_handle);
	linux_entity_release(entity);
}

internal void
rw_mutex_take(RWMutex mutex, B32 write_mode)
{
	if (MemoryIsZeroStruct(&mutex)) { return; }
	LinuxEntity *entity = (LinuxEntity *)mutex.u64[0];

	if (write_mode)
	{
		pthread_rwlock_wrlock(&entity->rwmutex_handle);
	}
	else
	{
		pthread_rwlock_rdlock(&entity->rwmutex_handle);
	}
}

internal void
rw_mutex_drop(RWMutex mutex, B32 write_mode)
{
	if (MemoryIsZeroStruct(&mutex)) { return; }
	LinuxEntity *entity = (LinuxEntity *)mutex.u64[0];
	pthread_rwlock_unlock(&entity->rwmutex_handle);
}


// condition variables

internal CondVar
cond_var_alloc(void)
{
	LinuxEntity *entity = linux_entity_alloc(LinuxEntityKind_ConditionVariable);
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
	S32 init_result = pthread_cond_init(&entity->cv.cond_handle, &attr);
	pthread_condattr_destroy(&attr);

	if (init_result == -1)
	{
		linux_entity_release(entity);
		entity = 0;
	}

	S32 init2_result = 0;
	if (entity)
	{
		init2_result = pthread_mutex_init(&entity->cv.rwlock_mutex_handle, 0);
	}
	if (init2_result == -1)
	{
		pthread_cond_destroy(&entity->cv.cond_handle);
		linux_entity_release(entity);
		entity = 0;
	}
	CondVar handle = {(U64)entity};
	return handle;
}

internal void
cond_var_release(CondVar cv)
{
	if (MemoryIsZeroStruct(&cv)) { return; }
	LinuxEntity *entity = (LinuxEntity *)cv.u64[0];

	pthread_cond_destroy(&entity->cv.cond_handle);
	pthread_mutex_destroy(&entity->cv.rwlock_mutex_handle);
	linux_entity_release(entity);
}

internal B32
cond_var_wait(CondVar cv, Mutex mutex, U64 endt_us)
{
	if (MemoryIsZeroStruct(&cv))		{ return 0; }
	if (MemoryIsZeroStruct(&mutex))	{ return 0; }
	LinuxEntity *cv_entity = (LinuxEntity *)cv.u64[0];
	LinuxEntity *mutex_entity = (LinuxEntity *)mutex.u64[0];
	struct timespec endt_timespec;
	endt_timespec.tv_sec = endt_us/Million(1);
	endt_timespec.tv_nsec = Thousand(1) * (endt_us - (endt_us/Million(1))*Million(1));
	S32 wait_result = pthread_cond_timedwait(&cv_entity->cv.cond_handle, &mutex_entity->mutex_handle, &endt_timespec);
	B32 result = (wait_result != ETIMEDOUT);
	return result;
}
	
internal B32
cond_var_wait_rw(CondVar cv, RWMutex mutex_rw, B32 write_mode, U64 endt_us)
{
	if(MemoryIsZeroStruct(&cv))			{ return 0; }
	if(MemoryIsZeroStruct(&mutex_rw))	{ return 0; }
	LinuxEntity *cv_entity = (LinuxEntity *)cv.u64[0];
	LinuxEntity *rw_mutex_entity = (LinuxEntity *)mutex_rw.u64[0];
	struct timespec endt_timespec;
	endt_timespec.tv_sec = endt_us/Million(1);
	endt_timespec.tv_nsec = Thousand(1) * (endt_us - (endt_us/Million(1))*Million(1));

	B32 result = 0;
	pthread_mutex_lock(&cv_entity->cv.rwlock_mutex_handle);
	pthread_rwlock_unlock(&rw_mutex_entity->rwmutex_handle);

	for (;;)
	{
		int wait_result = pthread_cond_timedwait(&cv_entity->cv.cond_handle, &cv_entity->cv.rwlock_mutex_handle, &endt_timespec);
		if (wait_result != ETIMEDOUT)
		{
			if (write_mode)
			{
				pthread_rwlock_wrlock(&rw_mutex_entity->rwmutex_handle);
			}
			else
			{
				pthread_rwlock_rdlock(&rw_mutex_entity->rwmutex_handle);
			}
			result = 1;
			break;
		}

		if (wait_result == ETIMEDOUT)
		{
			if (write_mode)
			{
				pthread_rwlock_wrlock(&rw_mutex_entity->rwmutex_handle);
			}
			else
			{
				pthread_rwlock_rdlock(&rw_mutex_entity->rwmutex_handle);
			}
			break;
		}
	}
	pthread_mutex_unlock(&cv_entity->cv.rwlock_mutex_handle);
	return result;
}

internal void
cond_var_signal(CondVar cv)
{
	if (MemoryIsZeroStruct(&cv)) { return; }
	LinuxEntity *cv_entity = (LinuxEntity *)cv.u64[0];
	pthread_cond_signal(&cv_entity->cv.cond_handle);
}

internal void
cond_var_broadcast(CondVar cv)
{
	if (MemoryIsZeroStruct(&cv)) { return; }
	LinuxEntity *cv_entity = (LinuxEntity *)cv.u64[0];
	pthread_cond_broadcast(&cv_entity->cv.cond_handle);
}

// Barriers

internal Barrier
barrier_alloc(U64 count)
{
	LinuxEntity *entity = linux_entity_alloc(LinuxEntityKind_Barrier);
	if (entity != 0)
	{
		pthread_barrier_init(&entity->barrier, 0, count);
	}
	Barrier result = {IntFromPtr(entity)};
	return result;
}

internal void
barrier_release(Barrier barrier)
{
	LinuxEntity *entity = (LinuxEntity *)PtrFromInt(barrier.u64[0]);
	if (entity != 0)
	{
		pthread_barrier_destroy(&entity->barrier);
		linux_entity_release(entity);
	}
}

internal void
barrier_wait(Barrier barrier)
{
	LinuxEntity *entity = (LinuxEntity *)PtrFromInt(barrier.u64[0]);
	if (entity != 0)
	{
		pthread_barrier_wait(&entity->barrier);
	}
}



// @per-os_impl File System (Implemented Per-OS)

// files


internal File
file_open(AccessFlags flags, String8 path)
{
	Temp scratch = scratch_begin(0, 0);

	int lnx_flags = 0;
	lnx_flags |= O_CLOEXEC;

	if (flags & AccessFlag_Read && flags & AccessFlag_Write)
	{
		lnx_flags = O_RDWR;
	}
	else if (flags & AccessFlag_Write)
	{
		lnx_flags = O_WRONLY;
	}
	else if (flags & AccessFlag_Read)
	{
		lnx_flags = O_RDONLY;
	}

	if (flags & AccessFlag_Append)
	{
		lnx_flags |= O_APPEND;
	}
	else if (flags & AccessFlag_Write)
	{
		lnx_flags |= O_TRUNC;
	}

	if (flags & (AccessFlag_Write|AccessFlag_Append))
	{
		lnx_flags |= O_CREAT;
	}

	if (flags & AccessFlag_CreateNew) { lnx_flags |= O_CREAT|O_EXCL; }

	String8 path_copy = push_str8_copy(scratch.arena, path);
	int fd = open ((char *)path_copy.str, lnx_flags, 0755);
	File handle = {0};
	if (fd != -1)
	{
		handle.u64[0] = fd;
	}

	scratch_end(scratch);
	return handle;
}


internal void
file_close(File file)
{
	if (file_match(file, file_zero())) { return; }
	int fd = (int)file.u64[0];
	close (fd);
}

internal FilePair
file_pipe_make(B32 read_inherited, B32 write_inherited)
{
	FilePair result = {0};
	int fds[2] = {0};
	if (pipe(fds) == 0)
	{
		if (!read_inherited) {fcntl(fds[0], F_SETFD, FD_CLOEXEC); }
		if (!write_inherited) {fcntl(fds[1], F_SETFD, FD_CLOEXEC); }
		result.read.u64[0] = fds[0];
		result.write.u64[0] = fds[1];
	}
	return result;
}


internal U64
file_pipe_read(File file, void *out_data, U64 size)
{
	ssize_t read_size = LINUX_RETRY_ON_EINTR(read((int)file.u64[0], out_data, size));
	return read_size > 0 ? (U64)read_size : 0;
}

internal U64
file_pipe_write(File file, void *data, U64 size)
{
	ssize_t write_size = LINUX_RETRY_ON_EINTR(write((int)file.u64[0], data, size));
	return write_size > 0 ? (U64)write_size : 0;
}


internal U64
file_pipe_bytes_available(File file)
{
	int size = 0;
	B32 is_ok = !file_match(file, file_zero()) && ioctl((int)file.u64[0], FIONREAD, &size) == 0;
	return is_ok && size > 0 ? (U64)size : 0;
}

internal B32
file_pipe_is_end(File file)
{
	struct pollfd poll_fd = {.fd = (int)file.u64[0], .events = POLLIN|POLLHUP };
	poll(&poll_fd, 1, 0);
	return !!(poll_fd.revents & (POLLHUP|POLLERR|POLLNVAL));
}

internal U64
file_read(File file, Range1U64 range, void *out_data)
{
	if (file_match(file, file_zero())) { return 0; }
	int fd = (int)file.u64[0];
	U64 total_num_bytes_to_read = dim_1u64(range);
	U64 total_num_bytes_read = 0;
	U64 total_num_bytes_left_to_read = total_num_bytes_to_read;
	for (; total_num_bytes_left_to_read > 0;)
	{
		int read_result = pread(fd, (U8 *)out_data + total_num_bytes_read, total_num_bytes_left_to_read, range.min + total_num_bytes_read);
		if (read_result >= 0)
		{
			total_num_bytes_read += read_result;
			total_num_bytes_left_to_read -= read_result;
		}
		else if (errno != EINTR)
		{
			break;
		}
	}
	return total_num_bytes_read;
}

internal U64
file_write(File file, Range1U64 range, void *data)
{
	if (file_match(file, file_zero())) { return 0; }
	int fd = (int)file.u64[0];
	U64 total_num_bytes_to_write = dim_1u64(range);
	U64 total_num_bytes_written = 0;
	U64 total_num_bytes_left_to_write = total_num_bytes_to_write;
	for (; total_num_bytes_left_to_write > 0;)
	{
		int write_result = pwrite(fd, (U8 *)data + total_num_bytes_written, total_num_bytes_left_to_write, range.min + total_num_bytes_written);
		if (write_result >= 0)
		{
			total_num_bytes_written += write_result;
			total_num_bytes_left_to_write -= write_result;
		}
		else if (errno != EINTR)
		{
			break;
		}
	}
	return total_num_bytes_written;
}


internal B32
file_set_times(File file, DateTime date_time)
{
	if (file_match(file, file_zero())) { return 0; }
	int fd = (int)file.u64[0];
	timespec time = lnx_timespec_from_date_time(date_time);
	timespec times[2] = {time, time};
	int futimens_result = futimens(fd, times);
	B32 good = (futimens_result != -1);
	return good;
}

internal FileProperties
properties_from_file(File file)
{
	if (file_match(file, file_zero())) { return (FileProperties){0}; }
	int fd = (int)file.u64[0];
	struct stat fd_stat = {0};
	int fstat_result = fstat(fd, &fd_stat);
	FileProperties props = {0};
	if(fstat_result != -1)
	{
		props = lnx_file_properties_from_stat(&fd_stat);
	}
	return props;
}


internal FileID
id_from_file(File file)
{
	if(file_match(file, file_zero())) { return (FileID){0}; }

	int fd = (int)file.u64[0];
	struct stat fd_stat = {0};
	int fstat_result = fstat(fd, &fd_stat);
	FileID id = {0};
	if (fstat_result != -1)
	{
		id.v[0] = fd_stat.st_dev;
		id.v[1] = fd_stat.st_ino;
	}
	return id;
}

internal B32
file_reserve_size(File file, U64 size)
{
	int status = fallocate((int)file.u64[0], FALLOC_FL_KEEP_SIZE, 0, size);
	return status == 0;
}

internal B32
file_set_size(File file, U64 size)
{
	return size <= max_S64 && ftruncate((int)file.u64[0], (off_t)size) == 0;
}


internal B32
file_flush(File file)
{
	return fsync((int)file.u64[0]) == 0;
}

internal B32
replace_file_path(String8 dst, String8 src)
{
	Temp scratch = scratch_begin(0, 0);
	char *dst_cstr = (char *)push_str8_copy(scratch.arena, dst).str;
	char *src_cstr = (char *)push_str8_copy(scratch.arena, src).str;
	B32 result = rename(src_cstr, dst_cstr) == 0;
	scratch_end(scratch);
	return result;
}

internal B32
delete_file_at_path(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	B32 result = 0;
	String8 path_copy = push_str8_copy(scratch.arena, path);
	if (remove((char *)path_copy.str) != -1)
	{
		result = 1;
	}
	scratch_end(scratch);
	return result;
}

internal B32
copy_file_path(String8 dst, String8 src)
{
	B32 result = 0;
	File src_h = file_open(AccessFlag_Read, src);
	File dst_h = file_open(AccessFlag_Write, dst);
	if (!file_match(src_h, file_zero()) &&
			!file_match(dst_h, file_zero()))
	{
		int src_fd = (int)src_h.u64[0];
		int dst_fd = (int)dst_h.u64[0];

		FileProperties src_props = properties_from_file(src_h);
		U64 size = src_props.size;
		U64 total_bytes_copied = 0;
		U64 bytes_left_to_copy = size;

		for (; bytes_left_to_copy > 0;)
		{
			off_t  sendfile_off = total_bytes_copied;
			int send_result = sendfile(dst_fd, src_fd, &sendfile_off, bytes_left_to_copy);
			if (send_result <= 0)
			{
				break;
			}
			U64 bytes_copied = (U64)send_result;
			bytes_left_to_copy -= bytes_copied;
			total_bytes_copied += bytes_copied;
		}
	}
	file_close(src_h);
	file_close(dst_h);
	return result;
}

internal B32
move_file_path(String8 dst, String8 src)
{
	B32 good = 0;
	Temp scratch = scratch_begin(0, 0);
	{
		char *src_cstr = (char *)str8_copy(scratch.arena, src).str;
		char *dst_cstr = (char *)str8_copy(scratch.arena, dst).str;
		int rename_result = rename(src_cstr, dst_cstr);
		good = (rename_result != -1);
	}
	scratch_end(scratch);
	return good;
}


internal String8
full_path_from_path(Arena *arena, String8 path)
{
	Temp scratch = scratch_begin(&arena, 1);
	String8 path_copy = str8_copy(scratch.arena, path);
	char buffer[PATH_MAX] = {0};
	realpath((char *)path_copy.str, buffer);
	String8 result = str8_copy(arena, str8_cstring(buffer));
	scratch_end(scratch);
	return result;
}

internal B32
file_path_exists(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String8 path_copy = push_str8_copy(scratch.arena, path);
	int access_result = access((char *)path_copy.str, F_OK);
	B32 result = 0;
	if (access_result == 0)
	{
		result = 1;
	}
	scratch_end(scratch);
	return result;
}

internal B32
folder_path_exists(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	B32 exists = 0;
	String8 path_copy = str8_copy(scratch.arena, path);
	DIR *handle = opendir((char *)path_copy.str);
	if (handle)
	{
		closedir(handle);
		exists = 1;
	}
	scratch_end(scratch);
	return exists;
}

internal FileProperties
properties_from_file_path(String8 path)
{
	Temp scratch = scratch_begin(0, 0);
	String8 path_copy = str8_copy(scratch.arena, path);
	struct stat f_stat = {0};
	int stat_result = stat((char *)path_copy.str, &f_stat);

	FileProperties props = {0};
	if (stat_result != -1)
	{
		props = lnx_file_properties_from_stat(&f_stat);
	}
	scratch_end(scratch);
	return props;
}

// file maps


// directory iteration
// TODO(Seppo): Implement 

internal FileIter *
file_iter_begin(Arena *arena, String8 path, FileIterFlags flags)
{
	FileIter *base_iter = push_array(arena, FileIter, 1);
	base_iter->flags = flags;
	LNX_FileIter *iter = (LNX_FileIter *)base_iter->memory;
	{
		String8 path_copy = push_str8_copy(arena, path);
		iter->dir = opendir((char *)path_copy.str);
		iter->path = path_copy;
	}
	return base_iter;
}

internal B32
file_iter_next(Arena *arena, FileIter *iter, FileInfo *info_out)
{
	B32 good = 0;
	LNX_FileIter *lnx_iter = (LNX_FileIter *)iter->memory;
	for (; lnx_iter->dir != 0;)
	{
		// get next entry
		lnx_iter->dp = readdir(lnx_iter->dir);
		good = (lnx_iter->dp != 0);

		// get next entry ifno
		struct stat st = {0};
		int stat_result = 0;
		if (good)
		{
			Temp scratch = scratch_begin(&arena, 1);
			String8 full_path = push_str8f(scratch.arena, "%S/%s", lnx_iter->path, lnx_iter->dp->d_name);
			stat_result = stat((char *)full_path.str, &st);
			scratch_end(scratch);
		}

		// determine if filtered
		B32 filtered = 0;
		if (good)
		{
			filtered = ((st.st_mode == S_IFDIR && iter->flags & FileIterFlag_SkipFolders) ||
									(st.st_mode == S_IFREG && iter->flags & FileIterFlag_SkipFiles) ||
									(lnx_iter->dp->d_name[0] == '.' && lnx_iter->dp->d_name[1] == 0) ||
									(lnx_iter->dp->d_name[0] == '.' && lnx_iter->dp->d_name[1] == '.' && lnx_iter->dp->d_name[2] == 0));
		}

		// output & exit if good & unfiltered
		if (good && !filtered)
		{
			info_out->name = push_str8_copy(arena, str8_cstring(lnx_iter->dp->d_name));
			if (stat_result != -1)
			{
				info_out->props = lnx_file_properties_from_stat(&st);
			}
			break;
		}

		// exit if not good
		if (!good)
		{
			break;
		}
	}
	return good;
}

internal void
file_iter_end(FileIter *iter)
{
	LNX_FileIter *lnx_iter = (LNX_FileIter *)iter->memory;
	closedir(lnx_iter->dir);
}

// directory creation

internal B32
make_directory(String8 path)
{
	Temp scratch = scratch_begin(0 ,0);
	B32 result = 0;
	String8 path_copy = push_str8_copy(scratch.arena, path);
	if (mkdir((char *)path_copy.str, 0775) != -1)
	{
		result = 1;
	}
	else
	{
		result = file_path_exists(path);
	}
	scratch_end(scratch);
	return result;
}


// Main Entry
int
main(int argc, char **argv)
{
	// Setup OS Layer
	{
		// Statically-allocated system/process info
		{
			U64 pages				= (U64)sysconf(_SC_PHYS_PAGES);
			U64 page_size		= (U64)sysconf(_SC_PAGESIZE);
			U64 commit_size = (U64)sysconf(_SC_AVPHYS_PAGES);

			SystemInfo *info = &linux_state.system_info;
			info->logical_processor_count = (U32)get_nprocs();
			info->page_size								= (U64)getpagesize();
			info->large_page_size					= MB(2);
			info->allocation_granularity  = info->page_size;
			info->physical_memory_size		= pages * page_size;
		}
		{
			ProcessInfo *info = &linux_state.process_info;
			info->pid = (U32)getpid();
		}

		// Setup Thread Context
		ThreadContext *tctx = thread_context_alloc();
		thread_context_select(tctx);

		linux_state.arena = arena_alloc();
		linux_state.entity_arena = arena_alloc();
		pthread_mutex_init(&linux_state.entity_mutex, 0);

		// cache default enviroment
		{
			U64 env_count = 0;
			for (; __environ[env_count] != 0; env_count += 1) {}

			char **default_env = push_array(linux_state.arena, char *, env_count+1);
			for EachIndex(idx, env_count)
			{
				String8 entry = str8_copy(linux_state.arena, str8_cstring(__environ[idx]));
				default_env[idx] = (char *)entry.str;
				str8_list_push(linux_state.arena, &linux_state.process_info.environment, entry);
			}
			default_env[env_count] = 0;
			linux_state.default_env_count = env_count;
			linux_state.default_env				= default_env;
		}

		// grab dynmaically allocated system info
		{
			Temp scratch = scratch_begin(0, 0);
			SystemInfo *info = &linux_state.system_info;

			B32 got_final_result = 0;
			U8 *buffer = 0;
			S32 size = 0;
			for (S64 cap = 4096, r = 0; r < 4; cap *= 2, r += 1)
			{
				scratch_end(scratch);
				buffer = push_array(scratch.arena, U8, cap);
				int gethostname_result = gethostname((char *)buffer, cap);
				size = cstring8_length(buffer);
				if (gethostname_result == 0 && size < cap)
				{
					got_final_result = 1;
					break;
				}
			}

			if (got_final_result && size > 0)			
			{
				info->machine_name.size = size;
				info->machine_name.str = push_array_no_zero(linux_state.arena, U8, info->machine_name.size + 1);
				MemoryCopy(info->machine_name.str, buffer, info->machine_name.size);
				info->machine_name.str[info->machine_name.size] = 0;
			}

			scratch_end(scratch);
		}

		// grab dynamically allocated process info
		{
			Temp scratch = scratch_begin(0, 0);
			ProcessInfo *info = &linux_state.process_info;

			// binary path
			{
				B32 got_final_result = 0;
				U8 *buffer = 0;
				S32 size = 0;
				for (S64 cap = PATH_MAX, r = 0; r < 4; cap *= 2, r += 1)
				{
					scratch_end(scratch);
					buffer = push_array_no_zero(scratch.arena, U8, cap);
					size = readlink("/proc/self/exe", (char *)buffer, cap);
					if (size < cap)
					{
						got_final_result = 1;
						break;
					}
				}

				// save
				if (got_final_result && size > 0)
				{
					String8 full_name = str8(buffer, size);
					info->binary_file_path = push_str8_copy(linux_state.arena, full_name);
					info->binary_path = str8_chop_last_slash(info->binary_file_path);
				}
			}

			// grab initial directory
			{
				info->initial_path = get_current_path(linux_state.arena);
			}

			// grab program/user data paths
			{
				char *home = getenv("HOME");
				char *xdg_config_home = getenv("XDG_CONFIG_HOME");
				char *xdg_cache_home = getenv("XDG_CACHE_HOME");
				char *xdg_state_home = getenv("XDG_STATE_HOME");
				if (xdg_config_home != 0)
				{
					info->user_program_config_data_path = str8_cstring(xdg_config_home);
				}
				else
				{
					info->user_program_config_data_path = str8f(linux_state.arena, "%s/.config", home);
				}

				if (xdg_cache_home != 0)
				{
					info->user_program_cache_data_path = str8_cstring(xdg_cache_home);
				}
				else
				{
					info->user_program_cache_data_path = str8f(linux_state.arena, "%s/.cache", home);
				}


				if (xdg_state_home != 0)
				{
					info->user_program_logs_data_path = str8_cstring(xdg_state_home);
				}
				else
				{
					info->user_program_logs_data_path = str8f(linux_state.arena, "%s/.local/state", home);
				}
			}

			scratch_end(scratch);
		}
	}

	main_thread_base_entry_point(argc, argv);
}
