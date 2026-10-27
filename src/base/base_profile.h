#ifndef BASE_PROFILE_H
#define BASE_PROFILE_H

#if !defined(PROFILE_SPALL)
#define PROFILE_SPALL 0
#endif
#if !defined(PROFILE_TRACY)
#define PROFILE_TRACY 0
#endif

#if PROFILE_SPALL
//#include "spall.h"
#elif PROFILE_TRACY
//#include "tracy.h"
#endif


#if PROFILE_SPALL
// TODO SPALL
#define ProfBegin(...)
#define ProfBeginDynamic(...)
#define ProfEnd(...)
#define ProfTick(...)
#define ProfIsCapturing(...)
#define ProfBeginCapture(...)
#define ProfEndCapture(...)
#define ProfThreadName(...)
#define ProfMsg(...)
#define ProfBeginLockWait(...)
#define ProfEndLockWait(...)
#define ProfLockTake(...)
#define ProfLockDrop(...)
#define ProfColor(color)
#define ProfBeginV(...)
#define ProfNoteV(...)

#endif 

#if PROFILE_TRACY

#define ProfBegin(...)
#define ProfBeginDynamic(...)
#define ProfEnd(...)
#define ProfTick(...)
#define ProfIsCapturing(...)
#define ProfBeginCapture(...)
#define ProfEndCapture(...)
#define ProfThreadName(...)
#define ProfMsg(...)
#define ProfBeginLockWait(...)
#define ProfEndLockWait(...)
#define ProfLockTake(...)
#define ProfLockDrop(...)
#define ProfColor(color)
#define ProfBeginV(...)
#define ProfNoteV(...)

#endif

#if !defined(ProfBegin)

#define ProfBegin(...)         (0)
#define ProfBeginDynamic(...)  (0)
#define ProfEnd(...)           (0)
#define ProfTick(...)          (0)
#define ProfIsCapturing(...)   (0)
#define ProfBeginCapture(...)  (0)
#define ProfEndCapture(...)    (0)
#define ProfThreadName(...)    (0)
#define ProfMsg(...)           (0)
#define ProfBeginLockWait(...) (0)
#define ProfEndLockWait(...)   (0)
#define ProfLockTake(...)      (0)
#define ProfLockDrop(...)      (0)
#define ProfColor(...)         (0)
#define ProfBeginV(...)        (0)
#define ProfNoteV(...)         (0)

#endif

#define ProfBeginFunction(...) ProfBegin(this_function_name)
#define ProfScope(...) DeferLoop(ProfBeginDynamic(__VA_ARGS__), ProfEnd())

#endif // BASE_PROFILE_H
