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

import os, random, sys
from shutil import copyfile

def backup(output='config_new.text'):
    if os.path.exists(output):
        backup_file = output.split('.')[0]+'.bak'
        copyfile(output,backup_file)
        
def configure(config_version = 1.0,
              output         = 'config_new.text',
              type           = 'plummer',
              soften         = 1.0,
              imag_freq      = 1,
              max_iter       = 100,
              number_bodies  = 1000,
              path           = '.',
              radius         = 1.0,
              theta          = 1.0):
    backup(output=output)
    config_factory = create_config_factory(type)
    with open(output,'w') as f:
        pass

def create_config_factory(type):
    config_factory_dict = {'plummer':plummer}
    if type in config_factory_dict: 
        return config_factory_dict[type]
    else:
        print ('Unrecognized config type: {0}'.format(type))
        sys.exit(1)
        
def plummer():
    pass

def serialize(x):
    return hex(struct.unpack('!q', struct.pack('!d',x))[0])[2:]
    
if __name__=='__main__':
    import argparse
    parser = argparse.ArgumentParser('Configure galaxy.exe')
    parser.add_argument(      '--dt', type=float, default=0.1,       help='Step size for integration')
    parser.add_argument(      '--output', default='config_new.text', help='Configuration file')
    parser.add_argument(      '--type', default='plummer',           help='Used to initialize distribution')
    parser.add_argument('-a', '--soften', type=float, default=1.0,   help='Softening length')
    parser.add_argument('-i', '--image_freq',                        help='Controls frequency for logging')
    parser.add_argument('-m', '--max_iter',                          help='Number of iterations')
    parser.add_argument('-n', '--number_bodies',                     help='Number of bodies')
    parser.add_argument('-p', '--path',                              help='Path for configuration files')
    parser.add_argument('-r', '--radius',                            help='Initial Radius')
    parser.add_argument('-s', '--seed',                              help='Initialize the random number generator')
    parser.add_argument('-t', '--theta',                             help='Theta-criterion of the Barnes-Hut algorithm')
    args = parser.parse_args()
    random.seed(args.seed)
    configure(output        = args.output,
              type          = args.type,
              imag_freq     = args.image_freq,
              number_bodies = args.number_bodies,
              path          = args.path,
              radius        = args.radius,
              theta         = args.theta)
    