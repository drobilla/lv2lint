/*
 * Copyright (c) 2016-2017 Hanspeter Portner (dev@open-music-kontrollers.ch)
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

	const bool needs_instance_access = lilv_world_ask(app->world, uri,
		app->uris.lv2_requiredFeature, app->uris.instance_access);

	if(needs_instance_access)
	{
		ret = &ret_instance_access[INSTANCE_ACCESS_DISCOURAGED];
	}

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

	const bool needs_data_access = lilv_world_ask(app->world, uri,
		app->uris.lv2_requiredFeature, app->uris.data_access);

	if(needs_data_access)
	{
		ret = &ret_data_access[DATA_ACCESS_DISCOURAGED];
	}

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

enum {
	BINARY_DEPRECATED,
};

static const ret_t ret_binary [] = {
	[BINARY_DEPRECATED]         = {LINT_FAIL, "ui:binary is deprecated, use lv2:binary instead", LV2_UI__binary},
};

static const ret_t *
_test_binary(app_t *app)
{
	const ret_t *ret = NULL;

	const bool has_ui_binary = lilv_world_ask(app->world,
		lilv_ui_get_uri(app->ui), app->uris.ui_binary, NULL);

	if(has_ui_binary)
	{
		ret = &ret_binary[BINARY_DEPRECATED];
	}

	return ret;
}

enum {
	RESIDENT_DEPRECATED,
};

static const ret_t ret_resident [] = {
	[RESIDENT_DEPRECATED]         = {LINT_FAIL, "ui:makeSONameResident is deprecated", LV2_UI_PREFIX"makeSONameResident"},
};

static const ret_t *
_test_resident(app_t *app)
{
	const ret_t *ret = NULL;

	const bool has_ui_resident = lilv_world_ask(app->world,
		lilv_ui_get_uri(app->ui), app->uris.ui_makeSONameResident, NULL);

	if(has_ui_resident)
	{
		ret = &ret_resident[RESIDENT_DEPRECATED];
	}

	return ret;
}

enum {
	IDLE_FEATURE_MISSING,
	IDLE_EXTENSION_MISSING
};

static const ret_t ret_idle [] = {
	[IDLE_FEATURE_MISSING]         = {LINT_WARN, "lv2:feature ui:idleInterface missing", LV2_UI__idleInterface},
	[IDLE_EXTENSION_MISSING]       = {LINT_FAIL, "lv2:extensionData ui:idleInterface missing", LV2_UI__idleInterface},
};

static const ret_t *
_test_idle_interface(app_t *app)
{
	const ret_t *ret = NULL;

	const bool has_idle_feature = lilv_world_ask(app->world,
			lilv_ui_get_uri(app->ui), app->uris.lv2_optionalFeature, app->uris.ui_idleInterface)
		|| lilv_world_ask(app->world,
			lilv_ui_get_uri(app->ui), app->uris.lv2_requiredFeature, app->uris.ui_idleInterface);
	const bool has_idle_extension = lilv_world_ask(app->world,
		lilv_ui_get_uri(app->ui), app->uris.lv2_extensionData, app->uris.ui_idleInterface);

	if(has_idle_feature && !has_idle_extension)
	{
		ret = &ret_idle[IDLE_EXTENSION_MISSING];
	}
	else if(!has_idle_feature && has_idle_extension)
	{
		ret = &ret_idle[IDLE_FEATURE_MISSING];
	}
	//FIXME check for presence of extensionData in binary

	return ret;
}

static const test_t tests [] = {
	{"Instance Access ", _test_instance_access},
	{"Data Access     ", _test_data_access},
	{"Mixed DSP/UI    ", _test_mixed},
	//{"UI Binary       ", _test_binary}, FIXME lilv does not support lv2:binary for UIs, yet
	{"UI SO Name      ", _test_resident},
	{"Idle Interface  ", _test_idle_interface},
};

static const int tests_n = sizeof(tests) / sizeof(test_t);

bool
test_ui(app_t *app)
{
	const bool atty = isatty(1);
	bool flag = true;
	bool msg = false;
	const ret_t *rets [tests_n];

	for(unsigned i=0; i<tests_n; i++)
	{
		const test_t *test = &tests[i];
		rets[i] = test->cb(app);
		if(rets[i] && (rets[i]->lint & app->show) )
			msg = true;
	}

	if(msg)
	{
		fprintf(stdout, "  %s<%s>%s\n",
			colors[atty][ANSI_COLOR_BOLD],
			lilv_node_as_uri(lilv_ui_get_uri(app->ui)),
			colors[atty][ANSI_COLOR_RESET]);

		for(unsigned i=0; i<tests_n; i++)
		{
			const test_t *test = &tests[i];
			const ret_t *ret = rets[i];

			if(ret)
			{
				switch(ret->lint & app->show)
				{
					case LINT_FAIL:
						fprintf(stdout, "    [%sFAIL%s]  %s=> %s <%s>\n",
							colors[atty][ANSI_COLOR_RED], colors[atty][ANSI_COLOR_RESET],
							test->id, ret->msg, ret->url);
						break;
					case LINT_WARN:
						fprintf(stdout, "    [%sWARN%s]  %s=> %s <%s>\n",
							colors[atty][ANSI_COLOR_YELLOW], colors[atty][ANSI_COLOR_RESET],
							test->id, ret->msg, ret->url);
						break;
					case LINT_NOTE:
						fprintf(stdout, "    [%sNOTE%s]  %s=> %s <%s>\n",
							colors[atty][ANSI_COLOR_CYAN], colors[atty][ANSI_COLOR_RESET],
							test->id, ret->msg, ret->url);
						break;
				}

				if(flag)
					flag = (ret->lint & app->mask) ? false : true;
			}
			else
			{
				/*
				fprintf(stdout, "    [%sPASS%s]  %s\n",
					colors[atty][ANSI_COLOR_GREEN], colors[atty][ANSI_COLOR_RESET],
					test->id);
				*/
			}
		}
	}

	return flag;
}
