global U64 global_update_tick_idx = 0;
global U64 async_threads_count = 0;
global CondVar async_tick_start_cond_var = {0};
global Mutex async_tick_start_mutex = {0};
global Mutex async_tick_stop_mutex = {0};
global B32 async_loop_again = 0;
global B32 async_loop_again_high_priority = 0;
global B32 global_async_exit = 0;

thread_static B32 is_async_thread = 0;


// NOTE(insomniac): defined by target
internal void entry_point(CmdLine *cmdline);


internal void main_thread_base_entry_point(int arguments_count, char **arguments)
{
	//ThreadNameF("main_thread");
	Temp scratch = scratch_begin(0, 0);

	//- insomniac: set up async thread group info
	async_tick_start_cond_var = cond_var_alloc();
	async_tick_start_mutex = mutex_alloc();
	async_tick_stop_mutex = mutex_alloc();


	//- insomniac: setup spall
	#if PROFILE_SPALL
	// TODO(insomniac): integrate spall
	#endif

	//- insomniac: setup tracy 
	#if PROFILE_TRACY
	// TODO(insomniac): integrate tracy
	#endif

	//- [insomniac]: parse command line
	String8List command_line_argument_strings = {0};
	{
		for EachIndex(idx, arguments_count)
		{
			str8_list_push(scratch.arena, &command_line_argument_strings, str8_cstring(arguments[idx]));
		}
	}

	CmdLine cmdline = cmd_line_from_string_list(scratch.arena, command_line_argument_strings);


//- [insomniac]: begin captures
	B32 capture = cmd_line_has_flag(&cmdline, str8_lit("capture"));
	if (capture)
	{
		ProfBeginCapture(arguments[0]);
		ProfMsg(BUILD_TITLE);
	}

	// Auto Init Layers
	//- theese for now

#if defined(SOCKET_H) && !defined(SOCKET_INIT_MANUAL)
	sock_init();
#endif

#if defined(HTTP_H) && !defined(HTTP_INIT_MANUAL)
	http_init();
#endif

#if defined(FILE_STREAM_H) && !defined(FS_INIT_MANUAL)
	fs_init();
#endif

#if defined(WINDOW_MANAGER_H) && !defined(WM_INIT_MANUAL)
	wm_init();
#endif

#if defined(RENDER_CORE_H) && !defined(R_INIT_MANUAL)
	r_init(&cmdline);
#endif


	// launch async threads
	#if NEED_ASYNC
	Thread *async_threads = 0;
	U64 lane_broadcast_val = 0;
	{
		U64 num_main_threads = 2;

		U64 num_async_threads = get_system_info()->logical_processor_count;
		U64 num_main_threads_clamped = Min(num_async_threads, num_main_threads);
		num_async_threads -= num_main_threads_clamped;
		String8 num_async_threads_string = cmd_line_string(&cmdline, str8_lit("async_thread_count"));
		if (num_async_threads_string.size != 0)
		{
			try_u64_from_str8_c_rules(num_async_threads_string, &num_async_threads);
		}



		num_async_threads = Max(1, num_async_threads);

		fprintf(stderr, "num_async_threads = %lu\n", num_async_threads);

		Barrier barrier = barrier_alloc(num_async_threads);
		LaneContext *lane_ctxs = push_array(scratch.arena, LaneContext, num_async_threads);
		async_threads_count = num_async_threads;
		async_threads = push_array(scratch.arena, Thread, async_threads_count);
		for EachIndex(idx, num_async_threads)
		{
			lane_ctxs[idx].lane_idx = idx;
			lane_ctxs[idx].lane_count = async_threads_count;
			lane_ctxs[idx].barrier = barrier;
			lane_ctxs[idx].broadcast_memory = &lane_broadcast_val;

			async_threads[idx] = thread_launch(async_thread_entry_point, &lane_ctxs[idx]);
		}
	}
	#endif
	
	entry_point(&cmdline);

	// join
	#if NEED_ASYNC
	atomic_u32_inc_eval(&global_async_exit);
	atomic_u32_inc_eval(&async_loop_again);
	cond_var_broadcast(async_tick_start_cond_var);
	for EachIndex(idx, async_threads_count)
	{
		thread_join(async_threads[idx], max_U64);
	}
	#endif

	if (capture)
	{
		ProfEndCapture();
	}

	scratch_end(scratch);
}


internal void
supplement_thread_base_entry_point(void (*entry_point)(void *params), void *params)
{
	ThreadContext *tctx = thread_context_alloc();
	thread_context_select(tctx);
	entry_point(params);
	thread_context_release(tctx);
}

internal U64
update_tick_idx(void)
{
	U64 result = atomic_u64_eval(&global_update_tick_idx);
	return result;
}

internal B32
update(void)
{
	ProfTick(0);
	atomic_u64_inc_eval(&global_update_tick_idx);

#if defined(FONT_CACHE_H)
	fnt_frame();
#endif

#if OS_FEATURE_GRAPHICAL
	B32 result = frame();
#else
	B32 result = 0;
#endif
	return result;
}

internal void
async_thread_entry_point(void *params)
{
	LaneContext lctx = *(LaneContext *)params;
	lane_ctx(lctx);

	is_async_thread = 1;
	//ThreadNameF("async_thread_%I64u", lane_idx());
	for(;;)
	{
		if (lane_idx() == 0)
		{
			if (!atomic_u32_eval(&async_loop_again))
			{
				MutexScope(async_tick_start_mutex) cond_var_wait(async_tick_start_cond_var, async_tick_start_mutex, now_time_us()+1000000);
			}

			atomic_u32_eval_assign(&async_loop_again, 0);
			atomic_u32_eval_assign(&async_loop_again_high_priority, 0);
		}
		lane_sync();

		ProfScope("async tick")
		{

#if defined(FILE_STREAM_H)
			fs_async_tick();
#endif

#if defined(SOCKET_H)
			sock_async_tick();
#endif

#if defined(HTTP_H)
			http_async_tick();
#endif
		}

		lane_sync();
		B32 need_exit = 0;
		if (lane_idx() == 0)
		{
			need_exit = atomic_u32_eval(&global_async_exit);
		}
	
		lane_sync_u64(&need_exit, 0);
		if(need_exit)
		{
			break;
		}
	}
}


