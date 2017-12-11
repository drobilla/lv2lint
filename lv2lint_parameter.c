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

#include <lv2lint.h>

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/extensions/units/units.h>

static const ret_t *
_test_label(app_t *app)
{
	static const ret_t ret_label_not_found = {
		LINT_FAIL, "rdfs:label not found", LV2_CORE_PREFIX"Parameter"},
	ret_label_not_a_string = {
		LINT_FAIL, "rdfs:label not a string", LILV_NS_DOAP"label"},
	ret_label_empty = {
		LINT_FAIL, "rdfs:label empty", LILV_NS_DOAP"label"};

	const ret_t *ret = NULL;

	LilvNode *label_node = lilv_world_get(app->world, app->parameter, app->uris.rdfs_label, NULL);
	if(label_node)
	{
		if(lilv_node_is_string(label_node))
		{
			const char *label = lilv_node_as_string(label_node);
			if(!label)
			{
				ret = &ret_label_empty;
			}
		}
		else // !is_string
		{
			ret = &ret_label_not_a_string;
		}
		lilv_node_free(label_node);
	}
	else // !label_node
	{
		ret = &ret_label_not_found;
	}

	return ret;
}


static const ret_t *
_test_comment(app_t *app)
{
	static const ret_t ret_comment_not_found = {
		LINT_NOTE, "rdfs:comment not found", LV2_CORE_PREFIX"Parameter"},
	ret_comment_not_a_string = {
		LINT_FAIL, "rdfs:comment not a string", LILV_NS_DOAP"comment"},
	ret_comment_empty = {
		LINT_FAIL, "rdfs:comment empty", LILV_NS_DOAP"comment"};

	const ret_t *ret = NULL;

	LilvNode *comment_node = lilv_world_get(app->world, app->parameter, app->uris.rdfs_comment, NULL);
	if(comment_node)
	{
		if(lilv_node_is_string(comment_node))
		{
			const char *comment = lilv_node_as_string(comment_node);
			if(!comment)
			{
				ret = &ret_comment_empty;
			}
		}
		else // !is_string
		{
			ret = &ret_comment_not_a_string;
		}
		lilv_node_free(comment_node);
	}
	else // !comment_node
	{
		ret = &ret_comment_not_found;
	}

	return ret;
}

static const ret_t *
_test_range(app_t *app)
{
	static const ret_t ret_range_not_found = {
		LINT_FAIL, "rdfs:range not found", LV2_CORE_PREFIX"Parameter"},
	ret_range_not_a_uri = {
		LINT_FAIL, "rdfs:range not a URI", LILV_NS_DOAP"range"},
	ret_range_not_an_atom = {
		LINT_WARN, "rdfs:range not an lv2:Atom", LV2_ATOM__Atom},
	ret_range_empty = {
		LINT_FAIL, "rdfs:range empty", LILV_NS_DOAP"range"},
	ret_range_minimum_not_found = {
		LINT_WARN, "lv2:minimum not found", LV2_CORE__minimum},
	ret_range_minimum_not_an_int = {
		LINT_FAIL, "lv2:minimum not an integer", LV2_CORE__minimum},
	ret_range_minimum_not_a_float = {
		LINT_FAIL, "lv2:minimum not a float", LV2_CORE__minimum},
	ret_range_maximum_not_found = {
		LINT_WARN, "lv2:maximum not found", LV2_CORE__maximum},
	ret_range_maximum_not_an_int = {
		LINT_FAIL, "lv2:maximum not an integer", LV2_CORE__maximum},
	ret_range_maximum_not_a_float = {
		LINT_FAIL, "lv2:maximum not a float", LV2_CORE__maximum},
	ret_range_invalid = {
		LINT_FAIL, "range invalid (min <= max)", LV2_CORE_PREFIX"Parameter"};

	const ret_t *ret = NULL;

	LilvNode *range_node = lilv_world_get(app->world, app->parameter, app->uris.rdfs_range, NULL);
	if(range_node)
	{
		if(lilv_node_is_uri(range_node))
		{
			const char *range = lilv_node_as_uri(range_node);
			if(!range)
			{
				ret = &ret_range_empty;
			}
			else
			{
				if(  lilv_node_equals(range_node, app->uris.atom_Int)
					|| lilv_node_equals(range_node, app->uris.atom_Long)
					|| lilv_node_equals(range_node, app->uris.atom_Float)
					|| lilv_node_equals(range_node, app->uris.atom_Double) )
				{
					LilvNode *minimum = lilv_world_get(app->world, app->parameter, app->uris.lv2_minimum, NULL);
					if(minimum)
					{
						if(  lilv_node_equals(range_node, app->uris.atom_Int)
							|| lilv_node_equals(range_node, app->uris.atom_Long) )
						{
							if(!lilv_node_is_int(minimum))
							{
								ret = &ret_range_minimum_not_an_int;
								app->min.i64 = 0; // fall-back
							}
							else
							{
								app->min.i64 = lilv_node_as_int(minimum);
							}
						}
						else if(lilv_node_equals(range_node, app->uris.atom_Float)
							|| lilv_node_equals(range_node, app->uris.atom_Double) )
						{
							if(!lilv_node_is_float(minimum))
							{
								ret = &ret_range_minimum_not_a_float;
								app->min.f64 = 0.0; // fall-back
							}
							else
							{
								app->min.f64 = lilv_node_as_float(minimum);
							}
						}

						lilv_node_free(minimum);
					}
					else
					{
						ret = &ret_range_minimum_not_found;
					}

					LilvNode *maximum = lilv_world_get(app->world, app->parameter, app->uris.lv2_maximum, NULL);
					if(maximum)
					{
						if(  lilv_node_equals(range_node, app->uris.atom_Int)
							|| lilv_node_equals(range_node, app->uris.atom_Long) )
						{
							if(!lilv_node_is_int(maximum))
							{
								ret = &ret_range_maximum_not_an_int;
								app->max.i64 = 1; // fall-back
							}
							else
							{
								app->max.i64 = lilv_node_as_int(maximum);
							}
						}
						else if(lilv_node_equals(range_node, app->uris.atom_Float)
							|| lilv_node_equals(range_node, app->uris.atom_Double) )
						{
							if(!lilv_node_is_float(maximum))
							{
								ret = &ret_range_maximum_not_a_float;
								app->max.f64 = 1.0; // fall-back
							}
							else
							{
								app->max.f64 = lilv_node_as_float(maximum);
							}
						}

						lilv_node_free(maximum);
					}
					else
					{
						ret = &ret_range_maximum_not_found;
					}

					if(minimum && maximum)
					{
						if(  lilv_node_equals(range_node, app->uris.atom_Int)
							|| lilv_node_equals(range_node, app->uris.atom_Long) )
						{
							if( !(app->min.i64 <= app->max.i64) )
							{
								ret = &ret_range_invalid;
							}
						}
						else if(lilv_node_equals(range_node, app->uris.atom_Float)
							|| lilv_node_equals(range_node, app->uris.atom_Double) )
						{
							if( !(app->min.f64 <= app->max.f64) )
							{
								ret = &ret_range_invalid;
							}
						}
					}
				}
				else if(lilv_node_equals(range_node, app->uris.atom_Bool)
					|| lilv_node_equals(range_node, app->uris.atom_String)
					|| lilv_node_equals(range_node, app->uris.atom_Literal)
					|| lilv_node_equals(range_node, app->uris.atom_Path)
					|| lilv_node_equals(range_node, app->uris.atom_Chunk)
					|| lilv_node_equals(range_node, app->uris.atom_URI)
					|| lilv_node_equals(range_node, app->uris.atom_URID)
					|| lilv_node_equals(range_node, app->uris.atom_Tuple)
					|| lilv_node_equals(range_node, app->uris.atom_Object)
					|| lilv_node_equals(range_node, app->uris.atom_Vector)
					|| lilv_node_equals(range_node, app->uris.atom_Sequence) )
				{
					// OK
				}
				else
				{
					ret = &ret_range_not_an_atom;
				}
			}
		}
		else // !is_string
		{
			ret = &ret_range_not_a_uri;
		}
		lilv_node_free(range_node);
	}
	else // !range_node
	{
		ret = &ret_range_not_found;
	}

	return ret;
}

static const ret_t *
_test_unit(app_t *app)
{
	static const ret_t ret_units_unit_not_found = {
		LINT_NOTE, "units:unit not found", LV2_UNITS__unit},
	ret_units_unit_not_a_uri = {
		LINT_FAIL, "units_unit not a URI", LV2_UNITS__unit};

	const ret_t *ret = NULL;


	LilvNode *unit = lilv_world_get(app->world, app->parameter, app->uris.units_unit, NULL);
	if(unit)
	{
		if(!lilv_node_is_uri(unit))
		{
			ret = &ret_units_unit_not_a_uri;
		}

		lilv_node_free(unit);
	}
	else
	{
		ret = &ret_units_unit_not_found;
	}

	return ret;
}

static const test_t tests [] = {
	{"Label           ", _test_label},
	{"Comment         ", _test_comment},
	{"Range           ", _test_range},
	{"Unit            ", _test_unit},
	//TODO scalePoint
};

static const unsigned tests_n = sizeof(tests) / sizeof(test_t);

bool
test_parameter(app_t *app)
{
	bool flag = true;
	bool msg = false;
	res_t rets [tests_n];

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

	const bool show_passes = LINT_PASS & app->show;

	if(msg || show_passes)
	{
		lv2lint_printf(app, "  %s<%s>%s\n",
			colors[app->atty][ANSI_COLOR_BOLD],
			lilv_node_as_uri(app->parameter),
			colors[app->atty][ANSI_COLOR_RESET]);

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
						lv2lint_printf(app, "    [%sFAIL%s]  %s=> %s <%s>\n",
							colors[app->atty][ANSI_COLOR_RED], colors[app->atty][ANSI_COLOR_RESET],
							test->id, repl ? repl : ret->msg, ret->url);
						break;
					case LINT_WARN:
						lv2lint_printf(app, "    [%sWARN%s]  %s=> %s <%s>\n",
							colors[app->atty][ANSI_COLOR_YELLOW], colors[app->atty][ANSI_COLOR_RESET],
							test->id, repl ? repl : ret->msg, ret->url);
						break;
					case LINT_NOTE:
						lv2lint_printf(app, "    [%sNOTE%s]  %s=> %s <%s>\n",
							colors[app->atty][ANSI_COLOR_CYAN], colors[app->atty][ANSI_COLOR_RESET],
							test->id, repl ? repl : ret->msg, ret->url);
						break;
				}

				if(repl)
					free(repl);

				if(flag)
					flag = (ret->lint & app->mask) ? false : true;
			}
			else if(show_passes)
			{
				lv2lint_printf(app, "    [%sPASS%s]  %s\n",
					colors[app->atty][ANSI_COLOR_GREEN], colors[app->atty][ANSI_COLOR_RESET],
					test->id);
			}
		}
	}

	return flag;
}
