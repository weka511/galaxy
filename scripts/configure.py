#!/usr/bin/env python

# Copyright (C) 2019-2025 Greenweaves Software Limited
#
# This is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this software.  If not, see <http://www.gnu.org/licenses/>

'''Create configuration for galaxy.exe'''

from abc import ABC,abstractmethod
from argparse import ArgumentParser
from time import time
from os import urandom
from os.path import basename, join, splitext,exists
import random
from sys import exit
from struct import pack,unpack
from shutil import copyfile
import xml.etree.ElementTree as ET
from matplotlib import rc
from matplotlib.pyplot import figure,show
import mpl_toolkits.mplot3d
import numpy as np

class Body:
    '''
    This class represents one body in the simulation

    Attributes:
        position
        mass
        velocity
    '''
    def __init__(self,position,mass,velocity):
        self.position = position
        self.mass = mass
        self.velocity = velocity

    def encode(self):
        '''
         Convert fields in body to a string for serialization
        '''
        fields = self.position + [self.mass] + self.velocity
        return ','.join([encode(f) for f in fields])

    def get_T(self):
        '''
        Calculate kinetic energy
        '''
        return 0.5 * self.mass * sum([v**2 for v in self.velocity])

    def get_distance(self,other):
        '''
        Calculate distance between two points

        Parameters:
            other
        '''
        return np.sqrt(sum([(self.position[i]-other.position[i])**2 for i in range(len(self.position))]))

    def get_V(self,other):
        '''
        Calculate potential energy between this body and one other, ignoring G
        '''
        return - self.mass * other.mass/self.get_distance(other)

    def create_with_offset(self,offset,vel):
        '''
        Create a new body shifted by an offset
        '''
        pos  = [sum(u) for u in zip(self.position,offset)]
        vels = [sum(v) for v in zip(self.velocity,vel)]
        return Body(pos,self.mass,vels)


class ConfigurationFactory(ABC):
    @staticmethod
    def create_config_factory(model):
        '''
        Instantiate method for creating configuration

        Parameters:
            model       Name of model
        '''
        config_factory_dict = {
            'plummer':Plummer()
        }
        if model in config_factory_dict:
            return config_factory_dict[model]
        else:
            raise RuntimeError(f'Unrecognized model: {model}')

    @abstractmethod
    def create(self,number_bodies=100,
                       radius=1.0):
        pass

    def randomize_on_sphere(self,radius=1.0):
        '''
        Randomly position point on surface of sphere
        '''
        theta = np.arccos(random.uniform(-1,1))
        phi = random.uniform(0,2*np.pi)
        return [radius * np.sin(theta) * np.cos(phi),
                radius * np.sin(theta) * np.sin(phi),
                radius * np.cos(theta)]

class Plummer(ConfigurationFactory):
    def create(self,number_bodies=100,
                       radius=1.0):             # TODO
        '''
        Instantiate configuration for Plummer model

        http://www.artcompsci.org/kali/vol/plummer/volume9.pdf
        '''
        return [self.create_body() for i in range(number_bodies)]

    def get_velocity_ratio(self):
        '''
        Sampling used in A comparison of Numerical Methods for the Study of Star Cluster Dynamics,
        by Sverre Aarseth, Michel Henon, and Roland Wielen, in Astron. Astroph.37, 183 (1974)
        http://articles.adsabs.harvard.edu/full/1974A%26A....37..183A
        '''
        x = 0.0
        y = 0.1
        while y > x**2 * (1-x**2)**3.5:
            x = random.uniform(0,1)
            y = random.uniform(0,0.1)
        return x

    def create_velocity(self,radius=1):
        '''Setup initial velocities'''
        return self.randomize_on_sphere(radius=self.get_velocity_ratio() * np.sqrt(2.0) *(1 + radius**2)**(-0.25))

    def create_body(self):
        '''Create one body with random position and velocity'''
        mass = 1.0/number_bodies
        radius = 1.0/np.sqrt(random.uniform(0,1)**(-2/3) - 1)
        return Body(self.randomize_on_sphere(radius=radius),mass,self.create_velocity(radius=radius))

def parse_args():
    '''Parse command line parameters'''
    parser = ArgumentParser(__doc__)
    model = 'plummer'
    output = 'config.txt'
    n = 100
    path = '../configs'
    r = 1.0
    sigma = 2.0

    parser.add_argument('--output', default=output, help=f'Configuration file [f{output}]')
    parser.add_argument('--model', default=model, help=f'Used to initialize distribution [{model}]')
    parser.add_argument('-n', '--number_bodies',  type=int, default=n, help=f'Number of bodies [{n}]')
    parser.add_argument('-p', '--path', default=path,  help=f'Path for configuration files [{path}]')
    parser.add_argument('-r', '--radius', type=float, default=r, help=f'Initial Radius [{r}]')
    parser.add_argument('--seed', help='Initialize the random number generator')
    parser.add_argument('--generate', action='store_true', default=False, help='Generate test data for serialization')
    parser.add_argument('--show', action='store_true', default=False, help='Show generated points')
    parser.add_argument('--nsigma', type=float, default=sigma, help=f'Scale data for show {sigma}')
    parser.add_argument('--xml', help='XML spec')
    parser.add_argument('--figs', default = './figs', help = 'Name of folder where plots are to be stored')
    parser.add_argument('-o', '--out', default = basename(splitext(__file__)[0]),help='Name of output file')
    return parser.parse_args()

def get_file_name(name,default_ext='png',figs='./figs',seq=None):
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
    if seq != None:
        base = f'{base}{seq}'
    qualified_name = f'{base}.{ext}'
    return join(figs,qualified_name) if ext == 'png' else qualified_name

def backup(output='config.txt'):
    '''
    Test for the presence of an existing configuration file. Back it up if present

    Parameters:
        output     Path of configuration file
    '''
    if exists(output):
        copyfile(output,output+'~')


def save_configuration(bodies,config_version=1.1,output='config_new.txt',number_bodies=1000):
    '''
    Save configuration to specified file

    Parameters:
        bodies
        config_version
        output
        number_bodies
    '''
    with open(output,'w') as f:
        f.write(f'Version={config_version}\n')
  
        for body in bodies:
            f.write(body.encode()+'\n' )
        f.write('End\n')

    print(f'Stored configuration in {output}')


def create_configuration(
    model          = 'plummer',
    number_bodies  = 1000,
    radius         = 1.0):
    '''
    Generate a configuration of bodies in phase space

    Parameters:
        model
        number_bodies
        radius
        G
    '''
    return  ConfigurationFactory.create_config_factory(model).create(number_bodies=number_bodies,radius=radius)

def create_configuration_from_xml(file):
    '''
    Parse XML file to generate complex configuration

    Sample configuration:
        <?xml version="1.0" encoding="UTF-8"?>
        <system name="Collision" model="plummer">
            <system name="1st Galaxy" pos="-100,0.2,0.3" vel="0.5,0,0"        numbodies="5000"/>
            <system name="2nd Galaxy" pos="100,0,0"     vel="-0.5,0.05,0.05" numbodies="1000"/>
        </system>

    Parameters:
        file     Path to configuration file
    '''
    root   = ET.parse(file).getroot()
    system = root.get('name')
    model  = root.get('model')
    print (f'Creating {system} using {model} model')
    n = sum([int(type_tag.get('numbodies')) for type_tag in root.findall('system')])

    product = []
    for type_tag in root.findall('system'):
        name = type_tag.get('name')
        pos = [float(x) for x in type_tag.get('pos').split(',')]
        vel = [float(x) for x in type_tag.get('vel').split(',')]
        numbodies = int(type_tag.get('numbodies'))
        print(f'subsystem: {name}, centre={pos},velocity={vel},N={numbodies}')
        for body in create_configuration(model=model,number_bodies = numbodies,radius = args.radius,G = args.G):
            product.append(body.create_with_offset(pos,vel))

    return product,n

def encode(x):
    '''
     Convert a floating point number to a string for serialization

    Parameters:
       x
    '''
    return str(unpack('!q', pack('!d',x))[0])


def plot_points(bodies=[],output='./',path='',n=2,T=0.0,V=0.0):
    '''
    Plot generated data
    '''

    fig = figure(figsize=(20,20))
    ax = fig.add_subplot(111,  projection='3d')
    sigma = n*max(np.std([body.position[0] for body in bodies]),
                  np.std([body.position[1] for body in bodies]),
                  np.std([body.position[2] for body in bodies]))
    ax.scatter([body.position[0] for body in bodies],
               [body.position[1] for body in bodies],
               [body.position[2] for body in bodies],
               s = 1,
               c = 'b')

    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_xlim(-n*sigma,n*sigma)
    ax.set_ylim(-n*sigma,n*sigma)
    ax.set_zlim(-n*sigma,n*sigma)
    n = len(bodies)
    centre = Body([0,0,0],0,[0,0,0])
    distances = sorted([centre.get_distance(b) for b in bodies])
    ax.set_title(f'$Q_1$: {distances[n//4]:.3f}[{(2**(4/3)-1)**(-1/2):.3f}],'
                 f'$Q_2$: {distances[n//2]:.3f}[{(2**(2/3)-1)**(-1/2):.3f}],'
                 f'$Q_3$: {distances[3*n//4]:.3f}[{(2**(4/3)*3**(-2/3)-1)**(-1/2):.3f}]')
    fig.suptitle(f'Initial configuration for {output}: T={T:.3f},V={V:.3f},E={T+V:.3f},Virial Ratio={-V/T:.3f}.')
    fig.savefig(get_file_name(args.out,figs=args.figs))

def generate_pairs(bodies):
    '''
    Generator used to iterate through distinct pairs of bodies

    Parameters:
        bodies     List of all bodies on model
    '''
    for i in range(0,len(bodies)):  #I have changed from range(1...)
        for j in range(i):
            yield(bodies[i],bodies[j])

if __name__=='__main__':
    rc('font',**{'family':'serif','serif':['Palatino']})
    rc('text', usetex=True)
    start  = time()
    args =   parse_args()

    if args.generate:
        for i in range(10):
            x = random.uniform(-5,5)
            print (f'\t\tREQUIRE(decode("{encode(x)}")=={x});')
        exit(0)

    seed = args.seed
    if args.seed==None:
        random_data = urandom(8)
        seed = int.from_bytes(random_data, byteorder='big')

    print (f'Number of bodies = {args.number_bodies}')
    print (f'Specified seed   = {args.seed}')
    print (f'Actual seed      = {seed}')

    random.seed(args.seed)
    config_file = join(args.path,args.output)

    backup(output=config_file)
    number_bodies = args.number_bodies
    if args.xml == None:
        bodies = create_configuration(model         = args.model,
                                      number_bodies = number_bodies,
                                      radius        = args.radius)

        save_configuration(bodies,
                           output        = config_file,
                           number_bodies = number_bodies)

        print (f'Created {args.model}: n={number_bodies}, r={args.radius}.')

    else:
        bodies,number_bodies = create_configuration_from_xml(args.xml)
        save_configuration(bodies,
                           output        = config_file,
                           number_bodies = args.number_bodies,
                           theta         = args.theta,
                           dt            = args.dt)
        print (f'Created {args.model}: n={number_bodies}, r={args.radius}.')

    if args.show:
        T =  sum([b.get_T() for b in bodies])
        V = args.G * sum([b1.get_V(b2) for (b1,b2) in generate_pairs(bodies)])
        plot_points(bodies=bodies,output=args.output,path=args.path,n=args.nsigma,T=T,V=V)

    elapsed = time() - start
    minutes = int(elapsed/60)
    seconds = elapsed - 60*minutes
    print (f'Elapsed Time {minutes} m {seconds:.2f} s')

    if args.show:
        show()


