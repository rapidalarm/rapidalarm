#!/bin/env python3
# Evan Widloski - 2020-04-16
# Convert ventMAP data to csv containing pressure waveform and pip/peep/rr labels
# for each sample
# https://github.com/hahnicity/ventMAP/

import io
import csv
import argparse

from ventmap.breath_meta import get_production_breath_meta
from ventmap.raw_utils import extract_raw
from ventmap.constants import META_HEADER

parser = argparse.ArgumentParser(description="Convert ventMAP csv to rapidalarm csv")
parser.add_argument('infile', type=str, help="path to input ventMAP csv")
parser.add_argument('outfile', type=str, help="path to output csv")
parser.add_argument('-r', type=str, default=50, help="input file samplerate")
args = parser.parse_args()

generator = extract_raw(io.open(args.infile), False)

# pressure, flow, PIP, PEEP, RR waveforms
pressure = []
flow = []
pip = []
peep = []
rr = []

# read each breath waveform and precomputed scalar metrics
for breath in generator:
    # load single breath
    prod_breath_meta = get_production_breath_meta(breath)

    # get breath pressure and ground truth PIP, PEEP, RR
    pressure += breath['pressure']
    flow += breath['flow']

    # expand scalar PIP/PEEP/RR metric to same length as breath
    breath_samples = len(breath['pressure'])
    pip += breath_samples * [prod_breath_meta[META_HEADER.index('PIP')]]
    peep += breath_samples * [prod_breath_meta[META_HEADER.index('PEEP')]]
    rr += breath_samples * [prod_breath_meta[META_HEADER.index('inst_RR')]]

with open(args.outfile, 'w') as f:
    # write samplerate as metadata header
    f.write("#samplerate,{}\n".format(args.r))
    # write column header
    f.write("pressure,flow,true_pip,true_peep,true_rr\n")

    for x in zip(pressure, flow, pip, peep, rr):
        f.write(",".join(map(str, x)) + "\n")
