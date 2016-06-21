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

#include <lv2lint.h>

enum {
	DEFAULT_NOT_FOUND,
	DEFAULT_NOT_AN_INT,
	DEFAULT_NOT_A_FLOAT,
	DEFAULT_NOT_A_BOOL,
};

static const ret_t ret_default [] = {
	[DEFAULT_NOT_FOUND]      = {LINT_WARN, "lv2:default not found", LV2_CORE__Port},
		[DEFAULT_NOT_AN_INT]   = {LINT_WARN, "lv2:default not an integer", LV2_CORE__default},
		[DEFAULT_NOT_A_FLOAT]  = {LINT_WARN, "lv2:default not a float", LV2_CORE__default},
		[DEFAULT_NOT_A_BOOL]   = {LINT_WARN, "lv2:default not a bool", LV2_CORE__default},
};

static const ret_t *
_test_default(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *lv2_ControlPort = lilv_new_uri(app->world, LV2_CORE__ControlPort);
	LilvNode *lv2_default = lilv_new_uri(app->world, LV2_CORE__default);
	LilvNode *lv2_integer = lilv_new_uri(app->world, LV2_CORE__integer);
	LilvNode *lv2_toggled = lilv_new_uri(app->world, LV2_CORE__toggled);
	const bool is_integer = lilv_port_has_property(app->plugin, app->port, lv2_integer);
	const bool is_toggled = lilv_port_has_property(app->plugin, app->port, lv2_toggled);

	if(lilv_port_is_a(app->plugin, app->port, lv2_ControlPort))
	{
		LilvNode *default_node = lilv_port_get(app->plugin, app->port, lv2_default);
		if(default_node)
		{
			if(is_integer && !lilv_node_is_int(default_node))
			{
				ret = &ret_default[DEFAULT_NOT_AN_INT];
			}
			else if(is_toggled && !lilv_node_is_bool(default_node))
			{
				ret = &ret_default[DEFAULT_NOT_A_BOOL];
			}
			else if(!lilv_node_is_float(default_node))
			{
				ret = &ret_default[DEFAULT_NOT_A_FLOAT];
			}

			lilv_node_free(default_node);
		}
		else // !default_node
		{
			ret = &ret_default[DEFAULT_NOT_FOUND];
		}
	}

	lilv_node_free(lv2_ControlPort);
	lilv_node_free(lv2_default);
	lilv_node_free(lv2_integer);
	lilv_node_free(lv2_toggled);

	return ret;
}

enum {
	MINIMUM_NOT_FOUND,
	MINIMUM_NOT_AN_INT,
	MINIMUM_NOT_A_FLOAT,
	MINIMUM_NOT_A_BOOL,
};

static const ret_t ret_minimum [] = {
	[MINIMUM_NOT_FOUND]      = {LINT_WARN, "lv2:minimum not found", LV2_CORE__Port},
		[MINIMUM_NOT_AN_INT]   = {LINT_WARN, "lv2:minimum not an integer", LV2_CORE__minimum},
		[MINIMUM_NOT_A_FLOAT]  = {LINT_WARN, "lv2:minimum not a float", LV2_CORE__minimum},
		[MINIMUM_NOT_A_BOOL]   = {LINT_WARN, "lv2:minimum not a bool", LV2_CORE__minimum},
};

static const ret_t *
_test_minimum(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *lv2_ControlPort = lilv_new_uri(app->world, LV2_CORE__ControlPort);
	LilvNode *lv2_minimum = lilv_new_uri(app->world, LV2_CORE__minimum);
	LilvNode *lv2_integer = lilv_new_uri(app->world, LV2_CORE__integer);
	LilvNode *lv2_toggled = lilv_new_uri(app->world, LV2_CORE__toggled);
	const bool is_integer = lilv_port_has_property(app->plugin, app->port, lv2_integer);
	const bool is_toggled = lilv_port_has_property(app->plugin, app->port, lv2_toggled);

	if(lilv_port_is_a(app->plugin, app->port, lv2_ControlPort))
	{
		LilvNode *minimum_node = lilv_port_get(app->plugin, app->port, lv2_minimum);
		if(minimum_node)
		{
			if(is_integer && !lilv_node_is_int(minimum_node))
			{
				ret = &ret_minimum[MINIMUM_NOT_AN_INT];
			}
			else if(is_toggled && !lilv_node_is_bool(minimum_node))
			{
				ret = &ret_minimum[MINIMUM_NOT_A_BOOL];
			}
			else if(!lilv_node_is_float(minimum_node))
			{
				ret = &ret_minimum[MINIMUM_NOT_A_FLOAT];
			}

			lilv_node_free(minimum_node);
		}
		else // !minimum_node
		{
			ret = &ret_minimum[MINIMUM_NOT_FOUND];
		}
	}

	lilv_node_free(lv2_ControlPort);
	lilv_node_free(lv2_minimum);
	lilv_node_free(lv2_integer);
	lilv_node_free(lv2_toggled);

	return ret;
}

enum {
	MAXIMUM_NOT_FOUND,
	MAXIMUM_NOT_AN_INT,
	MAXIMUM_NOT_A_FLOAT,
	MAXIMUM_NOT_A_BOOL,
};

static const ret_t ret_maximum [] = {
	[MAXIMUM_NOT_FOUND]      = {LINT_WARN, "lv2:maximum not found", LV2_CORE__Port},
		[MAXIMUM_NOT_AN_INT]   = {LINT_WARN, "lv2:maximum not an integer", LV2_CORE__maximum},
		[MAXIMUM_NOT_A_FLOAT]  = {LINT_WARN, "lv2:maximum not a float", LV2_CORE__maximum},
		[MAXIMUM_NOT_A_BOOL]   = {LINT_WARN, "lv2:maximum not a bool", LV2_CORE__maximum},
};

static const ret_t *
_test_maximum(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *lv2_ControlPort = lilv_new_uri(app->world, LV2_CORE__ControlPort);
	LilvNode *lv2_maximum = lilv_new_uri(app->world, LV2_CORE__maximum);
	LilvNode *lv2_integer = lilv_new_uri(app->world, LV2_CORE__integer);
	LilvNode *lv2_toggled = lilv_new_uri(app->world, LV2_CORE__toggled);
	const bool is_integer = lilv_port_has_property(app->plugin, app->port, lv2_integer);
	const bool is_toggled = lilv_port_has_property(app->plugin, app->port, lv2_toggled);

	if(lilv_port_is_a(app->plugin, app->port, lv2_ControlPort))
	{
		LilvNode *maximum_node = lilv_port_get(app->plugin, app->port, lv2_maximum);
		if(maximum_node)
		{
			if(is_integer && !lilv_node_is_int(maximum_node))
			{
				ret = &ret_maximum[MAXIMUM_NOT_AN_INT];
			}
			else if(is_toggled && !lilv_node_is_bool(maximum_node))
			{
				ret = &ret_maximum[MAXIMUM_NOT_A_BOOL];
			}
			else if(!lilv_node_is_float(maximum_node))
			{
				ret = &ret_maximum[MAXIMUM_NOT_A_FLOAT];
			}

			lilv_node_free(maximum_node);
		}
		else // !maximum_node
		{
			ret = &ret_maximum[MAXIMUM_NOT_FOUND];
		}
	}

	lilv_node_free(lv2_ControlPort);
	lilv_node_free(lv2_maximum);
	lilv_node_free(lv2_integer);
	lilv_node_free(lv2_toggled);

	return ret;
}

static const test_t tests [] = {
	{"Default         ", _test_default},
	{"Minimum         ", _test_minimum},
	{"Maximum         ", _test_maximum},

	{NULL, NULL}
};

bool
test_port(app_t *app)
{
	bool flag = true;

	for(const test_t *test = tests; test->id && test->cb; test++)
	{
		const ret_t *ret = test->cb(app);

		if(ret)
		{
			switch(ret->lint)
			{
				case LINT_FAIL:
					fprintf(stdout, "    ["ANSI_COLOR_RED"FAIL"ANSI_COLOR_RESET"]  %s=> %s <%s>\n", test->id, ret->msg, ret->url);
					flag = false;
					break;
				case LINT_WARN:
					fprintf(stdout, "    ["ANSI_COLOR_YELLOW"WARN"ANSI_COLOR_RESET"]  %s=> %s <%s>\n", test->id, ret->msg, ret->url);
					break;
				case LINT_NOTE:
					fprintf(stdout, "    ["ANSI_COLOR_CYAN"NOTE"ANSI_COLOR_RESET"]  %s=> %s <%s>\n", test->id, ret->msg, ret->url);
					break;
			}
		}
		else
		{
			fprintf(stdout, "    ["ANSI_COLOR_GREEN"PASS"ANSI_COLOR_RESET"]  %s\n", test->id);
		}
	}

	return flag;
}
