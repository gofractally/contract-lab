#include <wasi/api.h>

inline __wasi_errno_t __wasi_args_sizes_get_impl(__wasi_size_t* argc, __wasi_size_t* argv_buf_size)
{
   static_assert(sizeof(__wasi_size_t) == sizeof(uint32_t));
   [[clang::import_name("tester_get_arg_counts")]] void tester_get_arg_counts(
       __wasi_size_t * argc, __wasi_size_t * argv_buf_size);
   tester_get_arg_counts(argc, argv_buf_size);
   return 0;
}

extern "C" int32_t __imported_wasi_snapshot_preview1_args_sizes_get(int32_t arg0, int32_t arg1)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("args_sizes_get")))
{
   return __wasi_args_sizes_get_impl((__wasi_size_t*)arg0, (__wasi_size_t*)arg1);
}
