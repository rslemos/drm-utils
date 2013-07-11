/*******************************************************************************
 * BEGIN COPYRIGHT NOTICE
 * 
 * This file is part of program "drm-utils"
 * Copyright 2013  Rodrigo Lemos
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * END COPYRIGHT NOTICE
 ******************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libdrm/drm.h>
#include <xf86drm.h>


static char* getCapability(int fd, int cap) {
	uint64_t has;
	static char buffer[10];

	if (drmGetCap(fd, cap, &has) < 0) {
		return "unknown";
	} else {
		sprintf(buffer, "%lu", has);
		return buffer;
	}
}

static void printCapabilities(int fd) {
	fprintf(stderr, "  Capabilities:\n");
	fprintf(stderr, "    DRM_CAP_DUMB_BUFFER: %s\n",
			getCapability(fd, DRM_CAP_DUMB_BUFFER));
	fprintf(stderr, "    DRM_CAP_VBLANK_HIGH_CRTC: %s\n",
			getCapability(fd, DRM_CAP_VBLANK_HIGH_CRTC));
	fprintf(stderr, "    DRM_CAP_DUMB_PREFERRED_DEPTH: %s\n",
			getCapability(fd, DRM_CAP_DUMB_PREFERRED_DEPTH));
	fprintf(stderr, "    DRM_CAP_DUMB_PREFER_SHADOW: %s\n",
			getCapability(fd, DRM_CAP_DUMB_PREFER_SHADOW));
	fprintf(stderr, "    DRM_CAP_PRIME: %s\n",
			getCapability(fd, DRM_CAP_PRIME));
}

static void drminfo(char* device) {
	int fd;

	fd = open(device, O_RDONLY);

	fprintf(stderr, "Device %s:\n", device);
	printCapabilities(fd);
	fprintf(stderr, "\n");

	close(fd);
}

int main(int argc, char* argv[]) {
	argc--; argv++;

	while (argc) {
		drminfo(*argv);
		argc--; argv++;
	}

	return 0;
}

