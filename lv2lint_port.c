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

#include <lv2/lv2plug.in/ns/ext/event/event.h>
#include <lv2/lv2plug.in/ns/ext/port-groups/port-groups.h>

enum {
	CLASS_NOT_VALID,
};

static const ret_t ret_class [] = {
	[CLASS_NOT_VALID]         = {LINT_FAIL, "lv2:Port class not valid", LV2_CORE__Port},
};

static const ret_t *
_test_class(app_t *app)
{
	const ret_t *ret = NULL;


	LilvNodes *class = lilv_world_find_nodes(app->world,
		NULL, app->uris.rdfs_subClassOf, app->uris.lv2_Port);
	if(class)
	{
		const LilvNodes *supported= lilv_port_get_classes(app->plugin, app->port);
		if(supported)
		{
			LILV_FOREACH(nodes, itr, supported)
			{
				const LilvNode *node = lilv_nodes_get(supported, itr);

				if(!lilv_nodes_contains(class, node))
				{
					ret = &ret_class[CLASS_NOT_VALID];
					break;
				}
			}
		}

		lilv_nodes_free(class);
	}

	return ret;
}

enum {
	PROPERTIES_NOT_VALID,
};

static const ret_t ret_properties [] = {
	[PROPERTIES_NOT_VALID]         = {LINT_FAIL, "lv2:portProperty not valid", LV2_CORE__portProperty},
};

static const ret_t *
_test_properties(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNodes *properties = lilv_world_find_nodes(app->world,
		NULL, app->uris.rdf_type, app->uris.lv2_PortProperty);
	if(properties)
	{
		LilvNodes *supported = lilv_port_get_properties(app->plugin, app->port);
		if(supported)
		{
			LILV_FOREACH(nodes, itr, supported)
			{
				const LilvNode *node = lilv_nodes_get(supported, itr);

				if(!lilv_nodes_contains(properties, node))
				{
					ret = &ret_properties[PROPERTIES_NOT_VALID];
					break;
				}
			}

			lilv_nodes_free(supported);
		}

		lilv_nodes_free(properties);
	}

	return ret;
}

enum {
	RANGE_NOT_FOUND,
	RANGE_NOT_AN_INT,
	RANGE_NOT_A_FLOAT,
	RANGE_NOT_A_BOOL,
};

static inline const ret_t *
_test_range(LilvNode *node, const ret_t *rets, bool is_integer, bool is_toggled)
{
	const ret_t *ret = NULL;

	if(node)
	{
		if(is_integer)
		{
			if(  lilv_node_is_int(node)
				|| (lilv_node_is_float(node)
					&& (rintf(lilv_node_as_float(node)) == lilv_node_as_float(node))) )
			{
				// OK
			}
			else
			{
				ret = &rets[RANGE_NOT_AN_INT];
			}
		}
		else if(is_toggled)
		{
			if(  lilv_node_is_bool(node)
				|| (lilv_node_is_int(node)
					&& ((lilv_node_as_int(node) == 0) || (lilv_node_as_int(node) == 1)))
				|| (lilv_node_is_float(node)
					&& ((lilv_node_as_float(node) == 0.f) || (lilv_node_as_float(node) == 1.f))) )
			{
				// OK
			}
			else
			{
				ret = &rets[RANGE_NOT_A_BOOL];
			}
		}
		else if( lilv_node_is_float(node)
					|| lilv_node_is_int(node) )
		{
			// OK
		}
		else
		{
			ret = &rets[RANGE_NOT_A_FLOAT];
		}

		lilv_node_free(node);
	}
	else // !node
	{
		ret = &rets[RANGE_NOT_FOUND];
	}

	return ret;
}

static const ret_t ret_default [] = {
	[RANGE_NOT_FOUND]      = {LINT_WARN, "lv2:default not found", LV2_CORE__Port},
		[RANGE_NOT_AN_INT]   = {LINT_WARN, "lv2:default not an integer", LV2_CORE__default},
		[RANGE_NOT_A_FLOAT]  = {LINT_WARN, "lv2:default not a float", LV2_CORE__default},
		[RANGE_NOT_A_BOOL]   = {LINT_WARN, "lv2:default not a bool", LV2_CORE__default},
};

static const ret_t *
_test_default(app_t *app)
{
	const ret_t *ret = NULL;

	const bool is_integer = lilv_port_has_property(app->plugin, app->port, app->uris.lv2_integer);
	const bool is_toggled = lilv_port_has_property(app->plugin, app->port, app->uris.lv2_toggled);

	if(  (lilv_port_is_a(app->plugin, app->port, app->uris.lv2_ControlPort)
			|| lilv_port_is_a(app->plugin, app->port, app->uris.lv2_CVPort))
		&& lilv_port_is_a(app->plugin, app->port, app->uris.lv2_InputPort) )
	{
		LilvNode *default_node = lilv_port_get(app->plugin, app->port, app->uris.lv2_default);
		ret = _test_range(default_node, ret_default, is_integer, is_toggled);
	}

	return ret;
}

static const ret_t ret_minimum [] = {
	[RANGE_NOT_FOUND]      = {LINT_WARN, "lv2:minimum not found", LV2_CORE__Port},
		[RANGE_NOT_AN_INT]   = {LINT_WARN, "lv2:minimum not an integer", LV2_CORE__minimum},
		[RANGE_NOT_A_FLOAT]  = {LINT_WARN, "lv2:minimum not a float", LV2_CORE__minimum},
		[RANGE_NOT_A_BOOL]   = {LINT_WARN, "lv2:minimum not a bool", LV2_CORE__minimum},
};

static const ret_t *
_test_minimum(app_t *app)
{
	const ret_t *ret = NULL;

	const bool is_integer = lilv_port_has_property(app->plugin, app->port, app->uris.lv2_integer);
	const bool is_toggled = lilv_port_has_property(app->plugin, app->port, app->uris.lv2_toggled);

	if(  (lilv_port_is_a(app->plugin, app->port, app->uris.lv2_ControlPort)
			|| lilv_port_is_a(app->plugin, app->port, app->uris.lv2_CVPort))
		&& lilv_port_is_a(app->plugin, app->port, app->uris.lv2_InputPort)
		&& !is_toggled )
	{
		LilvNode *minimum_node = lilv_port_get(app->plugin, app->port, app->uris.lv2_minimum);
		ret = _test_range(minimum_node, ret_minimum, is_integer, is_toggled);
	}

	return ret;
}

static const ret_t ret_maximum [] = {
	[RANGE_NOT_FOUND]      = {LINT_WARN, "lv2:maximum not found", LV2_CORE__Port},
		[RANGE_NOT_AN_INT]   = {LINT_WARN, "lv2:maximum not an integer", LV2_CORE__maximum},
		[RANGE_NOT_A_FLOAT]  = {LINT_WARN, "lv2:maximum not a float", LV2_CORE__maximum},
		[RANGE_NOT_A_BOOL]   = {LINT_WARN, "lv2:maximum not a bool", LV2_CORE__maximum},
};

static const ret_t *
_test_maximum(app_t *app)
{
	const ret_t *ret = NULL;

	const bool is_integer = lilv_port_has_property(app->plugin, app->port, app->uris.lv2_integer);
	const bool is_toggled = lilv_port_has_property(app->plugin, app->port, app->uris.lv2_toggled);

	if(  (lilv_port_is_a(app->plugin, app->port, app->uris.lv2_ControlPort)
			|| lilv_port_is_a(app->plugin, app->port, app->uris.lv2_CVPort))
		&& lilv_port_is_a(app->plugin, app->port, app->uris.lv2_InputPort)
		&& !is_toggled )
	{
		LilvNode *maximum_node = lilv_port_get(app->plugin, app->port, app->uris.lv2_maximum);
		ret = _test_range(maximum_node, ret_maximum, is_integer, is_toggled);
	}

	return ret;
}

enum {
	EVENT_PORT_DEPRECATED,
};

static const ret_t ret_event_port [] = {
	[EVENT_PORT_DEPRECATED]         = {LINT_FAIL, "lv2:EventPort is deprecated, use atom:AtomPort instead", LV2_EVENT__EventPort},
};

static const ret_t *
_test_event_port(app_t *app)
{
	const ret_t *ret = NULL;

	if(lilv_port_is_a(app->plugin, app->port, app->uris.event_EventPort))
	{
		ret = &ret_event_port[EVENT_PORT_DEPRECATED];
	}

	return ret;
}

enum {
	COMMENT_NOT_FOUND,
	COMMENT_NOT_A_STRING,
};

static const ret_t ret_comment [] = {
	[COMMENT_NOT_FOUND]         = {LINT_NOTE, "rdfs:comment not found", LILV_NS_RDFS"comment"},
	[COMMENT_NOT_A_STRING]      = {LINT_FAIL, "rdfs:comment not a string", LILV_NS_RDFS"comment"},
};

static const ret_t *
_test_comment(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *comment = lilv_port_get(app->plugin, app->port, app->uris.rdfs_comment);
	if(comment)
	{
		if(!lilv_node_is_string(comment))
		{
			ret = &ret_comment[COMMENT_NOT_A_STRING];
		}

		lilv_node_free(comment);
	}
	else
	{
		ret = &ret_comment[COMMENT_NOT_FOUND];
	}

	return ret;
}

enum {
	GROUP_NOT_FOUND,
	GROUP_NOT_A_URI,
};

static const ret_t ret_group [] = {
	[GROUP_NOT_FOUND]         = {LINT_NOTE, "pg:group not found", LV2_PORT_GROUPS__group},
	[GROUP_NOT_A_URI]         = {LINT_FAIL, "pg:group not a URI", LV2_PORT_GROUPS__group},
};

static const ret_t *
_test_group(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *group = lilv_port_get(app->plugin, app->port, app->uris.pg_group);
	if(group)
	{
		if(!lilv_node_is_uri(group))
		{
			ret = &ret_group[GROUP_NOT_A_URI];
		}

		lilv_node_free(group);
	}
	else
	{
		ret = &ret_group[GROUP_NOT_FOUND];
	}

	return ret;
}

static const test_t tests [] = {
	{"Class           ", _test_class},
	{"PortProperties  ", _test_properties},
	{"Default         ", _test_default},
	{"Minimum         ", _test_minimum},
	{"Maximum         ", _test_maximum},
	{"Event Port      ", _test_event_port},
	{"Comment         ", _test_comment},
	{"Group           ", _test_group},
};

static const int tests_n = sizeof(tests) / sizeof(test_t);

bool
test_port(app_t *app)
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
		fprintf(stdout, "  %s{%d : %s}%s\n",
			colors[atty][ANSI_COLOR_BOLD],
			lilv_port_get_index(app->plugin, app->port),
			lilv_node_as_string(lilv_port_get_symbol(app->plugin, app->port)),
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
