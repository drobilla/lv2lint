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

	LilvNode *rdfs_label = lilv_new_uri(app->world, LILV_NS_RDFS"label");

	LilvNode *label_node = lilv_world_get(app->world, app->parameter, rdfs_label, NULL);
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

	lilv_node_free(rdfs_label);

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

	LilvNode *rdfs_comment = lilv_new_uri(app->world, LILV_NS_RDFS"comment");

	LilvNode *comment_node = lilv_world_get(app->world, app->parameter, rdfs_comment, NULL);
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

	lilv_node_free(rdfs_comment);

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

	LilvNode *rdfs_range = lilv_new_uri(app->world, LILV_NS_RDFS"range");
	LilvNode *lv2_minimum = lilv_new_uri(app->world, LV2_CORE__minimum);
	LilvNode *lv2_maximum = lilv_new_uri(app->world, LV2_CORE__maximum);

	LilvNode *range_node = lilv_world_get(app->world, app->parameter, rdfs_range, NULL);
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
				LilvNode *atom_Bool = lilv_new_uri(app->world, LV2_ATOM__Bool);
				LilvNode *atom_Int = lilv_new_uri(app->world, LV2_ATOM__Int);
				LilvNode *atom_Long = lilv_new_uri(app->world, LV2_ATOM__Long);
				LilvNode *atom_Float = lilv_new_uri(app->world, LV2_ATOM__Float);
				LilvNode *atom_Double = lilv_new_uri(app->world, LV2_ATOM__Double);
				LilvNode *atom_String = lilv_new_uri(app->world, LV2_ATOM__String);
				LilvNode *atom_Literal = lilv_new_uri(app->world, LV2_ATOM__Literal);
				LilvNode *atom_Path = lilv_new_uri(app->world, LV2_ATOM__Path);
				LilvNode *atom_Chunk = lilv_new_uri(app->world, LV2_ATOM__Chunk);
				LilvNode *atom_URI = lilv_new_uri(app->world, LV2_ATOM__URI);
				LilvNode *atom_URID = lilv_new_uri(app->world, LV2_ATOM__URID);

				if(  lilv_node_equals(range_node, atom_Int)
					|| lilv_node_equals(range_node, atom_Long)
					|| lilv_node_equals(range_node, atom_Float)
					|| lilv_node_equals(range_node, atom_Double) )
				{
					LilvNode *minimum = lilv_world_get(app->world, app->parameter, lv2_minimum, NULL);
					if(minimum)
					{
						if(  lilv_node_equals(range_node, atom_Int)
							|| lilv_node_equals(range_node, atom_Long) )
						{
							if(!lilv_node_is_int(minimum))
							{
								ret = &ret_range[RANGE_MINIMUM_NOT_AN_INT];
							}
						}
						else if(lilv_node_equals(range_node, atom_Float)
							|| lilv_node_equals(range_node, atom_Double) )
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

					LilvNode *maximum = lilv_world_get(app->world, app->parameter, lv2_maximum, NULL);
					if(maximum)
					{
						if(  lilv_node_equals(range_node, atom_Int)
							|| lilv_node_equals(range_node, atom_Long) )
						{
							if(!lilv_node_is_int(maximum))
							{
								ret = &ret_range[RANGE_MAXIMUM_NOT_AN_INT];
							}
						}
						else if(lilv_node_equals(range_node, atom_Float)
							|| lilv_node_equals(range_node, atom_Double) )
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
				else if(lilv_node_equals(range_node, atom_Bool)
					|| lilv_node_equals(range_node, atom_String)
					|| lilv_node_equals(range_node, atom_Literal)
					|| lilv_node_equals(range_node, atom_Path)
					|| lilv_node_equals(range_node, atom_Chunk)
					|| lilv_node_equals(range_node, atom_URI)
					|| lilv_node_equals(range_node, atom_URID) )
				{
					// OK
				}
				else
				{
					ret = &ret_range[RANGE_NOT_AN_ATOM];
				}

				lilv_node_free(atom_Bool);
				lilv_node_free(atom_Int);
				lilv_node_free(atom_Long);
				lilv_node_free(atom_Float);
				lilv_node_free(atom_Double);
				lilv_node_free(atom_String);
				lilv_node_free(atom_Literal);
				lilv_node_free(atom_Path);
				lilv_node_free(atom_Chunk);
				lilv_node_free(atom_URI);
				lilv_node_free(atom_URID);
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

	lilv_node_free(rdfs_range);
	lilv_node_free(lv2_minimum);
	lilv_node_free(lv2_maximum);

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
		fprintf(stdout, "  "ANSI_COLOR_BOLD"<%s>"ANSI_COLOR_RESET"\n", lilv_node_as_uri(app->parameter));

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
