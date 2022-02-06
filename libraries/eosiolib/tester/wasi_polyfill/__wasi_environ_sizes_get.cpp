#include <wasi/api.h>

[[clang::import_name("prints")]] extern "C" void prints(const char*);

inline __wasi_errno_t __wasi_environ_sizes_get_impl(__wasi_size_t* environc,
                                                    __wasi_size_t* environ_buf_size)
{
   [[clang::import_name("tester_abort"), noreturn]] void tester_abort();
   prints("__wasi_environ_sizes_get not implemented");
   tester_abort();
}

extern "C" int32_t __imported_wasi_snapshot_preview1_environ_sizes_get(int32_t arg0, int32_t arg1)
    __attribute__((__import_module__("wasi_snapshot_preview1"),
                   __import_name__("environ_sizes_get")))
{
   return __wasi_environ_sizes_get_impl((__wasi_size_t*)arg0, (__wasi_size_t*)arg1);
}
