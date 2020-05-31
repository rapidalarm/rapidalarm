# Called from setup.py on package installation.  Builds algorithm.c
# Evan Widloski - 2020-04-23

from cffi import FFI
from pathlib import Path

ffi = FFI()

code_dir = Path(__file__).parent.parent
source_file = code_dir.joinpath('firmware/algorithm.c').absolute()
include_dir = code_dir.joinpath('firmware/').absolute()
cdef = code_dir.joinpath('rapidalarm/algorithm.cdef').absolute()

# read in variable datatypes
ffi.cdef(open(cdef, 'r').read())

# build shared object
ffi.set_source(
    "rapidalarm.algorithm",
    open(source_file, 'r').read(),
    include_dirs=[include_dir],
)

def build():
    ffi.compile(
        verbose=True,
        tmpdir=code_dir
    )


if __name__ == '__main__':
    build()
