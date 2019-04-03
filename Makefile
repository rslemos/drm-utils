# BEGIN COPYRIGHT NOTICE
# 
# This file is part of program "drm-utils"
# Copyright 2013  Rodrigo Lemos
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# 
# END COPYRIGHT NOTICE
CFLAGS = -O0 -g
LDFLAGS = -Wall

CFLAGS += `pkg-config --cflags libdrm`
LDFLAGS += `pkg-config --libs libdrm`
LDLIBS += `pkg-config --libs libdrm`

all: drm-info
