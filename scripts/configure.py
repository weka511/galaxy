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

import argparse
import time
import os
import random
import sys
import struct
from shutil import copyfile
import xml.etree.ElementTree as ET
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


def parse_args():
    '''Parse command line parameters'''
    parser = argparse.ArgumentParser(__doc__)
    dt = 0.1
    model = 'plummer'
    output = 'config.txt'
    n = 100
    path = './' #FIXME
    r = 1.0
    theta = 0.5
    sigma = 2.0
    G = 1.0

    parser.add_argument('--dt', type=float, default=dt, help=f'Step size for integration [{dt}]')
    parser.add_argument('--output', default=output, help=f'Configuration file [f{output}]')
    parser.add_argument('--model', default=model, help=f'Used to initialize distribution [{model}]')
    parser.add_argument('-n', '--number_bodies',  type=int, default=n, help=f'Number of bodies [{n}]')
    parser.add_argument('-p', '--path', default=path,  help=f'Path for configuration files [{path}]')
    parser.add_argument('-r', '--radius', type=float, default=r, help=f'Initial Radius [{r}]')
    parser.add_argument('-s', '--seed', help='Initialize the random number generator')
    parser.add_argument('-t', '--theta', type=float, default=theta, help=f'Theta-criterion of the Barnes-Hut algorithm [{theta}]')
    parser.add_argument('--generate', action='store_true', default=False, help='Generate test data for serialization')
    parser.add_argument('--show', action='store_true', default=False, help='Show generated points')
    parser.add_argument('--nsigma', type=float, default=sigma, help=f'Scale data for show {sigma}')
    parser.add_argument('-G', '--G', type=float, default=G, help=f'Gravitational constant [{G}]')
    parser.add_argument('--xml', help='XML spec')
    return parser.parse_args()

def backup(output='config.txt'):
    '''
    Test for the presence of an existing configuration file. Back it up it present

    Parameters:
        output     Path of configuration file
    '''
    if os.path.exists(output):
        copyfile(output,output+'~')


def save_configuration(bodies,
                       config_version = 1.0,
                       output         = 'config_new.txt',
                       number_bodies  = 1000,
                       theta          = 1.0,
                       dt             = 0.1,
                       G              = 1.0):
    '''
    Save configuration to specified file

    Parameters:
        bodies
        config_version
        output
        number_bodies
        theta
        dt
        G
    '''
    with open(output,'w') as f:
        f.write('Version={0}\n'.format(config_version))
        f.write('iteration={0}\n'.format(0))
        f.write('theta={0}\n'.format(encode(theta)))
        f.write('G={0}\n'.format(encode(G)))
        f.write('dt={0}\n'.format(encode(dt)))

        for body in bodies:
            f.write(body.encode()+'\n' )
        f.write('End\n')
    print('Stored configuration in {0}'.format(output))


def generate_configuration(
    model          = 'plummer',
    number_bodies  = 1000,
    radius         = 1.0,    # TODO
    G              = 1.0):   # TODO
    '''
    Generate a configuration of bodies in phase space

    Parameters:
        model
        number_bodies
        radius
        G
    '''
    config_factory = create_config_factory(model)
    return config_factory(number_bodies=number_bodies)

def create_config_factory(model):
    '''
    Instantiate method for creating configuration

    Parameters:
        model       Name of model
    '''
    config_factory_dict = {'plummer':create_plummer}
    if model in config_factory_dict:
        return config_factory_dict[model]
    else:
        print ('Unrecognized model: {0}'.format(model))
        sys.exit(1)


def create_plummer(number_bodies=100,
                   radius=1.0,
                   G=1.0):             # TODO
    '''
    # Instantiate configuration for Plummer model

    http://www.artcompsci.org/kali/vol/plummer/volume9.pdf
    '''

    def randomize_on_sphere(radius=radius):
        '''
        Randomly position point on surface of sphere
        '''
        theta = np.arccos(random.uniform(-1,1))
        phi = random.uniform(0,2*np.pi)
        return [radius * np.sin(theta) * np.cos(phi),
                radius * np.sin(theta) * np.sin(phi),
                radius * np.cos(theta)]


    def get_velocity_ratio():
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

    def create_velocity(radius=1):
        '''Setup initial velocities'''
        return randomize_on_sphere(radius=get_velocity_ratio() * np.sqrt(2.0) *(1 + radius**2)**(-0.25))



    def create_body():
        '''Create one body with random position and velocity'''
        mass = 1.0/number_bodies
        radius = 1.0/np.sqrt(random.uniform(0,1)**(-2/3) - 1)
        return Body(randomize_on_sphere(radius=radius),mass,create_velocity(radius=radius))

    return [create_body() for i in range(number_bodies)]


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
    def shift(body,offset,vel):
        pos  = [sum(u) for u in zip(body[:3],offset)]
        vels = [sum(v) for v in zip(body[:3],vel)]
        return pos + body[3:4] + vels

    bodies = []
    root   = ET.parse(file).getroot()
    system = root.get('name')
    model  = root.get('model')
    print (system,model)
    n = sum([int(type_tag.get('numbodies')) for type_tag in root.findall('system')])

    for type_tag in root.findall('system'):
        name = type_tag.get('name')
        pos = [float(x) for x in type_tag.get('pos').split(',')]
        vel = [float(x) for x in type_tag.get('vel').split(',')]
        numbodies = int(type_tag.get('numbodies'))
        print(name,pos,vel,numbodies)
        subsystem = generate_configuration(model=model,number_bodies = numbodies,radius = args.radius,G = args.G)
        bodies = bodies + [shift(b,pos,vel) for b in subsystem]

    return bodies,n

def encode(x):
    '''
     Convert a floating point number to a string for serialization

    Parameters:
       x
    '''
    return str(struct.unpack('!q', struct.pack('!d',x))[0])


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
    ax.set_title(f'Q1: {distances[n//4]:.3f}[{(2**(4/3)-1)**(-1/2):.3f}],'
                 f'Q2: {distances[n//2]:.3f}[{(2**(2/3)-1)**(-1/2):.3f}],'
                 f'Q3: {distances[3*n//4]:.3f}[{(2**(4/3)*3**(-2/3)-1)**(-1/2):.3f}]')
    fig.suptitle(f'Initial configuration for {output}: T={T:.3f},V={V:.3f},E={T+V:.3f},Virial Ratio={-V/T:.3f}.')

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
    args =   parse_args()

    if args.generate:
        for i in range(10):
            x = random.uniform(-5,5)
            print ('\t\tREQUIRE(decode("{1}")=={0});'.format(x,encode(x)))
        sys.exit(0)

    seed = args.seed
    if args.seed==None:
        random_data = os.urandom(8)
        seed = int.from_bytes(random_data, byteorder="big")

    print ('Number of bodies = {0}\n'
           'Specified seed   = {1}\n'
           'Actual seed      = {2}\n'.format(args.number_bodies,args.seed,seed))

    random.seed(args.seed)
    config_file = os.path.join(args.path,args.output)
    start = time.time()
    backup(output=config_file)
    number_bodies = args.number_bodies
    if args.xml == None:
        bodies = generate_configuration(model         = args.model,
                                        number_bodies = args.number_bodies,
                                        radius        = args.radius,
                                        G             = args.G)

        save_configuration(bodies,
                           output        = config_file,
                           number_bodies = args.number_bodies,
                           theta         = args.theta,
                           dt            = args.dt)
        print ('Created {0}: n={1}, r={2}.'.format(args.model,number_bodies,args.radius))

    else:
        bodies,number_bodies = create_configuration_from_xml(args.xml)
        save_configuration(bodies,
                           output        = config_file,
                           number_bodies = args.number_bodies,
                           theta         = args.theta,
                           dt            = args.dt)
        print ('Created {0}: n={1}, r={2}.'.format(args.model,number_bodies,args.radius))

    if args.show:
        T =  sum([b.get_T() for b in bodies])
        V = args.G * sum([b1.get_V(b2) for (b1,b2) in generate_pairs(bodies)])
        plot_points(bodies=bodies,output=args.output,path=args.path,n=args.nsigma,T=T,V=V)
        show()

    elapsed_time = time.time()-start
    print ('Elapsed time={0:.1f} seconds. i.e. {1:.0f} msec per 1000 bodies'.format(elapsed_time,
                                                                                    1000*elapsed_time/(number_bodies/1000)))
