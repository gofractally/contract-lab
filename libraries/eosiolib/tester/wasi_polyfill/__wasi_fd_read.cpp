#include <wasi/api.h>

inline __wasi_errno_t __wasi_fd_read_impl(__wasi_fd_t fd,
                                          const __wasi_iovec_t* iovs,
                                          size_t iovs_len,
                                          __wasi_size_t* nread)
{
   [[clang::import_name("tester_read_file")]] uint32_t tester_read_file(
       int32_t fd, const void* data, uint32_t size, uint32_t& bytes_read);
   if (nread)
      *nread = 0;
   for (; iovs_len; --iovs_len, ++iovs)
   {
      uint32_t bytes_read = 0;
      auto error = tester_read_file(fd, iovs->buf, iovs->buf_len, bytes_read);
      if (error)
         return error;
      if (nread)
         *nread += bytes_read;
      if (bytes_read < iovs->buf_len)
         return 0;
   }
   return 0;
}

extern "C" int32_t __imported_wasi_snapshot_preview1_fd_read(int32_t arg0,
                                                             int32_t arg1,
                                                             int32_t arg2,
                                                             int32_t arg3)
    __attribute__((__import_module__("wasi_snapshot_preview1"), __import_name__("fd_read")))
{
   return __wasi_fd_read_impl(arg0, (const __wasi_iovec_t*)arg1, arg2, (__wasi_size_t*)arg3);
}
