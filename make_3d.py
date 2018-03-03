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

  Companion to test-kepler.cpp - plot images
'''
import os, re, sys, numpy as np, matplotlib.pyplot as plt,mpl_toolkits.mplot3d as trid,getopt, matplotlib.lines as lines

colours=['r','b','g','c','y','m']

'''
Plot points from input file
    Parameters:
	    fname_in  Input file
		n         Number of bodies
        
I have had to workaround the problem of the legend function not supporting the type returned by a 3D scatter.
See https://stackoverflow.com/questions/20505105/add-a-legend-in-a-3d-scatterplot-with-scatter-in-matplotlib for details.
'''
def plot(fname_in='kepler.csv',n=len(colours),m=sys.maxsize,scale_to_cube=False,out='./imgs'):    
    pos = np.loadtxt(fname_in,delimiter=',')
    plt.figure(figsize=(20,10))
    ax = plt.gcf().add_subplot(111, aspect='equal', projection='3d')
    scatterproxies=[]
    labels=[]
    for i in range(n):
        xs=[pos[j,0] for j in range(i,min(len(pos),m),n)]
        ys=[pos[j,1] for j in range(i,min(len(pos),m),n)]
        zs=[pos[j,2] for j in range(i,min(len(pos),m),n)]
        min_scale=min(min(xs),min(ys),min(zs))
        max_scale=max(max(xs),max(ys),max(zs))
        colour=colours[i%len(colours)]
        ax.scatter(xs,ys,zs,edgecolor=colour,s=1)
        if scale_to_cube:
            ax.set_xlim(min_scale,max_scale)
            ax.set_ylim(min_scale,max_scale)
            ax.set_zbound(min_scale,max_scale)
        scatterproxies.append( lines.Line2D([0],[0], linestyle="none", c=colour, marker = 'o'))
        labels.append('{0}'.format(i))
    plt.title(fname_in.replace('.csv',''));
    plt.xlabel('x')
    plt.ylabel('y')
    
    ax.legend(scatterproxies,labels, numpoints = 1) 
    img_file=os.path.join(out,os.path.basename(fname_in).replace('.csv','.png'))
    plt.savefig(img_file)
    return img_file
    
def usage():
    print ('python make_img.py -h -s [-m digits] [-n digits] file1.csv [file2.csv...]')

if __name__=='__main__':
    try:
        out='./imgs'
        n=len(colours)
        m=sys.maxsize
        show=False
        scale_to_cube=False
        i=0
        img_freq=20
        opts, args = getopt.getopt(sys.argv[1:], 'hm:n:sco', ['help', 'points','bodies=','show','cube','out'])
        for o,a in opts:
            if o in ['-h','--help']:
                usage()
                sys.exit(0)
            elif o in ['-n','--bodies']:
                n=int(a)
            elif o in ['-m','--points']:
                m=int(a)            
            elif o in ['-s','--show']:
                show=True
            elif o in ['-c','--cube']:
                scale_to_cube=True
            elif o in ['-o','--out']:
                out=a            
            else:
                print ('Unexpected option {0} {1}'.format(o,a))
                sys.exit(2) 
        for fname_in in args:
            if os.path.isfile(fname_in):
                img_file=plot(fname_in=fname_in,n=n,m=m,scale_to_cube=scale_to_cube)
                if i%img_freq==0:
                    print ('Created {0}'.format(img_file))
                i+=1
                if not show:
                    plt.close()                
            elif os.path.isdir(fname_in):
                for filename in os.listdir(fname_in):
                    if filename.endswith(".csv"):
                        img_file=plot(fname_in=os.path.join(fname_in,filename),n=n,m=m,scale_to_cube=scale_to_cube,out=out)
                        if i%img_freq==0:
                            print ('Created {0}'.format(img_file))
                        i+=1
                        if not show:
                            plt.close()
        if show:
            plt.show()
    except getopt.GetoptError as err:  # print help information and exit:      
        print(err)  # will print something like "option -a not recognized"
        usage()
        sys.exit(2)    