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

static const test_t tests [] = {
	/*
	{"Label", _test_label},
	{"Range", _test_range},
	{"Comment", _test_comment},
	{"Minimum", _test_minimum},
	{"Maximum", _test_maximum},
	*/

	{NULL, NULL}
};

bool
test_port(app_t *app)
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
			fprintf(stdout, "    ["ANSI_COLOR_GREEN"PASS"ANSI_COLOR_RESET"]  %s\n", test->id);
		}
	}

	return flag;
}
