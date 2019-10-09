# Copyright (C) 2018-2019 Greenweaves Software Limited
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

#  Companion to galaxy.exe - plot kinetic energy distribution,
#  and compare with boltzmann.

import numpy as np,matplotlib.pyplot as plt,os.path as op,sys
from matplotlib import rc
from scipy.optimize import curve_fit

# boltzmann
#
# Generate the Maxwell-Boltzmann distribution function

def boltzmann(n,m,beta):
    return m*np.exp(-beta*n)

popt_cached=(-1,-1) #So optimization step can use value from previous step as starting value

# plot_distribution
#
# Plot distribution of energies and compare with Boltzmann
def plot_distribution(
    name  = 'bodies00002',
    ext   = '.csv',
    path  = './configs',
    nbins = 200,
    out   = './imgs',
    show  = False):
    global popt_cached
    plt.figure(figsize=(10,10))
    
    config        = np.loadtxt(op.join(path,name+ext),delimiter=',')
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
    plt.savefig(op.join(out,name.replace('bodies','energy')+'.png'))
    if not show:
        plt.close()
    return popt[1],perr[1]

# plot_evolution_parameters
#
# Plot eveolution of Boltzmann beta and standard error

def plot_evolution_parameters(path,out):
    plt.figure(figsize=(10,10))
    plt.plot(betas,'b',label=r'$\beta$')
    plt.plot(sigmas,'r',label=r'$\sigma$')
    plt.title('Evolution of parameters')
    plt.legend()
    plt.savefig(op.join(path,out))
    
if __name__=='__main__':
    import argparse
    rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
    rc('text', usetex=True)
    
    parser = argparse.ArgumentParser(description='Plot distribution of energies')
    parser.add_argument('--N0', type=int,action='store',
                        help='Starting config sequence number',default=0)
    parser.add_argument('--N1', type=int,action='store',
                        help='Final config sequence number',default=sys.maxsize)    
    parser.add_argument('--step', type=int,action='store',
                        help='Step size',default=100) 
    parser.add_argument('--show', action='store_true',
                        help='Show image',default=False)
    parser.add_argument('--nbins', '-n',type=int,action='store',
                        help='Number of bins for histogram',default=200)
    parser.add_argument('--ext', action='store',
                        help='Extension for config files',default='.csv') 
    parser.add_argument('--path', action='store',
                        help='Path for config files',default='./configs') 
    parser.add_argument('--out', action='store',
                        help='Path for output files',default='./imgs') 
    parser.add_argument('--distribution', action='store',
                        help='Output file for plotting energy distribution',default='boltzmann.png')      
    args = parser.parse_args()
    
    betas=[]
    sigmas=[]
    try:
        for i in range(args.N0,args.N1,args.step):
            beta,sigma = plot_distribution('bodies{0:05d}'.format(i),show=args.show,ext=args.ext,path=args.path,out=args.out)
            betas.append(beta)
            sigmas.append(sigma)
    except FileNotFoundError:
        pass
    except OSError:  # End of files
        pass
    
    plot_evolution_parameters(args.out,args.distribution)
    if args.show:
        plt.show()