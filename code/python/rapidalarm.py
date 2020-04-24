#!/bin/env python3
# Evan Widloski - 2020-04-15
# Automated testing for alarm/algorithm.h

# Build alarm/algorithm.so with:
   # gcc -g -o algorithm.so -shared -fPIC algorithm.h

from cffi import FFI
import argparse
import logging
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import sys

from rapidalarm.algorithm import lib


def load_dataset(datafile):
    """Read a rapidalarm dataset into a Pandas dataframe

    Args:
        datafile (str): path to datafile

    Returns:
        DataFrame: has columns 't', 'pip', 'peep', 'pressure', 'flow'
            and attribute 'sample_rate'
    """

    f = open(datafile, 'r')
    # read sample_rate from first line
    sample_rate = int(f.readline().split(',')[1])

    x = pd.read_csv(f)
    x.sample_rate = sample_rate
    x['t'] = np.linspace(0, len(x) / sample_rate, len(x))

    return x


def run_pandas(dataset):
    """Run algorithm with recorded data and return pandas DataFrame with result

    Args:
        data (str): path to dataset

    Returns:
        pandas.DataFrame: dataframe containing alg. input and results
    """

    df = load_dataset(dataset)

    pip, peep, rr, p_max, p_min = [], [], [], [], []

    lib.init_algorithm(df.sample_rate)

    for p in df.pressure:
        lib.run_algorithm(p)

        pip.append(lib.pip)
        peep.append(lib.peep)
        rr.append(lib.respiration_rate)
        p_max.append(lib.p_max)
        p_min.append(lib.p_min)

    df['pip'] = pip
    df['peep'] = peep
    df['rr'] = rr
    df['p_max'] = p_max
    df['p_min'] = p_min

    return df


def run_csv(args):

    df = run_pandas(args.dataset)

    df.to_csv(sys.stdout, index=False)


def build():
    # build .c and return FFI library

    ffi = FFI()

    source_file = '../alarm/algorithm.c'
    include_dir = '../alarm/'
    types = 'algorithm.cdef'

    # read in variable datatypes
    ffi.cdef(open(types, 'r').read())

    # build shared object
    ffi.set_source(
        "algorithm",
        open(source_file, 'r').read(),
        include_dirs=[include_dir],
    )
    ffi.compile(verbose=True)

    from algorithm import lib

    return lib


def main():

    parser = argparse.ArgumentParser(description="Algorithm Tester")

    subparsers = parser.add_subparsers()
    subparsers.required = True

    parser.add_argument('--debug', action='store_true', default=False, help="enable debugging")

    csv_parser = subparsers.add_parser('csv', help="generate csv output")
    # csv_parser.add_argument('start', type=int, help="simulation start sample #")
    # csv_parser.add_argument('end', type=int, help="simulation end sample #")
    csv_parser.set_defaults(func=run_csv)

    args = parser.parse_args()

    if args.debug:
        log.setLevel(logging.DEBUG)
        log.debug("Debugging enabled")

    args.func(args)


if __name__ == '__main__':
    main()
