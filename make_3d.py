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

  Companion to barnes_hut.cpp - plot images
'''
import os, re, sys, numpy as np, matplotlib.pyplot as plt,mpl_toolkits.mplot3d as trid

fname_in='./configs/body_000449.dat'
pos = np.loadtxt(fname_in)
ax = plt.gcf().add_subplot(111, aspect='equal', projection='3d')
ax.scatter(pos[:,0],pos[:,2],pos[:,2])
#ax.set_xlim([-1, 1.0])
#ax.set_ylim([-1, 1.0])
#ax.set_zlim([-0.5, 0.5])  
plt.show()