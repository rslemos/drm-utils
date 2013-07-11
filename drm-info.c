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


static char* getList(uint32_t* values, uint32_t count) {
	static char buffer[512];
	int i;

	char* p = buffer;	

	for (i = 0; i < count; i++) {
		p += snprintf(p, 512 - (buffer - p), "%u, ", values[i]);
	}

	if (count > 0) {
		*(p - 2) = '\0';
	}

	return buffer;
}

static char* getConnectorType(uint32_t type) {
	return "";
}

static char* getConnectorSubpixel(drmModeSubpixel subpixel) {
	return "";
}

static char* getConnectorConnection(drmModeConnection connection) {
	return "";
}

static void printConnectors(int fd, uint32_t* ids, uint32_t count) {
	drmModeConnector *conn;
	int i;

	for (i = 0; i < count; i++) {
		conn = drmModeGetConnector(fd, ids[i]);
		if (!conn) {
			fprintf(stderr, "      Connector %u: cannot retrieve (%d): %m\n\n", ids[i], errno);
			continue;
		}

		fprintf(stderr, "      Connector %u:\n", conn->connector_id);
		fprintf(stderr, "        Type: %s\n", getConnectorType(conn->connector_type));
		fprintf(stderr, "        Dimensions: %umm x %umm\n", conn->mmWidth, conn->mmHeight);
		fprintf(stderr, "        Subpixel: %s\n", getConnectorSubpixel(conn->subpixel));
		fprintf(stderr, "        Connection: %s\n", getConnectorConnection(conn->connection));
		fprintf(stderr, "        Encoders: %d: %s\n", conn->count_encoders, getList(conn->encoders, conn->count_encoders));
		fprintf(stderr, "        Properties: %d: %s\n", conn->count_props, getList(conn->props, conn->count_props));
		fprintf(stderr, "        Properties values: %d: %s\n", conn->count_props, getList(conn->prop_values, conn->count_props));
		//fprintf(stderr, "        Modes: %d: %s\n", conn->count_modes, getList(conn->modes, conn->count_modes));

		drmModeFreeConnector(conn);
	}
}

 
static void printResources0(int fd, drmModeRes* res) {
	fprintf(stderr, "  Resources:\n");
	fprintf(stderr, "    Width: %u - %u\n", res->min_width, res->max_width);
	fprintf(stderr, "    Height: %u - %u\n", res->min_height, res->max_height);
	fprintf(stderr, "    Framebuffers: %d: %s\n", res->count_fbs, getList(res->fbs, res->count_fbs));
	fprintf(stderr, "    CRTCs: %d: %s\n", res->count_crtcs, getList(res->crtcs, res->count_crtcs));
	fprintf(stderr, "    Encoders: %d: %s\n", res->count_encoders, getList(res->encoders, res->count_encoders));
	fprintf(stderr, "    Connectors: %d\n", res->count_connectors);
	printConnectors(fd, res->connectors, res->count_connectors);
}

static void printResources(int fd) {
	drmModeRes *res;

	res = drmModeGetResources(fd);
	if (!res) {
		fprintf(stderr, "cannot retrieve DRM resources (%d): %m\n", errno);
		return;
	}

	printResources0(fd, res);

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

