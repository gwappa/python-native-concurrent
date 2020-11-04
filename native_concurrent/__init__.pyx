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

from libc.stdio cimport printf
from cpython cimport array as carray
import array

cdef carray.array chararray = array.array('b')

cdef carray.array err = carray.clone(chararray, BUFSIZ, zero=False)

cpdef void errorcheck(int code):
    cdef size_t valid
    if code != 0:
        valid = get_error(code, err.data.as_chars, BUFSIZ)
        raise RuntimeError(err.tobytes()[:valid].decode('utf8'))

cdef class Mutex:
    # cdef mutex_t    _mutex
    # cdef int        _err

    def __cinit__(self):
        self._err = 0
        errorcheck(mutex_init(&(self._mutex)))

    def __dealloc__(self):
        mutex_free(&(self._mutex))

    def lock(self):
        self._err = mutex_lock(&(self._mutex))
        return self._err

    def trylock(self):
        self._err = mutex_trylock(&(self._mutex))
        return self._err

    def unlock(self):
        self._err = mutex_unlock(&(self._mutex))
        return self._err

cdef class Condition:
    # cdef cond_t     _cond
    # cdef int        _err

    def __cinit__(self):
        self._err = 0
        errorcheck(cond_init(&(self._cond)))

    def __dealloc__(self):
        cond_free(&(self._cond))

    def wait(self, Mutex mutex, long timeout_msec):
        self._err = cond_wait(&(self._cond), &(mutex._mutex), timeout_msec)
        return self._err

    def notify(self):
        self._err = cond_notify(&(self._cond))
        return self._err

    def notify_all(self):
        self._err = cond_notify_all(&(self._cond))
        return self._err
