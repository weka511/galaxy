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

 Companion to galaxy.exe - cut up logfiles
'''

import re,matplotlib.pyplot as plt


def process(file_names,colours=['r','g','b','m','c','y']):
    for i in range(len(file_names)):
        energies=[]
        dt=0
        m=0
        n=0
        for line in open(file_names[i]):
            match=re.match(r'.*dt=([-+e\.0-9]+)',line.strip())
            if match:
                dt=float(match.group(1))
            match=re.match(r'.* Number of iterations=([-+e\.0-9]+)',line.strip())
            if match:
                m=int(match.group(1))
            match=re.match(r'.* Number of bodies=([-+e\.0-9]+)',line.strip())
            if match:
                n=int(match.group(1))            
            match=re.match(r'.*Energy +([-+e\.0-9]+)',line.strip())
            if match:
                energies.append(float(match.group(1)))
        plt.plot(energies,c=colours[i],label='dt={0},m={1},n={1}'.format(dt,m,n))
        plt.scatter(0,energies[0],c=colours[i])
    plt.title('Total Energy')
    plt.legend(loc='best')
    plt.savefig('energies.png')
    
if __name__=='__main__':
    process(['logfile_2018-02-14_09-19','logfile_2018-02-14_10-46','logfile_2018-02-14_17-23','logfile_2018-02-13_14-31'])
    plt.show()