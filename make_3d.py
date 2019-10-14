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

# Companion to galaxy.exe - plot images in 3D.
  
# The movie function assumes that ffmpeg, https://www.ffmpeg.org/, has been installed.

import os, re, sys, numpy as np, matplotlib.pyplot as plt,mpl_toolkits.mplot3d as trid,getopt, matplotlib.lines as lines, scipy, random

colours=['r','b','g','c','y','m']

def ensure_file_does_not_exist(filename):
    try:
        os.remove(filename)
    except OSError:
        pass
    
def get_limits(xs,nsigma=3):
    mu    = scipy.mean(xs)
    sigma = scipy.std(xs)
    return (mu-nsigma*sigma,mu+nsigma*sigma)
    
def colour_from_index(i,threshold=5000):
    return 'b' if i<threshold else 'r'

# Plot points from input file
#    Parameters:
#	    fname_in  Input file
#		n         Number of bodies
#        
# I have had to workaround the problem of the legend function not supporting the type returned by a 3D scatter.
# See https://stackoverflow.com/questions/20505105/add-a-legend-in-a-3d-scatterplot-with-scatter-in-matplotlib for details.
#
# I have noticed that there seems to be some red structure (2nd Galaxy)
# remaining after 100,000 iterations of galaxy.exe, but this may
# be an artifact of the sequence in which points are drawn.
# We will plot the points in random order, therefore, to minimize this possibility.

def plot(fname_in,n=len(colours),m=sys.maxsize,scale_to_cube=False,out='./imgs',nsigma=3,N=1000,get_colour=colour_from_index):
    plt.figure(figsize=(20,10))
    pos            = np.loadtxt(fname_in,delimiter=',')
    ax             = plt.gcf().add_subplot(111,  projection='3d')
    scatterproxies = []
    labels         = []
    indices        = list(range(len(pos)))
    random.shuffle(indices)
    xs             = [pos[j,0] for j in indices]
    ys             = [pos[j,1] for j in indices]
    zs             = [pos[j,2] for j in indices]
    colour         = [get_colour(j) for j in indices]
    ax.scatter(xs,ys,zs,edgecolor=colour,s=1)
    if scale_to_cube:
        ax.set_xlim(get_limits(xs,nsigma=nsigma))
        ax.set_ylim(get_limits(ys,nsigma=nsigma))
        ax.set_zbound(get_limits(zs,nsigma=nsigma))    
     
    plt.title(fname_in.replace('.csv',''));
    plt.xlabel('x')
    plt.ylabel('y')
    
    ax.legend(scatterproxies,labels, numpoints = 1) 
    img_file=os.path.join(out,os.path.basename(fname_in).replace('.csv','.png'))
    plt.savefig(img_file)
    return img_file
    
def make_movie(movie_maker,out,pattern,framerate,movie):
    if len(movie.split('.'))<2:
        movie = movie+'.mp4'

    ensure_file_does_not_exist(os.path.join(out,movie))
    cmd = '{0} -f image2 -i {1}{2} -framerate {3} {4}'.format(movie_maker,out+'/',pattern,framerate,os.path.join(out,movie))
    return_code = os.system(cmd)
    if return_code==0:
        print ('Created movie {0}'.format(movie))
        return True
    else:
        print ('{0} returned error {1}'.format(cmd,return_code))
        return False

# play_movie
#
# Play movie

def play_movie(movie,out,movie_maker_path,movie_player):
    if len(movie.split('.'))<2:
        movie=movie+'.mp4'     

    return  os.system(
        '{0} {1}'.format(
                     os.path.join(args.movie_maker_path,args.movie_player),
                     os.path.join(args.out,movie)))
    
if __name__=='__main__':
    import argparse,glob
    parser = argparse.ArgumentParser(description='Create movie showing evolution of galaxy')
    parser.add_argument('--bodies',           type=int, default=len(colours),     help='Number of bodies')
    parser.add_argument('--img_freq',         type=int, default=20,               help='Frequency of displaying progress')
    parser.add_argument('--points',           type=int, default=sys.maxsize,      help='Number of colours')
    parser.add_argument('--show',             action='store_true', default=False, help='Show images (as well as saving)')
    parser.add_argument('--cube',             action='store_true', default=False, help='Scale to cube')
    parser.add_argument('--out',              default='./imgs' ,                  help='Path name for images')
    parser.add_argument('--path',             default='./configs',                help='Path name for configurations')    
    parser.add_argument('--nsigma',           type=float, default=3,              help='Number of standard deviations in cube')
    parser.add_argument('--sample',           type=int, default=1000,             help='Number of samples')
    parser.add_argument('--movie',            default=None,                       help='Make movie')
    parser.add_argument('--movie_only',       default=None,                       help='Skip extracting images. Just make movie')    
    parser.add_argument('--colour_threshold', type=int, default=0,                help='Colour threshold')
    parser.add_argument('--movie_maker',      default='ffmpeg.exe',               help='Name of program which builds movie')    
    parser.add_argument('--movie_maker_path', default=r'C:\ffmpeg\bin',           help='Path name for program which builds movie')
    parser.add_argument('--movie_player',     default='ffplay.exe',               help='Name of program which plays movie')
    parser.add_argument('--play',             action='store_true', default=False, help='Play movie')  
    parser.add_argument('--framerate',        type=int, default=1,                help='Frame rate')
    args = parser.parse_args()

    if args.play:
        sys.exit(play_movie(args.movie,args.out,args.movie_maker_path,args.movie_player))
        
    if args.movie_only==None:
        i = 0
        for filename in os.listdir(args.path):
            if filename.endswith(".csv"):
                img_file = plot(fname_in      = os.path.join(args.path,filename),
                                n             = args.bodies,
                                m             = args.points,
                                scale_to_cube = args.cube,
                                out           = args.out,
                                N             = args.sample,
                                nsigma        = args.nsigma,
                                get_colour    = colour_from_index)
                
                if i%args.img_freq==0:
                    print ('Created {0}'.format(img_file))
                    
                i+=1
                
                if not args.show:
                    plt.close()
                    
        if args.show:
            plt.show()
        
        if args.movie!=None:
            if make_movie(os.path.join(args.movie_maker_path,args.movie_maker),
                          args.out,'bodies%05d.png',args.framerate, args.movie):
                play_movie(args.movie,args.out,args.movie_maker_path,args.movie_player)
    else:
        if make_movie(os.path.join(args.movie_maker_path,args.movie_maker),
                      args.out,
                      'bodies%05d.png',
                      args.framerate, 
                      args.movie_only):
            play_movie(args.movie_only,
                       args.out,
                       args.movie_maker_path,
                       args.movie_player)
