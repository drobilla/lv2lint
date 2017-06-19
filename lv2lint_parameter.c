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

enum {
	LABEL_NOT_FOUND,
	LABEL_NOT_A_STRING,
	LABEL_EMPTY
};

static const ret_t ret_label [] = {
	[LABEL_NOT_FOUND]      = {LINT_FAIL, "rdfs:label not found", LV2_CORE_PREFIX"Parameter"},
		[LABEL_NOT_A_STRING] = {LINT_FAIL, "rdfs:label not a string", LILV_NS_DOAP"label"},
		[LABEL_EMPTY]        = {LINT_FAIL, "rdfs:label empty", LILV_NS_DOAP"label"}
};

static const ret_t *
_test_label(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *label_node = lilv_world_get(app->world, app->parameter, app->uris.rdfs_label, NULL);
	if(label_node)
	{
		if(lilv_node_is_string(label_node))
		{
			const char *label = lilv_node_as_string(label_node);
			if(!label)
			{
				ret = &ret_label[LABEL_EMPTY];
			}
		}
		else // !is_string
		{
			ret = &ret_label[LABEL_NOT_A_STRING];
		}
		lilv_node_free(label_node);
	}
	else // !label_node
	{
		ret = &ret_label[LABEL_NOT_FOUND];
	}

	return ret;
}

enum {
	COMMENT_NOT_FOUND,
	COMMENT_NOT_A_STRING,
	COMMENT_EMPTY
};

static const ret_t ret_comment [] = {
	[COMMENT_NOT_FOUND]      = {LINT_NOTE, "rdfs:comment not found", LV2_CORE_PREFIX"Parameter"},
		[COMMENT_NOT_A_STRING] = {LINT_FAIL, "rdfs:comment not a string", LILV_NS_DOAP"comment"},
		[COMMENT_EMPTY]        = {LINT_FAIL, "rdfs:comment empty", LILV_NS_DOAP"comment"}
};

static const ret_t *
_test_comment(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *comment_node = lilv_world_get(app->world, app->parameter, app->uris.rdfs_comment, NULL);
	if(comment_node)
	{
		if(lilv_node_is_string(comment_node))
		{
			const char *comment = lilv_node_as_string(comment_node);
			if(!comment)
			{
				ret = &ret_comment[COMMENT_EMPTY];
			}
		}
		else // !is_string
		{
			ret = &ret_comment[COMMENT_NOT_A_STRING];
		}
		lilv_node_free(comment_node);
	}
	else // !comment_node
	{
		ret = &ret_comment[COMMENT_NOT_FOUND];
	}

	return ret;
}

enum {
	RANGE_NOT_FOUND,
	RANGE_NOT_A_URI,
	RANGE_NOT_AN_ATOM,
	RANGE_EMPTY,
	RANGE_MINIMUM_NOT_FOUND,
	RANGE_MINIMUM_NOT_AN_INT,
	RANGE_MINIMUM_NOT_A_FLOAT,
	RANGE_MAXIMUM_NOT_FOUND,
	RANGE_MAXIMUM_NOT_AN_INT,
	RANGE_MAXIMUM_NOT_A_FLOAT,
};

static const ret_t ret_range [] = {
	[RANGE_NOT_FOUND]               = {LINT_FAIL, "rdfs:range not found", LV2_CORE_PREFIX"Parameter"},
		[RANGE_NOT_A_URI]             = {LINT_FAIL, "rdfs:range not a URI", LILV_NS_DOAP"range"},
		[RANGE_NOT_AN_ATOM]           = {LINT_WARN, "rdfs:range not an lv2:Atom", LV2_ATOM__Atom},
		[RANGE_EMPTY]                 = {LINT_FAIL, "rdfs:range empty", LILV_NS_DOAP"range"},
		[RANGE_MINIMUM_NOT_FOUND]     = {LINT_WARN, "lv2:minimum not found", LV2_CORE__minimum},
			[RANGE_MINIMUM_NOT_AN_INT]  = {LINT_FAIL, "lv2:minimum not an integer", LV2_CORE__minimum},
			[RANGE_MINIMUM_NOT_A_FLOAT] = {LINT_FAIL, "lv2:minimum not a float", LV2_CORE__minimum},
		[RANGE_MAXIMUM_NOT_FOUND]     = {LINT_WARN, "lv2:maximum not found", LV2_CORE__maximum},
			[RANGE_MAXIMUM_NOT_AN_INT]  = {LINT_FAIL, "lv2:maximum not an integer", LV2_CORE__maximum},
			[RANGE_MAXIMUM_NOT_A_FLOAT] = {LINT_FAIL, "lv2:maximum not a float", LV2_CORE__maximum},
};

static const ret_t *
_test_range(app_t *app)
{
	const ret_t *ret = NULL;

	LilvNode *range_node = lilv_world_get(app->world, app->parameter, app->uris.rdfs_range, NULL);
	if(range_node)
	{
		if(lilv_node_is_uri(range_node))
		{
			const char *range = lilv_node_as_uri(range_node);
			if(!range)
			{
				ret = &ret_range[RANGE_EMPTY];
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
								ret = &ret_range[RANGE_MINIMUM_NOT_AN_INT];
							}
						}
						else if(lilv_node_equals(range_node, app->uris.atom_Float)
							|| lilv_node_equals(range_node, app->uris.atom_Double) )
						{
							if(!lilv_node_is_float(minimum))
							{
								ret = &ret_range[RANGE_MINIMUM_NOT_A_FLOAT];
							}
						}

						lilv_node_free(minimum);
					}
					else
					{
						ret = &ret_range[RANGE_MINIMUM_NOT_FOUND];
					}

					LilvNode *maximum = lilv_world_get(app->world, app->parameter, app->uris.lv2_maximum, NULL);
					if(maximum)
					{
						if(  lilv_node_equals(range_node, app->uris.atom_Int)
							|| lilv_node_equals(range_node, app->uris.atom_Long) )
						{
							if(!lilv_node_is_int(maximum))
							{
								ret = &ret_range[RANGE_MAXIMUM_NOT_AN_INT];
							}
						}
						else if(lilv_node_equals(range_node, app->uris.atom_Float)
							|| lilv_node_equals(range_node, app->uris.atom_Double) )
						{
							if(!lilv_node_is_float(maximum))
							{
								ret = &ret_range[RANGE_MAXIMUM_NOT_A_FLOAT];
							}
						}

						lilv_node_free(maximum);
					}
					else
					{
						ret = &ret_range[RANGE_MAXIMUM_NOT_FOUND];
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
					ret = &ret_range[RANGE_NOT_AN_ATOM];
				}
			}
		}
		else // !is_string
		{
			ret = &ret_range[RANGE_NOT_A_URI];
		}
		lilv_node_free(range_node);
	}
	else // !range_node
	{
		ret = &ret_range[RANGE_NOT_FOUND];
	}

	return ret;
}

static const test_t tests [] = {
	{"Label           ", _test_label},
	{"Comment         ", _test_comment},
	{"Range           ", _test_range},
	//TODO scalePoint
};

static const int tests_n = sizeof(tests) / sizeof(test_t);

bool
test_parameter(app_t *app)
{
	const bool atty = isatty(1);
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

	if(msg)
	{
		fprintf(stdout, "  %s<%s>%s\n",
			colors[atty][ANSI_COLOR_BOLD],
			lilv_node_as_uri(app->parameter),
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

	return flag;
}
