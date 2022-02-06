#include <wasi/api.h>
#include "polyfill_constants.hpp"

inline __wasi_errno_t __wasi_path_open_impl(__wasi_fd_t fd,
                                            __wasi_lookupflags_t dirflags,
                                            const char* path,
                                            size_t path_len,
                                            __wasi_oflags_t oflags,
                                            __wasi_rights_t fs_rights_base,
                                            __wasi_rights_t fs_rights_inherting,
                                            __wasi_fdflags_t fdflags,
                                            __wasi_fd_t* opened_fd)
{
   [[clang::import_name("tester_open_file")]] uint32_t tester_open_file(
       const char* path, size_t path_len, uint16_t oflags, uint64_t fs_rights_base,
       uint16_t fdflags, int* opened_fd);
   if (fd != polyfill_root_dir_fd)
      return __WASI_ERRNO_BADF;
   return tester_open_file(path, path_len, oflags, fs_rights_base, fdflags, opened_fd);
}

extern "C" int32_t __imported_wasi_snapshot_preview1_path_open(int32_t arg0,
                                                               int32_t arg1,
                                                               int32_t arg2,
                                                               int32_t arg3,
                                                               int32_t arg4,
                                                               int64_t arg5,
                                                               int64_t arg6,
                                                               int32_t arg7,
                                                               int32_t arg8)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("path_open")))
{
   return __wasi_path_open_impl(arg0, arg1, (const char*)arg2, arg3, arg4, arg5, arg6, arg7,
                                (__wasi_fd_t*)arg8);
}
