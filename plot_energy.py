'''
 Copyright (C) 2018 Greenweaves Software Pty Ltd

 This is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This software is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this software.  If not, see <http://www.gnu.org/licenses/>

  Companion to galaxy.exe - plot kinetic energy distribution,
  and compare with Bolzmann.
'''
import numpy as np,matplotlib.pyplot as plt,os.path as op,sys
from matplotlib import rc
from scipy.optimize import curve_fit

def bolzmann(n,m,beta):
    return m*np.exp(-beta*n)

def plot(name='bodies00002',ext='.csv',path='./configs',nbins=200,out='./imgs',show=False): 
    plt.figure(figsize=(10,10))
    config = np.loadtxt(op.join(path,name+ext),delimiter=',')
    energies=[0.5*config[j,6]*(config[j,3]**2+config[j,4]**2+config[j,5]**2) for j in range(len(config))]
    n,bins,_=plt.hist(energies,bins=nbins,label='Energies')
    xx=[0.5*(bins[i]+bins[i-1]) for i in range(1,len(bins))]
    popt, pcov = curve_fit(bolzmann, xx, n, p0=(sum(n), 1e-6))
    perr = np.sqrt(np.diag(pcov))
    yy=[bolzmann(x,*popt) for x in xx]
    plt.plot( xx, yy,c='r',label=r'Bolzmann: N={0:.0f}({2:.0f}),$\beta$={1:.0f}({3:.0f})'.format(popt[0],popt[1],perr[0],perr[1]))
    plt.title(name)
    plt.legend()
    plt.savefig(op.join(out,name.replace('bodies','energy')+'png'))
    if not show:
        plt.close()
        
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
    args = parser.parse_args()
    
    try:
        for i in range(args.N0,args.N1,args.step):
            plot('bodies{0:05d}'.format(i),show=args.show,ext=args.ext,path=args.path,out=args.out)
    except FileNotFoundError:
        pass

    if args.show:
        plt.show()