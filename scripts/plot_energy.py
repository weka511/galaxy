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

'''
   Companion to galaxy.exe - plot kinetic energy distribution,
   and compare with boltzmann.
'''

from argparse import ArgumentParser
from glob import glob
from os.path import join
from sys import maxsize
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rc
from scipy.optimize import curve_fit
from configure import Body, calculate_energy

def parse_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('--N0', type=int, default=0, help='Starting config sequence number')
    parser.add_argument('--N1', type=int, default=maxsize, help='Final config sequence number')
    parser.add_argument('--show', default=False, action='store_true', help='Show image')
    parser.add_argument('--nbins', type=int, default=200, help='Number of bins for histogram')
    parser.add_argument('--ext', default='.csv', help='Extension for config files')
    parser.add_argument('--path', default='../configs', help='Path for config files')
    parser.add_argument('--out', default='./figs', help='Path for output files')
    parser.add_argument('--distribution', default='boltzmann.png', help='Output file for plotting energy distribution')
    parser.add_argument('--prefix',default='galaxy')
    return parser.parse_args()

def boltzmann(n,m,beta):
    '''Generate the Maxwell-Boltzmann distribution function'''
    return m*np.exp(-beta*n)

class DistributionPlotter:
    def __init__(self):
        self.popt_cached=(-1,-1) #So optimization step can use value from previous step as starting value

    def create_particle(self,row):
        position = row[1:4]
        velocity = row[4:7]
        mass = row[7]
        return Body(position,mass,velocity)

    def plot(self,
        name  = 'bodies00002',
        ext   = '.csv',
        path  = './configs',
        nbins = 200,
        out   = './imgs',
        show  = False):
        '''
        Plot distribution of energies and compare with Boltzmann
        '''
        global popt_cached
        plt.figure(figsize=(10,10))

        config = np.loadtxt(name,delimiter=',') #id,x,y,z,vx,vy,vz,m
        n,_ = config.shape
        bodies = [self.create_particle(config[i,:]) for i in range(n)]# list([tuple(config[i,j] for j in [0,1,2,6,3,4,5]) for i in range(len(config))])
        kinetic_energy,potential_energy = configure.calculate_energy(bodies)
        print (kinetic_energy,potential_energy,-potential_energy/kinetic_energy)
        energies      = [0.5*config[j,6]*(config[j,3]**2+config[j,4]**2+config[j,5]**2) for j in range(len(config))]
        n,bins,_      = plt.hist(energies,bins=nbins,label='Energies')
        energy_levels = [0.5*(bins[i]+bins[i-1]) for i in range(1,len(bins))]
        if popt_cached[0]<0:
            popt_cached=(n[0], 100)
        popt, pcov   = curve_fit(boltzmann, energy_levels, n, p0=popt_cached)
        popt_cached  = popt
        perr         = np.sqrt(np.diag(pcov))
        probabiities = [boltzmann(e,*popt) for e in energy_levels]
        plt.plot( energy_levels, probabiities,
                  c='r',label=r'Boltzmann: N={0:.0f}({2:.0f}),$\beta$={1:.0f}({3:.0f})'.format(popt[0],popt[1],perr[0],perr[1]))
        plt.title(name)
        plt.legend()
        plt.savefig(join(out,name.replace('bodies','energy')+'.png'))
        if not show:
            plt.close()
        return popt[1],perr[1]


def plot_evolution_parameters(path,out):
    '''
    Plot evolution of Boltzmann beta and standard error
    '''
    plt.figure(figsize=(10,10))
    plt.plot(betas,'b',label=r'$\beta$')
    plt.plot(sigmas,'r',label=r'$\sigma$')
    plt.title('Evolution of parameters')
    plt.legend()
    plt.savefig(join(path,out))

def find_seq(path='./imgs',prefix='energy',ext='png'):
    '''
    Find largest sequence number in a set of files.

    Parameters:
        path     Identifies where files are stored
        prefix   File names match this prefix, followed by sequence number
        ext      File names have this extension

    Returns:      largest sequence number, or -1 if no files found
    '''
    try:
        files = sorted(glob(join(path,'{0}*.{1}'.format(prefix,ext))))
        return get_seq(files[-1],prefix=prefix,ext=ext)
    except IndexError:
        return -1


if __name__=='__main__':
    rc('font',**{'family':'serif', 'serif': ['cmr10']}) # see http://matplotlib.1069221.n5.nabble.com/computer-modern-td22253.html
    rc('text', usetex=True)
    args = parse_args()

    betas = []
    sigmas = []
    try:
        input_files = sorted(glob(join(args.path, args.prefix) + '*.csv'))
        start = args.N0
        for i in range(0,12):#FIXME start,args.N1,args.step):
            distribution_plotter = DistributionPlotter()
            beta,sigma = distribution_plotter.plot(input_files[i],show=args.show,ext=args.ext,path=args.path,out=args.out)
            betas.append(beta)
            sigmas.append(sigma)
    except FileNotFoundError:
        pass
    except OSError:  # End of files
        pass

    plot_evolution_parameters(args.out,args.distribution)
    if args.show:
        plt.show()
