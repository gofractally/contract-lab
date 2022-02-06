#include <wasi/api.h>

inline __wasi_errno_t __wasi_clock_time_get_impl(__wasi_clockid_t id,
                                                 __wasi_timestamp_t precision,
                                                 __wasi_timestamp_t* time)
{
   [[clang::import_name("tester_clock_time_get")]] uint16_t tester_clock_time_get(
       uint32_t id, uint64_t precision, uint64_t * time);
   return tester_clock_time_get(id, precision, time);
}

extern "C" int32_t __imported_wasi_snapshot_preview1_clock_time_get(int32_t arg0,
                                                                    int64_t arg1,
                                                                    int32_t arg2)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("clock_time_get")))
{
   return __wasi_clock_time_get_impl(arg0, arg1, (__wasi_timestamp_t*)arg2);
}
