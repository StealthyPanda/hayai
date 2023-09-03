
@def int size_t 

@def int __builtin_va_list 

@def int __gnuc_va_list 

@def int va_list 

@def int __int8_t 

@def int __uint8_t 

@def int __int16_t 

@def int __uint16_t 

@def int __int_least16_t 

@def int __uint_least16_t 

@def int __int32_t 

@def int __uint32_t 

@def int __int64_t 

@def int __uint64_t 

@def int __int_least32_t 

@def int __uint_least32_t 

@def int __s8 

@def int __u8 

@def int __s16 

@def int __u16 

@def int __s32 

@def int __u32 

@def int __s64 

@def int __u64 

@def int _LOCK_T 

@def int _LOCK_RECURSIVE_T 

@def int _off_t 

@def int __dev_t 

@def int __uid_t 

@def int __gid_t 

@def int _off64_t 

@def int _fpos_t 

@def int _ssize_t 

@def int wint_t 

@def int _mbstate_t 

@def int _flock_t 

@def int _iconv_t 

@def int __ULong 

@def int __FILE 

@def int ptrdiff_t 

@def int wchar_t 

@def int char16_t 

@def int char32_t 

@def int __off_t 

@def int __pid_t 

@def int __loff_t 

@def int u_char 

@def int u_short 

@def int u_int 

@def int u_long 

@def int ushort 

@def int uint 

@def int clock_t 

@def int time_t 

@def int daddr_t 

@def int caddr_t 

@def int ino_t 

@def int off_t 

@def int dev_t 

@def int uid_t 

@def int gid_t 

@def int pid_t 

@def int key_t 

@def int ssize_t 

@def int mode_t 

@def int nlink_t 

@def int fd_mask 

@def int _types_fd_set 

@def int clockid_t 

@def int timer_t 

@def int useconds_t 

@def int suseconds_t 

@def int FILE 

@def int fpos_t 

@def int cookie_read_function_t 

@def int cookie_write_function_t 

@def int cookie_seek_function_t 

@def int cookie_close_function_t 

@def int cookie_io_functions_t 

@def int div_t 

@def int ldiv_t 

@def int lldiv_t 

@def int sigset_t 

@def int __sigset_t 

@def int _sig_func_ptr 

@def int sig_atomic_t 

@def int __tzrule_type 

@def int __tzinfo_type 

@def int mbstate_t 

@def int sem_t 

@def int pthread_t 

@def int pthread_attr_t 

@def int pthread_mutex_t 

@def int pthread_mutexattr_t 

@def int pthread_cond_t 

@def int pthread_condattr_t 

@def int pthread_key_t 

@def int pthread_once_t 

@def int pthread_rwlock_t 

@def int pthread_rwlockattr_t 

@def int pthread_spinlock_t 

@def int pthread_barrier_t 

@def int pthread_barrierattr_t 

@def int jmp_buf 

@def int rlim_t 

@def int sa_family_t 

@def int sigjmp_buf 

@def int stack_t 

@def int siginfo_t 

@def int z_stream 

@def int int8_t 

@def int uint8_t 

@def int int16_t 

@def int uint16_t 

@def int int32_t 

@def int uint32_t 

@def int int64_t 

@def int uint64_t 

@def int int_least8_t 

@def int uint_least8_t 

@def int int_least16_t 

@def int uint_least16_t 

@def int int_least32_t 

@def int uint_least32_t 

@def int int_least64_t 

@def int uint_least64_t 

@def int int_fast8_t 

@def int uint_fast8_t 

@def int int_fast16_t 

@def int uint_fast16_t 

@def int int_fast32_t 

@def int uint_fast32_t 

@def int int_fast64_t 

@def int uint_fast64_t 

@def int intptr_t 

@def int uintptr_t 

@def int intmax_t 

@def int uintmax_t 

@def _Bool bool 

struct MirConnection {
}

struct MirSurface {
}

struct MirSurfaceSpec {
}

struct MirScreencast {
}

struct MirPromptSession {
}

struct MirBufferStream {
}

struct MirPersistentId {
}

struct MirBlob {
}

struct MirDisplayConfig {
}

struct xcb_connection_t {
}

@def uint32_t xcb_window_t 

@def uint32_t xcb_visualid_t 

@def _Bool atomic_bool 

@def char atomic_char 

@def signed atomic_schar 

@def unsigned atomic_uchar 

@def short atomic_short 

@def unsigned atomic_ushort 

@def int atomic_int 

@def unsigned atomic_uint 

@def long atomic_long 

@def unsigned atomic_ulong 

@def long atomic_llong 

@def unsigned atomic_ullong 

@def uint_least16_t atomic_char16_t 

@def uint_least32_t atomic_char32_t 

@def wchar_t atomic_wchar_t 

@def int_least8_t atomic_int_least8_t 

@def uint_least8_t atomic_uint_least8_t 

@def int_least16_t atomic_int_least16_t 

@def uint_least16_t atomic_uint_least16_t 

@def int_least32_t atomic_int_least32_t 

@def uint_least32_t atomic_uint_least32_t 

@def int_least64_t atomic_int_least64_t 

@def uint_least64_t atomic_uint_least64_t 

@def int_fast8_t atomic_int_fast8_t 

@def uint_fast8_t atomic_uint_fast8_t 

@def int_fast16_t atomic_int_fast16_t 

@def uint_fast16_t atomic_uint_fast16_t 

@def int_fast32_t atomic_int_fast32_t 

@def uint_fast32_t atomic_uint_fast32_t 

@def int_fast64_t atomic_int_fast64_t 

@def uint_fast64_t atomic_uint_fast64_t 

@def intptr_t atomic_intptr_t 

@def uintptr_t atomic_uintptr_t 

@def size_t atomic_size_t 

@def ptrdiff_t atomic_ptrdiff_t 

@def intmax_t atomic_intmax_t 

@def uintmax_t atomic_uintmax_t 

struct atomic_flag {
	atomic_bool _Value;
}

struct vector3 {
	f32 x;
	f32 y;
	f32 z;
}

@sig fun add(clone unsafe ptr[1] vector3 a, clone unsafe ptr[1] vector3 b) : vector3 {}

@sig fun multiply(clone unsafe ptr[1] vector3 a, clone f32 f) : vector3 {}
