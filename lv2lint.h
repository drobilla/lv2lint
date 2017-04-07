/*
 * Copyright (c) 2016 Hanspeter Portner (dev@open-music-kontrollers.ch)
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the Artistic License 2.0 as published by
 * The Perl Foundation.
 *
 * This source is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Artistic License 2.0 for more details.
 *
 * You should have received a copy of the Artistic License 2.0
 * along the source as a COPYING file. If not, obtain it from
 * http://www.perlfoundation.org/artistic_license_2_0.
 */

#ifndef _LV2LINT_H
#define _LV2LINT_H

#include <lilv/lilv.h>

#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>

#define ANSI_COLOR_BOLD    "\x1b[1m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define MAX_URIDS 8192

typedef enum _lint_t lint_t;
typedef struct _urid_t urid_t;
typedef struct _app_t app_t;
typedef struct _test_t test_t;
typedef struct _ret_t ret_t;
typedef const ret_t *(*test_cb_t)(app_t *app);

enum _lint_t {
	LINT_NOTE     = (1 << 1),
	LINT_WARN     = (1 << 2),
	LINT_FAIL     = (1 << 3)
};

struct _urid_t {
	LV2_URID urid;
	char *uri;
};

struct _ret_t {
	lint_t lint;
	const char *msg;
	const char *url;
};

struct _app_t {
	LilvWorld *world;
	const LilvPlugin *plugin;
	LilvInstance *instance;
	const LilvPort *port;
	const LilvNode *parameter;
	const LilvUI *ui;
	const LV2_Worker_Interface *work_iface;
	const LV2_State_Interface *state_iface;
	lint_t show;
	lint_t mask;
	urid_t urids [MAX_URIDS];
	LV2_URID urid;
	struct {
		LilvNode *rdfs_label;
		LilvNode *rdfs_comment;
		LilvNode *rdfs_range;
		LilvNode *rdfs_subClassOf;

		LilvNode *rdf_type;

		LilvNode *doap_description;
		LilvNode *doap_license;
		LilvNode *doap_name;
		LilvNode *doap_shortdesc;

		LilvNode *lv2_minimum;
		LilvNode *lv2_maximum;
		LilvNode *lv2_Port;
		LilvNode *lv2_PortProperty;
		LilvNode *lv2_default;
		LilvNode *lv2_ControlPort;
		LilvNode *lv2_CVPort;
		LilvNode *lv2_InputPort;
		LilvNode *lv2_integer;
		LilvNode *lv2_toggled;
		LilvNode *lv2_Feature;
		LilvNode *lv2_minorVersion;
		LilvNode *lv2_microVersion;
		LilvNode *lv2_ExtensionData;
		LilvNode *lv2_requiredFeature;

		LilvNode *atom_Bool;
		LilvNode *atom_Int;
		LilvNode *atom_Long;
		LilvNode *atom_Float;
		LilvNode *atom_Double;
		LilvNode *atom_String;
		LilvNode *atom_Literal;
		LilvNode *atom_Path;
		LilvNode *atom_Chunk;
		LilvNode *atom_URI;
		LilvNode *atom_URID;

		LilvNode *state_loadDefaultState;
		LilvNode *state_state;
		LilvNode *state_interface;

		LilvNode *work_schedule;
		LilvNode *work_interface;

		LilvNode *patch_writable;
		LilvNode *patch_readable;

		LilvNode *pg_group;

		LilvNode *ui_binary;
		LilvNode *ui_makeSONameResident;

		LilvNode *event_EventPort;
		LilvNode *uri_map;
		LilvNode *instance_access;
		LilvNode *data_access;
	} uris;
};

struct _test_t {
	const char *id;
	test_cb_t cb;
};

bool
test_plugin(app_t *app);

bool
test_port(app_t *app);

bool
test_parameter(app_t *app);

bool
test_ui(app_t *app);

#endif
