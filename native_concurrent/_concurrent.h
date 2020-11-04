/*
 * MIT License
 *
 * Copyright (c) 2020 Keisuke Sehara
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <stddef.h>

#ifdef _WIN32
#include <winsock2.h> // instead of windows.h

typedef CRITICAL_SECTION    _opaquemutex_t;
typedef CONDITION_VARIABLE  _opaquecond_t;

#else
#include <pthread.h>

typedef pthread_mutex_t     _opaquemutex_t;
typedef pthread_cond_t      _opaquecond_t;
#endif

typedef struct _nativemutex {
    _opaquemutex_t  _opaque;
} nativemutex;

typedef struct _nativecond {
    _opaquecond_t   _opaque;
} nativecond;

/**
*   the below function returns zero if no error.
*/

#ifdef __clang__
    #pragma clang diagnostic ignored "-Wunused-function"
#elif __GNUC__
    #pragma gcc diagnostic ignored "-Wunused-function"
#elif _MSC_VER
    #pragma warning( disable: 4930 )
    // should be: unused-function
#endif

int nativemutex_init      (nativemutex *mutex);
int nativemutex_lock      (nativemutex *mutex);
int nativemutex_trylock   (nativemutex *mutex);
int nativemutex_unlock    (nativemutex *mutex);
int nativemutex_free      (nativemutex *mutex);

int nativecond_init       (nativecond *cond);
int nativecond_wait       (nativecond *cond, nativemutex *mutex, long timeout_msec);
int nativecond_notify     (nativecond *cond);
int nativecond_notify_all (nativecond *cond);
int nativecond_free       (nativecond *cond);

size_t get_error        (int code, char *buf, size_t buflen);
