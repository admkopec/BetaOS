//
//  unimplemented.m
//  Objective-C Runtime
//
//  Created by Adam Kopeć on 11/28/17.
//

#define UNIMPLEMENTED(x) void x() { ULTDBG(__func__); }

#ifdef ULTDEBUG
#define ULTDBG(x...) printf(x)
#else
#define ULTDBG(x...)
#endif

UNIMPLEMENTED(getline)
UNIMPLEMENTED(getsectiondata)
UNIMPLEMENTED(__error)
UNIMPLEMENTED(malloc_zone_from_ptr)
UNIMPLEMENTED(snprintf_l)
UNIMPLEMENTED(vdprintf)
UNIMPLEMENTED(fprintf)
UNIMPLEMENTED(dlsym)
UNIMPLEMENTED(dladdr)
UNIMPLEMENTED(close)
UNIMPLEMENTED(read)
UNIMPLEMENTED(asl_log)
UNIMPLEMENTED(__swbuf)
UNIMPLEMENTED(_tlv_bootstrap)
UNIMPLEMENTED(malloc_default_zone)
UNIMPLEMENTED(getenv)

/*
 * Math functions
 */


UNIMPLEMENTED(arc4random)
UNIMPLEMENTED(arc4random_uniform)
UNIMPLEMENTED(cos)
UNIMPLEMENTED(cosf)
UNIMPLEMENTED(exp)
UNIMPLEMENTED(exp2)
UNIMPLEMENTED(exp2f)
UNIMPLEMENTED(expf)
UNIMPLEMENTED(fma)
UNIMPLEMENTED(fmaf)
UNIMPLEMENTED(fmal)
UNIMPLEMENTED(fmod)
UNIMPLEMENTED(fmodf)
UNIMPLEMENTED(fmodl)
UNIMPLEMENTED(log)
UNIMPLEMENTED(log10)
UNIMPLEMENTED(log10f)
UNIMPLEMENTED(log2)
UNIMPLEMENTED(log2f)
UNIMPLEMENTED(logf)
UNIMPLEMENTED(nearbyint)
UNIMPLEMENTED(nearbyintf)
UNIMPLEMENTED(sin)
UNIMPLEMENTED(sinf)
UNIMPLEMENTED(lgamma_r)
UNIMPLEMENTED(lgammaf_r)
UNIMPLEMENTED(lgammal_r)

/*
 * Misc
 */

UNIMPLEMENTED(strtod_l)
UNIMPLEMENTED(strtof_l)
UNIMPLEMENTED(strtold_l)
UNIMPLEMENTED(remainder)
UNIMPLEMENTED(remainderf)
UNIMPLEMENTED(remainderl)
UNIMPLEMENTED(__memcpy_chk)
UNIMPLEMENTED(__memmove_chk)
UNIMPLEMENTED(__strlcpy_chk)

UNIMPLEMENTED(sysconf)
UNIMPLEMENTED(trunc)
UNIMPLEMENTED(truncf)
UNIMPLEMENTED(truncl)

UNIMPLEMENTED(fcntl)
UNIMPLEMENTED(ioctl)

UNIMPLEMENTED(open)
UNIMPLEMENTED(openat)
UNIMPLEMENTED(sem_open)

UNIMPLEMENTED(__fpclassifyd)
UNIMPLEMENTED(__fpclassifyf)


// Temp
//UNIMPLEMENTED(OSAtomicCompareAndSwap32Barrier)
//UNIMPLEMENTED(OSAtomicCompareAndSwapPtrBarrier)
//UNIMPLEMENTED(OSAtomicDecrement32Barrier)
//UNIMPLEMENTED(OSAtomicIncrement32Barrier)
//UNIMPLEMENTED(OSAtomicOr32Barrier)
//UNIMPLEMENTED(OSAtomicXor32Barrier)
//
//UNIMPLEMENTED(fstat$INODE64)
//UNIMPLEMENTED(lstat$INODE64)
//
//UNIMPLEMENTED(_Block_object_assign)
//UNIMPLEMENTED(_Block_object_dispose)
UNIMPLEMENTED(_DefaultRuneLocale)
//UNIMPLEMENTED(_NSConcreteGlobalBlock)
//UNIMPLEMENTED(_NSConcreteStackBlock)
//UNIMPLEMENTED(_NSGetEnviron)
//UNIMPLEMENTED(_Unwind_GetCFA)
//UNIMPLEMENTED(_Unwind_GetIP)
//UNIMPLEMENTED(mach_port_deallocate)
//UNIMPLEMENTED(unw_get_proc_info)
//UNIMPLEMENTED(unw_getcontext)
//UNIMPLEMENTED(unw_get_reg)
//UNIMPLEMENTED(unw_init_local)
//UNIMPLEMENTED(unw_step)
//UNIMPLEMENTED(vm_deallocate)
//UNIMPLEMENTED(backtrace_symbols)
//UNIMPLEMENTED(backtrace_symbols_fd)
//UNIMPLEMENTED(dispatch_get_current_queue)
//UNIMPLEMENTED(dispatch_queue_get_label)
//UNIMPLEMENTED(fileno)
//UNIMPLEMENTED(fsync)
//UNIMPLEMENTED(ftruncate)
//UNIMPLEMENTED(geteuid)
//UNIMPLEMENTED(getpid)
//UNIMPLEMENTED(getsegmentdata)
//UNIMPLEMENTED(issetugid)
//UNIMPLEMENTED(pread)
//UNIMPLEMENTED(mach_task_self_)
//UNIMPLEMENTED(pthread_is_threaded_np)
//UNIMPLEMENTED(pthread_mach_thread_np)
//UNIMPLEMENTED(pthread_getname_np)
//UNIMPLEMENTED(task_threads)
//UNIMPLEMENTED(thread_get_state)
//UNIMPLEMENTED(__cxa_allocate_exception)
//UNIMPLEMENTED(__cxa_begin_catch)
//UNIMPLEMENTED(__cxa_end_catch)
//UNIMPLEMENTED(__cxa_throw)
//UNIMPLEMENTED(__cxa_rethrow)
//UNIMPLEMENTED(__cxa_pure_virtual)
//UNIMPLEMENTED(__cxa_current_exception_type)
//UNIMPLEMENTED(strcspn)
//UNIMPLEMENTED(strstr)
//UNIMPLEMENTED(sprintf)
//UNIMPLEMENTED(_mh_dylib_header)

