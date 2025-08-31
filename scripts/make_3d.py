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

from abc import abstractmethod
from argparse import ArgumentParser
from glob import glob
from os import system, remove, getcwd
from os.path import join, basename, isfile, splitext
from re import split
from sys import maxsize, exit
import numpy as np
from matplotlib.pyplot import figure,show, close
import mpl_toolkits.mplot3d as trid
import matplotlib.lines as lines

def get_file_name(name = basename(splitext(__file__)[0]),default_ext='npz'):
    '''
    Used to create file names

    Parameters:
        name          Basis for file name
        default_ext   Extension if non specified
        figs          Directory for storing figures
        seq           Used if there are multiple files
    '''
    base,ext = splitext(name)
    if len(ext) == 0:
        ext = default_ext

    return f'{base}.{ext}'


class ConfigurationPlotter:
    '''
    This class plots a configuration of particles and saves
    the plot as a png.
    '''
    def __init__(self,
                 figs = './figs',
                 nsigma = 3,
                 rng = np.random.default_rng(),
                 prefix='galaxy',
                 colours = None,
                 size = 25):
        self.indices = None
        self.first = True
        self.figs = figs
        self.nsigma = nsigma
        self.rng = rng
        self.seq = 0
        self.prefix = prefix
        self.limits = np.zeros((2,3))
        self.colours = colours
        self.size = size

    def plot_configuration(self,fname_in,restarting = False):
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

        if restarting:
            npzfile = np.load(get_file_name())
            self.indices = npzfile['indices']
            self.limits = npzfile['limits']
            self.first = False
        elif self.first:
            self.indices = particles[:,0].astype(int)
            self.rng.shuffle(self.indices)
            for j in range(3):
                self.limits[:,j] = self.get_limits(positions[:,j])
            np.savez(get_file_name(),indices=self.indices,limits=self.limits)

        colours = np.vectorize(lambda index : self.colours[index])(self.indices)
        ax.scatter(positions[self.indices,0],positions[self.indices,1],positions[self.indices,2],
                   c= colours,edgecolor= colours,s=self.size)

        ax.set_title(fname_in.replace('.csv',''));
        ax.set_xlabel('x')
        ax.set_ylabel('y')
        ax.set_xlim(self.limits[:,0])
        ax.set_ylim(self.limits[:,1])
        ax.set_zbound(self.limits[:,2])

        img_file = join(self.figs,f'{self.prefix}{self.seq:06d}.png')
        self.seq += 1
        fig.savefig(img_file)
        return fig,img_file

    def get_limits(self,xs):
        '''
        Used when we plot to establish limits for each axis, based on mean and standard deviation

        Parameters:
            xs
            nsigma
        '''
        mu = np.mean(xs)
        sigma = np.std(xs)
        return (mu-self.nsigma*sigma,mu+self.nsigma*sigma)

    def get_image_file_count(self):
        pattern = join(self.figs,f'{self.prefix}[0-9]*.png')
        return len(glob(pattern))

class MovieMaker:
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

    def make(self,out,pattern='figs/galaxy%06d.png',movie='bar.mp4'):
        '''
        Convert a sequence of images into a movie
        '''
        if len(movie.split('.')) < 2:
            movie = movie+'.mp4'
        if not isfile(self.movie_maker):
            print (f'Cannot find {self.movie_maker}')
            return False

        movie_file = join(out,movie)
        self.ensure_file_does_not_exist(movie_file)

        cmd = self.create_command(pattern,movie_file)
        return_code = system(cmd)
        if return_code == 0:
            print (f'Created movie {movie}')
            return True
        else:
            print (f'{cmd} returned error {return_code}')
            return False


    def play(self,out,movie='bar.mp4'):
        '''
        Play movie
        '''
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
    parser.add_argument('-o', '--out', default = basename(splitext(__file__)[0]),help='Name of output file')
    parser.add_argument('--figs', default = './figs', help = 'Name of folder where plots are to be stored')
    parser.add_argument('--path', default='../configs', help='Path name for configurations')
    parser.add_argument('--nsigma', type=float, default=3, help='Number of standard deviations in cube')
    parser.add_argument('--movie', action='store_true', default=False, help='Make movie')
    parser.add_argument('--colour_threshold', type=int, default=0, help='Colour threshold')
    parser.add_argument('--movie_maker', default='ffmpeg.exe', help='Name of program which builds movie')
    parser.add_argument('--movie_maker_path', default=r'C:\ffmpeg\bin', help='Path name for program which builds movie')
    parser.add_argument('--movie_player', default='ffplay.exe', help='Name of program which plays movie')
    parser.add_argument('--play', action='store_true', default=False, help='Play movie')
    parser.add_argument('--framerate', type=int, default=1, help='Frame rate')
    parser.add_argument('--prefix',default='galaxy')
    parser.add_argument('--extract', action='store_true', default=False, help='Extract images from csvs')
    parser.add_argument('--max', type=int, default=-1, help='Limit number of input files (for testing).')
    parser.add_argument('-s','--size', type=int, default=25, help='Size for stars')
    action = parser.add_mutually_exclusive_group()
    action.add_argument('--resume', action='store_true', default=False, help='Pick up processing where presvious run left off')
    action.add_argument('--overwrite', action='store_true', default=False, help='Pverwrite existing inputs')
    return parser.parse_args()

class ColourModel:
    '''
    A Colour model is used to assign colours to individual particles
    '''
    @abstractmethod
    def __getitem__(self, index):
        '''
        '''

class XKCD_ColourModel(ColourModel):
    '''
    Use XKCD colours -- https://xkcd.com/color/rgb.txt
    '''
    def __init__(self,file_name = 'rgb.txt',prefix = 'xkcd:'):
        '''
        Create array of XKCD colours, starting with the most poular.

        Parameters:
            file_name Where XKCD colours live
            prefix    Use to prefix each colour with "xkcd:"
        '''
        self.colours = []
        self.prefix = prefix
        with open(file_name) as colours:
            for row in colours:
                parts = split(r'\s+#',row.strip())
                if len(parts) > 1:
                    self.colours.append(parts[0])
        self.colours = np.array(self.colours[::-1],dtype=str)

    def __getitem__(self, index):
        return f'{self.prefix}{self.colours[index % len(self.colours)]}'



if __name__=='__main__':
    args = parse_args()
    movie_maker = MovieMaker(join(args.movie_maker_path,args.movie_maker),
                             join(args.movie_maker_path,args.movie_player),
                             args.framerate)

    if args.extract:
        colours = XKCD_ColourModel()
        plotter = ConfigurationPlotter( figs = args.figs,
                                        nsigma = args.nsigma,
                                        rng = np.random.default_rng(args.seed),
                                        prefix = args.prefix,
                                        colours = colours,
                                        size = args.size)
        n_outputs = plotter.get_image_file_count()
        n_start = 0
        restarting = False
        if n_outputs > 0:
            if args.resume:
                n_start = n_outputs
                restarting = True
            elif args.overwrite:
                pass
            else:
                print(f'There are {n_outputs} output files already: must have resume or overwrite')
                exit(2)

        input_files = sorted(glob(join(args.path, args.prefix) + '*.csv'))
        plotter.seq = n_outputs
        for i in range(n_start,len(input_files)):
            fig,img_file = plotter.plot_configuration(fname_in = join(args.path,input_files[i]),restarting=restarting)
            restarting = False
            if i%args.img_freq == 0:
                print ('Created {0}'.format(img_file))

            if not args.show:
                close(fig)

            if args.max > 0 and i > args.max: break

    if args.movie:
        movie_maker.make(args.figs,join(args.figs,f'{args.prefix}%06d.png'), args.out)

    if args.play:
        movie_maker.play(args.figs,args.out)

    if args.show:
        show()
