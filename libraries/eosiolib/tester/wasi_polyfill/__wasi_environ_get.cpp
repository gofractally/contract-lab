#include <wasi/api.h>

[[clang::import_name("prints")]] extern "C" void prints(const char*);
[[clang::import_name("tester_abort"), noreturn]] extern "C" void tester_abort();

inline __wasi_errno_t __wasi_environ_get_impl(uint8_t** environ, uint8_t* environ_buf)
{
   prints("__wasi_environ_get not implemented");
   tester_abort();
}

extern "C" int32_t __imported_wasi_snapshot_preview1_environ_get(int32_t arg0, int32_t arg1)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("environ_get")))
{
   return __wasi_environ_get_impl((uint8_t**)arg0, (uint8_t*)arg1);
}
