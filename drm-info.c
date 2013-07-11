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
#include <xf86drmMode.h>

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

static void printResources0(drmModeRes* res) {
	fprintf(stderr, "  Resources:\n");
	fprintf(stderr, "    Width: %u - %u\n", res->min_width, res->max_width);
	fprintf(stderr, "    Height: %u - %u\n", res->min_height, res->max_height);
	fprintf(stderr, "    Framebuffers: %d\n", res->count_fbs);
	fprintf(stderr, "    CRTCs: %d\n", res->count_crtcs);
	fprintf(stderr, "    Encoders: %d\n", res->count_encoders);
	fprintf(stderr, "    Connectors: %d\n", res->count_connectors);
}

static void printResources(int fd) {
	drmModeRes *res;

	res = drmModeGetResources(fd);
	if (!res) {
		fprintf(stderr, "cannot retrieve DRM resources (%d): %m\n", errno);
		return;
	}

	printResources0(res);

	drmModeFreeResources(res);
}

static void drminfo(char* device) {
	int fd;

	fd = open(device, O_RDONLY);

	fprintf(stderr, "Device %s:\n", device);
	printCapabilities(fd);
	fprintf(stderr, "\n");
	printResources(fd);

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

