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

void *_sbrk(int incr) { return (void *)-1; }

void *realloc(void *mem, size_t newsize)
{
    if (newsize == 0)
    {
        free(mem);
        return NULL;
    }
    void *p;
    p = malloc(newsize);
    if (p)
    {
        if (mem != NULL)
        {
            memcpy(p, mem, newsize);
            free(mem);
        }
    }
    return p;
}

void *calloc(size_t count, size_t size)
{
    size_t alloc_size = count * size;
    void *new = malloc(alloc_size);
    if (new)
    {
        memset(new, 0, alloc_size);
        return new;
    }
    return NULL;
}

//////////////////////////////////////////////////////////////////////////////

int _isatty(int fd) { return (unsigned int)fd <= STDERR_FILENO; }

int _fstat_r(struct _reent *r, int fd, struct stat *st) { return -EINVAL; }

_off_t _lseek_r(struct _reent *r, int fd, _off_t where, int whence)
{
    int err = -EINVAL;
    errno = (err < 0) ? -err : 0;
    return err;
}

int _close_r(struct _reent *r, int fd)
{
    int err = 0;
    errno = err;
    return err;
}

_ssize_t _read_r(struct _reent *r, int fd, void *buf, size_t len)
{
    int err = -EINVAL;
    errno = (err < 0) ? -err : 0;
    return err;
}

_ssize_t _write_r(struct _reent *r, int fd, const void *buf, size_t len)
{
    int err = -EINVAL;
    errno = (err < 0) ? -err : 0;
    return err;
}

//////////////////////////////////////////////////////////////////////////////
