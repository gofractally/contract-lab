#include <wasi/api.h>

#include <stdio.h>

extern "C" int32_t __imported_wasi_snapshot_preview1_fd_fdstat_set_flags(int32_t arg0, int32_t arg1)
    __attribute__((__import_module__("wasi_snapshot_preview1"),
                   __import_name__("fd_fdstat_set_flags")))
{
   // TODO
   return 0;
}
