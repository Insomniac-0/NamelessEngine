#ifndef BASE_THREAD_CONTEXT_H
#define BASE_THREAD_CONTEXT_H

// Lane Context
typedef struct LaneContext LaneContext;
struct LaneContext
{
	U64 lane_idx;
	U64 lane_count;
	Barrier barrier;
	U64* broadcast_memory;
};

// Access Scopes

typedef struct AccessPt AccessPt;
struct AccessPt
{
	U64 access_refcount;
	U64 last_time_touched_us;
	U64 last_update_idx_touched;
};

typedef struct AccessPtExpireParams AccessPtExpireParams;
struct AccessPtExpireParams
{
	U64 time;
	U64 update_idxs;
};

typedef struct Touch Touch;
struct Touch
{
	Touch *next;
	AccessPt *pt;
	CondVar cv;
};

typedef struct Access Access;
struct Access
{
	Access *next;
	Touch *top_touch;
};



// Base Per-Thread State

typedef struct ThreadContext ThreadContext;
struct ThreadContext
{
	// Scratch arenas;
	Arena* arenas[2];

	U8 thread_name[32];
	U64 thread_name_size;

	LaneContext lane_ctx;

	char* file_name;
	U64 line_number;


	U64* progress_counter_ptr;
	U64* progress_target_ptr;
};

extern thread_static ThreadContext* tctx_thread_local;

// Thread Context Functions

internal ThreadContext* thread_context_alloc(void);
internal void thread_context_release(ThreadContext* tctx);
internal void thread_context_select(ThreadContext* tctx);
internal ThreadContext* thread_context_selected(void);

// scratch arenas
internal Arena* thread_context_get_scratch(Arena **conflicts, U64 count);
#define scratch_begin(conflicts, count) temp_begin(thread_context_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)

// lane metadata

internal LaneContext thread_context_set_lane_ctx(LaneContext lane_ctx);
internal void thread_context_lane_barrier_wait(void *broadcast_ptr, U64 broadcast_size, U64 broadcast_src_lane_idx);

#define lane_idx() (thread_context_selected()->lane_ctx.lane_idx)
#define lane_count() (thread_context_selected()->lane_ctx.lane_count)
#define lane_ctx(ctx) thread_context_set_lane_ctx((ctx))
#define lane_sync() thread_context_lane_barrier_wait(0, 0, 0)
#define lane_sync_u64(ptr, src_lane_idx) thread_context_lane_barrier_wait((ptr), sizeof(*(ptr)), (src_lane_idx))


#endif // BASE_THREAD_CONTEXT_H
