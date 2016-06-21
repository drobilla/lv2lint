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

#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include <lilv/lilv.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define COLOR_PASS ANSI_COLOR_GREEN
#define COLOR_NOTE ANSI_COLOR_BLUE
#define COLOR_WARN ANSI_COLOR_YELLOW
#define COLOR_FAIL ANSI_COLOR_RED

typedef enum _lint_t lint_t;
typedef struct _app_t app_t;
typedef struct _test_t test_t;
typedef struct _ret_t ret_t;
typedef const ret_t *(*test_cb_t)(app_t *app);

enum _lint_t {
	LINT_NOTE,
	LINT_WARN,
	LINT_FAIL
};

struct _ret_t {
	lint_t lint;
	const char *msg;
	const char *url;
};

struct _app_t {
	LilvWorld *world;
	const LilvPlugin *plugin;
};

struct _test_t {
	const char *id;
	test_cb_t cb;
};

static const ret_t ret_verification = {LINT_FAIL, "failed", NULL};

static const ret_t *
_test_verification(app_t *app)
{
	const ret_t *ret = NULL;
	if(!lilv_plugin_verify(app->plugin))
	{
		ret = &ret_verification;
	}

	return ret;
}

enum {
	NAME_NOT_FOUND,
	NAME_NOT_A_STRING,
	NAME_EMPTY
};

static const ret_t ret_name [] = {
	[NAME_NOT_FOUND]      = {LINT_FAIL, "doap:name not found", LV2_CORE__Plugin},
		[NAME_NOT_A_STRING] = {LINT_FAIL, "doap:name not a string", LILV_NS_DOAP"name"},
		[NAME_EMPTY]        = {LINT_FAIL, "doap:name empty", LILV_NS_DOAP"name"}
};

static const ret_t *
_test_name(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *name_node = lilv_plugin_get_name(app->plugin);
	if(name_node)
	{
		if(lilv_node_is_string(name_node))
		{
			const char *name = lilv_node_as_string(name_node);
			if(!name)
			{
				ret = &ret_name[NAME_EMPTY];
			}
		}
		else // !is_string
		{
			ret = &ret_name[NAME_NOT_A_STRING];
		}
		lilv_node_free(name_node);
	}
	else // !name_node
	{
		ret = &ret_name[NAME_NOT_FOUND];
	}

	return ret;
}

enum {
	LICENSE_NOT_FOUND,
	LICENSE_NOT_AN_URI,
	LICENSE_EMPTY,
};

static const ret_t ret_license [] = {
	[LICENSE_NOT_FOUND]    = {LINT_WARN, "doap:license not found", LV2_CORE__Plugin},
		[LICENSE_NOT_AN_URI] = {LINT_FAIL, "doap:license not an URI", LILV_NS_DOAP"license"},
		[LICENSE_EMPTY]      = {LINT_FAIL, "doap:license empty", LILV_NS_DOAP"license"},
};

static const ret_t * 
_test_license(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *doap_license_node = lilv_new_uri(app->world, LILV_NS_DOAP"license");

	LilvNode *license_node = lilv_world_get(app->world, lilv_plugin_get_uri(app->plugin), doap_license_node, NULL);
	if(license_node)
	{
		if(lilv_node_is_uri(license_node))
		{
			if(!lilv_node_as_uri(license_node))
			{
				ret = &ret_license[LICENSE_EMPTY];
			}
		}
		else
		{
			ret = &ret_license[LICENSE_NOT_AN_URI];
		}
		lilv_node_free(license_node);
	}
	else
	{
		ret = &ret_license[LICENSE_NOT_FOUND];
	}

	lilv_node_free(doap_license_node);

	return ret;
}

enum {
	AUTHOR_NAME_NOT_FOUND,
	AUTHOR_NAME_NOT_A_STRING,
	AUTHOR_NAME_EMPTY,
	AUTHOR_EMAIL_NOT_FOUND,
	AUTHOR_EMAIL_NOT_AN_URI,
	AUTHOR_EMAIL_EMPTY,
	AUTHOR_HOMEPAGE_NOT_FOUND,
	AUTHOR_HOMEPAGE_NOT_AN_URI,
	AUTHOR_HOMEPAGE_EMPTY,
};

static const ret_t ret_author [] = {
	[AUTHOR_NAME_NOT_FOUND]        = {LINT_WARN, "foaf:name not found", LV2_CORE__project},
		[AUTHOR_NAME_NOT_A_STRING]   = {LINT_FAIL, "foaf:name not an string", LILV_NS_FOAF"name"},
		[AUTHOR_NAME_EMPTY]          = {LINT_FAIL, "foaf:name empty", LILV_NS_FOAF"name"},
	[AUTHOR_EMAIL_NOT_FOUND]       = {LINT_WARN, "foaf:email not found", LV2_CORE__project},
		[AUTHOR_EMAIL_NOT_AN_URI]    = {LINT_FAIL, "foaf:email not an URI", LILV_NS_FOAF"email"},
		[AUTHOR_EMAIL_EMPTY]         = {LINT_FAIL, "foaf:email empty", LILV_NS_FOAF"email"},
	[AUTHOR_HOMEPAGE_NOT_FOUND]    = {LINT_WARN, "foaf:homepage not found", LV2_CORE__project},
		[AUTHOR_HOMEPAGE_NOT_AN_URI] = {LINT_FAIL, "foaf:homepage not an URI", LILV_NS_FOAF"homepage"},
		[AUTHOR_HOMEPAGE_EMPTY]      = {LINT_FAIL, "foaf:homepage empty", LILV_NS_FOAF"homepage"},
};

static const ret_t * 
_test_author_name(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *author_name = lilv_plugin_get_author_name(app->plugin);
	if(author_name)
	{
		if(lilv_node_is_string(author_name))
		{
			if(!lilv_node_as_string(author_name))
			{
				ret = &ret_author[AUTHOR_NAME_EMPTY];
			}
		}
		else
		{
			ret = &ret_author[AUTHOR_NAME_NOT_A_STRING];
		}
		lilv_node_free(author_name);
	}
	else
	{
		ret = &ret_author[AUTHOR_NAME_NOT_FOUND];
	}

	return ret;
}

static const ret_t *
_test_author_email(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *author_email = lilv_plugin_get_author_email(app->plugin);
	if(author_email)
	{
		if(lilv_node_is_uri(author_email))
		{
			if(!lilv_node_as_uri(author_email))
			{
				ret = &ret_author[AUTHOR_EMAIL_EMPTY];
			}
		}
		else
		{
			ret = &ret_author[AUTHOR_EMAIL_NOT_AN_URI];
		}
		lilv_node_free(author_email);
	}
	else
	{
		ret = &ret_author[AUTHOR_EMAIL_NOT_FOUND];
	}

	return ret;
}

static const ret_t *
_test_author_homepage(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *author_homepage = lilv_plugin_get_author_homepage(app->plugin);
	if(author_homepage)
	{
		if(lilv_node_is_uri(author_homepage))
		{
			if(!lilv_node_as_uri(author_homepage))
			{
				ret = &ret_author[AUTHOR_HOMEPAGE_EMPTY];
			}
		}
		else
		{
			ret = &ret_author[AUTHOR_HOMEPAGE_NOT_AN_URI];
		}
	}
	else
	{
		ret = &ret_author[AUTHOR_HOMEPAGE_NOT_FOUND];
	}

	return ret;
}

enum {
	VERSION_MINOR_NOT_FOUND,
	VERSION_MINOR_NOT_AN_INT,
	VERSION_MINOR_UNSTABLE,
	VERSION_MICRO_NOT_FOUND,
	VERSION_MICRO_NOT_AN_INT,
	VERSION_MICRO_UNSTABLE,
};

static const ret_t ret_version [] = {
	[VERSION_MINOR_NOT_FOUND]      = {LINT_FAIL, "lv2:minorVersion not found", LV2_CORE__minorVersion},
		[VERSION_MINOR_NOT_AN_INT]   = {LINT_FAIL, "lv2:minorVersion not an integer", LV2_CORE__minorVersion},
		[VERSION_MINOR_UNSTABLE]     = {LINT_NOTE, "lv2:minorVersion denotes an unstable version", LV2_CORE__minorVersion},
	[VERSION_MICRO_NOT_FOUND]      = {LINT_FAIL, "lv2:microVersion not found", LV2_CORE__microVersion},
		[VERSION_MICRO_NOT_AN_INT]   = {LINT_FAIL, "lv2:microVersion not an integer", LV2_CORE__microVersion},
		[VERSION_MICRO_UNSTABLE]     = {LINT_NOTE, "lv2:microVersion denotes an unstable version", LV2_CORE__microVersion},
};

static const ret_t * 
_test_version_minor(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *minor_version_uri = lilv_new_uri(app->world, LV2_CORE__minorVersion);

	LilvNode *minor_version_nodes = lilv_plugin_get_value(app->plugin , minor_version_uri);
	if(minor_version_nodes)
	{
		const LilvNode *minor_version_node = lilv_nodes_get_first(minor_version_nodes);
		if(minor_version_node)
		{
			if(lilv_node_is_int(minor_version_node))
			{
				const int minor_version = lilv_node_as_int(minor_version_node);
				if( (minor_version % 2 != 0) || (minor_version == 0) )
				{
					ret = &ret_version[VERSION_MINOR_UNSTABLE];
				}
			}
			else
			{
				ret = &ret_version[VERSION_MINOR_NOT_AN_INT];
			}
		}
		else
		{
			ret = &ret_version[VERSION_MINOR_NOT_FOUND];
		}
		lilv_nodes_free(minor_version_nodes);
	}
	else
	{
		ret = &ret_version[VERSION_MINOR_NOT_FOUND];
	}

	lilv_node_free(minor_version_uri);

	return ret;
}

static const ret_t * 
_test_version_micro(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *micro_version_uri = lilv_new_uri(app->world, LV2_CORE__microVersion);

	LilvNode *micro_version_nodes = lilv_plugin_get_value(app->plugin , micro_version_uri);
	if(micro_version_nodes)
	{
		const LilvNode *micro_version_node = lilv_nodes_get_first(micro_version_nodes);
		if(micro_version_node)
		{
			if(lilv_node_is_int(micro_version_node))
			{
				const int micro_version = lilv_node_as_int(micro_version_node);
				if(micro_version % 2 != 0)
				{
					ret = &ret_version[VERSION_MICRO_UNSTABLE];
				}
			}
			else
			{
				ret = &ret_version[VERSION_MICRO_NOT_AN_INT];
			}
		}
		else
		{
			ret = &ret_version[VERSION_MICRO_NOT_FOUND];
		}
		lilv_nodes_free(micro_version_nodes);
	}
	else
	{
		ret = &ret_version[VERSION_MICRO_NOT_FOUND];
	}

	lilv_node_free(micro_version_uri);

	return ret;
}

static const ret_t * 
_test_extension_data(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *extension_data_nodes = lilv_plugin_get_extension_data(app->plugin);
	if(extension_data_nodes)
	{
		LILV_FOREACH(nodes, itr, extension_data_nodes)
		{
			const LilvNode *extension_data_node = lilv_nodes_get(extension_data_nodes, itr);
			if(extension_data_node)
			{
				// check with lilv_instanace_get_extension_data
			}
		}
		//FIXME flag = true; //FIXME

		lilv_nodes_free(extension_data_nodes);
	}

	return ret;
}

enum {
	PROJECT_NOT_FOUND,
	PROJECT_NAME_NOT_FOUND,
	PROJECT_NAME_NOT_A_STRING,
	PROJECT_NAME_EMPTY,
};

static const ret_t ret_project [] = {
	[PROJECT_NOT_FOUND]           = {LINT_NOTE, "lv2:project not found", LV2_CORE__project},
		[PROJECT_NAME_NOT_FOUND]    = {LINT_WARN, "lv2:project doap:name not found", LV2_CORE__project},
		[PROJECT_NAME_NOT_A_STRING] = {LINT_FAIL, "lv2:project doap:name not a string", LILV_NS_DOAP"name"},
		[PROJECT_NAME_EMPTY]        = {LINT_FAIL, "lv2:project doap:name empty", LILV_NS_DOAP"name"},
};

static const ret_t *
_test_project(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *doap_name_node = lilv_new_uri(app->world, LILV_NS_DOAP"name");

	LilvNode *project_node = lilv_plugin_get_project(app->plugin);
	if(project_node)
	{
		LilvNode *project_name_node = lilv_world_get(app->world, project_node, doap_name_node, NULL);
		if(project_name_node)
		{
			if(lilv_node_is_string(project_name_node))
			{
				if(!lilv_node_as_string(project_name_node))
				{
					ret = &ret_project[PROJECT_NAME_EMPTY];
				}
			}
			else
			{
				ret = &ret_project[PROJECT_NAME_NOT_A_STRING];
			}
			lilv_free(project_name_node);
		}
		else // !doap_name_node
		{
			ret = &ret_project[PROJECT_NAME_NOT_FOUND];
		}
		lilv_node_free(project_node);
	}
	else // !project_node
	{
		ret = &ret_project[PROJECT_NOT_FOUND];
	}

	lilv_free(doap_name_node);

	return ret;
}

static const test_t tests [] = {
	{"Verification    ", _test_verification},
	{"Name            ", _test_name},
	{"License         ", _test_license},
	{"Author Name     ", _test_author_name},
	{"Author Email    ", _test_author_email},
	{"Author Homepage ", _test_author_homepage},
	{"Version Minor   ", _test_version_minor},
	{"Version Micro   ", _test_version_micro},
	{"Project         ", _test_project},
	//{"extension_data", _test_extension_data},
	{NULL, NULL}
};

static bool
_test(app_t *app)
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
					fprintf(stdout, "  ["ANSI_COLOR_RED"FAIL"ANSI_COLOR_RESET"]  %s=> %s <%s>\n", test->id, ret->msg, ret->url);
					flag = false;
					break;
				case LINT_WARN:
					fprintf(stdout, "  ["ANSI_COLOR_YELLOW"WARN"ANSI_COLOR_RESET"]  %s=> %s <%s>\n", test->id, ret->msg, ret->url);
					break;
				case LINT_NOTE:
					fprintf(stdout, "  ["ANSI_COLOR_CYAN"NOTE"ANSI_COLOR_RESET"]  %s=> %s <%s>\n", test->id, ret->msg, ret->url);
					break;
			}
		}
		else
		{
			fprintf(stdout, "  ["ANSI_COLOR_GREEN"PASS"ANSI_COLOR_RESET"]  %s\n", test->id);
		}
	}

	return flag;
}

int
main(int argc, char **argv)
{
	static app_t app;

	fprintf(stderr,
		"%s "LV2LINT_VERSION"\n"
		"Copyright (c) 2016 Hanspeter Portner (dev@open-music-kontrollers.ch)\n"
		"Released under Artistic License 2.0 by Open Music Kontrollers\n",
		argv[0]);
	
	int c;
	while( (c = getopt(argc, argv, "vhW:") ) != -1)
	{
		switch(c)
		{
			case 'v':
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
				return 0;
			case 'h':
				fprintf(stderr,
					"--------------------------------------------------------------------\n"
					"USAGE\n"
					"   %s [OPTIONS] PLUGIN_URI\n"
					"\n"
					"OPTIONS\n"
					"   [-v]                 print version information\n"
					"   [-h]                 print usage information\n"
					"   [-W all]             show warnings for all checks\n"
					"   [-W err]             treat warnings as errors\n"
					"   [-W pedantic]        treat notes and warnings as errors\n\n"
					, argv[0]);
				return 0;
			case 'W':
				if(!strcmp(optarg, "all"))
					; //FIXME
				else if(!strcmp(optarg, "err"))
					; //FIXME
				else if(!strcmp(optarg, "pedantic"))
					; //FIXME
				break;
			case '?':
				if(optopt == 'W')
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

	app.world = lilv_world_new();
	if(!app.world)
		return -1;

	lilv_world_load_all(app.world);

	int ret = 0;
	for(int i=optind; i<argc; i++)
	{
		const char *plugin_uri = argv[i];
		if(plugin_uri)
		{
			LilvNode *plugin_uri_node = lilv_new_uri(app.world, plugin_uri);
			if(plugin_uri_node)
			{
				const LilvPlugin *plugins = lilv_world_get_all_plugins(app.world);
				if(plugins)
				{
					app.plugin = lilv_plugins_get_by_uri(plugins, plugin_uri_node);
					if(app.plugin)
					{
						fprintf(stdout, "<%s>\n", argv[i]);
						if(!_test(&app))
							ret = -1;
						fprintf(stdout, "\n");
					}
				}
			}
			lilv_node_free(plugin_uri_node);
		}
	}

	lilv_world_free(app.world);

	return ret;
}
