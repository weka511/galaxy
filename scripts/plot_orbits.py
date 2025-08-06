#!/usr/bin/env python

# Copyright 2025 Simon Crase
#
# This is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this software.  If not, see <http://www.gnu.org/licenses/>

'''Plot orbits from Barnes Hut Galaxy Simulator, galaxy.exe'''

from argparse import ArgumentParser
from os import listdir
from os.path import join
from re import search
from sys import exit
import numpy as np
from matplotlib.pyplot import figure, show
import mpl_toolkits.mplot3d

def parse_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('--bodies','-b', type=int, default=1000, help='Number of bodies from simulation')
    parser.add_argument('--dpi', type=int, default=300, help='Dots per inch for displaying and saving figure')
    parser.add_argument('--norbits','-n', type=int, default=7, help='Number of orbits')
    parser.add_argument('--maxsamples','-m', type=int, default=1000, help='Maximum number of sample per orbit (-1 to process all samples)')
    parser.add_argument('--prefix', default='galaxy', help='Prefix for configuration files')
    parser.add_argument('--suffix','-s', default='csv', help='Suffix for configuration files')
    parser.add_argument('--delimiter','-d', default=',', help='Delimiter for fields in config file')
    parser.add_argument('--nsigma','-g', type=int,  default=3, help='Number of standard deviations to use for scaling')
    parser.add_argument('--seed', type=int,  default=None, help='Seed for random number generator')
    parser.add_argument('--images','-i',  default='./figs', help='Path to store images')
    parser.add_argument('--path','-p', default='../csrc/configs', help='Path for config files')
    parser.add_argument('--show', action = 'store_true', help   = 'Show plot')
    return parser.parse_args()


def get_limits(xs,n=1):
    '''
    Used to establish limits for plotting.

    Parameters:
       xs    Data from one axis
       n     Controls the limits on data width

    Returns:  mean _/- n standard deviations
    '''
    mean  = np.mean(xs)
    sigma = np.std(xs)
    return (mean-n*sigma,mean+n*sigma)

def plot(Orbits,selector,
         colours=['xkcd:purple','xkcd:green','xkcd:blue','xkcd:pink','xkcd:brown','xkcd:red',
                  'xkcd:light blue', 'xkcd:teal', 'xkcd:orange', 'xkcd:light green', 'xkcd:magenta', 'xkcd:yellow'],
         n=2,
         images='.',
         linestyles = ['-', '--', '-.', ':'],
         dpi=300):
    '''
    Plot orbits
    Parameters:
        data      Positions of sampled points in all orbits
        selector  Used to selectd which points will be plotted
        colours   Colours used to distinguish orbits
    '''
    m,K,_ = Orbits.shape
    if m == 0:
        print ('No data - check path')
        sys.exit(2)

    fig = figure(figsize=(20,20))
    ax = fig.add_subplot(111,  projection='3d')
    for k in range(K):
        ax.plot(Orbits[:,k,0],Orbits[:,k,1],Orbits[:,k,2],
                c=colours[k%len(colours)],
                linestyle=linestyles[(k//len(colours)) % len(linestyles)])
        ax.scatter(Orbits[0,k,0],Orbits[0,k,1],Orbits[0,k,2],
                   c=colours[k%len(colours)],
                   label=f'{k}',
                   s=50,marker='x')
    ax.legend(loc='best',title='Starts of Orbits')
    ax.set_title(f'Orbits of {K} randomly selected stars out of {m}')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_xlim(get_limits(Orbits[:,:,0],n=n))
    ax.set_ylim(get_limits(Orbits[:,:,1],n=n))
    ax.set_zlim(get_limits(Orbits[:,:,2],n=n))
    fig.savefig(join(images,f'orbits-{m}-{K}.png'), dpi=dpi)
    return True

def extract(config_path = './configs/',
            k = 6,
            maxsamples=1000,
            prefix='bodies',
            suffix='csv',
            delimiter=',',
            rng = np.random.default_rng()):
    '''
    Extract data from configuration files

       Parameters:
           config_path     Location of data
           k
           maxsamples      Maximum number of points sampled in each orbit
           prefix          Prefix for configuration files
           suffix          Suffix for configuration files
           delimiter       Delimiter for fields in config file
    '''
    m = None
    Result = None
    n = len(listdir(config_path))   # Total number of points
    skip = 1                              # Used to skip over data so number of points won't exceed maxsamples
    while maxsamples > 0 and n//skip > maxsamples:
        skip *= 10

    i = 0
    for file_name in listdir(config_path):
        match = search(r'{0}[0-9]+\.{1}'.format(prefix,suffix),file_name)
        if match:
            record = np.loadtxt(join(config_path,match.group(0)),delimiter=delimiter)
            if i%skip == 0:
                if m == None:
                    m = len(record)
                    selector = rng.choice(m,size=k)
                    Result = np.zeros((len(listdir(config_path)),k,3))
            Result[i,:,:] = record[selector,1:4]
            i += 1

    return (Result,selector)


if __name__=='__main__':
    args = parse_args()

    rng = np.random.default_rng(args.seed)

    Orbits,selector = extract(config_path = args.path,
                              maxsamples = args.maxsamples,
                              prefix = args.prefix,
                              suffix = args.suffix,
                              delimiter = args.delimiter,
                              k = args.norbits)
    plot(Orbits,selector)

    if args.show:
        show()
