PC	[^\"]
AN	[A-Z_a-z0-9]
%%

error_table	return ERROR_TABLE;
et		return ERROR_TABLE;
error_code	return ERROR_CODE_ENTRY;
ec		return ERROR_CODE_ENTRY;
end		return END;

[\t\n ]		;

\"{PC}*\"	{ register char *p; yylval.dynstr = ds(yytext+1);
		  if (p=rindex(yylval.dynstr, '"')) *p='\0';
		  return QUOTED_STRING;
		}

{AN}*	{ yylval.dynstr = ds(yytext); return STRING; }

#.*\n		;

.		{ return (*yytext); }
%%
#ifndef lint
static char rcsid_et_lex_lex_l[] = "$Header: /afs/.athena.mit.edu/astaff/project/moiradev/repository/moira/util/et/et_lex.lex.l,v 1.1 1996-07-08 22:29:12 danw Exp $";
#endif