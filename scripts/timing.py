#!/usr/bin/env python

#   Copyright (C) 2025 Simon Crase

#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

'''Determine execution times for sections of code'''

from argparse import ArgumentParser
from os.path import basename, join, splitext
from time import time
import numpy as np
from matplotlib import rc
from matplotlib.pyplot import figure, show
from re import compile
from statistics import mean,stdev

def parse_arguments():
    '''Parse command line arguments'''
    parser = ArgumentParser(__doc__)
    parser.add_argument('--seed',type=int,default=None,help='Seed for random number generator')
    parser.add_argument('-o', '--out', default = basename(splitext(__file__)[0]),help='Name of output file')
    parser.add_argument('--input', default = r'C:\cygwin64\home\Weka\galaxy\logs',help='Name of output file')
    parser.add_argument('--figs', default = './figs', help = 'Name of folder where plots are to be stored')
    parser.add_argument('--show', action = 'store_true', help   = 'Show plot')
    parser.add_argument('-d', '--delta', type=float, default=0.005, help='Range for histogram')
    parser.add_argument('--bins', default='scott', type=get_bins, help = 'Binning strategy or number of bins')
    parser.add_argument('file')
    return parser.parse_args()

def get_bins(bins):
    '''
    Used to parse args.bins: either a number of bins, or the name of a binning strategy.
    '''
    try:
        return int(bins)
    except ValueError:
        if bins in ['auto', 'fd', 'doane', 'scott', 'sturges', 'sqrt', 'stone', 'rice']:
            return bins
        raise ArgumentTypeError(f'Invalid binning strategy "{bins}"')

def extract_observations(file_name):
    pattern = compile(r'(\S+)\s(\d+):\s([.0123456789]+)')
    previous = None
    dt = dict()
    with open(file_name) as f:
        for line in f:
            matched = pattern.match(line.strip())
            if matched:
                file = matched.group(1)
                n = matched.group(2)
                t = float(matched.group(3))
                if previous != None:
                    file0,n0,t0 = previous
                    key = f'{file} {n}:'
                    if not key in dt:
                        dt[key] = []
                    dt[key].append( t - t0)

                previous = (file,n,t)
    return dt

def get_file_name(name,default_ext='png',figs='./figs',seq=None):
    '''
    Used to create file names

    Parameters:
        name          Basis for file name
        default_ext   Extension if non specified
        figs          Directory for storing figures
        seq           Used if there are multiple files
    '''
    base,ext = splitext(name)
    if len(ext) == 0:
        ext = default_ext
    if seq != None:
        base = f'{base}{seq}'
    qualified_name = f'{base}.{ext}'
    return join(figs,qualified_name) if ext == 'png' else qualified_name

if __name__=='__main__':
    rc('font',**{'family':'serif','serif':['Palatino']})
    rc('text', usetex=True)
    start  = time()
    args = parse_arguments()
    dt = extract_observations(join(args.input,args.file))

    fig = figure(figsize=(12,12))
    ax = fig.add_subplot(1,1,1)

    for key,observations in dt.items():
        if len(observations) > 1:
            ax.hist(observations, args.bins, density=True, histtype='bar', stacked=True,
                    label=f'{key} {mean(observations):.5f} {stdev(observations):.5f}')

    ax.set_xlim((0,args.delta))
    ax.set_xlabel('Time (milliseconds)')
    ax.legend(title=f'Bins={args.bins}')
    ax.set_title(args.file)
    fig.savefig(get_file_name(args.out,figs=args.figs))
    elapsed = time() - start
    minutes = int(elapsed/60)
    seconds = elapsed - 60*minutes
    print (f'Elapsed Time {minutes} m {seconds:.2f} s')

    if args.show:
        show()
