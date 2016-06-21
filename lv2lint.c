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

#include <lv2lint.h>

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
						if(!test_plugin(&app))
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
