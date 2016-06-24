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

#include <math.h>

#include <lv2lint.h>

#include <lv2/lv2plug.in/ns/ext/instance-access/instance-access.h>
#include <lv2/lv2plug.in/ns/ext/data-access/data-access.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

enum {
	INSTANCE_ACCESS_DISCOURAGED,
};

static const ret_t ret_instance_access [] = {
	[INSTANCE_ACCESS_DISCOURAGED]         = {LINT_WARN, "usage of instance-access is highly discouraged", LV2_INSTANCE_ACCESS_URI},
};

static const ret_t *
_test_instance_access(app_t *app)
{
	const ret_t *ret = NULL;

	const LilvNode *uri = lilv_ui_get_uri(app->ui);
	LilvNode *instance_access = lilv_new_uri(app->world, LV2_INSTANCE_ACCESS_URI);
	LilvNode *lv2_requiredFeature = lilv_new_uri(app->world, LV2_CORE__requiredFeature);

	const bool needs_instance_access = lilv_world_ask(app->world, uri,
		lv2_requiredFeature, instance_access);

	if(needs_instance_access)
	{
		ret = &ret_instance_access[INSTANCE_ACCESS_DISCOURAGED];
	}

	lilv_node_free(instance_access);
	lilv_node_free(lv2_requiredFeature);

	return ret;
}

enum {
	DATA_ACCESS_DISCOURAGED,
};

static const ret_t ret_data_access [] = {
	[DATA_ACCESS_DISCOURAGED]         = {LINT_WARN, "usage of data-access is highly discouraged", LV2_DATA_ACCESS_URI},
};

static const ret_t *
_test_data_access(app_t *app)
{
	const ret_t *ret = NULL;

	const LilvNode *uri = lilv_ui_get_uri(app->ui);
	LilvNode *data_access = lilv_new_uri(app->world, LV2_DATA_ACCESS_URI);
	LilvNode *lv2_requiredFeature = lilv_new_uri(app->world, LV2_CORE__requiredFeature);

	const bool needs_data_access = lilv_world_ask(app->world, uri,
		lv2_requiredFeature, data_access);

	if(needs_data_access)
	{
		ret = &ret_data_access[DATA_ACCESS_DISCOURAGED];
	}

	lilv_node_free(data_access);
	lilv_node_free(lv2_requiredFeature);

	return ret;
}

enum {
	MIXED_NOT_VALID,
};

static const ret_t ret_mixed [] = {
	[MIXED_NOT_VALID]         = {LINT_WARN, "mixing DSP and UI code in same binary is discouraged", LV2_UI_PREFIX},
};

static const ret_t *
_test_mixed(app_t *app)
{
	const ret_t *ret = NULL;

	const LilvNode *library_uri = lilv_plugin_get_library_uri(app->plugin);

	const LilvNode *ui_library_uri = lilv_ui_get_binary_uri(app->ui);
	if(ui_library_uri && lilv_node_equals(library_uri, ui_library_uri))
	{
		ret = &ret_mixed[MIXED_NOT_VALID];
	}

	return ret;
}

static const test_t tests [] = {
	{"Instance Access ", _test_instance_access},
	{"Data Access     ", _test_data_access},
	{"Mixed DSP/UI    ", _test_mixed},
};

static const int tests_n = sizeof(tests) / sizeof(test_t);

bool
test_ui(app_t *app)
{
	bool flag = true;
	bool msg = false;
	const ret_t *rets [tests_n];

	for(unsigned i=0; i<tests_n; i++)
	{
		const test_t *test = &tests[i];
		rets[i] = test->cb(app);
		if(rets[i])
			msg = true;
	}

	if(msg)
	{
		fprintf(stdout, "  "ANSI_COLOR_BOLD"<%s>"ANSI_COLOR_RESET"\n",
			lilv_node_as_uri(lilv_ui_get_uri(app->ui)));

		for(unsigned i=0; i<tests_n; i++)
		{
			const test_t *test = &tests[i];
			const ret_t *ret = rets[i];

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
				//fprintf(stdout, "    ["ANSI_COLOR_GREEN"PASS"ANSI_COLOR_RESET"]  %s\n", test->id);
			}
		}
	}

	return flag;
}
