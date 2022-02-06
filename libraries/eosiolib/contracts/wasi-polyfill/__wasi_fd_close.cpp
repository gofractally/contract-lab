#include <stdlib.h>
#include <wasi/api.h>

[[clang::import_name("prints")]] extern "C" void prints(const char*);

extern "C" int32_t __imported_wasi_snapshot_preview1_fd_close(int32_t fd)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("fd_close")))
{
   prints("__wasi_fd_close not implemented");
   abort();
}
