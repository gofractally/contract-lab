#include <wasi/api.h>
#include "polyfill_constants.hpp"

inline __wasi_errno_t __wasi_fd_prestat_get_impl(__wasi_fd_t fd, __wasi_prestat_t* buf)
{
   if (fd == polyfill_root_dir_fd)
   {
      buf->tag = __WASI_PREOPENTYPE_DIR;
      buf->u.dir = {
          1  // strlen("/")
      };
      return 0;
   }
   return __WASI_ERRNO_BADF;
}

extern "C" int32_t __imported_wasi_snapshot_preview1_fd_prestat_get(int32_t arg0, int32_t arg1)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("fd_prestat_get")))
{
   return __wasi_fd_prestat_get_impl(arg0, (__wasi_prestat_t*)arg1);
}
