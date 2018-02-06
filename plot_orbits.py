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

 Companion to galaxy.exe - plot orbits of randomly selected stars
'''
import os, re, sys, numpy as np, matplotlib.pyplot as plt,mpl_toolkits.mplot3d,random

def get_limits(bodies,n=1):
    m=np.mean(bodies)
    sigma=np.std(bodies)
    return (m-n*sigma,m+n*sigma)

'''
Plot orbits

    Parameters:
        data      Positions of sampled points in all orbits
        selector  Used to selectd which points will be plotted
        colours   Colours used to distinguish orbits
'''
def plot(data,selector=[],colours=['r','g','b','m','c','y'],n=2):
    def get_coordinates(body,i):
        return [d[body][i] for d in data]
    plt.figure(figsize=(20,20)) 
    ax = plt.gcf().add_subplot(111,  projection='3d')
    xs=[]
    ys=[]
    zs=[]
    for body in range(len(data[0])):
        ax.plot(get_coordinates(body,0),get_coordinates(body,1),get_coordinates(body,2),c=colours[body%len(colours)]) 
        ax.scatter(get_coordinates(body,0)[0],get_coordinates(body,1)[0],get_coordinates(body,2)[0],c=colours[body%len(colours)],
                       label="Body: {0}".format(selector[body]),s=50,marker='x')
        xs.append(get_coordinates(body,0))
        ys.append(get_coordinates(body,1))
        zs.append(get_coordinates(body,2))
    x0,x1=get_limits(xs,n=n)
    ax.set_xlim(x0,x1)
    y0,y1=get_limits(xs,n=n)
    ax.set_ylim(y0,y1)
    z0,z1=get_limits(xs,n=n)
    ax.set_zlim(z0,z1)
    plt.legend(loc='best')
    plt.title('Orbits of randomly selected stars')
    plt.savefig('orbits.png')

'''
Extract data from configuration files

    Parameters:
        config_path     Location of data
        selector        Used to select points that will be extracted
        maxsamples      Maximum number of points sampled in each orbit
        prefix          Prefix for configuration files
        suffix          Suffix for configuration files
        delimiter       Delimiter for fields in config file
'''
def extract(config_path = './configs/',selector=[0,1,2,55,100,400],maxsamples=1000,prefix='bodies',suffix='csv',delimiter=','):
    result=[]
    n=len(os.listdir(config_path))   # Total number of points
    skip=1                           # Used to skip over data so number of points won't exceed maxsamples
    while n//skip>maxsamples:
        skip*=10
    i=0
    for file_name in os.listdir(config_path):
        m = re.search(r'{0}[0-9]+\.{1}'.format(prefix,suffix),file_name)
        if m:
            if i%skip == 0:
                positions = np.loadtxt(os.path.join(config_path,m.group(0)),delimiter=delimiter)
                result.append([positions[i] for i in selector])
            i+=1
    return result


if __name__=='__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Plot orbits from Barnes Hut Galaxy Simulator, galaxy.exe')
    parser.add_argument('--bodies','-b', type=int,action='store',
                        help='Number of bodies from simulation',default=1000)    
    parser.add_argument('--norbits','-n', type=int,action='store',
                        help='Number of orbits',default=6)
    parser.add_argument('--maxsamples','-m', type=int,action='store',
                        help='Maximum number of sample per orbit',default=1000)    
    parser.add_argument('--prefix','-p', action='store',
                        help='Prefix for configuration files',default='bodies') 
    parser.add_argument('--suffix','-s', action='store',
                        help='Suffix for configuration files',default='csv') 
    parser.add_argument('--delimiter','-d', action='store',
                        help='Delimiter for fields in config file',default=',')
    parser.add_argument('--nsigma','-g', type=int,action='store',
                        help='Number of standard deviations to use for scaling',default=3)    
    args = parser.parse_args()    
    selector=random.sample(range(args.bodies),args.norbits) 
    data=extract(selector=selector,maxsamples=args.maxsamples,prefix=args.prefix,suffix=args.suffix,delimiter=args.delimiter)
    plot(data,selector=selector,n=args.nsigma)
    plt.show()
  