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

#include <lv2/lv2plug.in/ns/ext/patch/patch.h>
#include <lv2/lv2plug.in/ns/ext/worker/worker.h>
#include <lv2/lv2plug.in/ns/ext/uri-map/uri-map.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>

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

	LilvNode *license_node = lilv_world_get(app->world, lilv_plugin_get_uri(app->plugin), app->uris.doap_license, NULL);
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

	LilvNode *minor_version_nodes = lilv_plugin_get_value(app->plugin , app->uris.lv2_minorVersion);
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

	return ret;
}

static const ret_t * 
_test_version_micro(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *micro_version_nodes = lilv_plugin_get_value(app->plugin , app->uris.lv2_microVersion);
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

	LilvNode *project_node = lilv_plugin_get_project(app->plugin);
	if(project_node)
	{
		LilvNode *project_name_node = lilv_world_get(app->world, project_node, app->uris.doap_name, NULL);
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

	return ret;
}

enum {
	CLASS_NOT_FOUND,
	CLASS_IS_BASE_CLASS,
	CLASS_NOT_VALID,
};

static const ret_t ret_class [] = {
	[CLASS_NOT_FOUND]           = {LINT_FAIL, "lv2:class not found", LV2_CORE__Plugin},
		[CLASS_IS_BASE_CLASS]     = {LINT_WARN, "lv2:class is base class", LV2_CORE__Plugin},
		[CLASS_NOT_VALID]         = {LINT_FAIL, "lv2:class not valid", LV2_CORE__Plugin},
};

static inline bool
_test_class_equals(const LilvPluginClass *base, const LilvPluginClass *class)
{
	return lilv_node_equals(
		lilv_plugin_class_get_uri(base),
		lilv_plugin_class_get_uri(class) );
}

static inline bool
_test_class_match(const LilvPluginClass *base, const LilvPluginClass *class)
{
	if(_test_class_equals(base, class))
		return true;

	LilvPluginClasses *children= lilv_plugin_class_get_children(base);
	if(children)
	{
		LILV_FOREACH(plugin_classes, itr, children)
		{
			const LilvPluginClass *child = lilv_plugin_classes_get(children, itr);
			if(_test_class_match(child, class))
				return true;
		}
		lilv_plugin_classes_free(children);
	}

	return false;
}

static const ret_t *
_test_class(app_t *app)
{
	const ret_t *ret = NULL;

	const LilvPluginClass *class = lilv_plugin_get_class(app->plugin);
	if(class)
	{
		const LilvPluginClass *base = lilv_world_get_plugin_class(app->world);
		if(_test_class_equals(base, class))
		{
			ret = &ret_class[CLASS_IS_BASE_CLASS];
		}
		else if(!_test_class_match(base, class))
		{
			ret = &ret_class[CLASS_NOT_VALID];
		}
	}
	else // !class
	{
		ret = &ret_class[CLASS_NOT_FOUND];
	}

	return ret;
}

enum {
	FEATURES_NOT_VALID,
};

static const ret_t ret_features [] = {
	[FEATURES_NOT_VALID]         = {LINT_FAIL, "lv2:[optional|required]Feature not valid", LV2_CORE__Feature},
};

static const ret_t *
_test_features(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNodes *features = lilv_world_find_nodes(app->world,
		NULL, app->uris.rdf_type, app->uris.lv2_Feature);
	if(features)
	{
		LilvNodes *supported = lilv_plugin_get_supported_features(app->plugin);
		if(supported)
		{
			LILV_FOREACH(nodes, itr, supported)
			{
				const LilvNode *node = lilv_nodes_get(supported, itr);

				if(!lilv_nodes_contains(features, node))
				{
					ret = &ret_features[FEATURES_NOT_VALID];
					break;
				}
			}

			lilv_nodes_free(supported);
		}

		lilv_nodes_free(features);
	}

	return ret;
}

enum {
	EXTENSIONS_NOT_VALID,
};

static const ret_t ret_extensions [] = {
	[EXTENSIONS_NOT_VALID]         = {LINT_FAIL, "lv2:extensionData not valid", LV2_CORE__ExtensionData},
};

static const ret_t *
_test_extensions(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNodes *extensions = lilv_world_find_nodes(app->world,
		NULL, app->uris.rdf_type, app->uris.lv2_ExtensionData);
	if(extensions)
	{
		LilvNodes *data = lilv_plugin_get_extension_data(app->plugin);
		if(data)
		{
			LILV_FOREACH(nodes, itr, data)
			{
				const LilvNode *node = lilv_nodes_get(data, itr);

				if(!lilv_nodes_contains(extensions, node))
				{
					ret = &ret_extensions[EXTENSIONS_NOT_VALID];
					break;
				}
			}

			lilv_nodes_free(data);
		}

		lilv_nodes_free(extensions);
	}

	return ret;
}

enum {
	WORKER_SCHEDULE_NOT_FOUND,
	WORKER_INTERFACE_NOT_FOUND
};

static const ret_t ret_worker [] = {
	[WORKER_SCHEDULE_NOT_FOUND]         = {LINT_FAIL, "work:schedule not defined", LV2_WORKER__schedule},
	[WORKER_INTERFACE_NOT_FOUND]        = {LINT_FAIL, "work:interface not defined", LV2_WORKER__interface},
};

static const ret_t *
_test_worker(app_t *app)
{
	const ret_t *ret = NULL;

	const bool has_work_schedule= lilv_plugin_has_feature(app->plugin, app->uris.work_schedule);
	const bool has_work_iface = lilv_plugin_has_extension_data(app->plugin, app->uris.work_interface);

	if(has_work_schedule || has_work_iface)
	{
		if(!has_work_schedule)
		{
			ret = &ret_worker[WORKER_SCHEDULE_NOT_FOUND];
		}
		else if(!has_work_iface)
		{
			ret = &ret_worker[WORKER_INTERFACE_NOT_FOUND];
		}
	}

	return ret;
}

enum {
	URI_MAP_DEPRECATED,
};

static const ret_t ret_uri_map [] = {
	[URI_MAP_DEPRECATED]         = {LINT_FAIL, "uri-map is deprecated, use urid:map instead", LV2_URI_MAP_URI},
};

static const ret_t *
_test_uri_map(app_t *app)
{
	const ret_t *ret = NULL;

	if(lilv_plugin_has_feature(app->plugin, app->uris.uri_map))
	{
		ret = &ret_uri_map[URI_MAP_DEPRECATED];
	}

	return ret;
}

enum {
	STATE_LOAD_DEFAULT_NOT_FOUND,
	STATE_INTERFACE_NOT_FOUND,
	STATE_DEFAULT_NOT_FOUND,
};

static const ret_t ret_state [] = {
	[STATE_LOAD_DEFAULT_NOT_FOUND]      = {LINT_FAIL, "state:loadDefaultState not found", LV2_STATE__loadDefaultState},
	[STATE_INTERFACE_NOT_FOUND]         = {LINT_FAIL, "state:interface not found", LV2_STATE__interface},
	[STATE_DEFAULT_NOT_FOUND]           = {LINT_WARN, "state:state not found", LV2_STATE__state},
};

static const ret_t *
_test_state(app_t *app)
{
	const ret_t *ret = NULL;

	const bool has_load_default = lilv_plugin_has_feature(app->plugin, app->uris.state_loadDefaultState);
	const bool has_state = lilv_world_ask(app->world,
		lilv_plugin_get_uri(app->plugin), app->uris.state_state, NULL);
	const bool has_iface = lilv_plugin_has_extension_data(app->plugin, app->uris.state_interface);

	if(has_load_default || has_state || has_iface)
	{
		if(has_state)
		{
			if(!has_load_default)
			{
				ret = &ret_state[STATE_LOAD_DEFAULT_NOT_FOUND];
			}
			else if(!has_iface)
			{
				ret = &ret_state[STATE_INTERFACE_NOT_FOUND];
			}
		}
		else if(has_load_default)
		{
			if(!has_state)
			{
				ret = &ret_state[STATE_DEFAULT_NOT_FOUND];
			}
			else if(!has_iface)
			{
				ret = &ret_state[STATE_INTERFACE_NOT_FOUND];
			}
		}
		else if(!has_iface)
		{
			ret = &ret_state[STATE_INTERFACE_NOT_FOUND];
		}
	}

	return ret;
}

enum {
	COMMENT_NOT_FOUND,
	COMMENT_NOT_A_STRING,
	DESCRIPTION_NOT_A_STRING,
};

static const ret_t ret_comment [] = {
	[COMMENT_NOT_FOUND]         = {LINT_NOTE, "rdfs:comment or doap:description not found", LV2_CORE__Plugin},
	[COMMENT_NOT_A_STRING]      = {LINT_FAIL, "rdfs:comment not a string", LILV_NS_RDFS"comment"},
	[DESCRIPTION_NOT_A_STRING]  = {LINT_FAIL, "doap:description not a string", LILV_NS_DOAP"description"},
};

static const ret_t *
_test_comment(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *comment = lilv_world_get(app->world,
		lilv_plugin_get_uri(app->plugin), app->uris.rdfs_comment, NULL);
	LilvNode *description= lilv_world_get(app->world,
		lilv_plugin_get_uri(app->plugin), app->uris.doap_description, NULL);

	if(comment)
	{
		if(!lilv_node_is_string(comment))
		{
			ret = &ret_comment[COMMENT_NOT_A_STRING];
		}

		lilv_node_free(comment);
		if(description)
			lilv_node_free(description);
	}
	else if(description)
	{
		if(!lilv_node_is_string(description))
		{
			ret = &ret_comment[DESCRIPTION_NOT_A_STRING];
		}

		lilv_node_free(description);
	}
	else
	{
		ret = &ret_comment[COMMENT_NOT_FOUND];
	}

	return ret;
}

enum {
	SHORTDESC_NOT_FOUND,
	SHORTDESC_NOT_A_STRING,
};

static const ret_t ret_shortdesc [] = {
	[SHORTDESC_NOT_FOUND]         = {LINT_NOTE, "doap:shortdesc not found", LILV_NS_DOAP"shortdesc"},
	[SHORTDESC_NOT_A_STRING]      = {LINT_FAIL, "doap:shortdesc not a string", LILV_NS_DOAP"shortdesc"},
};

static const ret_t *
_test_shortdesc(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *shortdesc = lilv_world_get(app->world,
		lilv_plugin_get_uri(app->plugin), app->uris.doap_shortdesc, NULL);
	if(shortdesc)
	{
		if(!lilv_node_is_string(shortdesc))
		{
			ret = &ret_shortdesc[SHORTDESC_NOT_A_STRING];
		}

		lilv_node_free(shortdesc);
	}
	else
	{
		ret = &ret_shortdesc[SHORTDESC_NOT_FOUND];
	}

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
	{"Class           ", _test_class},
	{"Features        ", _test_features},
	{"Extension Data  ", _test_extensions},
	{"Worker          ", _test_worker},
	{"URI-Map         ", _test_uri_map},
	{"State           ", _test_state},
	{"Comment         ", _test_comment},
	{"Shortdesc       ", _test_shortdesc},
};

static const unsigned tests_n = sizeof(tests) / sizeof(test_t);

bool
test_plugin(app_t *app)
{
	bool flag = true;
	bool msg = false;
	const ret_t *rets [tests_n];

	fprintf(stdout, ANSI_COLOR_BOLD"<%s>"ANSI_COLOR_RESET"\n", lilv_node_as_uri(lilv_plugin_get_uri(app->plugin)));

	for(unsigned i=0; i<tests_n; i++)
	{
		const test_t *test = &tests[i];
		rets[i] = test->cb(app);
		if(rets[i] && (rets[i]->lint & app->show) )
			msg = true;
	}

	if(msg)
	{
		for(unsigned i=0; i<tests_n; i++)
		{
			const test_t *test = &tests[i];
			const ret_t *ret = rets[i];

			if(ret)
			{
				switch(ret->lint & app->show)
				{
					case LINT_FAIL:
						fprintf(stdout, "    ["ANSI_COLOR_RED"FAIL"ANSI_COLOR_RESET"]  %s=> %s <%s>\n", test->id, ret->msg, ret->url);
						break;
					case LINT_WARN:
						fprintf(stdout, "    ["ANSI_COLOR_YELLOW"WARN"ANSI_COLOR_RESET"]  %s=> %s <%s>\n", test->id, ret->msg, ret->url);
						break;
					case LINT_NOTE:
						fprintf(stdout, "    ["ANSI_COLOR_CYAN"NOTE"ANSI_COLOR_RESET"]  %s=> %s <%s>\n", test->id, ret->msg, ret->url);
						break;
				}

				if(flag)
					flag = (ret->lint & app->mask) ? false : true;
			}
			else
			{
				//fprintf(stdout, "    ["ANSI_COLOR_GREEN"PASS"ANSI_COLOR_RESET"]  %s\n", test->id);
			}
		}
	}

	const uint32_t num_ports = lilv_plugin_get_num_ports(app->plugin);
	for(unsigned i=0; i<num_ports; i++)
	{
		bool port_flag = true;

		app->port = lilv_plugin_get_port_by_index(app->plugin, i);
		if(app->port)
		{
			if(!test_port(app))
				port_flag = false;
			app->port = NULL;
		}
		else
			port_flag = false;

		if(flag && !port_flag)
			flag = port_flag;
	}

	LilvNodes *writables = lilv_plugin_get_value(app->plugin, app->uris.patch_writable);
	if(writables)
	{
		LILV_FOREACH(nodes, itr, writables)
		{
			bool param_flag = true;

			app->parameter = lilv_nodes_get(writables, itr);
			if(app->parameter)
			{
				if(!test_parameter(app))
					param_flag = false;
				app->parameter = NULL;
			}
			else
				param_flag = false;

			if(flag && !param_flag)
				flag = param_flag;
		}

		lilv_nodes_free(writables);
	}

	LilvNodes *readables = lilv_plugin_get_value(app->plugin, app->uris.patch_readable);
	if(readables)
	{
		LILV_FOREACH(nodes, itr, readables)
		{
			bool param_flag = true;

			app->parameter = lilv_nodes_get(readables, itr);
			if(app->parameter)
			{
				if(!test_parameter(app))
					param_flag = false;
				app->parameter = NULL;
			}
			else
				param_flag = false;

			if(flag && !param_flag)
				flag = param_flag;
		}

		lilv_nodes_free(readables);
	}

	LilvUIs *uis = lilv_plugin_get_uis(app->plugin);
	if(uis)
	{
		LILV_FOREACH(uis, itr, uis)
		{
			bool ui_flag = true;

			app->ui = lilv_uis_get(uis, itr);
			if(app->ui)
			{
				const LilvNode *ui_uri = lilv_ui_get_uri(app->ui);
				lilv_world_load_resource(app->world, ui_uri);

				if(!test_ui(app))
					ui_flag = false;
				app->ui = NULL;

				lilv_world_unload_resource(app->world, ui_uri);
			}
			else
				ui_flag = false;

			if(flag && !ui_flag)
				flag = ui_flag;
		}

		lilv_uis_free(uis);
	}


	fprintf(stdout, "\n");

	return flag;
}
