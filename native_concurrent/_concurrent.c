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

#include "_concurrent.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define get_opaque(ptr) (&((ptr)->_opaque))
#define MILLION 1000000
#define BILLION 1000000000

int nativemutex_init      (nativemutex *mutex)
{
#ifdef _WIN32
    InitializeCriticalSectionAndSpinCount(get_opaque(mutex), 0x400); // try setting spin count this way for now
    return 0;
#else
    if( pthread_mutex_init(get_opaque(mutex), 0) ) // try setting mutexatttr this way for now
    {
        return errno;
    } else {
        return 0;
    }
#endif
}

int nativemutex_lock      (nativemutex *mutex)
{
#ifdef _WIN32
    EnterCriticalSection(get_opaque(mutex));
    return 0;
#else
    if( pthread_mutex_lock(get_opaque(mutex)) )
    {
        return errno;
    } else {
        return 0;
    }
#endif
}

int nativemutex_trylock   (nativemutex *mutex)
{
#ifdef _WIN32
    return (TryEnterCriticalSection(get_opaque(mutex)) != 0)? 0 : 0xA7; // returns LOCK_FAILED
#else
    switch( pthread_mutex_trylock(get_opaque(mutex)) )
    {
    case 0:
        // successful
        return 0;
    case EBUSY:
        // already locked by another thread
        return EBUSY;
    case EINVAL:
    default:
        // invalid mutex
        return errno;
    }
#endif
}

int nativemutex_unlock    (nativemutex *mutex)
{
#ifdef _WIN32
    LeaveCriticalSection(get_opaque(mutex));
    return 0;
#else
    if( pthread_mutex_unlock(get_opaque(mutex)) )
    {
        return errno;
    } else {
        return 0;
    }
#endif
}

int nativemutex_free      (nativemutex *mutex)
{
#ifdef _WIN32
    DeleteCriticalSection(get_opaque(mutex));
    return 0;
#else
    if( pthread_mutex_destroy(get_opaque(mutex)) )
    {
        return errno;
    } else {
        return 0;
    }
#endif
}

int nativecond_init       (nativecond *cond)
{
#ifdef _WIN32
    InitializeConditionVariable(get_opaque(cond));
    return 0;
#else
    if( pthread_cond_init(get_opaque(cond), 0) )
    {
        return errno;
    } else {
        return 0;
    }
#endif
}

int nativecond_wait       (nativecond *cond, nativemutex *mutex, long timeout_msec)
{
#ifdef _WIN32
    if (SleepConditionVariableCS(get_opaque(cond), get_opaque(mutex), (timeout_msec>=0)? timeout_msec: INFINITE) == 0)
    {
        return GetLastError();
    } else {
        return 0;
    }
#else
    int err;
    if( timeout_msec < 0 ){
        err = pthread_cond_wait(get_opaque(cond), get_opaque(mutex));
    } else {
        struct timespec timeout;
        timeout.tv_nsec = (timeout_msec * MILLION) % BILLION;
        timeout.tv_sec = time(0) + ( timeout_msec - timeout.tv_nsec );
        err = pthread_cond_timedwait(get_opaque(cond), get_opaque(mutex), &timeout);
    }
    if( err ){
        return errno;
    } else {
        return 0;
    }
#endif
}

int nativecond_notify     (nativecond *cond)
{
#ifdef _WIN32
    WakeConditionVariable(get_opaque(cond));
    return 0;
#else
    if( pthread_cond_signal(get_opaque(cond)) )
    {
        return errno;
    } else {
        return 0;
    }
#endif
}

int nativecond_notify_all (nativecond *cond)
{
#ifdef _WIN32
    WakeAllConditionVariable(get_opaque(cond));
    return 0;
#else
    if( pthread_cond_broadcast(get_opaque(cond)) )
    {
        return errno;
    } else {
        return 0;
    }
#endif
}

int nativecond_free       (nativecond *cond)
{
#ifdef _WIN32
    // seems to need nothing for condition variable
    return 0;
#else
    if( pthread_cond_destroy(get_opaque(cond)) )
    {
        return errno;
    } else {
        return 0;
    }
#endif
}

size_t get_error           (int code, char *buf, size_t buflen)
{
#ifdef _WIN32
    size_t ret = FormatMessage(
                    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    code,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPSTR)buf,
                    (DWORD)buflen,
                    NULL);
    if( ret == 0 ){
        fprintf(stderr, "***unexpected error occurred during platform::get_error(): %d\n", GetLastError());
    }
    return ret;
#else
    strncpy(buf, strerror(code), buflen-1);
    buf[buflen-1] = '\0';
    return strlen(buf);
#endif
}
