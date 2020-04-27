#include "defs.h"
#include "data.h"
#include "decl.h"

// Parsin of declarations
// Copyright (c) 2019 Warren Toomey, GPL3




// Parse the declaration of a variable
void var_declaration(void)
{
	// Ensure we have an 'int' token followed by an identifier
	// and a semicolon. Text now has the identifier's name.
	// Add it as a know identifier
	match(T_INT, "int");
	ident();
	addlob(Text);
	genglobsym(Text);
	semi();
}
