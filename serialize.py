# Copyright (C) 2018-2019 Greenweaves Software Limited
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
#
# Serializaion https://stackoverflow.com/questions/46422692/serializing-float32-values-in-python-and-deserializing-them-in-c
# https://stackoverflow.com/questions/24790722/c-python-equivalent-of-matlabs-num2hex

import math,struct

if __name__ =='__main__':
    print (math.pi,
           hex(struct.unpack('!q', struct.pack('!d',math.pi))[0])[2:])