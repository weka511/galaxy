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

import os, random, sys, struct, math, matplotlib.pyplot as plt,mpl_toolkits.mplot3d,numpy as np
from shutil import copyfile
import xml.etree.ElementTree as ET

# backup
#
# Test for the presence of an exisitng gonfugyration file. Back it up it present

def backup(output='config.txt'):
    if os.path.exists(output):
        copyfile(output,output+'~')

# save_configuration
#
# Save configuration to specified file

def save_configuration(bodies,
                       config_version = 1.0,
                       output         = 'config_new.txt',
                       number_bodies  = 1000,
                       theta          = 1.0,
                       dt             = 0.1,
                       G              = 1.0):
    with open(output,'w') as f:
        f.write('Version={0}\n'.format(config_version))
        f.write('iteration={0}\n'.format(0))
        f.write('theta={0}\n'.format(encode(theta)))
        f.write('G={0}\n'.format(encode(G)))
        f.write('dt={0}\n'.format(encode(dt)))
  
        for body in bodies:
            f.write(','.join([encode(b) for b in body])+'\n' )
        f.write('End\n')
    print('Stored configuration in {0}'.format(output))
        
# generate_configuration_file
#
# Generate a configuration of bodies in phase space

def generate_configuration(
    model          = 'plummer',
    number_bodies  = 1000,
    radius         = 1.0,    # TODO
    G              = 1.0):   # TODO
    config_factory = create_config_factory(model)
    return config_factory(number_bodies=number_bodies)  
            
# create_config_factory
#
# Instantiate method for creatong configuration
#
# Parameters:
#     model       Name of model

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
def create_plummer(number_bodies=100,
                   radius=1.0,
                   G=1.0):             # TODO
    
    # randomize_on_sphere
    #
    # Randomly position point on sphere
    def randomize_on_sphere(radius=radius):
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

    # create_velocity
    #
    # Setup initial velocities
    
    def create_velocity(radius=1):
        return randomize_on_sphere(radius=get_velocity_ratio() * math.sqrt(2.0) *(1 + radius*radius)**(-0.25))
    
    # create_body
    #
    # Create one body with random position and velocity
    def create_body():
        mass     = 1.0/number_bodies
        radius   = 1.0/math.sqrt(random.uniform(0,1)**(-2/3) - 1)
        return randomize_on_sphere(radius=radius) + [mass] + create_velocity(radius=radius)
    
    return [create_body() for i in range(number_bodies)]

# create_configuration_from_xml
#
# Parse XML file to generate complex configuration

def create_configuration_from_xml(file):
    def shift(body,offset,vel):
        pos  = [sum(u) for u in zip(body[:3],offset)]
        vels = [sum(v) for v in zip(body[:3],vel)]
        return pos + body[3:4] + vels
    
    bodies = []
    root   = ET.parse(file).getroot()
    system = root.get('name')
    model  = root.get('model')
    print (system,model)
    ns = [int(type_tag.get('numbodies')) for type_tag in root.findall('system')]
    n  = sum(ns)
        
    for type_tag in root.findall('system'):
        name      = type_tag.get('name')
        pos       = [float(x) for x in type_tag.get('pos').split(',')]
        vel       = [float(x) for x in type_tag.get('vel').split(',')]
        numbodies = int(type_tag.get('numbodies'))
        print(name,pos,vel,numbodies)  
        subsystem = generate_configuration(
                                        model=model,
                                        number_bodies = numbodies,
                                        radius        = args.radius,
                                        G             = args.G)
        bodies = bodies + [shift(b,pos,vel) for b in subsystem]
    
    return bodies,n

# encode
#
#  Convert a floating point number to a string for serialization

def encode(x):
    return str(struct.unpack('!q', struct.pack('!d',x))[0])

# plot_points
#
# Plot generated data

def plot_points(bodies=[],output='./',path='',n=2):
    plt.figure(figsize=(20,20)) 
    ax    = plt.gcf().add_subplot(111,  projection='3d')
    sigma = n*max(np.std([body[0] for body in bodies]),
                  np.std([body[1] for body in bodies]),
                  np.std([body[2] for body in bodies]))    
    ax.scatter([body[0] for body in bodies],
               [body[1] for body in bodies],
               [body[2] for body in bodies],
               s = 1,
               c = 'b')
 
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_xlim(-n*sigma,n*sigma)
    ax.set_ylim(-n*sigma,n*sigma)
    ax.set_zlim(-n*sigma,n*sigma)
    plt.title('Initial configuration for {0}'.format(output))

# Generator used to iterate through distinct pairs of bodies

def pairs(bodies):
    for i in range(1,len(bodies)):
        for j in range(i):
            yield(bodies[i],bodies[j])

# dist
#
# Calculate distance between two points

def dist(b1,b2):
    return math.sqrt(sum([(b1[i]-b2[i])*(b1[i]-b2[i]) for i in range(3)]))

# Calculate energy, and work out ratio from Virial theorem (should be 2)

def check_energy(bodies=[],G=1.0):
    kinetic_energy   = 0.5 * sum([b[3]*sum(b[i]*b[i] for i in range(4,7)) for b in bodies])
    potential_energy = -  G * sum([G * b1[3] * b2[3]/dist(b1,b2) for (b1,b2) in pairs(bodies)])
    print ('Total Energy           = {0}\n'
           'Kinetic Energy         = {1}\n'
           'Potential Energy       = {2}\n'
           'Ratio (Virial Theorem) = {3}'.format(kinetic_energy+potential_energy,
                                           kinetic_energy,
                                           potential_energy,
                                           -potential_energy/kinetic_energy))
 
def  check_quartiles(bodies):
    n         = len(bodies)
    distances = sorted([dist(b,[0,0,0,1,0,0,0]) for b in bodies])
    print ('Q1: was {0:4f}, expected {1:4f}'.format(distances[n//4],(2**(4/3)-1)**(-1/2)))
    print ('Q2: was {0:4f}, expected {1:4f}'.format(distances[n//2],(2**(2/3)-1)**(-1/2)))
    print ('Q3: was {0:4f}, expected {1:4f}'.format(distances[3*n//4],(2**(4/3)*3**(-2/3)-1)**(-1/2)))


        
if __name__=='__main__':
    import argparse, time
    
    parser = argparse.ArgumentParser('Configure galaxy.exe')
    parser.add_argument(      '--dt', type=float,                default=0.1,          help='Step size for integration')
    parser.add_argument(      '--output',                        default='config.txt', help='Configuration file')
    parser.add_argument(      '--model',                         default='plummer',    help='Used to initialize distribution')
    parser.add_argument('-n', '--number_bodies',  type=int,      default=100,          help='Number of bodies')
    parser.add_argument('-p', '--path',                          default='./configs',  help='Path for configuration files')
    parser.add_argument('-r', '--radius',   type=float,          default=1.0,          help='Initial Radius')
    parser.add_argument('-s', '--seed',                                                help='Initialize the random number generator')
    parser.add_argument('-t', '--theta',    type=float,          default=0.5,          help='Theta-criterion of the Barnes-Hut algorithm')
    parser.add_argument(      '--generate', action='store_true', default=False,        help='Generate test data for serialization')
    parser.add_argument(      '--show',     action='store_true', default=False,        help='Show generated points')
    parser.add_argument(      '--nsigma',   type=float,          default=2,            help='Scale data for show')
    parser.add_argument(      '--energy',   action='store_true', default=False,        help='Check energy')
    parser.add_argument('-G',  '--G',       type=float,          default=1.0,          help='Radius')
    parser.add_argument(      '--quartile', action='store_true', default=False,        help='Check quartiles')
    parser.add_argument(      '--xml',                                                 help='XML spec')
    args = parser.parse_args()
    
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
    start       = time.time()
    backup(output=config_file)
    number_bodies = args.number_bodies
    if args.xml==None:
        bodies      = generate_configuration(model         = args.model,
                                             number_bodies = args.number_bodies,
                                             radius        = args.radius,
                                             G             = args.G)
       
        save_configuration(bodies,
                           output        = config_file,
                           number_bodies = args.number_bodies,
                           theta         = args.theta,
                           dt            = args.dt)
        print ('Created {0}: n={1}, r={2}.'.format(args.model,number_bodies,args.radius))
        
        if args.energy:
            check_energy(bodies=bodies,G=args.G)
        
        if args.quartile:
            check_quartiles(bodies)
    else:
        bodies,number_bodies = create_configuration_from_xml(args.xml)
        save_configuration(bodies,
                           output        = config_file,
                           number_bodies = args.number_bodies,
                           theta         = args.theta,
                           dt            = args.dt)
        print ('Created {0}: n={1}, r={2}.'.format(args.model,number_bodies,args.radius))
        
    if args.show:
        plot_points(bodies=bodies,output=args.output,path=args.path,n=args.nsigma)
        plt.show()
        
    elapsed_time = time.time()-start
    print ('Elapsed time={0:.1f} seconds. i.e. {1:.0f} msec per 1000 bodies'.format(elapsed_time,
                                                                                    1000*elapsed_time/(number_bodies/1000)))