#!/usr/bin/env python

# Copyright (C) 2019-2025 Greenweaves Software Limited
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

'''Functions shared by python scripts'''

from glob import glob
from os.path import join
import re

def get_seq(name,prefix='energy',ext='png'):
    '''
    Extract sequence number from file name

    Parameters:
        prefix  File names match this prefix, followed by sequence number
        ext     File names have this extension
    '''
    pattern = re.compile(r'.*{0}(\d+).{1}'.format(prefix,ext))
    return int(re.match(pattern,name).group(1))


def find_seq(path='./imgs',prefix='energy',ext='png'):
    '''
    Find largest sequence number in a set of files.

    Parameters:
        path     Identifies where files are stored
        prefix   File names match this prefix, followed by sequence number
        ext      File names have this extension

    Returns:      largest sequence number, or -1 if no files found
    '''
    try:
        files = sorted(glob(join(path,'{0}*.{1}'.format(prefix,ext))))
        return get_seq(files[-1],prefix=prefix,ext=ext)
    except IndexError:
        return -1
