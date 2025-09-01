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
from matplotlib.pyplot import figure, show, close
from matplotlib import rc
from scipy.optimize import curve_fit
from configure import Body

def parse_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('--N0', type=int, default=0, help='Starting config sequence number')
    parser.add_argument('--N1', type=int, default=maxsize, help='Final config sequence number')
    parser.add_argument('--show', default=False, action='store_true', help='Show image')
    parser.add_argument('--bins',  default='sqrt', help='Number of bins for histogram')
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
    def __init__(self,bins='sqrt',
                 ext   = '.csv',
                 path  = './configs',
                 out   = './imgs',
                 show  = False):
        self.bins = bins
        self.popt_cached=(-1,-1) #So optimization step can use value from previous step as starting value
        self.show = show

    def create_particle(self,row):
        position = row[1:4]
        velocity = row[4:7]
        mass = row[7]
        return Body(position,mass,velocity)

    def plot(self,
        name  = 'bodies00002'):
        '''
        Plot distribution of energies and compare with Boltzmann
        '''
        config = np.loadtxt(name,delimiter=',')
        n,_ = config.shape
        bodies = [self.create_particle(config[i,:]) for i in range(n)]
        # kinetic_energy,potential_energy = configure.calculate_energy(bodies)
        # print (kinetic_energy,potential_energy,-potential_energy/kinetic_energy)
        energies      = [particle.get_T() for particle in bodies]
        fig = figure(figsize=(10,10))
        ax = fig.add_subplot(1,1,1)
        n,bins = np.histogram(energies,bins=self.bins)
        energy_levels = [0.5*(bins[i]+bins[i-1]) for i in range(1,len(bins))]
        width = [0.5*(bins[i]-bins[i-1]) for i in range(1,len(bins))]
        ax.bar(energy_levels,n,width=width)
        if self.popt_cached[0]<0:
            self.popt_cached=(n[0], 100)
        popt, pcov   = curve_fit(boltzmann, energy_levels, n, p0=self.popt_cached)
        self.popt_cached  = popt
        perr         = np.sqrt(np.diag(pcov))
        probabiities = [boltzmann(e,*popt) for e in energy_levels]
        ax.plot( energy_levels, probabiities,
                  c='r',label=r'Boltzmann: N={0:.0f}({2:.0f}),$\beta$={1:.0f}({3:.0f})'.format(popt[0],popt[1],perr[0],perr[1]))
        ax.set_xlim(0,energy_levels[-1])
        # ax.set_title(name)
        ax.legend()
        # plt.savefig(join(out,name.replace('bodies','energy')+'.png'))
        if not self.show:
            close(fig)
        return popt[1],perr[1]


def plot_evolution_parameters(path,out):
    '''
    Plot evolution of Boltzmann beta and standard error
    '''
    fig = figure(figsize=(10,10))
    ax = fig.add_subplot(1,1,1)

    ax.plot(betas,'b',label=r'$\beta$')
    ax.plot(sigmas,'r',label=r'$\sigma$')
    ax.set_title('Evolution of parameters')
    ax.legend()
    # ax.savefig(join(path,out))

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
    rc('font',**{'family':'serif','serif':['Palatino']})
    rc('text', usetex=True)
    args = parse_args()

    betas = []
    sigmas = []
    try:
        input_files = sorted(glob(join(args.path, args.prefix) + '*.csv'))
        start = args.N0
        for i in range(250,251):#FIXME start,args.N1,args.step):
            distribution_plotter = DistributionPlotter(bins=args.bins,show=args.show,ext=args.ext,path=args.path,out=args.out)
            beta,sigma = distribution_plotter.plot(input_files[i])
            betas.append(beta)
            sigmas.append(sigma)
    except FileNotFoundError:
        pass
    except OSError:  # End of files
        pass

    plot_evolution_parameters(args.out,args.distribution)
    if args.show:
        show()
