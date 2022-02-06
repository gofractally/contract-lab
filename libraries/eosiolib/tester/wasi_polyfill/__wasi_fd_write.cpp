#include <wasi/api.h>

inline __wasi_errno_t __wasi_fd_write_impl(__wasi_fd_t fd,
                                           const __wasi_ciovec_t* iovs,
                                           size_t iovs_len,
                                           __wasi_size_t* nwritten)
{
   [[clang::import_name("tester_write_file")]] uint32_t tester_write_file(
       int32_t fd, const void* data, uint32_t size);
   if (nwritten)
      *nwritten = 0;
   for (; iovs_len; --iovs_len, ++iovs)
   {
      auto error = tester_write_file(fd, iovs->buf, iovs->buf_len);
      if (error)
         return error;
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
