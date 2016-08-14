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

#define ANSI_COLOR_BOLD    "\x1b[1m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum _lint_t lint_t;
typedef struct _app_t app_t;
typedef struct _test_t test_t;
typedef struct _ret_t ret_t;
typedef const ret_t *(*test_cb_t)(app_t *app);

enum _lint_t {
	LINT_NOTE     = (1 << 1),
	LINT_WARN     = (1 << 2),
	LINT_FAIL     = (1 << 3)
};

struct _ret_t {
	lint_t lint;
	const char *msg;
	const char *url;
};

struct _app_t {
	LilvWorld *world;
	const LilvPlugin *plugin;
	const LilvPort *port;
	const LilvNode *parameter;
	const LilvUI *ui;
	lint_t show;
	lint_t mask;
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
