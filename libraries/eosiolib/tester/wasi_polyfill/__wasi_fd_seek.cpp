#include <wasi/api.h>

[[clang::import_name("prints")]] extern "C" void prints(const char*);

inline __wasi_errno_t __wasi_fd_seek_impl(__wasi_fd_t fd,
                                          __wasi_filedelta_t offset,
                                          __wasi_whence_t whence,
                                          __wasi_filesize_t* newoffset)
{
   [[clang::import_name("tester_abort"), noreturn]] void tester_abort();
   prints("__wasi_fd_seek not implemented");
   tester_abort();
}

extern "C" int32_t __imported_wasi_snapshot_preview1_fd_seek(int32_t arg0,
                                                             int64_t arg1,
                                                             int32_t arg2,
                                                             int32_t arg3)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("fd_seek")))
{
   return __wasi_fd_seek_impl(arg0, arg1, arg2, (__wasi_filesize_t*)arg3);
}
