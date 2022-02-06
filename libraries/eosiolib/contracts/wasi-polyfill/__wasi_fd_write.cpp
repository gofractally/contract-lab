#include <wasi/api.h>

inline __wasi_errno_t __wasi_fd_write_impl(__wasi_fd_t fd,
                                           const __wasi_ciovec_t* iovs,
                                           size_t iovs_len,
                                           __wasi_size_t* nwritten)
{
   [[clang::import_name("prints_l")]] void prints_l(const void*, uint32_t);
   if (nwritten)
      *nwritten = 0;
   for (; iovs_len; --iovs_len, ++iovs)
   {
      prints_l(iovs->buf, iovs->buf_len);
      if (nwritten)
         *nwritten += iovs->buf_len;
   }
   return 0;
}

extern "C" int32_t __imported_wasi_snapshot_preview1_fd_write(int32_t arg0,
                                                              int32_t arg1,
                                                              int32_t arg2,
                                                              int32_t arg3)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("fd_write")))
{
   return __wasi_fd_write_impl(arg0, (const __wasi_ciovec_t*)arg1, arg2, (__wasi_size_t*)arg3);
}
