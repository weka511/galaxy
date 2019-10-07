# Copyright (C) 2019 Greenweaves Software Limited
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
#
# Create configuration for galaxy.exe

import os, random, sys, struct, math
from shutil import copyfile

# backup
#
# Test for the presence of an exisitng gonfugyration file. Back it up it present

def backup(output='config_new.text'):
    if os.path.exists(output):
        copyfile(output,output+'~')

# generate_configuration_file
#
# Generate a configuration and save it

def generate_configuration_file(
    config_version = 1.0,
    output         = 'config_new.txt',
    model          = 'plummer',
    soften         = 1.0,
    imag_freq      = 1,
    max_iter       = 100,
    number_bodies  = 1000,
    radius         = 1.0,
    theta          = 1.0,
    dt             = 0.1,
    G              = 1.0):
    config_factory = create_config_factory(model)
    with open(output,'w') as f:
        f.write('Version={0}\n'.format(config_version))
        f.write('iteration={0}\n'.format(0))
        f.write('theta={0}\n'.format(encode2(theta)))
        f.write('G={0}\n'.format(encode2(G)))
        f.write('dt={0}\n'.format(encode2(dt)))
        for body in config_factory(number_bodies=number_bodies):
            f.write(','.join([encode2(b) for b in body])+'\n' )
            

def create_config_factory(model):
    config_factory_dict = {'plummer':create_plummer}
    if model in config_factory_dict: 
        return config_factory_dict[model]
    else:
        print ('Unrecognized model: {0}'.format(model))
        sys.exit(1)

# create_plummer
#
# Instantiate configuration for Plummer model
#
# http://www.artcompsci.org/kali/vol/plummer/volume9.pdf       
def create_plummer(number_bodies=100):
    def randomize_on_sphere(radius=1):
        theta  = math.acos(random.uniform(-1,1))
        phi    = random.uniform(0,2*math.pi)
        return [radius * math.sin(theta) * math.cos(phi),
                radius * math.sin(theta) * math.sin(phi),
                radius * math.cos(theta)]

# get_velocity_ratio
#
#  Sampling used in A comparison of Numerical Methods for the Study of Star Cluster Dynamics, 
#  by Sverre Aarseth, Michel Henon, and Roland Wielen, in Astron. Astroph.37, 183 (1974)
#  http://articles.adsabs.harvard.edu/full/1974A%26A....37..183A

    def get_velocity_ratio():
        x = 0.0
        y = 0.1
        while y > x*x*(1-x*x)**3.5:
            x = random.uniform(0,1)
            y = random.uniform(0,0.1)
        return x
    
    def create_velocity(radius=1):
        return randomize_on_sphere(radius=get_velocity_ratio() * math.sqrt(2.0) *(1 + radius*radius)**(-0.25))
    
    def create_body():
        mass     = 1.0/number_bodies
        radius   = 1.0/math.sqrt(random.uniform(0,1)**(-2/3) - 1)
        return [mass] +randomize_on_sphere(radius=radius) + create_velocity(radius=radius)
    
    return [create_body() for i in range(number_bodies)]

# encode2

def encode2(x):
    return hex(struct.unpack('!q', struct.pack('!d',x))[0])[2:]
    
if __name__=='__main__':
    import argparse, time
    parser = argparse.ArgumentParser('Configure galaxy.exe')
    parser.add_argument(      '--dt', type=float,              default=0.1,              help='Step size for integration')
    parser.add_argument(      '--output',                      default='config_new.txt', help='Configuration file')
    parser.add_argument(      '--model',                       default='plummer',        help='Used to initialize distribution')
    parser.add_argument('-a', '--soften', type=float,          default=1.0,              help='Softening length')
    parser.add_argument('-i', '--image_freq',                                            help='Controls frequency for logging')
    parser.add_argument('-m', '--max_iter',                                              help='Number of iterations')
    parser.add_argument('-n', '--number_bodies',  type=int,    default=100,              help='Number of bodies')
    parser.add_argument('-p', '--path',                        default='./configs',      help='Path for configuration files')
    parser.add_argument('-r', '--radius', type=float,          default=1.0,              help='Initial Radius')
    parser.add_argument('-s', '--seed',                                                  help='Initialize the random number generator')
    parser.add_argument('-t', '--theta', type=float,           default=0.5,              help='Theta-criterion of the Barnes-Hut algorithm')
    parser.add_argument(      '--generate',action='store_true',default=False,            help='Generate test data for serialization')
    args = parser.parse_args()
    if args.generate:
        for i in range(10):
            x = random.uniform(0,5)
            print ('\t\tREQUIRE(deserialize("{1}")=={0});'.format(x,encode2(x))) 
        sys.exit(0)
    random.seed(args.seed)
    config_file = os.path.join(args.path,args.output)
    start       = time.time()
    backup(output=config_file)
    generate_configuration_file(
        output        = config_file,
        model         = args.model,
        imag_freq     = args.image_freq,
        number_bodies = args.number_bodies,
        radius        = args.radius,
        theta         = args.theta,
        dt            = args.dt)
    print ('Created {0}, n={1}, r={2}, stored in {3}'.format(args.model,args.number_bodies,args.radius,config_file))
    elapsed_time = time.time()-start
    print ('Elapsed time={0:.1f} seconds. i.e. {1:.0f} msec per 1000 bodies'.format(elapsed_time,
                                                                                    1000*elapsed_time/(args.number_bodies/1000)))