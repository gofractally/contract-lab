#include <wasi/api.h>

extern "C" int32_t __imported_wasi_snapshot_preview1_fd_close(int32_t fd)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("fd_close")))
{
   [[clang::import_name("tester_close_file")]] uint32_t tester_close_file(int32_t fd);
   return tester_close_file(fd);
}
