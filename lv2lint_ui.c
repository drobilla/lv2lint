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
#include <string.h>

#ifdef _WIN32
#	include <windows.h>
#	define dlopen(path, flags) LoadLibrary(path)
#	define dlclose(lib) FreeLibrary((HMODULE)lib)
#	define inline __inline
#	define snprintf _snprintf
static inline char* dlerror(void) { return "Unknown error"; }
#else
#	include <dlfcn.h>
#endif

#include <lv2lint.h>

#include <lv2/lv2plug.in/ns/ext/instance-access/instance-access.h>
#include <lv2/lv2plug.in/ns/ext/data-access/data-access.h>

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
	IDLE_EXTENSION_MISSING,
	IDLE_EXTENSION_NOT_RETURNED,
};

static const ret_t ret_idle [] = {
	[IDLE_FEATURE_MISSING]         = {LINT_WARN, "lv2:feature ui:idleInterface missing", LV2_UI__idleInterface},
	[IDLE_EXTENSION_MISSING]       = {LINT_FAIL, "lv2:extensionData ui:idleInterface missing", LV2_UI__idleInterface},
	[IDLE_EXTENSION_NOT_RETURNED]  = {LINT_FAIL, "ui:idleInterface not returned by 'extention_data'", LV2_UI__idleInterface},
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

	if( (has_idle_extension || app->ui_idle_iface) && !has_idle_feature)
	{
		ret = &ret_idle[IDLE_FEATURE_MISSING];
	}
	else if( (has_idle_feature || app->ui_idle_iface) && !has_idle_extension)
	{
		ret = &ret_idle[IDLE_EXTENSION_MISSING];
	}
	else if( (has_idle_extension || has_idle_feature) && !app->ui_idle_iface)
	{
		ret = &ret_idle[IDLE_EXTENSION_NOT_RETURNED];
	}

	return ret;
}

enum {
	SHOW_EXTENSION_MISSING,
	SHOW_EXTENSION_NOT_RETURNED,
};

static const ret_t ret_show [] = {
	[SHOW_EXTENSION_MISSING]       = {LINT_FAIL, "lv2:extensionData ui:showInterface missing", LV2_UI__showInterface},
	[SHOW_EXTENSION_NOT_RETURNED]  = {LINT_FAIL, "ui:showInterface not returned by 'extention_data'", LV2_UI__showInterface},
};

static const ret_t *
_test_show_interface(app_t *app)
{
	const ret_t *ret = NULL;

	const bool has_show_extension = lilv_world_ask(app->world,
		lilv_ui_get_uri(app->ui), app->uris.lv2_extensionData, app->uris.ui_showInterface);

	if(app->ui_show_iface && !has_show_extension)
	{
		ret = &ret_show[SHOW_EXTENSION_MISSING];
	}
	else if(has_show_extension && !app->ui_show_iface)
	{
		ret = &ret_show[SHOW_EXTENSION_NOT_RETURNED];
	}

	return ret;
}

enum {
	RESIZE_EXTENSION_MISSING,
	RESIZE_EXTENSION_NOT_RETURNED,
};

static const ret_t ret_resize [] = {
	[RESIZE_EXTENSION_MISSING]       = {LINT_FAIL, "lv2:extensionData ui:resize missing", LV2_UI__resize},
	[RESIZE_EXTENSION_NOT_RETURNED]  = {LINT_FAIL, "ui:resize not returned by 'extention_data'", LV2_UI__resize},
};

static const ret_t *
_test_resize_interface(app_t *app)
{
	const ret_t *ret = NULL;

	const bool has_resize_extension = lilv_world_ask(app->world,
		lilv_ui_get_uri(app->ui), app->uris.lv2_extensionData, app->uris.ui_resize);

	if(app->ui_resize_iface && !has_resize_extension)
	{
		ret = &ret_resize[RESIZE_EXTENSION_MISSING];
	}
	else if(has_resize_extension && !app->ui_resize_iface)
	{
		ret = &ret_resize[RESIZE_EXTENSION_NOT_RETURNED];
	}

	return ret;
}

static const test_t tests [] = {
	{"Instance Access ", _test_instance_access},
	{"Data Access     ", _test_data_access},
	{"Mixed DSP/UI    ", _test_mixed},
	//{"UI Binary       ", _test_binary}, FIXME lilv does not support lv2:binary for UIs, yet
	{"UI SO Name      ", _test_resident},
	{"Idle Interface  ", _test_idle_interface},
	{"Show Interface  ", _test_show_interface},
	{"Resize Interface", _test_resize_interface},
};

static const int tests_n = sizeof(tests) / sizeof(test_t);

bool
test_ui(app_t *app)
{
	const bool atty = isatty(1);
	bool flag = true;
	bool msg = false;
	res_t rets [tests_n];

	void *lib = NULL;
	const LV2UI_Descriptor *descriptor = NULL;

	{
		const LilvNode *ui_uri_node = lilv_ui_get_uri(app->ui);
		const LilvNode *ui_binary_node = lilv_ui_get_binary_uri(app->ui);

		const char *ui_uri = lilv_node_as_uri(ui_uri_node);
		const char *ui_binary_uri = lilv_node_as_uri(ui_binary_node);
		char *ui_binary_path = lilv_file_uri_parse(ui_binary_uri, NULL);

		dlerror();

		lib = dlopen(ui_binary_path, RTLD_NOW);
		if(!lib)
		{
			fprintf(stderr, "Unable to open UI library %s (%s)\n", ui_binary_path, dlerror());
			return -1;
		}

		// Get discovery function
#ifdef _WIN32
		LV2UI_DescriptorFunction df = GetProcAddress(lib, "lv2ui_descriptor");
#else
		LV2UI_DescriptorFunction df = dlsym(lib, "lv2ui_descriptor");
#endif
		if(!df)
		{
			fprintf(stderr, "Broken LV2 UI %s (no lv2ui_descriptor symbol found)\n", ui_binary_path);
			dlclose(lib);
			return -1;
		}

		// Get UI descriptor
		for(uint32_t i = 0; ; i++)
		{
			const LV2UI_Descriptor *ld = df(i);
			if(!ld)
			{
				break; // sentinel
			}
			else if(!strcmp(ld->URI, ui_uri))
			{
				descriptor = ld;
				break;
			}
		}

		if(!descriptor)
		{
			fprintf(stderr, "Failed to find descriptor for <%s> in %s\n", ui_uri, ui_binary_path);
			dlclose(lib);
			return -1;
		}

		if(ui_binary_path)
			lilv_free(ui_binary_path);

		app->ui_idle_iface = descriptor->extension_data
			? descriptor->extension_data(LV2_UI__idleInterface)
			: NULL;
		app->ui_show_iface = descriptor->extension_data
			? descriptor->extension_data(LV2_UI__showInterface)
			: NULL;
		app->ui_resize_iface = descriptor->extension_data
			? descriptor->extension_data(LV2_UI__resize)
			: NULL;
	}

	for(unsigned i=0; i<tests_n; i++)
	{
		const test_t *test = &tests[i];
		res_t *res = &rets[i];

		res->urn = NULL;
		app->urn = &res->urn;
		res->ret = test->cb(app);
		if(res->ret && (res->ret->lint & app->show) )
			msg = true;
	}

	app->ui_idle_iface = NULL;
	app->ui_show_iface = NULL;
	app->ui_resize_iface = NULL;

	if(msg)
	{
		fprintf(stdout, "  %s<%s>%s\n",
			colors[atty][ANSI_COLOR_BOLD],
			lilv_node_as_uri(lilv_ui_get_uri(app->ui)),
			colors[atty][ANSI_COLOR_RESET]);

		for(unsigned i=0; i<tests_n; i++)
		{
			const test_t *test = &tests[i];
			res_t *res = &rets[i];
			const ret_t *ret = res->ret;

			if(ret)
			{
				char *repl = NULL;

				if(res->urn)
				{
					if(strstr(ret->msg, "%s"))
					{
						if(asprintf(&repl, ret->msg, res->urn) == -1)
							repl = NULL;
					}

					free(res->urn);
				}

				switch(ret->lint & app->show)
				{
					case LINT_FAIL:
						fprintf(stdout, "    [%sFAIL%s]  %s=> %s <%s>\n",
							colors[atty][ANSI_COLOR_RED], colors[atty][ANSI_COLOR_RESET],
							test->id, repl ? repl : ret->msg, ret->url);
						break;
					case LINT_WARN:
						fprintf(stdout, "    [%sWARN%s]  %s=> %s <%s>\n",
							colors[atty][ANSI_COLOR_YELLOW], colors[atty][ANSI_COLOR_RESET],
							test->id, repl ? repl : ret->msg, ret->url);
						break;
					case LINT_NOTE:
						fprintf(stdout, "    [%sNOTE%s]  %s=> %s <%s>\n",
							colors[atty][ANSI_COLOR_CYAN], colors[atty][ANSI_COLOR_RESET],
							test->id, repl ? repl : ret->msg, ret->url);
						break;
				}

				if(repl)
					free(repl);

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

	if(lib)
	{
		dlclose(lib);
		lib = NULL;
	}

	return flag;
}
