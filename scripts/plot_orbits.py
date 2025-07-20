#!/usr/bin/env python

# Copyright (C) 2018-2025 Greenweaves Software Limited
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
#

# Companion to galaxy.exe - plot orbits of randomly selected stars

import argparse
import os
import os.path
import re
import sys
import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d,random

# get_limits
#
# Used to establish limits for plotting.
#
# Parameters:
#    xs    Data from one axis
#    n     Controls the limits on data width
#
# Returns:  mean _/- n standard deviations

def get_limits(xs,n=1):
    mean  = np.mean(xs)
    sigma = np.std(xs)
    return (mean-n*sigma,mean+n*sigma)

# plot
#
# Plot orbits
#
#    Parameters:
#        data      Positions of sampled points in all orbits
#        selector  Used to selectd which points will be plotted
#        colours   Colours used to distinguish orbits

def plot(rr,
         selector=[],
         colours=['r','g','b','m','c','y','k'],
         n=2,
         images='.',
         linestyles = ['-', '--', '-.', ':'],
         dpi=300):
    (m,data)=rr
    if len(data)==0:
        print ('No data - check path')
        return False
    def get_coordinates(body,i):
        return [d[body][i] for d in data]
    plt.figure(figsize=(20,20))
    ax = plt.gcf().add_subplot(111,  projection='3d')
    xs=[]
    ys=[]
    zs=[]
    for body in range(len(data[0])):
        ax.plot(get_coordinates(body,0),get_coordinates(body,1),get_coordinates(body,2),
                c=colours[body%len(colours)],
                linestyle=linestyles[(body//len(colours))%len(linestyles)])
        ax.scatter(get_coordinates(body,0)[0],get_coordinates(body,1)[0],get_coordinates(body,2)[0],c=colours[body%len(colours)],
                       label="Body: {0}".format(selector[body]),s=50,marker='x')
        xs.append(get_coordinates(body,0))
        ys.append(get_coordinates(body,1))
        zs.append(get_coordinates(body,2))

    ax.set_xlim(get_limits(xs,n=n))
    ax.set_ylim(get_limits(ys,n=n))
    ax.set_zlim(get_limits(zs,n=n))

    plt.legend(loc='best')
    plt.title('Orbits of {0} randomly selected stars out of {1}'.format(len(data[0]),m))
    plt.savefig(os.path.join(images,'orbits-{1}-{0}.png'.format(len(data[0]),m)), dpi=dpi)
    return True

# extract
#
# Extract data from configuration files
#
#    Parameters:
#        config_path     Location of data
#        selector        Used to select points that will be extracted
#        maxsamples      Maximum number of points sampled in each orbit
#        prefix          Prefix for configuration files
#        suffix          Suffix for configuration files
#        delimiter       Delimiter for fields in config file

def extract(config_path = './configs/',
            selector=[0,1,2,55,100,400],
            maxsamples=1000,
            prefix='bodies',
            suffix='csv',
            delimiter=','):
    m      = None
    result = []
    n      = len(os.listdir(config_path))   # Total number of points
    skip   = 1                              # Used to skip over data so number of points won't exceed maxsamples
    while maxsamples>0 and n//skip>maxsamples:
        skip*=10

    i = 0
    for file_name in os.listdir(config_path):
        match = re.search(r'{0}[0-9]+\.{1}'.format(prefix,suffix),file_name)
        if match:
            if i%skip == 0:
                positions = np.loadtxt(os.path.join(config_path,match.group(0)),delimiter=delimiter)
                m         = len(positions)
                result.append([positions[i] for i in selector])
            i+=1

    return (m,result)


if __name__=='__main__':
    parser = argparse.ArgumentParser(description='Plot orbits from Barnes Hut Galaxy Simulator, galaxy.exe')
    parser.add_argument('--bodies','-b', type=int, default=1000, help='Number of bodies from simulation')
    parser.add_argument('--dpi', type=int, default=300, help='Dots per inch for displaying and saving figure')
    parser.add_argument('--norbits','-n', type=int, default=7, help='Number of orbits')
    parser.add_argument('--maxsamples','-m', type=int, default=1000,        help='Maximum number of sample per orbit (-1 to process all samples)')
    parser.add_argument('--prefix',                    default='bodies',    help='Prefix for configuration files')
    parser.add_argument('--suffix','-s',               default='csv',       help='Suffix for configuration files')
    parser.add_argument('--delimiter','-d',            default=',',         help='Delimiter for fields in config file')
    parser.add_argument('--nsigma','-g',    type=int,  default=3,           help='Number of standard deviations to use for scaling')
    parser.add_argument('--seed',           type=int,  default=None,        help='Seed for random number generator')
    parser.add_argument('--images','-i',               default='./imgs',    help='Path to store images')
    parser.add_argument('--path','-p',                 default='./configs', help='Path for config files')
    args = parser.parse_args()

    random.seed(args.seed)

    selector=random.sample(range(args.bodies),args.norbits)
    if plot(
        extract(
            config_path = args.path,
            selector    = selector,
            maxsamples  = args.maxsamples,
            prefix      = args.prefix,
            suffix      = args.suffix,
            delimiter   = args.delimiter),
        selector = selector,
        n        = args.nsigma,
        images   = args.images,
        dpi      = args.dpi):

        plt.show()
