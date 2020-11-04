#
# MIT License
#
# Copyright (c) 2020 Keisuke Sehara
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

# cython: language_level = 3

""" a thin wrapper for _concurrent.

you can have access to all the functions, plus:

+ mutex/conditional type is renamed as mutex_t and cond_t, respectively.
+ Mutex/Condition wrapper Python class is available.
+ errorcheck() Python function is available for raising RuntimeError's.

"""

cdef extern from "_concurrent.h":

    ctypedef struct mutex_t "nativemutex":
        pass
    ctypedef struct cond_t "nativecond":
        pass

    int mutex_init      "nativemutex_init"    (mutex_t *mutex) nogil
    int mutex_lock      "nativemutex_lock"    (mutex_t *mutex) nogil
    int mutex_trylock   "nativemutex_trylock" (mutex_t *mutex) nogil
    int mutex_unlock    "nativemutex_unlock"  (mutex_t *mutex) nogil
    int mutex_free      "nativemutex_free"    (mutex_t *mutex) nogil

    int cond_init       "nativecond_init"     (cond_t *cond) nogil
    int cond_wait       "nativecond_wait"     (cond_t *cond, mutex_t *mutex, long timeout_msec) nogil
    int cond_notify     "nativecond_notify"   (cond_t *cond) nogil
    int cond_notify_all "nativecond_notify_all" (cond_t *cond) nogil
    int cond_free       "nativecond_free"     (cond_t *cond) nogil

    int get_error    (int code, char *buf, int buflen) nogil

cdef  size_t  BUFSIZ  = 2048

cpdef void    errorcheck(int code)

cdef class Mutex:
    cdef mutex_t    _mutex
    cdef int        _err


cdef class Condition:
    cdef cond_t     _cond
    cdef int        _err
