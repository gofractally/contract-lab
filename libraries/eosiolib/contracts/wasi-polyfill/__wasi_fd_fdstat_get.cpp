#include <wasi/api.h>

inline __wasi_errno_t __wasi_fd_fdstat_get_impl(__wasi_fd_t fd, __wasi_fdstat_t* stat)
{
   stat->fs_filetype = __WASI_FILETYPE_CHARACTER_DEVICE;
   stat->fs_flags = __WASI_FDFLAGS_APPEND;
   stat->fs_rights_base = __WASI_RIGHTS_FD_READ | __WASI_RIGHTS_FD_WRITE;
   stat->fs_rights_inheriting = 0;
   return 0;
}

extern "C" int32_t __imported_wasi_snapshot_preview1_fd_fdstat_get(int32_t arg0, int32_t arg1)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("fd_fdstat_get")))
{
   return __wasi_fd_fdstat_get_impl(arg0, (__wasi_fdstat_t*)arg1);
}
