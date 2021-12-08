////////////////////////////////////////////////////////////////////////////////////////
//
//      2021 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////////////

#include "interface.h"

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void) __attribute__((weak));
extern void (*__init_array_start[])(void) __attribute__((weak));
extern void (*__init_array_end[])(void) __attribute__((weak));
extern void (*__fini_array_start[])(void) __attribute__((weak));
extern void (*__fini_array_end[])(void) __attribute__((weak));

extern void _init(void) __attribute__((weak));
extern void _fini(void) __attribute__((weak));

void __libc_init_array(void)
{
    size_t count;
    size_t i;
    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++)
        __preinit_array_start[i]();
    _init();
    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++)
        __init_array_start[i]();
}

void __libc_fini_array(void)
{
    size_t count;
    size_t i;
    count = __fini_array_end - __fini_array_start;
    for (i = count; i > 0; i--)
        __fini_array_start[i - 1]();
    _fini();
}

void abort(void)
{
    printf("[ABORT]");    
    while (1)
    {
        Ql_Sleep(1000);
#ifdef ABORT_RESET
        Ql_Reset(0);
#endif
    }
}

void __cxa_finalize(void *handle) {}
void __cxa_pure_virtual(void) { abort(); }
void __cxa_deleted_virtual(void) { abort(); }

//////////////////////////////////////////////////////////////////////////////

/* Prevent STD::MEMORY allocation */

void *_sbrk(int incr) { return (void *)-1; }

void *malloc(size_t size)
{
    if (0 != size)
    {
        return Ql_MEM_Alloc(size);
    }
    return NULL;
}

void free(void *ptr)
{
    if (NULL != ptr)
    {
        Ql_MEM_Free(ptr);
    }
}

void *realloc(void *mem, size_t newsize)
{
    if (0 == newsize)
    {
        free(mem);
        return NULL;
    }
    void *ptr = Ql_MEM_Alloc(newsize);
    if (NULL != ptr)
    {
        if (NULL != mem)
        {
            memcpy(ptr, mem, newsize);
            free(mem);
        }
    }
    return ptr;
}

void *calloc(size_t count, size_t size)
{
    size_t alloc_size = count * size;
    void *ptr = Ql_MEM_Alloc(alloc_size);
    if (NULL != ptr)
    {
        memset(ptr, 0, alloc_size);
        return ptr;
    }
    return NULL;
}

inline void _free_r(struct _reent *ignore, void *ptr) { free(ptr); }

inline void *_malloc_r(struct _reent *ignore, size_t size) { return malloc(size); }

inline void *_realloc_r(struct _reent *ignored, void *ptr, size_t size) { return realloc(ptr, size); }

inline void *_calloc_r(struct _reent *ignored, size_t element, size_t size) { return calloc(element, size); }

//////////////////////////////////////////////////////////////////////////////

Enum_SerialPort stdio_port = UART_PORT_END;

static int fs_file_flags(int flag)
{
    int result = 0;
    if ((flag & 3) == O_RDONLY)     //
        result |= QL_FS_READ_ONLY;  // 0x00000100
    if ((flag & 3) == O_RDWR)       //
        result |= QL_FS_READ_WRITE; // 0x00000000
    if (flag & O_CREAT)             //
        result |= QL_FS_CREATE;     // 0x00010000
    return result;
}

int _isatty(int fd) { return (unsigned int)fd <= STDERR_FILENO; }

int _fstat_r(struct _reent *ignore, int fd, struct stat *st)
{
    int err = -EINVAL;
    errno = err;
    return -err;
}

_off_t _lseek_r(struct _reent *ignore, int fd, _off_t where, int whence)
{ /* only for files */
    int err = -EINVAL;
    if (fd > STDERR_FILENO)
    {
        err = Ql_FS_Seek(fd - 3, where, whence);
    }
    errno = (err < 0) ? -err : 0;
    return err;
}

int _open_r(struct _reent *ignore, const char *path, int flags, int mode)
{ /* only for files */
    int err = -EINVAL;
    int fd = -1;
    if (path)
    {
        fd = Ql_FS_Open(path, fs_file_flags(flags));
    }
    errno = (fd < 0) ? -err : 0;
    return err == 0 ? fd + 3 : -1;
}

int _close_r(struct _reent *ignore, int fd)
{ /* only for files */
    int err = 0;
    if (fd > STDERR_FILENO)
    {
        Ql_FS_Close(fd - 3);
    }
    errno = 0;
    return err;
}

_ssize_t _read_r(struct _reent *ignore, int fd, void *buf, size_t len)
{ /* only for files */
    int err = -EINVAL;
    if (fd > -1 && buf && len)
    {
        if (fd > STDERR_FILENO)
        {
            int bytes;
            err = Ql_FS_Read(fd - 3, buf, len, &bytes) ? 0 : bytes;
        }
    }
    errno = (err < 0) ? -err : 0;
    return err;
}

_ssize_t _write_r(struct _reent *ignore, int fd, const void *buf, size_t len)
{ /* only for files & printf */
    int err = -EINVAL;
    if (fd > -1 && buf && len)
    {
        if (_isatty(fd) && (stdio_port == UART_PORT1 || stdio_port == UART_PORT2 || stdio_port == UART_PORT3))
        {
            err = Ql_UART_Write(stdio_port, buf, len);
        }
        else
        {
            int bytes;
            err = Ql_FS_Write(fd - 3, buf, len, &bytes) ? 0 : bytes;
        }
    }
    errno = (err < 0) ? -err : 0;
    return err;
}

//////////////////////////////////////////////////////////////////////////////

int rename(const char *Old_Path, const char *New_Path)
{
    int err = Ql_FS_Rename(Old_Path, New_Path);
    errno = (err < 0) ? -err : 0;
    return err;
}

int rmdir(const char *path)
{
    int err = Ql_FS_CreateDir(path);
    errno = (err < 0) ? -err : 0;
    return err;
}

int mkdir(const char *path, mode_t mode)
{
    int err = Ql_FS_CreateDir(path);
    errno = (err < 0) ? -err : 0;
    return err;
}

//////////////////////////////////////////////////////////////////////////////

unsigned int *Ql_convertIP(unsigned int ip)
{
    static char m_ipAddress[4];
    unsigned int *p = (unsigned int *)m_ipAddress;
    *p = ip;
    return p;
}

int Ql_inet_aton(const char *cp, uint32_t *ip)
{
    if (!ip || !cp)
        return 0;
    if (0 == Ql_IpHelper_ConvertIpAddr((u8 *)cp, (u32 *)ip))
        return 1;
    return 0;
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////