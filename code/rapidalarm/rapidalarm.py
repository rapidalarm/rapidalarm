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
from rapidalarm.build_algorithm import build


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


def load_pandas(dataset, rebuild=False, end=None):
    """Run algorithm with recorded data and return pandas DataFrame with result

    Args:
        data (str): path to dataset

    Returns:
        pandas.DataFrame: dataframe containing alg. input and results
    """
    df = load_dataset(dataset)

    if rebuild:
        build()

    from rapidalarm.algorithm import lib

    lib.init_algorithm(df.sample_rate)

    pip, peep, rr, v_high, v_low, alarm_raised = [], [], [], [], [], []
    debug = []

    # build dict from enums
    alarms = [
        'ALARM_HP', 'ALARM_HR', 'ALARM_LP', 'ALARM_LR', 'ALARM_NC', 'ALARM_NONE'
    ]
    alarms = {getattr(lib, s):s for s in alarms}

    for p in df.pressure[:end]:
        lib.run_algorithm(p)

        pip.append(lib.pip)
        peep.append(lib.peep)
        rr.append(lib.respiration_rate)
        v_high.append(lib.v_high)
        v_low.append(lib.v_low)
        alarm_raised.append(alarms[lib.alarm_raised])
        # alarm_raised.append(lib.alarm_raised)
        debug.append(lib.t_peak)

    df['pip'] = pip
    df['peep'] = peep
    df['rr'] = rr
    df['v_high'] = v_high
    df['v_low'] = v_low
    # df['alarm_raised'] = pd.Categorical(alarm_raised, categories=alarms.values())
    df['alarm_raised'] = alarm_raised
    df['debug'] = debug

    return df


def csv(args):

    df = load_pandas(args.dataset, args.rebuild)

    df.to_csv(sys.stdout, index=False)

def plot(args):

    df = load_pandas(args.dataset, args.rebuild)

    fig, [plt1, plt2, plt3] = plt.subplots(3, 1, sharex=True, figsize=(15, 6))

    plt1.plot(df.t[args.s:args.e], df.pressure[args.s:args.e], 'k', linewidth=0.5)
    plt1.plot(df.t[args.s:args.e], df.v_high[args.s:args.e], 'r', linewidth=0.5)
    plt1.plot(df.t[args.s:args.e], df.v_low[args.s:args.e], 'b', linewidth=0.5)
    plt1.plot(df.t[args.s:args.e], df.pip[args.s:args.e], 'r')
    plt1.plot(df.t[args.s:args.e], df.peep[args.s:args.e], 'b')
    plt1.legend(['Pressure', 'High Envelope', 'Low Envelope', 'PIP', 'PEEP'])

    plt2.plot(df.t[args.s:args.e], df.rr[args.s:args.e])
    # plt2.plot(df.t[args.s:args.e], df.debug[args.s:args.e])
    plt2.set_ylabel('breaths/min')
    plt2.legend(['RR'])
    plt2.grid(True)


    plt3.plot(df.t[args.s:args.e], df.alarm_raised[args.s:args.e])
    plt3.grid(True)

    plt.xlabel('time (s)')
    plt.show()


def main():

    parser = argparse.ArgumentParser(description="Algorithm Tester")

    parser.add_argument('--rebuild', action='store_true', default=False, help="rebuild algorithm.c")
    parser.add_argument('--debug', action='store_true', default=False, help="enable debugging")

    subparsers = parser.add_subparsers()
    subparsers.required = True

    csv_parser = subparsers.add_parser('csv', help="generate csv output")
    # csv_parser.add_argument('start', type=int, help="simulation start sample #")
    # csv_parser.add_argument('end', type=int, help="simulation end sample #")
    csv_parser.set_defaults(func=csv)

    plot_parser = subparsers.add_parser('plot', help="plot input waveform along with estimated parameters")
    plot_parser.add_argument('dataset', type=str, help="path to dataset")
    plot_parser.add_argument('-s', metavar='START', type=int, default=0, help="simulation start sample num")
    plot_parser.add_argument('-e', metavar='END', type=int, default=None, help="simulation end sample num")
    plot_parser.set_defaults(func=plot)

    args = parser.parse_args()

    if args.debug:
        log.setLevel(logging.DEBUG)
        log.debug("Debugging enabled")

    args.func(args)


if __name__ == '__main__':
    main()
