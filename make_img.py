'''
 Copyright (C) 2017-2018 Greenweaves Software Pty Ltd

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

  Companion to barnes_hut.cpp - plot images
'''
import os, re, sys, numpy as np, matplotlib.pyplot as plt

def plot(fname_in,fname_out):
    pos = np.loadtxt(fname_in)
    ax = plt.gcf().add_subplot(111, aspect='equal')
    ax.cla()
    ax.scatter(pos[:,0], pos[:,1], 1)
    #ax.set_xlim([0., 1.0])
    #ax.set_ylim([0., 1.0])
    plt.gcf().savefig(fname_out)
    plt.close()
    
if __name__=='__main__':
    config_path = './configs/'
    out_path = './imgs/'
    for file_name in os.listdir(config_path):
        m = re.search('body_[0-9]+.dat',file_name)
        if m:
            print (m.group(0))
            plot(os.path.join(config_path,m.group(0)),
                             os.path.join(out_path,m.group(0).replace('dat','png')))
        
