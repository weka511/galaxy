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
from glob import glob
from os.path import basename, join, splitext
from re import search
from sys import exit
import numpy as np
from matplotlib.pyplot import figure, show
import mpl_toolkits.mplot3d

def parse_args():
    dpi = 333
    norbits = 7
    maxsamples = -1
    nsigma = 3
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('--seed',type=int,default=None,help='Seed for random number generator')
    parser.add_argument('-o', '--out', default = basename(splitext(__file__)[0]),help='Name of output file')
    parser.add_argument('--figs', default = './figs', help = 'Name of folder where plots are to be stored')
    parser.add_argument('--show', action = 'store_true', help = 'Show plot')
    parser.add_argument('--dpi', type=int, default=dpi, help=f'Dots per inch for displaying and saving figure [{dpi}]')
    parser.add_argument('--norbits','-K', type=int, default=norbits, help=f'Number of orbits [{norbits}]')
    parser.add_argument('--maxsamples','-m', type=int, default=maxsamples, help=f'Maximum number of sample per orbit [{maxsamples} (process all samples)]')
    parser.add_argument('--prefix', default='galaxy', help='Prefix for configuration files')
    parser.add_argument('--suffix','-s', default='csv', help='Suffix for configuration files')
    parser.add_argument('--delimiter','-d', default=',', help='Delimiter for fields in config file')
    parser.add_argument('--nsigma','-g', type=int,  default=nsigma, help=f'Number of standard deviations to use for scaling [{nsigma}]')
    parser.add_argument('--path','-p', default='../csrc/configs', help='Path for config files')
    parser.add_argument('--title','-t',default=None,help='Title for plot')
    parser.add_argument('--centered', action = 'store_true', help = 'Reset centre to origin')
    return parser.parse_args()

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

def read_files(config_path = './configs/',
               K = 6,
               maxsamples=1000,
               prefix='bodies',
               suffix='csv',
               delimiter=',',
               rng = np.random.default_rng(),
               centered = True):
    '''
    Extract data from configuration files

       Parameters:
           config_path     Location of data
           n
           maxsamples      Maximum number of points sampled in each orbit
           prefix          Prefix for configuration files
           suffix          Suffix for configuration files
           delimiter       Delimiter for fields in config file
    '''
    def get_n(path_names,maxsamples):
        '''
        Determine total number of points in a single orbit and the number of
        points to skip so the number of sample won't exceed maxsamples
        '''
        n = len(path_names)
        skip = 1
        while maxsamples > 0 and n//skip > maxsamples:
            skip *= 2
        return skip,n//skip
    path_names = sorted(glob(join(config_path, prefix) + f'*.{suffix}'))
    skip,n = get_n(path_names,maxsamples)

    record = np.loadtxt(path_names[0],delimiter=delimiter)
    number_of_bodies,_ = record.shape
    Data = np.zeros((number_of_bodies,n,3))
    selector = rng.choice(number_of_bodies,size=K,replace=False)
    for time_step in range(n):
        record = np.loadtxt(path_names[skip*time_step],delimiter=delimiter)
        Data[:,time_step,:] = record[:,1:4]
        if centered:
            Weights = record[:,-1]
            Centres = np.average(Data[:,time_step,:],axis=0,weights=Weights)
            np.subtract(Data[:,time_step,:],Centres,out=Data[:,time_step,:])

    return Data[selector,:,:]

def plot(Data,
         number_of_bodies=100,
         colours=['xkcd:purple','xkcd:green','xkcd:blue','xkcd:pink','xkcd:brown','xkcd:red',
                  'xkcd:light blue', 'xkcd:teal', 'xkcd:orange', 'xkcd:light green', 'xkcd:magenta', 'xkcd:yellow'],
         images='.',
         linestyles = ['-', '--', '-.', ':'],
         dpi=300,
         title=None):
    '''
    Plot orbits
    Parameters:
        data      Positions of sampled points in all orbits
        selector  Used to selectd which points will be plotted
        colours   Colours used to distinguish orbits
    '''
    K,m,_ = Data.shape
    if m == 0:
        print ('No data - check path')
        sys.exit(2)

    fig = figure(figsize=(20,20))
    ax = fig.add_subplot(111,  projection='3d')

    for k in range(K):
        ax.plot(Data[k,:,0],Data[k,:,1],Data[k,:,2],
                c=colours[k%len(colours)],
                linestyle=linestyles[(k//len(colours)) % len(linestyles)])
    if title == None:
        title = f'Orbits of {K} randomly selected stars out of {number_of_bodies}'
    ax.set_title(title)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    fig.savefig(get_file_name(args.out,figs=args.figs), dpi=dpi)

if __name__=='__main__':
    args = parse_args()

    rng = np.random.default_rng(args.seed)

    Data = read_files(config_path = args.path,
                      maxsamples = args.maxsamples,
                      prefix = args.prefix,
                      suffix = args.suffix,
                      delimiter = args.delimiter,
                      K = args.norbits,
                      centered = args.centered)
    plot(Data,title=args.title)

    if args.show:
        show()
