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
#include <errno.h>
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
	fprintf(stdout, "  Capabilities:\n");
	fprintf(stdout, "    DRM_CAP_DUMB_BUFFER: %s\n",
			getCapability(fd, DRM_CAP_DUMB_BUFFER));
	fprintf(stdout, "    DRM_CAP_VBLANK_HIGH_CRTC: %s\n",
			getCapability(fd, DRM_CAP_VBLANK_HIGH_CRTC));
	fprintf(stdout, "    DRM_CAP_DUMB_PREFERRED_DEPTH: %s\n",
			getCapability(fd, DRM_CAP_DUMB_PREFERRED_DEPTH));
	fprintf(stdout, "    DRM_CAP_DUMB_PREFER_SHADOW: %s\n",
			getCapability(fd, DRM_CAP_DUMB_PREFER_SHADOW));
	fprintf(stdout, "    DRM_CAP_PRIME: %s\n",
			getCapability(fd, DRM_CAP_PRIME));
}


static char* getList32(uint32_t* values, uint32_t count) {
	static char buffer[512];
	int i;

	char* p = buffer;
	*p = '\0';

	for (i = 0; i < count; i++) {
		p += snprintf(p, 512 - (buffer - p), "%u, ", values[i]);
	}

	if (count > 0) {
		*(p - 2) = '\0';
	}

	return buffer;
}

static char* getList64(uint64_t* values, uint32_t count) {
	static char buffer[512];
	int i;

	char* p = buffer;
	*p = '\0';

	for (i = 0; i < count; i++) {
		p += snprintf(p, 512 - (buffer - p), "%lu, ", values[i]);
	}

	if (count > 0) {
		*(p - 2) = '\0';
	}

	return buffer;
}

static void printProperty(int fd, drmModePropertyRes* prop, uint64_t value) {
    fprintf(stdout, "          Property: %i\n", prop->prop_id);
    fprintf(stdout, "            Name: %s\n", prop->name);
    fprintf(stdout, "            Flags: 0x%08x\n", prop->flags);
    fprintf(stdout, "            Values: %d: %s\n", prop->count_values, getList64(prop->values, prop->count_values));

    if (prop->flags & DRM_MODE_PROP_BLOB) {
		drmModePropertyBlobRes* blob;

		fprintf(stdout, "            BLOB: ");

		blob = drmModeGetPropertyBlob(fd, value);
		if (!blob) {
		   fprintf(stdout, "error getting blob %lu\n", value);
		} else {
		   fprintf(stdout, "%d bytes, %08X...\n", blob->length, *(uint32_t *)blob->data);
		   drmModeFreePropertyBlob(blob);
		}
    } else {
    	int i;

        fprintf(stdout, "            Enum: %lu\n", value);
		for (i = 0; i < prop->count_enums; i++) {
			fprintf(stdout, "              %1s %4lld = %s\n",
					prop->enums[i].value == value ? "*" : "",
					prop->enums[i].value, prop->enums[i].name);
		}
    }

    fprintf(stdout, "\n");
}

static void printProperties(int fd, uint32_t* props, uint64_t* values, int count) {
	drmModePropertyRes* prop;
	int i;

	for (i = 0; i < count; i++) {
		prop = drmModeGetProperty(fd, props[i]);
		printProperty(fd, prop, values[i]);
		drmModeFreeProperty(prop);
    }
}

static char* getConnectorType(uint32_t type) {
	switch (type) {
	case DRM_MODE_CONNECTOR_VGA:
		return "VGA";
	case DRM_MODE_CONNECTOR_DVII:
		return "DVII";
	case DRM_MODE_CONNECTOR_DVID:
		return "DVID";
	case DRM_MODE_CONNECTOR_DVIA:
		return "DVIA";
	case DRM_MODE_CONNECTOR_Composite:
		return "Composite";
	case DRM_MODE_CONNECTOR_SVIDEO:
		return "SVIDEO";
	case DRM_MODE_CONNECTOR_LVDS:
		return "LVDS";
	case DRM_MODE_CONNECTOR_Component:
		return "Component";
	case DRM_MODE_CONNECTOR_9PinDIN:
		return "9PinDIN";
	case DRM_MODE_CONNECTOR_DisplayPort:
		return "DisplayPort";
	case DRM_MODE_CONNECTOR_HDMIA:
		return "HDMIA";
	case DRM_MODE_CONNECTOR_HDMIB:
		return "HDMIB";
	case DRM_MODE_CONNECTOR_TV:
		return "TV";
	case DRM_MODE_CONNECTOR_eDP:
		return "eDP";
	default:
		return "Unknown";
	}
}

static char* getConnectorSubPixel(drmModeSubPixel subpixel) {
	switch (subpixel) {
	case DRM_MODE_SUBPIXEL_HORIZONTAL_RGB:
		return "HORIZONTAL_RGB";
	case DRM_MODE_SUBPIXEL_HORIZONTAL_BGR:
		return "HORIZONTAL_BGR";
	case DRM_MODE_SUBPIXEL_VERTICAL_RGB:
		return "VERTICAL_RGB";
	case DRM_MODE_SUBPIXEL_VERTICAL_BGR:
		return "VERTICAL_BGR";
	case DRM_MODE_SUBPIXEL_NONE:
		return "NONE";
	default:
		return "unknown";
	}
}

static char* getConnectorConnection(drmModeConnection connection) {
	switch (connection) {
	case DRM_MODE_CONNECTED:
		return "connected";
	case DRM_MODE_DISCONNECTED:
		return "disconnected";
	default:
		return "unknown";
	}
}

static char* getModeLine(drmModeModeInfo* mode) {
	static char buffer[512];
	char *p = buffer;

	p += snprintf(p, 512 - (p - buffer), "%10s: ", mode->name);
	p += snprintf(p, 512 - (p - buffer), "%4ux%4u (%3ufps), ", mode->hdisplay, mode->vdisplay, mode->vrefresh);
	p += snprintf(p, 512 - (p - buffer), "(%6ukHz, h:%4u-%4u %4u %2u, v:%4u-%4u %4u %2u), ", mode->clock,
			mode->hsync_start, mode->hsync_end, mode->htotal, mode->hskew,
			mode->vsync_start, mode->vsync_end, mode->vtotal, mode->vscan);
	p += snprintf(p, 512 - (p - buffer), "0x%08x, %5d", mode->flags, mode->type);

	return buffer;
}

static void printModes(drmModeModeInfo* modes, uint32_t count) {
	int i;

	for (i = 0; i < count; i++) {
		fprintf(stdout, "          %2d. %s\n", i, getModeLine(&modes[i]));
	}
}

static void printConnectors(int fd, uint32_t* ids, uint32_t count) {
	drmModeConnector *conn;
	int i;

	for (i = 0; i < count; i++) {
		conn = drmModeGetConnector(fd, ids[i]);
		if (!conn) {
			fprintf(stdout, "      Connector %u: cannot retrieve (%d): %m\n\n", ids[i], errno);
			continue;
		}

		fprintf(stdout, "      Connector %u:\n", conn->connector_id);
		fprintf(stdout, "        Type: %s\n", getConnectorType(conn->connector_type));
		fprintf(stdout, "        Dimensions: %umm x %umm\n", conn->mmWidth, conn->mmHeight);
		fprintf(stdout, "        SubPixel: %s\n", getConnectorSubPixel(conn->subpixel));
		fprintf(stdout, "        Connection: %s\n", getConnectorConnection(conn->connection));
		fprintf(stdout, "        Encoders: %d: %s\n", conn->count_encoders, getList32(conn->encoders, conn->count_encoders));
		fprintf(stdout, "        Properties: %d:\n", conn->count_props);
		printProperties(fd, conn->props, conn->prop_values, conn->count_props);
		fprintf(stdout, "        Modes: %d:\n", conn->count_modes);
		printModes(conn->modes, conn->count_modes);

		fprintf(stdout, "\n");

		drmModeFreeConnector(conn);
	}
}

 
static void printResources0(int fd, drmModeRes* res) {
	fprintf(stdout, "  Resources:\n");
	fprintf(stdout, "    Width: %u - %u\n", res->min_width, res->max_width);
	fprintf(stdout, "    Height: %u - %u\n", res->min_height, res->max_height);
	fprintf(stdout, "    Framebuffers: %d: %s\n", res->count_fbs, getList32(res->fbs, res->count_fbs));
	fprintf(stdout, "    CRTCs: %d: %s\n", res->count_crtcs, getList32(res->crtcs, res->count_crtcs));
	fprintf(stdout, "    Encoders: %d: %s\n", res->count_encoders, getList32(res->encoders, res->count_encoders));
	fprintf(stdout, "    Connectors: %d\n", res->count_connectors);
	printConnectors(fd, res->connectors, res->count_connectors);
}

static void printResources(int fd) {
	drmModeRes *res;

	res = drmModeGetResources(fd);
	if (!res) {
		fprintf(stdout, "cannot retrieve DRM resources (%d): %m\n", errno);
		return;
	}

	printResources0(fd, res);

	drmModeFreeResources(res);
}

static void drminfo(char* device) {
	int fd;

	fd = open(device, O_RDONLY);

	fprintf(stdout, "Device %s:\n", device);
	printCapabilities(fd);
	fprintf(stdout, "\n");
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

