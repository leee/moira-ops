# $Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/clients/mmoira/menus.awk,v 1.2 1991-06-05 13:17:32 mar Exp $

BEGIN		{ numfields = 0; nummenus = 0;
		  printf("/* This file is machine generated, do not edit. */");
		  printf("\n#include <Xm/Text.h>\n");
		  printf("#include \"mmoira.h\"\n\n");
		}

/^;/		{ next }
NF == 0		{ next }

$1 == "="	{ if (menuname != "") {
			menuno = menumap[menuname] = nummenus++;
			printf("static MenuItem *menu%dsub[] = {\n", menuno);
			for (i = 0; i < count; i++)
				printf("\t&menu%d,\n", save[i]);
			printf("\t(MenuItem *) NULL\n};\n");
			printf("static MenuItem menu%d = { \"%s\", menu%dsub };\n", \
			menuno, menuname, menuno);
		  }

		  menuname = $2;
		  for (i = 3; i <= NF; i++)
			menuname = sprintf("%s %s", menuname, $i);
		  count = 0;
		  next
		}

$NF == "}"	{ itemname = $1;
		  for (i = 2; i <= NF && $i != "{"; i++)
			itemname = sprintf("%s %s", itemname, $i);
		  menuno = nummenus++;
		  printf("static MenuItem menu%d = { \"%s\", NULL, %s \"%s\", \"%s\", %d };\n", \
			menuno, itemname, \
			$(i + 1), $(i + 2), $(i + 3), $(i + 4));
		  save[count++] = menuno;
		  next
		}

		{ itemname = $1;
		  for (i = 2; i <= NF && $i != "{"; i++)
			itemname = sprintf("%s %s", itemname, $i);
		  save[count++] = menumap[itemname];
		  next
		}

END		{ menuno = menumap[menuname] = nummenus++;
		  printf("static MenuItem *menu%dsub[] = {\n", menuno);
		  for (i = 0; i < count; i++)
			printf("\t&menu%d,\n", save[i]);
		  printf("\t(MenuItem *) NULL\n};\n");
		  printf("MenuItem MenuRoot = { \"%s\", menu%dsub };\n", \
			menuname, menuno);
		}