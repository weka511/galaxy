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

  Companion to galaxy.exe - plot images in 3D.
  
  The movie function assumes that ffmpeg, https://www.ffmpeg.org/, has been installed in /usr/local/bin
'''
import os, re, sys, numpy as np, matplotlib.pyplot as plt,mpl_toolkits.mplot3d as trid,getopt, matplotlib.lines as lines, scipy, random

colours=['r','b','g','c','y','m']

def ensure_file_not_exists(filename):
    try:
        os.remove(filename)
    except OSError:
        pass
    
def get_limits(xs,nsigma=3):
    mu=scipy.mean(xs)
    sigma=scipy.std(xs)
    return (mu-nsigma*sigma,mu+nsigma*sigma)
    
def colour_from_index(i,threshold=5000):
    return 'b' if i<threshold else 'r'

'''
Plot points from input file
    Parameters:
	    fname_in  Input file
		n         Number of bodies
        
I have had to workaround the problem of the legend function not supporting the type returned by a 3D scatter.
See https://stackoverflow.com/questions/20505105/add-a-legend-in-a-3d-scatterplot-with-scatter-in-matplotlib for details.
'''
def plot(fname_in,n=len(colours),m=sys.maxsize,scale_to_cube=False,out='./imgs',nsigma=3,N=1000,get_colour=colour_from_index):
    pos = np.loadtxt(fname_in,delimiter=',')
    plt.figure(figsize=(20,10))
    ax = plt.gcf().add_subplot(111, aspect='equal', projection='3d')
    scatterproxies=[]
    labels=[]
    # I have noticed that there seems to be some red structure (2nd Galaxy)
    # remaining after 100,000 iterations of galaxy.exe, but this may
    # be an artifact of the sequnce in which points are drawn.
    # We will plot the points in random order, therefore, to minimize this possibility.
    indices=list(range(len(pos)))
    random.shuffle(indices)
    xs=[pos[j,0] for j in indices]
    ys=[pos[j,1] for j in indices]
    zs=[pos[j,2] for j in indices]
    colour=[get_colour(j) for j in indices]
    ax.scatter(xs,ys,zs,edgecolor=colour,s=1)
    if scale_to_cube:
        ax.set_xlim(get_limits(xs,nsigma=nsigma))
        ax.set_ylim(get_limits(ys,nsigma=nsigma))
        ax.set_zbound(get_limits(zs,nsigma=nsigma))    
     
        #scatterproxies.append( lines.Line2D([0],[0], linestyle="none", c=colour, marker = 'o'))
        #labels.append('{0}'.format(i))
    plt.title(fname_in.replace('.csv',''));
    plt.xlabel('x')
    plt.ylabel('y')
    
    ax.legend(scatterproxies,labels, numpoints = 1) 
    img_file=os.path.join(out,os.path.basename(fname_in).replace('.csv','.png'))
    plt.savefig(img_file)
    return img_file
    
def make_movie(movie_maker,out,pattern,framerate,movie):
    if len(movie.split('.'))<2:
        movie=movie+'.mp4'
    if not out.endswith('/'):
        out=out+'/'
    ensure_file_not_exists(movie)
    cmd='{0} -f image2 -i {1}{2} -framerate {3} {4}'.format(movie_maker,out,pattern,framerate,movie)
    return_code = os.system(cmd)
    if return_code==0:
        print ('Created movie {0}'.format(movie))
    else:
        print ('{0} returned error {1}'.format(cmd,return_code))
        
if __name__=='__main__':
    import argparse,glob
    parser = argparse.ArgumentParser(description='Plot distribution of energies')
    parser.add_argument('--bodies', '-n',type=int,action='store',help='Number of bodies',default=len(colours))
    parser.add_argument('--img_freq', '-f',type=int,action='store',help='Frequency of displaying progress',default=20)
    parser.add_argument('--points', '-m',type=int,action='store',help='Number of colours',default=sys.maxsize)
    parser.add_argument('--show', '-s',action='store_true',help='Show images (as well as saving)',default=False)
    parser.add_argument('--cube', '-c',action='store_true',help='Scale to cube',default=False)
    parser.add_argument('--out', '-o',action='store',help='Path name for images',default='./imgs')
    parser.add_argument('--path', '-p',action='store',help='Path name for configurations',default='./configs')    
    parser.add_argument('--nsigma', '-g',type=float,action='store',help='Number of standard deviations in cube',default=3)
    parser.add_argument('--sample', '-N',type=int,action='store',help='Number of samples',default=1000)
    parser.add_argument('--movie', '-v',action='store',help='Make movie',default=None)
    parser.add_argument('--movie_only',action='store',help='Skip extracting images. Just make movie',default=None)    
    parser.add_argument('--colour_threshold', '-t',type=int,action='store',help='Colour threshold',default=0) 

    args = parser.parse_args()

    i             = 0
    movie_maker   = 'ffmpeg.exe'
    framerate     = 1
    pattern       = 'bodies%05d.png'

    if args.movie_only==None:
        for filename in os.listdir(args.path):
            if filename.endswith(".csv"):
                img_file=plot(fname_in=os.path.join(args.path,filename),n=args.bodies,m=args.points,scale_to_cube=args.cube,
                              out=args.out,N=args.sample,nsigma=args.nsigma,get_colour=colour_from_index)
                if i%args.img_freq==0:
                    print ('Created {0}'.format(img_file))
                i+=1
                if not args.show:
                    plt.close()
                    
        if args.show:
            plt.show()
        
        if args.movie!=None:
            make_movie(movie_maker, args.out,pattern,framerate, args.movie)
    else:
        make_movie(movie_maker, args.out,pattern,framerate, args.movie_only)

