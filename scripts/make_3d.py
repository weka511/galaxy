#!/usr/bin/env python

# Copyright (C) 2025 Simon Crase
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

'''
    Create movie showing evolution of galaxy --Companion to galaxy.exe - plot images in 3D.

    The movie function assumes that ffmpeg, https://www.ffmpeg.org/, has been installed.
'''

from argparse import ArgumentParser
from glob import glob
from os import system, remove
from os.path import join, basename, isfile
from sys import maxsize, exit
import numpy as np
from matplotlib.pyplot import figure,show, close
import mpl_toolkits.mplot3d as trid
import matplotlib.lines as lines
from scipy import mean, std

class ConfigurationPlotter:
    '''
    This class plots a configuration of particles and saves
    the plot as a png.
    '''
    def __init__(self,
                scale_to_cube = False,
                out = './imgs',
                 nsigma = 3,
                 rng = np.random.default_rng(),
                 prefix='galaxy'):
        self.scale_to_cube = scale_to_cube
        self.indices = None
        self.first = True
        self.out = out
        self.nsigma = nsigma
        self.rng = rng
        self.seq = 0
        self.prefix = prefix

    def plot_configuration(self,fname_in):
        '''
        Plot points from input file
        Parameters:
            fname_in  Input file
          I have had to workaround the problem of the legend function not supporting the type returned by a 3D scatter.
        See https://stackoverflow.com/questions/20505105/add-a-legend-in-a-3d-scatterplot-with-scatter-in-matplotlib for details.

        I have noticed that there seems to be some red structure (2nd Galaxy)
        remaining after 100,000 iterations of galaxy.exe, but this may
        be an artifact of the sequence in which points are drawn.
        We will plot the points in random order, therefore, to minimize this possibility.
        '''
        fig = figure(figsize=(20,10))
        particles = np.loadtxt(fname_in,delimiter=',')
        positions = particles[:,1:4]
        positions -= np.average(positions,axis=0,weights=particles[:,-1])
        ax = fig.add_subplot(111,  projection='3d')

        if self.first:
            self.indices = particles[:,0].astype(int)
            self.rng.shuffle(self.indices)
            self.first = False

        get_colour = np.vectorize(lambda x : self.colour_from_index(x,threshold=50))

        ax.scatter(positions[self.indices,0],positions[self.indices,1],positions[self.indices,2],
                   edgecolor= get_colour(self.indices),s=1)

        if self.scale_to_cube:
            ax.set_xlim(get_limits(positions[:,0]))
            ax.set_ylim(get_limits(positions[:,1]))
            ax.set_zbound(get_limits(positions[:,2]))

        ax.set_title(fname_in.replace('.csv',''));
        ax.set_xlabel('x')
        ax.set_ylabel('y')

        img_file = join(self.out,f'{self.prefix}{self.seq:06d}.png')
        self.seq += 1
        fig.savefig(img_file)
        return fig,img_file

    def colour_from_index(self,index,threshold=50):
        '''
        Used to distinguish points based on the cluster in when the occur.
        '''
        return 'b' if index < threshold else 'r'

    def get_limits(self,xs):
        '''
        Used when we plot to establish limits for each axis, based on mean and standard deviation.r

        Parameters:
            xs
            nsigma
        '''
        mu = mean(xs)
        sigma = std(xs)
        return (mu-self.nsigma*sigma,mu+self.nsigma*sigma)



class Movie:
    '''
    This class is a wrapper for ffmpeg and ffplay
    '''
    def __init__(self,movie_maker,movie_player,framerate=1):
        self.movie_maker = movie_maker
        self.framerate = framerate
        self.movie_player = movie_player

    def ensure_file_does_not_exist(self,filename):
        '''
        Used to ensure that named file does not exist. Delete if necessary.

        Parameters:
            filename
        '''
        try:
            remove(filename)
        except OSError:
            pass

    def create_command(self,pattern,movie_file):
        return f'{self.movie_maker} -f image2 -i {pattern} -framerate {self.framerate} {movie_file}'

    def make_movie(self,out,pattern='figs/galaxy%06d.png',movie='bar.mp4'):
        if len(movie.split('.')) < 2:
            movie = movie+'.mp4'
        if not isfile(self.movie_maker):
            print (f'Cannot find {self.movie_maker}')
            return False

        movie_file = join(out,movie)
        self.ensure_file_does_not_exist(movie_file)

        return_code = system(self.create_command(pattern,movie_file))
        if return_code == 0:
            print (f'Created movie {movie}')
            return True

        print (f'{cmd} returned error {return_code}')
        return False


    def play_movie(self,movie,out):
        if not isfile(self.movie_player):
            print (f'Cannot find {args.movie_player}')
            return False

        if len(movie.split('.')) < 2:
            movie = movie+'.mp4'

        return  system(
            f'{self.movie_player} {join(out,movie)}')

def parse_args():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('--seed',type=int,default=None,help='Seed for random number generator')
    parser.add_argument('--img_freq', type=int, default=20, help='Frequency of displaying progress')
    parser.add_argument('--show', action='store_true', default=False, help='Show images (as well as saving)')
    parser.add_argument('--cube', action='store_true', default=False, help='Scale to cube')
    parser.add_argument('--out', default='./figs' , help='Path name for images')
    parser.add_argument('--path', default='../configs', help='Path name for configurations')
    parser.add_argument('--nsigma', type=float, default=3, help='Number of standard deviations in cube')
    parser.add_argument('--sample', type=int, default=1000, help='Number of samples')
    parser.add_argument('--movie', default=None, help='Make movie')
    parser.add_argument('--movie_only', default=None, help='Skip extracting images. Just make movie')
    parser.add_argument('--colour_threshold', type=int, default=0, help='Colour threshold')
    parser.add_argument('--movie_maker', default='ffmpeg.exe', help='Name of program which builds movie')
    parser.add_argument('--movie_maker_path', default=r'C:\ffmpeg\bin', help='Path name for program which builds movie')
    parser.add_argument('--movie_player', default='ffplay.exe', help='Name of program which plays movie')
    parser.add_argument('--play', action='store_true', default=False, help='Play movie')
    parser.add_argument('--framerate', type=int, default=1, help='Frame rate')
    parser.add_argument('--prefix',default='galaxy')
    return parser.parse_args()


if __name__=='__main__':

    args = parse_args()
    movie = Movie(join(args.movie_maker_path,args.movie_maker),
                  join(args.movie_maker_path,args.movie_player),
                  args.framerate)
    if args.play:
        exit(movie.play_movie(args.movie,args.out))

    if args.movie_only == None:
        plotter = ConfigurationPlotter(scale_to_cube = args.cube,
                                        out = args.out,
                                        nsigma = args.nsigma,
                                        rng = np.random.default_rng(args.seed),
                                        prefix=args.prefix)
        for i,filename in enumerate(sorted(glob(join(args.path, args.prefix) + '*.csv'))):
            fig,img_file = plotter.plot_configuration(fname_in = join(args.path,filename))

            if i%args.img_freq == 0:
                print ('Created {0}'.format(img_file))

            if not args.show:
                close(fig)

        if args.movie != None:
            if movie.make_movie(args.out,join('figs',f'{args.prefix}%06d.png'), args.movie):
                movie.play_movie(args.movie,args.out)
    else:
        if movie.make_movie(args.out,join('figs',f'{args.prefix}%06d.png'), args.movie_only):
            movie.play_movie(args.movie_only, args.out)

    if args.show:
        show()
