#include <wasi/api.h>
#include "polyfill_constants.hpp"

inline __wasi_errno_t __wasi_fd_prestat_dir_name_impl(__wasi_fd_t fd,
                                                      uint8_t* path,
                                                      __wasi_size_t path_len)
{
   if (fd == polyfill_root_dir_fd)
   {
      if (path_len > 0)
         path[0] = '/';
      return 0;
   }
   return __WASI_ERRNO_BADF;
}

extern "C" int32_t __imported_wasi_snapshot_preview1_fd_prestat_dir_name(int32_t arg0,
                                                                         int32_t arg1,
                                                                         int32_t arg2)
    __attribute__((__import_module__("wasi_snapshot_preview1"),
                   __import_name__("fd_prestat_dir_name")))
{
   return __wasi_fd_prestat_dir_name_impl(arg0, (uint8_t*)arg1, arg2);
}
