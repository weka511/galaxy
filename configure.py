# Copyright (C) 2019 Greenweaves Software Limited
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

import os, random, sys, struct, math
from shutil import copyfile

def backup(output='config_new.text'):
    if os.path.exists(output):
        backup_file = output.split('.')[0]+'.bak'
        copyfile(output,backup_file)
        
def configure(config_version = 1.0,
              output         = 'config_new.txt',
              type           = 'plummer',
              soften         = 1.0,
              imag_freq      = 1,
              max_iter       = 100,
              number_bodies  = 1000,
              path           = '.',
              radius         = 1.0,
              theta          = 1.0,
              iter           = 0,
              dt             = 0.1,
              G              = 1.0):
    backup(output=output)
    config_factory = create_config_factory(type)
    with open(output,'w') as f:
        f.write('Version={0}\n'.format(config_version))
        f.write('iteration={0}\n'.format(iter))
        f.write('theta={0}\n'.format(encode(theta)))
        f.write('G={0}\n'.format(encode(G)))
        f.write('dt={0}\n'.format(encode(dt)))
        for body in config_factory(number_bodies=number_bodies):
            f.write(','.join([encode(b) for b in body])+'\n' )
            

def create_config_factory(type):
    config_factory_dict = {'plummer':plummer}
    if type in config_factory_dict: 
        return config_factory_dict[type]
    else:
        print ('Unrecognized config type: {0}'.format(type))
        sys.exit(1)
        
def plummer(number_bodies=100):
    def create_pos(radius=1):
        theta  = math.acos(random.uniform(-1,1))
        phi    = random.uniform(0,2*math.pi)
        x      = radius * math.sin(theta) * math.cos(phi)
        y      = radius * math.sin(theta) * math.sin(phi)
        z      = radius * math.cos(theta)
        return (x,y,z)
    
    def create_velocity(radius=1):
        x = 0.0
        y = 0.1
        while y > x*x*(1-x*x)**3.5:
            x = random.uniform(0,1)
            y = random.uniform(0,0.1)
        velocity = x * math.sqrt(2.0) *(1 + radius*radius)**(-0.25)
        theta    = math.acos(random.uniform(-1,1))
        phi      = random.uniform(0,2*math.pi)
        vx       = velocity * math.sin(theta) * math.cos(phi)
        vy       = velocity * math.sin(theta) * math.sin(phi)
        vz       = velocity * math.cos(theta)
        return (vx,vy,vz)
    
    def create_body():
        mass     = 1.0/number_bodies
        radius   = 1.0/math.sqrt(random.uniform(0,1)**(-2/3) - 1)
        x,y,z    = create_pos(radius=radius)
        vx,vy,vz = create_velocity(radius=radius)
        return [mass,x,y,z,vx,vy,vz]
    
    return [create_body() for i in range(number_bodies)]

def encode(x):
    return hex(struct.unpack('!q', struct.pack('!d',x))[0])[2:]
    
if __name__=='__main__':
    import argparse
    parser = argparse.ArgumentParser('Configure galaxy.exe')
    parser.add_argument(      '--dt', type=float, default=0.1,       help='Step size for integration')
    parser.add_argument(      '--output', default='config_new.txt', help='Configuration file')
    parser.add_argument(      '--type', default='plummer',           help='Used to initialize distribution')
    parser.add_argument('-a', '--soften', type=float, default=1.0,   help='Softening length')
    parser.add_argument('-i', '--image_freq',                        help='Controls frequency for logging')
    parser.add_argument('-m', '--max_iter',                          help='Number of iterations')
    parser.add_argument('-n', '--number_bodies', default=100,        help='Number of bodies')
    parser.add_argument('-p', '--path',                              help='Path for configuration files')
    parser.add_argument('-r', '--radius', type=float, default=1.0,   help='Initial Radius')
    parser.add_argument('-s', '--seed',                              help='Initialize the random number generator')
    parser.add_argument('-t', '--theta', type=float, default=0.5,    help='Theta-criterion of the Barnes-Hut algorithm')
    args = parser.parse_args()
    random.seed(args.seed)
    configure(output        = args.output,
              type          = args.type,
              imag_freq     = args.image_freq,
              number_bodies = args.number_bodies,
              path          = args.path,
              radius        = args.radius,
              theta         = args.theta,
              dt            = args.dt)
    