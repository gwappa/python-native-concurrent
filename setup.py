from setuptools import setup, find_packages
from distutils.extension import Extension
from Cython.Build import cythonize
import os, re

with open("README.md", "r") as fh:
    long_description = fh.read()

# parse version from README
version_pattern = re.compile(r"Version ([0-9][0-9\.]+)")
__VERSION__ = None
for line in long_description.split("\n"):
    has_version = version_pattern.search(line)
    if has_version:
        __VERSION__ = has_version.group(1)
if __VERSION__ is None:
    raise RuntimeError("failed to parse version string from README.md")

srcdir = "native_concurrent"

lib_c       = os.path.join(srcdir, "_concurrent.c")
lib_pyx     = os.path.join(srcdir, "__init__.pyx")
lib_link    = []
if os.name == "posix":
    lib_link += ['pthread']

lib = Extension('native_concurrent',
                sources      = [lib_c, lib_pyx],
                include_dirs = [srcdir],
                libraries    = lib_link )
setup(
    name             = "native-concurrent",
    version          = __VERSION__,
    description      ='platform-native mutex/conditional variables for cython.',
    long_description=long_description,
    long_description_content_type="text/markdown",
    url              = 'https://github.com/gwappa/python-native-concurrent',
    author           = 'Keisuke Sehara',
    author_email     = 'keisuke.sehara@gmail.com',
    license          = 'MIT',
    install_requires = [],
    python_requires  = '>=3',
    classifiers      = [
        'Development Status :: 3 - Alpha',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3',
        'Intended Audience :: Science/Research',
        'Operating System :: POSIX :: Linux',
        'Programming Language :: Cython',
    ],
    packages         = find_packages(),
    ext_modules      = cythonize([lib]),
    zip_safe         = False
)
