#include <wasi/api.h>

inline __wasi_errno_t __wasi_args_get_impl(uint8_t** argv, uint8_t* argv_buf)
{
   [[clang::import_name("tester_get_args")]] void tester_get_args(uint8_t * *argv,
                                                                  uint8_t * argv_buf);
   tester_get_args(argv, argv_buf);
   return 0;
}

extern "C" int32_t __imported_wasi_snapshot_preview1_args_get(int32_t arg0, int32_t arg1)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("args_get")))
{
   return __wasi_args_get_impl((uint8_t**)arg0, (uint8_t*)arg1);
}
