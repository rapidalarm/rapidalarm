#!/bin/env python3
# Evan Widloski - 2020-04-15
# Automated testing for alarm/algorithm.h

# Build alarm/algorithm.so with:
   # gcc -g -o algorithm.so -shared -fPIC algorithm.h

from cffi import FFI

source_file = '../alarm/algorithm.c'
header_file = '../alarm/algorithm.h'
include_dir = '../alarm'

ffi = FFI()

# build shared object
ffi.set_source("algorithm", open(source_file, 'r').read(), include_dirs=[include_dir])
ffi.compile(target='algorithm.so')
lib = ffi.dlopen('./algorithm.so')

# read header to get datatypes, strip #ifdef/#define lines
header_lines = open(header_file, 'r').readlines()
header_lines = header_lines[8:-3]
ffi.cdef('\n'.join(header_lines))


pip, peep, rr = [], [], []

for _ in range(100):
    lib.run_algorithm(100, 100)
    pip.append(lib.pip)
    peep.append(lib.peep)
    rr.append(lib.rr)
