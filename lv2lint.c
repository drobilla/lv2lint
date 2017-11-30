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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <lv2lint.h>

#include <lv2/lv2plug.in/ns/ext/patch/patch.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/port-groups/port-groups.h>
#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>
#include <lv2/lv2plug.in/ns/ext/instance-access/instance-access.h>
#include <lv2/lv2plug.in/ns/ext/parameters/parameters.h>
#include <lv2/lv2plug.in/ns/ext/port-props/port-props.h>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include <lv2/lv2plug.in/ns/ext/resize-port/resize-port.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>
#include <lv2/lv2plug.in/ns/ext/data-access/data-access.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

#ifdef ENABLE_ONLINE_TESTS
#	include <curl/curl.h>
#endif

const char *colors [2][ANSI_COLOR_MAX] = {
	{
		[ANSI_COLOR_BOLD]    = "",
		[ANSI_COLOR_RED]     = "",
		[ANSI_COLOR_GREEN]   = "",
		[ANSI_COLOR_YELLOW]  = "",
		[ANSI_COLOR_BLUE]    = "",
		[ANSI_COLOR_MAGENTA] = "",
		[ANSI_COLOR_CYAN]    = "",
		[ANSI_COLOR_RESET]   = ""
	},
	{
		[ANSI_COLOR_BOLD]    = "\x1b[1m",
		[ANSI_COLOR_RED]     = "\x1b[31m",
		[ANSI_COLOR_GREEN]   = "\x1b[32m",
		[ANSI_COLOR_YELLOW]  = "\x1b[33m",
		[ANSI_COLOR_BLUE]    = "\x1b[34m",
		[ANSI_COLOR_MAGENTA] = "\x1b[35m",
		[ANSI_COLOR_CYAN]    = "\x1b[36m",
		[ANSI_COLOR_RESET]   = "\x1b[0m"
	}
};

static void
_map_uris(app_t *app)
{
	app->uris.rdfs_label = lilv_new_uri(app->world, LILV_NS_RDFS"label");
	app->uris.rdfs_comment = lilv_new_uri(app->world, LILV_NS_RDFS"comment");
	app->uris.rdfs_range = lilv_new_uri(app->world, LILV_NS_RDFS"range");
	app->uris.rdfs_subClassOf = lilv_new_uri(app->world, LILV_NS_RDFS"subClassOf");

	app->uris.rdf_type = lilv_new_uri(app->world, LILV_NS_RDF"type");

	app->uris.doap_description = lilv_new_uri(app->world, LILV_NS_DOAP"description");
	app->uris.doap_license = lilv_new_uri(app->world, LILV_NS_DOAP"license");
	app->uris.doap_name = lilv_new_uri(app->world, LILV_NS_DOAP"name");
	app->uris.doap_shortdesc = lilv_new_uri(app->world, LILV_NS_DOAP"shortdesc");

	app->uris.lv2_minimum = lilv_new_uri(app->world, LV2_CORE__minimum);
	app->uris.lv2_maximum = lilv_new_uri(app->world, LV2_CORE__maximum);
	app->uris.lv2_Port = lilv_new_uri(app->world, LV2_CORE__Port);
	app->uris.lv2_PortProperty = lilv_new_uri(app->world, LV2_CORE__PortProperty);
	app->uris.lv2_default = lilv_new_uri(app->world, LV2_CORE__default);
	app->uris.lv2_ControlPort = lilv_new_uri(app->world, LV2_CORE__ControlPort);
	app->uris.lv2_CVPort = lilv_new_uri(app->world, LV2_CORE__CVPort);
	app->uris.lv2_InputPort = lilv_new_uri(app->world, LV2_CORE__InputPort);
	app->uris.lv2_integer = lilv_new_uri(app->world, LV2_CORE__integer);
	app->uris.lv2_toggled = lilv_new_uri(app->world, LV2_CORE__toggled);
	app->uris.lv2_Feature = lilv_new_uri(app->world, LV2_CORE__Feature);
	app->uris.lv2_minorVersion = lilv_new_uri(app->world, LV2_CORE__minorVersion);
	app->uris.lv2_microVersion = lilv_new_uri(app->world, LV2_CORE__microVersion);
	app->uris.lv2_ExtensionData = lilv_new_uri(app->world, LV2_CORE__ExtensionData);
	app->uris.lv2_requiredFeature = lilv_new_uri(app->world, LV2_CORE__requiredFeature);
	app->uris.lv2_optionalFeature = lilv_new_uri(app->world, LV2_CORE__optionalFeature);
	app->uris.lv2_extensionData = lilv_new_uri(app->world, LV2_CORE__extensionData);
	app->uris.lv2_isLive = lilv_new_uri(app->world, LV2_CORE__isLive);
	app->uris.lv2_inPlaceBroken = lilv_new_uri(app->world, LV2_CORE__inPlaceBroken);
	app->uris.lv2_hardRTCapable = lilv_new_uri(app->world, LV2_CORE__hardRTCapable);

	app->uris.atom_Bool = lilv_new_uri(app->world, LV2_ATOM__Bool);
	app->uris.atom_Int = lilv_new_uri(app->world, LV2_ATOM__Int);
	app->uris.atom_Long = lilv_new_uri(app->world, LV2_ATOM__Long);
	app->uris.atom_Float = lilv_new_uri(app->world, LV2_ATOM__Float);
	app->uris.atom_Double = lilv_new_uri(app->world, LV2_ATOM__Double);
	app->uris.atom_String = lilv_new_uri(app->world, LV2_ATOM__String);
	app->uris.atom_Literal = lilv_new_uri(app->world, LV2_ATOM__String);
	app->uris.atom_Path = lilv_new_uri(app->world, LV2_ATOM__Path);
	app->uris.atom_Chunk = lilv_new_uri(app->world, LV2_ATOM__Chunk);
	app->uris.atom_URI = lilv_new_uri(app->world, LV2_ATOM__URI);
	app->uris.atom_URID = lilv_new_uri(app->world, LV2_ATOM__URID);
	app->uris.atom_Tuple = lilv_new_uri(app->world, LV2_ATOM__Tuple);
	app->uris.atom_Object = lilv_new_uri(app->world, LV2_ATOM__Object);
	app->uris.atom_Vector = lilv_new_uri(app->world, LV2_ATOM__Vector);
	app->uris.atom_Sequence = lilv_new_uri(app->world, LV2_ATOM__Sequence);

	app->uris.state_loadDefaultState = lilv_new_uri(app->world, LV2_STATE__loadDefaultState);
	app->uris.state_state = lilv_new_uri(app->world, LV2_STATE__state);
	app->uris.state_interface = lilv_new_uri(app->world, LV2_STATE__interface);
	app->uris.state_threadSafeRestore = lilv_new_uri(app->world, LV2_STATE_PREFIX"threadSafeRestore");
	app->uris.state_makePath = lilv_new_uri(app->world, LV2_STATE__makePath);

	app->uris.work_schedule = lilv_new_uri(app->world, LV2_WORKER__schedule);
	app->uris.work_interface = lilv_new_uri(app->world, LV2_WORKER__interface);

	app->uris.idisp_queue_draw = lilv_new_uri(app->world, LV2_INLINEDISPLAY__queue_draw);
	app->uris.idisp_interface = lilv_new_uri(app->world, LV2_INLINEDISPLAY__interface);

	app->uris.opts_options = lilv_new_uri(app->world, LV2_OPTIONS__options);
	app->uris.opts_interface = lilv_new_uri(app->world, LV2_OPTIONS__interface);
	app->uris.opts_requiredOption = lilv_new_uri(app->world, LV2_OPTIONS__requiredOption);
	app->uris.opts_supportedOption = lilv_new_uri(app->world, LV2_OPTIONS__supportedOption);

	app->uris.patch_writable = lilv_new_uri(app->world, LV2_PATCH__writable);
	app->uris.patch_readable = lilv_new_uri(app->world, LV2_PATCH__readable);

	app->uris.pg_group = lilv_new_uri(app->world, LV2_PORT_GROUPS__group);

	app->uris.ui_binary = lilv_new_uri(app->world, LV2_UI__binary);
	app->uris.ui_makeSONameResident = lilv_new_uri(app->world, LV2_UI_PREFIX"makeSONameResident");
	app->uris.ui_idleInterface = lilv_new_uri(app->world, LV2_UI__idleInterface);
	app->uris.ui_showInterface = lilv_new_uri(app->world, LV2_UI__showInterface);
	app->uris.ui_resize = lilv_new_uri(app->world, LV2_UI__resize);
	app->uris.ui_UI= lilv_new_uri(app->world, LV2_UI__UI);
	app->uris.ui_X11UI = lilv_new_uri(app->world, LV2_UI__X11UI);
	app->uris.ui_WindowsUI = lilv_new_uri(app->world, LV2_UI__WindowsUI);
	app->uris.ui_CocoaUI = lilv_new_uri(app->world, LV2_UI__CocoaUI);
	app->uris.ui_GtkUI = lilv_new_uri(app->world, LV2_UI__GtkUI);
	app->uris.ui_Gtk3UI = lilv_new_uri(app->world, LV2_UI__Gtk3UI);
	app->uris.ui_Qt4UI = lilv_new_uri(app->world, LV2_UI__Qt4UI);
	app->uris.ui_Qt5UI = lilv_new_uri(app->world, LV2_UI__Qt5UI);

	app->uris.event_EventPort = lilv_new_uri(app->world, LV2_EVENT__EventPort);
	app->uris.uri_map = lilv_new_uri(app->world, LV2_URI_MAP_URI);
	app->uris.instance_access = lilv_new_uri(app->world, LV2_INSTANCE_ACCESS_URI);
	app->uris.data_access = lilv_new_uri(app->world, LV2_DATA_ACCESS_URI);

	app->uris.log_log = lilv_new_uri(app->world, LV2_LOG__log);

	app->uris.urid_map = lilv_new_uri(app->world, LV2_URID__map);
	app->uris.urid_unmap = lilv_new_uri(app->world, LV2_URID__unmap);

	app->uris.rsz_resize = lilv_new_uri(app->world, LV2_RESIZE_PORT__resize);

	app->uris.bufsz_boundedBlockLength = lilv_new_uri(app->world, LV2_BUF_SIZE__boundedBlockLength);
	app->uris.bufsz_fixedBlockLength = lilv_new_uri(app->world, LV2_BUF_SIZE__fixedBlockLength);
	app->uris.bufsz_powerOf2BlockLength = lilv_new_uri(app->world, LV2_BUF_SIZE__powerOf2BlockLength);
	app->uris.bufsz_coarseBlockLength = lilv_new_uri(app->world, LV2_BUF_SIZE_PREFIX"coarseBlockLength");

	app->uris.pprops_supportsStrictBounds = lilv_new_uri(app->world, LV2_PORT_PROPS__supportsStrictBounds);
}

static void
_unmap_uris(app_t *app)
{
	lilv_node_free(app->uris.rdfs_label);
	lilv_node_free(app->uris.rdfs_comment);
	lilv_node_free(app->uris.rdfs_range);
	lilv_node_free(app->uris.rdfs_subClassOf);

	lilv_node_free(app->uris.rdf_type);

	lilv_node_free(app->uris.doap_description);
	lilv_node_free(app->uris.doap_license);
	lilv_node_free(app->uris.doap_name);
	lilv_node_free(app->uris.doap_shortdesc);

	lilv_node_free(app->uris.lv2_minimum);
	lilv_node_free(app->uris.lv2_maximum);
	lilv_node_free(app->uris.lv2_Port);
	lilv_node_free(app->uris.lv2_PortProperty);
	lilv_node_free(app->uris.lv2_default);
	lilv_node_free(app->uris.lv2_ControlPort);
	lilv_node_free(app->uris.lv2_CVPort);
	lilv_node_free(app->uris.lv2_InputPort);
	lilv_node_free(app->uris.lv2_integer);
	lilv_node_free(app->uris.lv2_toggled);
	lilv_node_free(app->uris.lv2_Feature);
	lilv_node_free(app->uris.lv2_minorVersion);
	lilv_node_free(app->uris.lv2_microVersion);
	lilv_node_free(app->uris.lv2_ExtensionData);
	lilv_node_free(app->uris.lv2_requiredFeature);
	lilv_node_free(app->uris.lv2_optionalFeature);
	lilv_node_free(app->uris.lv2_extensionData);
	lilv_node_free(app->uris.lv2_isLive);
	lilv_node_free(app->uris.lv2_inPlaceBroken);
	lilv_node_free(app->uris.lv2_hardRTCapable);

	lilv_node_free(app->uris.atom_Bool);
	lilv_node_free(app->uris.atom_Int);
	lilv_node_free(app->uris.atom_Long);
	lilv_node_free(app->uris.atom_Float);
	lilv_node_free(app->uris.atom_Double);
	lilv_node_free(app->uris.atom_String);
	lilv_node_free(app->uris.atom_Literal);
	lilv_node_free(app->uris.atom_Path);
	lilv_node_free(app->uris.atom_Chunk);
	lilv_node_free(app->uris.atom_URI);
	lilv_node_free(app->uris.atom_URID);
	lilv_node_free(app->uris.atom_Tuple);
	lilv_node_free(app->uris.atom_Object);
	lilv_node_free(app->uris.atom_Vector);
	lilv_node_free(app->uris.atom_Sequence);

	lilv_node_free(app->uris.state_loadDefaultState);
	lilv_node_free(app->uris.state_state);
	lilv_node_free(app->uris.state_interface);
	lilv_node_free(app->uris.state_threadSafeRestore);
	lilv_node_free(app->uris.state_makePath);

	lilv_node_free(app->uris.work_schedule);
	lilv_node_free(app->uris.work_interface);

	lilv_node_free(app->uris.idisp_queue_draw);
	lilv_node_free(app->uris.idisp_interface);

	lilv_node_free(app->uris.opts_options);
	lilv_node_free(app->uris.opts_interface);
	lilv_node_free(app->uris.opts_requiredOption);
	lilv_node_free(app->uris.opts_supportedOption);

	lilv_node_free(app->uris.patch_writable);
	lilv_node_free(app->uris.patch_readable);

	lilv_node_free(app->uris.pg_group);

	lilv_node_free(app->uris.ui_binary);
	lilv_node_free(app->uris.ui_makeSONameResident);
	lilv_node_free(app->uris.ui_idleInterface);
	lilv_node_free(app->uris.ui_showInterface);
	lilv_node_free(app->uris.ui_resize);
	lilv_node_free(app->uris.ui_UI);
	lilv_node_free(app->uris.ui_X11UI);
	lilv_node_free(app->uris.ui_WindowsUI);
	lilv_node_free(app->uris.ui_CocoaUI);
	lilv_node_free(app->uris.ui_GtkUI);
	lilv_node_free(app->uris.ui_Gtk3UI);
	lilv_node_free(app->uris.ui_Qt4UI);
	lilv_node_free(app->uris.ui_Qt5UI);

	lilv_node_free(app->uris.event_EventPort);
	lilv_node_free(app->uris.uri_map);
	lilv_node_free(app->uris.instance_access);
	lilv_node_free(app->uris.data_access);

	lilv_node_free(app->uris.log_log);

	lilv_node_free(app->uris.urid_map);
	lilv_node_free(app->uris.urid_unmap);

	lilv_node_free(app->uris.rsz_resize);

	lilv_node_free(app->uris.bufsz_boundedBlockLength);
	lilv_node_free(app->uris.bufsz_fixedBlockLength);
	lilv_node_free(app->uris.bufsz_powerOf2BlockLength);
	lilv_node_free(app->uris.bufsz_coarseBlockLength);

	lilv_node_free(app->uris.pprops_supportsStrictBounds);
}

static LV2_URID
_map(LV2_URID_Map_Handle instance, const char *uri)
{
	app_t *app = instance;

	for(unsigned i = 0; i < app->nurids; i++)
	{
		urid_t *itm = &app->urids[i];

		if(!strcmp(itm->uri, uri))
			return i + 1;
	}

	app->nurids += 1;
	app->urids = realloc(app->urids, app->nurids*sizeof(urid_t));
	if(app->urids)
	{
		urid_t *itm = &app->urids[app->nurids - 1];
		itm->uri = strdup(uri);
		return app->nurids;
	}

	return 0; // failed
}

static const char *
_unmap(LV2_URID_Unmap_Handle instance, LV2_URID urid)
{
	app_t *app = instance;

	if( (urid > 0) && (urid <= app->nurids) )
	{
		urid_t *itm = &app->urids[urid - 1];
		return itm->uri;
	}

	return NULL; // failed
}

static void
_free_urids(app_t *app)
{
	for(unsigned i = 0; i < app->nurids; i++)
	{
		urid_t *itm = &app->urids[i];

		if(itm->uri)
			free(itm->uri);
	}
	free(app->urids);

	app->urids = NULL;
	app->nurids = 0;
}

static LV2_Worker_Status
_respond(LV2_Worker_Respond_Handle instance, uint32_t size, const void *data)
{
	app_t *app = instance;

	if(app->work_iface && app->work_iface->work_response)
		return app->work_iface->work_response(&app->instance, size, data);

	else return LV2_WORKER_ERR_UNKNOWN;
}

static LV2_Worker_Status
_sched(LV2_Worker_Schedule_Handle instance, uint32_t size, const void *data)
{
	app_t *app = instance;

	LV2_Worker_Status status = LV2_WORKER_SUCCESS;
	if(app->work_iface && app->work_iface->work)
		status |= app->work_iface->work(&app->instance, _respond, app, size, data);
	if(app->work_iface && app->work_iface->end_run)
		status |= app->work_iface->end_run(&app->instance);

	return status;
}

static int
_vprintf(void *data, LV2_URID type, const char *fmt, va_list args)
{
	vfprintf(stderr, fmt, args);

	return 0;
}

static int
_printf(void *data, LV2_URID type, const char *fmt, ...)
{
  va_list args;
	int ret;

  va_start (args, fmt);
	ret = _vprintf(data, type, fmt, args);
  va_end(args);

	return ret;
}

static char *
_mkpath(LV2_State_Make_Path_Handle instance, const char *abstract_path)
{
	char *absolute_path = NULL;

	if(asprintf(&absolute_path, "/tmp/%s", abstract_path) == -1)
		absolute_path = NULL;

	return absolute_path;
}

static LV2_Resize_Port_Status
_resize(LV2_Resize_Port_Feature_Data instance, uint32_t index, size_t size)
{
	return LV2_RESIZE_PORT_SUCCESS;
}

static uint32_t
_uri_to_id(LV2_URI_Map_Callback_Data instance, const char *map, const char *uri)
{
	app_t *app = instance;
	(void)map;

	return _map(app, uri);
}

static void
_queue_draw(LV2_Inline_Display_Handle instance)
{
	app_t *app = instance;
	(void)app;
}

static void
_version(void)
{
	fprintf(stderr,
		"--------------------------------------------------------------------\n"
		"This is free software: you can redistribute it and/or modify\n"
		"it under the terms of the Artistic License 2.0 as published by\n"
		"The Perl Foundation.\n"
		"\n"
		"This source is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
		"Artistic License 2.0 for more details.\n"
		"\n"
		"You should have received a copy of the Artistic License 2.0\n"
		"along the source as a COPYING file. If not, obtain it from\n"
		"http://www.perlfoundation.org/artistic_license_2_0.\n\n");
}

static void
_usage(char **argv)
{
	fprintf(stderr,
		"--------------------------------------------------------------------\n"
		"USAGE\n"
		"   %s [OPTIONS] {PLUGIN_URI}*\n"
		"\n"
		"OPTIONS\n"
		"   [-v]                 print version information\n"
		"   [-h]                 print usage information\n"
#ifdef ENABLE_ONLINE_TESTS
		"   [-o]                 run in offline mode\n"
#endif

		"   [-S] warn|note       show warnings and/or notes\n"
		"   [-E] warn|note       treat warnings and/or notes as errors\n\n"
		, argv[0]);
}

#ifdef ENABLE_ONLINE_TESTS
static const char *http_prefix = "http://";
static const char *https_prefix = "https://";
static const char *ftp_prefix = "ftp://";
static const char *ftps_prefix = "ftps://";

bool
is_url(const char *uri)
{
	const bool is_http = strncmp(uri, http_prefix, strlen(http_prefix));
	const bool is_https = strncmp(uri, https_prefix, strlen(https_prefix));
	const bool is_ftp = strncmp(uri, ftp_prefix, strlen(ftp_prefix));
	const bool is_ftps = strncmp(uri, ftps_prefix, strlen(ftps_prefix));

	return is_http || is_https || is_ftp || is_ftps;
}

bool
test_url(const char *url)
{
	CURL *curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L); // secs
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L); //secs

		const CURLcode resp = curl_easy_perform(curl);

		long http_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

		curl_easy_cleanup(curl);

		if( (resp == CURLE_OK) && (http_code == 200) )
		{
			return true;
		}
	}

	return false;
}
#endif

int
main(int argc, char **argv)
{
	static app_t app;
	app.show = LINT_FAIL; // always report failed tests
	app.mask = LINT_FAIL; // always fail at failed tests

	fprintf(stderr,
		"%s "LV2LINT_VERSION"\n"
		"Copyright (c) 2016-2017 Hanspeter Portner (dev@open-music-kontrollers.ch)\n"
		"Released under Artistic License 2.0 by Open Music Kontrollers\n",
		argv[0]);
	
	int c;
#ifdef ENABLE_ONLINE_TESTS
	while( (c = getopt(argc, argv, "vhoS:E:") ) != -1)
#else
	while( (c = getopt(argc, argv, "vhS:E:") ) != -1)
#endif
	{
		switch(c)
		{
			case 'v':
				_version();
				return 0;
			case 'h':
				_usage(argv);
				return 0;
#ifdef ENABLE_ONLINE_TESTS
			case 'o':
				app.offline = true;
				break;
#endif
			case 'S':
				if(!strcmp(optarg, "warn"))
				{
					app.show |= LINT_WARN;
				}
				else if(!strcmp(optarg, "note"))
				{
					app.show |= LINT_NOTE;
				}
				break;
			case 'E':
				if(!strcmp(optarg, "warn"))
				{
					app.show |= LINT_WARN;
					app.mask |= LINT_WARN;
				}
				else if(!strcmp(optarg, "note"))
				{
					app.show |= LINT_NOTE;
					app.mask |= LINT_NOTE;
				}
				break;
			case '?':
				if( (optopt == 'S') || (optopt == 'E') )
					fprintf(stderr, "Option `-%c' requires an argument.\n", optopt);
				else if(isprint(optopt))
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
				return -1;
			default:
				return -1;
		}
	}

	if(optind == argc) // no URI given
	{
		_usage(argv);
		return -1;
	}

	app.world = lilv_world_new();
	if(!app.world)
		return -1;

	_map_uris(&app);

	lilv_world_load_all(app.world);

	LV2_URID_Map map = {
		.handle = &app,
		.map = _map
	};
	LV2_URID_Unmap unmap = {
		.handle = &app,
		.unmap = _unmap
	};
	LV2_Worker_Schedule sched = {
		.handle = &app,
		.schedule_work = _sched
	};
	LV2_Log_Log log = {
		.handle = &app,
		.printf = _printf,
		.vprintf = _vprintf
	};
	LV2_State_Make_Path mkpath = {
		.handle = &app,
		.path = _mkpath
	};
	LV2_Resize_Port_Resize rsz = {
		.data = &app,
		.resize = _resize
	};
	LV2_URI_Map_Feature urimap = {
		.callback_data = &app,
		.uri_to_id = _uri_to_id
	};
	LV2_Inline_Display queue_draw = {
		.handle = &app,
		.queue_draw = _queue_draw
	};

	const LV2_URID atom_Float = map.map(map.handle, LV2_ATOM__Float);
	const LV2_URID atom_Int = map.map(map.handle, LV2_ATOM__Int);
	const LV2_URID param_sampleRate = map.map(map.handle, LV2_PARAMETERS__sampleRate);
	const LV2_URID ui_updateRate = map.map(map.handle, LV2_UI__updateRate);
	const LV2_URID bufsz_minBlockLength = map.map(map.handle, LV2_BUF_SIZE__minBlockLength);
	const LV2_URID bufsz_maxBlockLength = map.map(map.handle, LV2_BUF_SIZE__maxBlockLength);
	const LV2_URID bufsz_nominalBlockLength = map.map(map.handle, LV2_BUF_SIZE_PREFIX"nominalBlockLength");
	const LV2_URID bufsz_sequenceSize = map.map(map.handle, LV2_BUF_SIZE__sequenceSize);

	const float param_sample_rate = 48000.f;
	const float ui_update_rate = 25.f;
	const int32_t bufsz_min_block_length = 256;
	const int32_t bufsz_max_block_length = 256;
	const int32_t bufsz_nominal_block_length = 256;
	const int32_t bufsz_sequence_size = 2048;

	LV2_Options_Option opts [] = {
		{
			.key = param_sampleRate,
			.size = sizeof(float),
			.type = atom_Float,
			.value = &param_sample_rate
		},
		{
			.key = ui_updateRate,
			.size = sizeof(float),
			.type = atom_Float,
			.value = &ui_update_rate
		},
		{
			.key = bufsz_minBlockLength,
			.size = sizeof(int32_t),
			.type = atom_Int,
			.value = &bufsz_min_block_length
		},
		{
			.key = bufsz_maxBlockLength,
			.size = sizeof(int32_t),
			.type = atom_Int,
			.value = &bufsz_max_block_length
		},
		{
			.key = bufsz_nominalBlockLength,
			.size = sizeof(int32_t),
			.type = atom_Int,
			.value = &bufsz_nominal_block_length
		},
		{
			.key = bufsz_sequenceSize,
			.size = sizeof(int32_t),
			.type = atom_Int,
			.value = &bufsz_sequence_size
		},
		{
			.key = 0,
			.value =NULL
		}
	};

	const LV2_Feature feat_map = {
		.URI = LV2_URID__map,
		.data = &map
	};
	const LV2_Feature feat_unmap = {
		.URI = LV2_URID__unmap,
		.data = &unmap
	};
	const LV2_Feature feat_sched = {
		.URI = LV2_WORKER__schedule,
		.data = &sched
	};
	const LV2_Feature feat_log = {
		.URI = LV2_LOG__log,
		.data = &log
	};
	const LV2_Feature feat_mkpath = {
		.URI = LV2_STATE__makePath,
		.data = &mkpath
	};
	const LV2_Feature feat_rsz = {
		.URI = LV2_RESIZE_PORT__resize,
		.data = &rsz
	};
	const LV2_Feature feat_opts = {
		.URI = LV2_OPTIONS__options,
		.data = opts
	};
	const LV2_Feature feat_urimap = {
		.URI = LV2_URI_MAP_URI,
		.data = &urimap
	};

	const LV2_Feature feat_islive = {
		.URI = LV2_CORE__isLive
	};
	const LV2_Feature feat_inplacebroken = {
		.URI = LV2_CORE__inPlaceBroken
	};
	const LV2_Feature feat_hardrtcapable = {
		.URI = LV2_CORE__hardRTCapable
	};
	const LV2_Feature feat_supportsstrictbounds = {
		.URI = LV2_PORT_PROPS__supportsStrictBounds
	};
	const LV2_Feature feat_boundedblocklength = {
		.URI = LV2_BUF_SIZE__boundedBlockLength
	};
	const LV2_Feature feat_fixedblocklength = {
		.URI = LV2_BUF_SIZE__fixedBlockLength
	};
	const LV2_Feature feat_powerof2blocklength = {
		.URI = LV2_BUF_SIZE__powerOf2BlockLength
	};
	const LV2_Feature feat_coarseblocklength = {
		.URI = LV2_BUF_SIZE_PREFIX"coarseBlockLength"
	};
	const LV2_Feature feat_loaddefaultstate = {
		.URI = LV2_STATE__loadDefaultState
	};
	const LV2_Feature feat_threadsaferestore = {
		.URI = LV2_STATE_PREFIX"threadSafeRestore"
	};
	const LV2_Feature feat_idispqueuedraw = {
		.URI = LV2_INLINEDISPLAY__queue_draw,
		.data = &queue_draw
	};

	int ret = 0;
	const LilvPlugin *plugins = lilv_world_get_all_plugins(app.world);
	if(plugins)
	{
		for(int i=optind; i<argc; i++)
		{
			const char *plugin_uri = argv[i];
			LilvNode *plugin_uri_node = lilv_new_uri(app.world, plugin_uri);
			if(plugin_uri_node)
			{
				app.plugin = lilv_plugins_get_by_uri(plugins, plugin_uri_node);
				if(app.plugin)
				{
					const int MAX_FEATURES = 20;
					const LV2_Feature *features [MAX_FEATURES];
					int f = 0;

					LilvNodes *required_features = lilv_plugin_get_required_features(app.plugin);
					if(required_features)
					{
						LILV_FOREACH(nodes, itr, required_features)
						{
							const LilvNode *feature = lilv_nodes_get(required_features, itr);

							if(lilv_node_equals(feature, app.uris.urid_map))
								features[f++] = &feat_map;
							else if(lilv_node_equals(feature, app.uris.urid_unmap))
								features[f++] = &feat_unmap;
							else if(lilv_node_equals(feature, app.uris.work_schedule))
								features[f++] = &feat_sched;
							else if(lilv_node_equals(feature, app.uris.log_log))
								features[f++] = &feat_log;
							else if(lilv_node_equals(feature, app.uris.state_makePath))
								features[f++] = &feat_mkpath;
							else if(lilv_node_equals(feature, app.uris.rsz_resize))
								features[f++] = &feat_rsz;
							else if(lilv_node_equals(feature, app.uris.opts_options))
								features[f++] = &feat_opts;
							else if(lilv_node_equals(feature, app.uris.uri_map))
								features[f++] = &feat_urimap;
							else if(lilv_node_equals(feature, app.uris.lv2_isLive))
								features[f++] = &feat_islive;
							else if(lilv_node_equals(feature, app.uris.lv2_inPlaceBroken))
								features[f++] = &feat_inplacebroken;
							else if(lilv_node_equals(feature, app.uris.lv2_hardRTCapable))
								features[f++] = &feat_hardrtcapable;
							else if(lilv_node_equals(feature, app.uris.pprops_supportsStrictBounds))
								features[f++] = &feat_supportsstrictbounds;
							else if(lilv_node_equals(feature, app.uris.bufsz_boundedBlockLength))
								features[f++] = &feat_boundedblocklength;
							else if(lilv_node_equals(feature, app.uris.bufsz_fixedBlockLength))
								features[f++] = &feat_fixedblocklength;
							else if(lilv_node_equals(feature, app.uris.bufsz_powerOf2BlockLength))
								features[f++] = &feat_powerof2blocklength;
							else if(lilv_node_equals(feature, app.uris.bufsz_coarseBlockLength))
								features[f++] = &feat_coarseblocklength;
							else if(lilv_node_equals(feature, app.uris.state_loadDefaultState))
								features[f++] = &feat_loaddefaultstate;
							else if(lilv_node_equals(feature, app.uris.state_threadSafeRestore))
								features[f++] = &feat_threadsaferestore;
							else if(lilv_node_equals(feature, app.uris.idisp_queue_draw))
								features[f++] = &feat_idispqueuedraw;
						}
						lilv_nodes_free(required_features);
					}

					features[f++] = NULL; // sentinel
					assert(f <= MAX_FEATURES);

					const bool atty = isatty(1);
					fprintf(stdout, "%s<%s>%s\n",
						colors[atty][ANSI_COLOR_BOLD],
						lilv_node_as_uri(lilv_plugin_get_uri(app.plugin)),
						colors[atty][ANSI_COLOR_RESET]);

					app.instance = lilv_plugin_instantiate(app.plugin, param_sample_rate, features);

					if(app.instance)
					{
						app.work_iface = lilv_instance_get_extension_data(app.instance, LV2_WORKER__interface);
						app.idisp_iface = lilv_instance_get_extension_data(app.instance, LV2_INLINEDISPLAY__interface);
						app.state_iface = lilv_instance_get_extension_data(app.instance, LV2_STATE__interface);
						app.opts_iface = lilv_instance_get_extension_data(app.instance, LV2_OPTIONS__interface);
					}

					if(!test_plugin(&app))
					{
						ret += 1;
					}

					if(app.instance)
					{
						lilv_instance_free(app.instance);
						app.instance = NULL;
						app.work_iface = NULL;
						app.idisp_iface = NULL;
						app.state_iface= NULL;
						app.opts_iface = NULL;
					}

					app.plugin = NULL;
				}
				else
				{
					ret += 1;
				}
			}
			else
			{
				ret += 1;
			}
			lilv_node_free(plugin_uri_node);
		}
	}
	else
	{
		ret = -1;
	}

	_unmap_uris(&app);
	_free_urids(&app);

	lilv_world_free(app.world);

	return ret;
}
