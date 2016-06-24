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

	LilvNode *rdf_type = lilv_new_uri(app->world, LILV_NS_RDF"type");
	LilvNode *lv2_Feature = lilv_new_uri(app->world, LV2_CORE__Feature);

	LilvNodes *features = lilv_world_find_nodes(app->world,
		NULL, rdf_type, lv2_Feature);
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

	lilv_node_free(rdf_type);
	lilv_node_free(lv2_Feature);

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

	LilvNode *rdf_type = lilv_new_uri(app->world, LILV_NS_RDF"type");
	LilvNode *lv2_ExtensionData = lilv_new_uri(app->world, LV2_CORE__ExtensionData);

	LilvNodes *extensions = lilv_world_find_nodes(app->world,
		NULL, rdf_type, lv2_ExtensionData);
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

	lilv_node_free(rdf_type);
	lilv_node_free(lv2_ExtensionData);

	return ret;
}

enum {
	MIXED_NOT_VALID,
};

static const ret_t ret_mixed [] = {
	[MIXED_NOT_VALID]         = {LINT_WARN, "plugin mixes DSP and UI code in same binary", LV2_UI_PREFIX},
};

static const ret_t *
_test_mixed(app_t *app)
{
	const ret_t *ret = NULL;

	const LilvNode *library_uri = lilv_plugin_get_library_uri(app->plugin);

	LilvUIs *all_uis = lilv_plugin_get_uis(app->plugin);
	if(all_uis)
	{
		LILV_FOREACH(uis, ptr, all_uis)
		{
			const LilvUI *ui = lilv_uis_get(all_uis, ptr);
			if(!ui)
				continue;

			const LilvNode *ui_uri_node = lilv_ui_get_uri(ui);
			if(!ui_uri_node)
				continue;

			// nedded if ui ttl referenced via rdfs#seeAlso
			lilv_world_load_resource(app->world, ui_uri_node);

			const LilvNode *ui_library_uri= lilv_ui_get_binary_uri(ui);
			if(ui_library_uri && lilv_node_equals(library_uri, ui_library_uri))
			{
				ret = &ret_mixed[MIXED_NOT_VALID];
			}

			lilv_world_unload_resource(app->world, ui_uri_node);
		}

		lilv_uis_free(all_uis);
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
	{"Mixed DSP/UI    ", _test_mixed},
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
		if(rets[i])
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

	const uint32_t num_ports = lilv_plugin_get_num_ports(app->plugin);
	for(unsigned i=0; i<num_ports; i++)
	{
		app->port = lilv_plugin_get_port_by_index(app->plugin, i);
		if(app->port)
		{
			if(!test_port(app))
				flag = false;
			app->port = NULL;
		}
		else
			flag = false;
	}

	LilvNode *patch_writable = lilv_new_uri(app->world, LV2_PATCH__writable);
	LilvNode *patch_readable = lilv_new_uri(app->world, LV2_PATCH__readable);

	LilvNodes *writables = lilv_plugin_get_value(app->plugin, patch_writable);
	if(writables)
	{
		LILV_FOREACH(nodes, itr, writables)
		{
			app->parameter = lilv_nodes_get(writables, itr);
			if(app->parameter)
			{
				if(!test_parameter(app))
					flag = false;
				app->parameter = NULL;
			}
			else
				flag = false;
		}

		lilv_nodes_free(writables);
	}

	LilvNodes *readables = lilv_plugin_get_value(app->plugin, patch_readable);
	if(readables)
	{
		LILV_FOREACH(nodes, itr, readables)
		{
			app->parameter = lilv_nodes_get(readables, itr);
			if(app->parameter)
			{
				if(!test_parameter(app))
					flag = false;
				app->parameter = NULL;
			}
			else
				flag = false;
		}

		lilv_nodes_free(readables);
	}

	lilv_node_free(patch_writable);
	lilv_node_free(patch_readable);

	LilvUIs *uis = lilv_plugin_get_uis(app->plugin);
	if(uis)
	{
		LILV_FOREACH(uis, itr, uis)
		{
			app->ui = lilv_uis_get(uis, itr);
			if(app->ui)
			{
				const LilvNode *ui_uri = lilv_ui_get_uri(app->ui);
				lilv_world_load_resource(app->world, ui_uri);

				if(!test_ui(app))
					flag = false;
				app->ui = NULL;

				lilv_world_unload_resource(app->world, ui_uri);
			}
			else
				flag = false;
		}

		lilv_uis_free(uis);
	}


	fprintf(stdout, "\n");

	return flag;
}
