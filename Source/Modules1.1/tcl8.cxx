/*******************************************************************************
 * Simplified Wrapper and Interface Generator  (SWIG)
 * 
 * Author : David Beazley
 *
 * Department of Computer Science        
 * University of Chicago
 * 1100 E 58th Street
 * Chicago, IL  60637
 * beazley@cs.uchicago.edu
 *
 * Please read the file LICENSE for the copyright and terms by which SWIG
 * can be used and distributed.
 *******************************************************************************/

static char cvsroot[] = "$Header$";

/***********************************************************************
 * $Header$
 *
 * tcl8.cxx
 *
 * Module for creating Tcl 8.0 native wrapper functions. 
 ***********************************************************************/

#include "mod11.h"
#include "tcl8.h"
#include <ctype.h>

static char *Tcl_config=(char*)"swigtcl.swg";
static char *usage = (char*)"\
Tcl 8.0 Options (available with -tcl)\n\
     -module name    - Set name of module\n\
     -prefix name    - Set a prefix to be appended to all names\n\
     -namespace      - Build module into a Tcl 8 namespace. \n\
     -noobject       - Omit code for object oriented interface.\n\
     -old            - Use old SWIG interface (same as -noobject).\n\n";

static char *ns_name = 0;

static String mod_init;
static String mod_extern;
static String cmd_info;
static String var_info;

// ---------------------------------------------------------------------
// TCL8::parse_args(int argc, char *argv[])
//
// Parse tcl specific command line options
// ---------------------------------------------------------------------

void TCL8::parse_args(int argc, char *argv[]) {
  
  int i = 1;
  sprintf(LibDir,"%s",tcl_path);

  // Look for certain command line options

  for (i = 1; i < argc; i++) {
      if (argv[i]) {
	  if (strcmp(argv[i],"-prefix") == 0) {
	    if (argv[i+1]) {
	      prefix = new char[strlen(argv[i+1])+2];
	      strcpy(prefix, argv[i+1]);
	      Swig_mark_arg(i);
	      Swig_mark_arg(i+1);
	      i++;
	    } else {
	      Swig_arg_error();
	    }
	  } else if (strcmp(argv[i],"-module") == 0) {
	    if (argv[i+1]) {
	      set_module(argv[i+1],0);
	      Swig_mark_arg(i);
	      Swig_mark_arg(i+1);
	      i++;
	    } else {
	      Swig_arg_error();
	    }
	  } else if (strcmp(argv[i],"-namespace") == 0) {
	    nspace = 1;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-old") == 0) {
	    shadow = 0;
	    Swig_mark_arg(i);
          } else if (strcmp(argv[i],"-noobject") == 0) {
	    shadow = 0;
	    Swig_mark_arg(i);
	  } else if (strcmp(argv[i],"-help") == 0) {
	    fputs(usage,stderr);
	  }
      }
  }

  // If a package has been specified, make sure it ends with a '_'

  if (prefix) {
    ns_name = copy_string(prefix);
    if (prefix[strlen(prefix)] != '_') {
      prefix[strlen(prefix)+1] = 0;
      prefix[strlen(prefix)] = '_';
    }
  } else 
    prefix = (char*)"";

  // Create a symbol SWIGTCL

  Preprocessor_define((void *) "SWIGTCL 1",0);
  Preprocessor_define((void *) "SWIGTCL8 1", 0);

  // Set name of typemaps

  typemap_lang = (char*)"tcl8";

}

// ---------------------------------------------------------------------
// void TCL8::parse()
//
// Start parsing an interface file for Tcl.
// ---------------------------------------------------------------------

void TCL8::parse() {

  fprintf(stderr,"Making wrappers for Tcl 8.x\n");

  // Print out TCL specific headers
  
  headers();
  
  // Run the parser
  
  yyparse();

}

// ---------------------------------------------------------------------
// TCL8::set_module(char *mod_name,char **mod_list)
//
// Sets the module name.
// Does nothing if it's already set (so it can be overridden as a command
// line option).
//
// mod_list is a NULL-terminated list of additional modules.  This
// is really only useful when building static executables and other
// things.
//----------------------------------------------------------------------

void TCL8::set_module(char *mod_name, char **mod_list) {

  char temp[256], *c;
  int  i;

  if (module) return;
  
  module = new char[strlen(mod_name)+1];
  strcpy(module,mod_name);

  // Fix capitalization for Tcl 

  c = module;
  while (*c) {
    *c = (char) tolower(*c);
    c++;
  }

  // Now create an initialization function

  sprintf(temp,"%s_Init", module);
  init_name = new char[strlen(temp) + 1];
  strcpy(init_name, temp);
  *init_name = toupper(*init_name);

  if (!ns_name) ns_name = copy_string(module);

  // If namespaces have been specified, set the prefix to the module name

  if ((nspace) && (strlen(prefix) < 1)) {
    prefix = new char[strlen(module)+2];
    strcpy(prefix,module);
    prefix[strlen(module)] = '_';
    prefix[strlen(module)+1] = 0;
  }

  // If additional modules have been specified, create some code for
  // initializing them.

  if (mod_list) {
    i = 0;
    while (mod_list[i]) {
      c = mod_list[i];
      while (*c) {
	*c = (char) tolower(*c);
	c++;
      }
      sprintf(temp,"%s_Init",mod_list[i]);
      temp[0] = toupper(temp[0]);
      
      // Dump out some initialization code
      
      mod_init << tab4 << "if (" << temp << "(" << interp_name << ") == TCL_ERROR) {\n"
	       << tab8 << "return TCL_ERROR;\n"
	       << tab4 << "}\n\n";
      mod_extern << "extern int " << temp << "(Tcl_Interp *);\n";
      i++;
    }
  }
}


// ---------------------------------------------------------------------
// TCL8::set_init(char *iname)
//
// Sets the initialization function name.
// Does nothing if it's already set
//
//----------------------------------------------------------------------

void TCL8::set_init(char *iname) {

  if (init_name) return;
  init_name = new char[strlen(iname)+1];
  strcpy(init_name, iname);

}

// ---------------------------------------------------------------------
// TCL8::headers(void)
//
// Generate the appropriate header files for TCL interface.
// ----------------------------------------------------------------------

void TCL8::headers(void)
{

  emit_banner(f_runtime);
  
  // Include a Tcl configuration file for Unix,Mac,Wintel.

  if (NoInclude) {
    fprintf(f_runtime,"#define SWIG_NOINCLUDE\n");
  }

  if (Swig_insert_file("swigtcl8.swg",f_runtime) == -1) {
    fprintf(stderr,"SWIG : Fatal error. Unable to locate 'swigtcl8.swg' in SWIG library.\n");
    SWIG_exit(1);
  }
}

// --------------------------------------------------------------------
// TCL8::initialize(void)
//
// Produces an initialization function.   Assumes that the init function
// name has already been specified.
// ---------------------------------------------------------------------

void TCL8::initialize() 
{

  if ((!ns_name) && (nspace)) {
    fprintf(stderr,"Tcl error.   Must specify a namespace.\n");
    SWIG_exit(1);
  }

  if (!init_name) {
    fprintf(stderr,"*** Error. No module name specified.\n");
    SWIG_exit(1);
  }

  fprintf(f_header,"#define SWIG_init    %s\n", init_name);
  if (!module) module = (char*)"swig";
  fprintf(f_header,"#define SWIG_name    \"%s\"\n", module);
  if (nspace) {
    fprintf(f_header,"#define SWIG_prefix  \"%s::\"\n", ns_name);
    fprintf(f_header,"#define SWIG_namespace \"%s\"\n\n", ns_name);
  } else {
    fprintf(f_header,"#define SWIG_prefix  \"%s\"\n", prefix);
    fprintf(f_header,"#define SWIG_namespace \"\"\n\n");
  }
  fprintf(f_header,"#ifdef __cplusplus\n");
  fprintf(f_header,"extern \"C\" {\n");
  fprintf(f_header,"#endif\n");
  fprintf(f_header,"#ifdef MAC_TCL\n");
  fprintf(f_header,"#pragma export on\n");
  fprintf(f_header,"#endif\n");
  fprintf(f_header,"SWIGEXPORT(int) %s(Tcl_Interp *);\n", init_name);
  fprintf(f_header,"#ifdef MAC_TCL\n");
  fprintf(f_header,"#pragma export off\n");
  fprintf(f_header,"#endif\n");
  fprintf(f_header,"#ifdef __cplusplus\n");
  fprintf(f_header,"}\n");
  fprintf(f_header,"#endif\n");
  

  fprintf(f_init,"SWIGEXPORT(int) %s(Tcl_Interp *%s) {\n", init_name, interp_name);
  fprintf(f_init,"\t int i;\n");
  if (nspace) {
    fprintf(f_init,"#ifdef ITCL_NAMESPACES\n");
    fprintf(f_init,"\t Itcl_Namespace spaceId;\n");
    fprintf(f_init,"#endif\n");
  }
  
  fprintf(f_init,"\t if (%s == 0) \n", interp_name);
  fprintf(f_init,"\t\t return TCL_ERROR;\n");

  /* Check to see if other initializations need to be performed */

  if (strlen(mod_extern.get())) {
    fprintf(f_init,"%s\n",mod_init.get());
    fprintf(f_header,"#ifdef __cplusplus\n");
    fprintf(f_header,"extern \"C\" {\n");
    fprintf(f_header,"#endif\n");
    fprintf(f_header,"%s\n",mod_extern.get());
    fprintf(f_header,"#ifdef __cplusplus\n");
    fprintf(f_header,"}\n");
    fprintf(f_header,"#endif\n");
  }


  /* Check to see if we're adding support for Tcl8 nspaces */
  if (nspace) {
    fprintf(f_init,"#if (TCL_MAJOR_VERSION >= 8)\n");
    fprintf(f_init,"\t Tcl_Eval(%s,\"namespace eval %s { }\");\n", interp_name, ns_name);
    fprintf(f_init,"#endif\n");
  }
  
  cmd_info << "\nstatic _swig_command_info _swig_commands[] = {\n";
  var_info << "\nstatic _swig_var_info _swig_variables[] = {\n";

  String init;
  init << tab4 << "{\n"
       << tab8 << "int i;\n"
       << tab8 << "for (i = 0; _swig_types_initial[i]; i++) {\n"
       << tab8 << tab4 << "_swig_types[i] = SWIG_TypeRegister(_swig_types_initial[i]);\n"
       << tab8 << "}\n"
       << tab4 << "}\n";
  fprintf(f_init,"%s", init.get());
}

// ---------------------------------------------------------------------
// TCL8::close(void)
//
// Wrap things up.  Close initialization function.
// ---------------------------------------------------------------------

void TCL8::close(void)
{
  extern void emit_type_table();
  cmd_info << tab4 << "{0, 0, 0}\n"
	   << "};\n";

  var_info << tab4 << "{0,0,0,0}\n"
	   << "};\n";

  fprintf(f_wrappers,"%s", cmd_info.get());
  fprintf(f_wrappers,"%s", var_info.get());

  fprintf(f_init,"\tfor (i = 0; _swig_commands[i].name; i++) {\n");
  fprintf(f_init,"\t     Tcl_CreateObjCommand(interp, (char *) _swig_commands[i].name, _swig_commands[i].wrapper, _swig_commands[i].clientdata, NULL);\n");
  fprintf(f_init,"\t}\n");

  fprintf(f_init,"\tfor (i = 0; _swig_variables[i].name; i++) {\n");
  fprintf(f_init,"\t     Tcl_SetVar(interp, (char *) _swig_variables[i].name, \"\", TCL_GLOBAL_ONLY);\n");
  fprintf(f_init,"\t     Tcl_TraceVar(interp, (char *) _swig_variables[i].name, TCL_TRACE_READS | TCL_GLOBAL_ONLY, _swig_variables[i].get, (ClientData) _swig_variables[i].addr);\n");
  fprintf(f_init,"\t     Tcl_TraceVar(interp, (char *) _swig_variables[i].name, TCL_TRACE_WRITES | TCL_GLOBAL_ONLY, _swig_variables[i].set, (ClientData) _swig_variables[i].addr);\n");
  fprintf(f_init,"\t}\n");

  // Dump the pointer equivalency table

  emit_type_table();

  //  emit_ptr_equivalence(f_init);

  // Close the init file and quit

  fprintf(f_init,"%s",postinit.get());
  fprintf(f_init,"\t return TCL_OK;\n");
  fprintf(f_init,"}\n");

}

// ----------------------------------------------------------------------
// TCL8::get_pointer(char *iname, char *srcname, char *src, char *dest,
//                  DataType *t, String &f, char *ret)
//
// iname     = name of function or variable
// srcname   = name of source
// src       = source variable in wrapper code
// dest      = destination variable in wrapper code
// t         = Datatype 
// f         = String where output is going to go
// ret       = Return action
// ----------------------------------------------------------------------

void TCL8::get_pointer(char *iname, char *srcname, char *src, char *dest,
		      DataType *t, String &f, char *ret) {

  // Pointers are read as hex-strings with encoded type information
  
  t->remember();
  f << tab4 << "if ((SWIG_ConvertPtr(interp," << src << ",(void **) &" << dest << ",";

  if (t->type == T_VOID) f << "0)) == TCL_ERROR) { return TCL_ERROR; }\n";
  else 
    f << "SWIGTYPE" << t->print_mangle() << ")) == TCL_ERROR) { return TCL_ERROR; }\n";

}


// ----------------------------------------------------------------------
// TCL8::create_command(char *cname, char *iname)
//
// Creates a Tcl command from a C function.
// ----------------------------------------------------------------------

void TCL8::create_command(char *cname, char *iname) {

  char *wname = name_wrapper(cname,prefix);

  cmd_info << tab4 << "{ SWIG_prefix \"" << iname << "\", " << wname << ", NULL},\n";
  
  // Add interpreter name to repeatcmd hash table.  This hash is used in C++ code
  // generation to try and find repeated wrapper functions.

  Setattr(repeatcmd,iname,wname);
}

// ----------------------------------------------------------------------
// TCL8::create_function(char *name, char *iname, DataType *d, ParmList *l)
//
// Create a function declaration and register it with the interpreter.
// ----------------------------------------------------------------------

void TCL8::create_function(char *name, char *iname, DataType *d, ParmList *l)
{
  Parm            *p;
  int              pcount,i,j;
  char            *wname;
  char            *usage = 0, *tm;
  char             source[64];
  char             target[64];
  char             argnum[32];
  WrapperFunction  f;
  String           incode, cleanup, outarg, build;
  int              numopt= 0;
  int              have_build = 0;
  String           argstr;
  String           args;

  // Make a wrapper name for this function

  wname = name_wrapper(iname,prefix);

  // Now write the wrapper function itself....this is pretty ugly

  f.def << "static int " << wname << "(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[]) {";

  // Print out variables for storing arguments.

  pcount = emit_args(d, l, f);
  numopt = l->numopt();

  // Create a local variable for holding the interpreter result value

  //  f.add_local("Tcl_Obj *", "tcl_result");

  // Check the number of arguments

  usage = usage_func(iname,d,l);       // Create a usage string

  //
  //  f.code << tab4 << "if ((objc < " << (pcount-numopt) +1 << ") || (objc > " << l->numarg()+1 << ")) {\n"
  //	 << tab8 << "Tcl_SetResult(interp, \"Wrong # args. " << usage << "\", TCL_STATIC);\n"
  //	 << tab8 << "return TCL_ERROR;\n"
  //	 << tab4 << "}\n";

  argstr = "\"";
  args = "";

  // Extract parameters.    This case statement should be used to extract
  // Function parameters.   Add more cases if you want to do more.

  i = 0;
  j = 0;
  p = l->get_first();
  while (p != 0) {
    // Produce string representations of the source and target arguments
    sprintf(source,"objv[%d]",j+1);
    sprintf(target,"_arg%d",i);
    sprintf(argnum,"%d",j+1);

    // See if this argument is being ignored
    
    if (!p->ignore) {
      if (j == (pcount-numopt)) 
	argstr << "|";
      //	f.code << tab4 << "if (objc >" << j+1 << ") { \n";

      if ((tm = typemap_lookup((char*)"in",(char*)"tcl8",p->t,p->name,source,target,&f))) {
	argstr << "o";
	args << ",0";

	// Yep.  Use it instead of the default
	incode << tm << "\n";
	incode.replace("$argnum",argnum);
	incode.replace("$arg",source);
      } else {
	if (!p->t->is_pointer) {
	
	  // Extract a parameter by value.
	
	  switch(p->t->type) {
	    
	    // Signed Integers
	  
	  case T_INT:
	  case T_SINT:
	  case T_UINT:
	    argstr << "i";
	    args << ",&" << target;
	    break;
	  case T_BOOL:
	    argstr << "i";
	    {
	      char tb[32];
	      sprintf(tb,"tempb%d",i);
	      f.add_local((char*)"int",tb);
	      args << ",&" << tb;
	      incode << tab4 << target << " = (bool) " << tb << ";\n";
	    }
	    break;
	  case T_SHORT:
	  case T_SSHORT:
	  case T_USHORT:
	    argstr << "h";
	    args << ",&" << target;
	    break;
	  case T_LONG:
	  case T_SLONG:
	  case T_ULONG:
	    argstr << "l";
	    args << ",&" << target;
	    break;
	  case T_SCHAR:
	  case T_UCHAR:
	    argstr << "b";
	    args << ",&" << target;
	    break;
	  
	  // Floating point

	  case T_FLOAT:
	    argstr << "f";
	    args << ",&" << target;
	    break;

	  case T_DOUBLE:
	    argstr << "d";
	    args << ",&" << target;
	    break;
	  
	    // A single character
	  case T_CHAR :
	    argstr << "c";
	    args << ",&" << target;
	    break;
	  
	    // Void.. Do nothing.
	  
	  case T_VOID :
	    break;
	    // User defined.   This is an error.
	    
	  case T_USER:
	    // Unsupported data type
	    
	  default :
	    fprintf(stderr,"%s : Line %d: Unable to use type %s as a function argument.\n",
		    input_file, line_number, p->t->print_type());
	    break;
	  }
	} else {
	  
	  // Function argument is some sort of pointer
	  // Look for a string.   Otherwise, just pull off a pointer.
	  
	  if ((p->t->type == T_CHAR) && (p->t->is_pointer == 1)) {
	    argstr << "s";
	    args << ",&" << target;
	  } else {
	    p->t->remember();
	    argstr << "p";
	    args << ",&" << target << ", SWIGTYPE" << p->t->print_mangle();
	  }
	}
      }
      j++;
    }

    // Check to see if there is any sort of "build" typemap (highly complicated)

    if ((tm = typemap_lookup((char*)"build",(char*)"tcl8",p->t,p->name,source,target))) {
      build << tm << "\n";
      have_build = 1;
    }

    // Check to see if there was any sort of a constaint typemap
    if ((tm = typemap_lookup((char*)"check",(char*)"tcl8",p->t,p->name,source,target))) {
      // Yep.  Use it instead of the default
      incode << tm << "\n";
      incode.replace("$argnum",argnum);
      incode.replace("$arg",source);
    }

    // Check if there was any cleanup code (save it for later)
    if ((tm = typemap_lookup((char*)"freearg",(char*)"tcl8",p->t,p->name,target,(char*)"tcl_result"))) {
      // Yep.  Use it instead of the default
      cleanup << tm << "\n";
      cleanup.replace("$argnum",argnum);
      cleanup.replace("$arg",source);
    }
    // Look for output arguments
    if ((tm = typemap_lookup((char*)"argout",(char*)"tcl8",p->t,p->name,target,(char*)"tcl_result"))) {
      outarg << tm << "\n";
      outarg.replace("$argnum",argnum);
      outarg.replace("$arg",source);
    }
    i++;
    p = l->get_next();   // Get next parameter and continue
  }

  argstr << ":" << usage << "\"";
  f.code << tab4 << "if (SWIG_GetArgs(interp, objc, objv," << argstr << args << ") == TCL_ERROR) return TCL_ERROR;\n";

  f.code << incode;

  // If there was a "build" typemap, we need to go in and perform a serious hack
  
  if (have_build) {
    char temp1[32];
    char temp2[256];
    l->sub_parmnames(build);            // Replace all parameter names
    j = 1;
    for (i = 0; i < l->nparms; i++) {
      p = l->get(i);
      if (strlen(p->name) > 0) {
	sprintf(temp1,"_in_%s", p->name);
      } else {
	sprintf(temp1,"_in_arg%d", i);
      }
      sprintf(temp2,"argv[%d]",j);
      build.replaceid(temp1,temp2);
      if (!p->ignore) 
	j++;
    }
    f.code << build;
  }

  // Now write code to make the function call

  emit_func_call(name,d,l,f);

  // Return value if necessary 

  if ((tm = typemap_lookup((char*)"out",(char*)"tcl8",d,name,(char*)"_result",(char*)"tcl_result"))) {
    // Yep.  Use it instead of the default
    f.code << tm << "\n";
  } else if ((d->type != T_VOID) || (d->is_pointer)) {
    if (!d->is_pointer) {
	
      // Function returns a "value"
	
      switch(d->type) {
	// Is an integer
      case T_BOOL:
      case T_INT:
      case T_SINT:
      case T_SHORT:
      case T_SSHORT:
      case T_LONG :
      case T_SLONG:
      case T_SCHAR:
      case T_UINT:
      case T_USHORT:
      case T_ULONG:
      case T_UCHAR:
	f.code << tab4 << "Tcl_SetObjResult(interp,Tcl_NewIntObj((long) _result));\n";
	break;
	
	// Is a single character.  Assume we return it as a string
      case T_CHAR :
	f.code << tab4 << "Tcl_SetObjResult(interp,Tcl_NewStringObj(&_result,1));\n";
	break;
	
	// Floating point number
      case T_DOUBLE :
      case T_FLOAT :
	f.code << tab4 << "Tcl_SetObjResult(interp,Tcl_NewDoubleObj((double) _result));\n";
	break;
	
	// User defined type
      case T_USER :
	
	// Okay. We're returning malloced memory at this point.
	// Probably dangerous, but who said safety was a good thing?
	
	//	f.add_local("char","resultchar[256]");
	d->is_pointer++;
	d->remember();
	f.code << tab4 << "Tcl_SetObjResult(interp,SWIG_NewPointerObj((void *) _result,SWIGTYPE" << d->print_mangle() << "));\n";
	
	d->is_pointer--;
	break;
	
	// Unknown type
      default :
	fprintf(stderr,"%s : Line %d: Unable to use return type %s in function %s.\n",
		input_file, line_number, d->print_type(), name);
	break;
      }
    } else {
	
      // Is a pointer return type
      
      if ((d->type == T_CHAR) && (d->is_pointer == 1)) {
	// Return a character string
	f.code << tab4 << "Tcl_SetObjResult(interp,Tcl_NewStringObj(_result,-1));\n";
      } else {
	d->remember();
	f.code << tab4 << "Tcl_SetObjResult(interp,SWIG_NewPointerObj((void *) _result,SWIGTYPE" << d->print_mangle() << "));\n";	
      }
    }
  } else {
    //    f.code << tab4 << "Tcl_SetResult(interp,"",TCL_STATIC);\n";
  }

  // Dump output argument code
  f.code << outarg;

  // Dump the argument cleanup code
  f.code << cleanup;

  // Look for any remaining cleanup

  if (NewObject) {
    if ((tm = typemap_lookup((char*)"newfree",(char*)"tcl8",d,iname,(char*)"_result",(char*)""))) {
      f.code << tm << "\n";
    }
  }

  if ((tm = typemap_lookup((char*)"ret",(char*)"tcl8",d,name,(char*)"_result",(char*)""))) {
    // Yep.  Use it instead of the default
    f.code << tm << "\n";
  }
  
  f.code << tab4 << "return TCL_OK;\n}";

  // Substitute the cleanup code
  f.code.replace("$cleanup",cleanup);
  f.code.replace("$name",iname);

  // Dump out the function

  f.print(f_wrappers);

  // Now register the function with Tcl

  cmd_info << tab4 << "{ SWIG_prefix \"" << iname << "\", " << wname << ", NULL},\n";
}

// -----------------------------------------------------------------------
// TCL8::link_variable(char *name, char *iname, DataType *t, int ex)
//
// Create a TCL link to a variable.
// -----------------------------------------------------------------------

void TCL8::link_variable(char *name, char *iname, DataType *t)
{
  String    s;
  char      *tm, *tm1;

  // See if there were any typemaps

  tm = typemap_lookup((char*)"varin",(char*)"tcl8",t,name,(char*)"",(char*)"");
  tm1 = typemap_lookup((char*)"varout",(char*)"tcl8",t,name,(char*)"",(char*)"");
  if (tm || tm1) {
    fprintf(stderr,"%s : Line %d. Warning. varin/varout typemap methods not supported.",
	    input_file, line_number);
  }

  // Dump a collection of set/get functions suitable for variable tracing
  if (!Getattr(setget,t->print_type())) {
    Setattr(setget,t->print_type(),"1");
    WrapperFunction get;
    WrapperFunction set;
    set.def << "static char *_swig_" << t->print_mangle() << "_set(ClientData clientData, Tcl_Interp *interp, char *name1, char *name2, int flags) {";
    get.def << "static char *_swig_" << t->print_mangle() << "_get(ClientData clientData, Tcl_Interp *interp, char *name1, char *name2, int flags) {";
    t->is_pointer++;
    get.add_local(t->print_type(),(char*)"addr");
    set.add_local(t->print_type(),(char*)"addr");
    set.code << tab4 << "addr = " << t->print_cast() << " clientData;\n";
    get.code << tab4 << "addr = " << t->print_cast() << " clientData;\n";
    t->is_pointer--;
    set.add_local((char*)"char *",(char*)"value");
    get.add_local((char*)"Tcl_Obj *",(char*)"value");

    set.code << tab4 << "value = Tcl_GetVar2(interp, name1, name2, flags);\n"
	     << tab4 << "if (!value) return NULL;\n";

    if (!t->is_pointer) {
      switch(t->type) {
      case T_INT:
      case T_SINT:
      case T_SHORT:
      case T_USHORT:
      case T_SSHORT:
      case T_LONG:
      case T_SLONG:
      case T_UCHAR:
      case T_SCHAR:
      case T_BOOL:
	set.code << tab4 << "*(addr) = " << t->print_cast() << "atol(value);\n";
	break;
      case T_UINT:
      case T_ULONG:
	set.code << tab4 << "*(addr) = " << t->print_cast() << "strtoul(value,0,0);\n";
	break;
      case T_FLOAT:
      case T_DOUBLE:
	set.code << tab4 << "*(addr) = " << t->print_cast() << "atof(value);\n";
	break;
      case T_CHAR:  /* Single character. */
	set.code << tab4 << "*(addr) = *value;\n";
	break;
      case T_USER:
	// User defined type.  We return it as a pointer
	t->is_pointer++;
	t->remember();
	set.code << tab4 << "{\n"
		 << tab8 << "void *ptr;\n"
		 << tab8 << "if (SWIG_ConvertPtrFromString(interp,value,&ptr,SWIGTYPE" << t->print_mangle() << ") != TCL_OK) {\n"
		 << tab8 << tab4 << "return \"Type Error\";\n"
		 << tab8 << "}\n"
		 << tab8 << "*(addr) = *(" << t->print_cast() << " ptr);\n"
		 << tab4 << "}\n";

	t->is_pointer--;
	break;
      default:
	fprintf(stderr,"Unknown type %d!\n", t->type);
	break;
      }
    } else {
      if ((t->is_pointer == 1) && (t->type == T_CHAR)) {
	set.code << tab4 << "if (*addr) free(*addr);\n"
		 << tab4 << "*addr = (char *) malloc(strlen(value)+1);\n"
		 << tab4 << "strcpy(*addr,value);\n";
      } else {
	// User defined type.  We return it as a pointer
	t->remember();
	set.code << tab4 << "{\n"
		 << tab8 << "void *ptr;\n"
		 << tab8 << "if (SWIG_ConvertPtrFromString(interp,value,&ptr,SWIGTYPE" << t->print_mangle() << ") != TCL_OK) {\n"
		 << tab8 << tab4 << "return \"Type Error\";\n"
		 << tab8 << "}\n"
		 << tab8 << "*(addr) = " << t->print_cast() << " ptr;\n"
		 << tab4 << "}\n";

	/* A Pointer */
      }
    }
    set.code << tab4 << "return NULL;\n"
	     << "}\n";


    if (!t->is_pointer) {
      switch(t->type) {
      case T_INT:
      case T_SINT:
      case T_UINT:
      case T_SHORT:
      case T_USHORT:
      case T_SSHORT:
      case T_LONG:
      case T_ULONG:
      case T_SLONG:
      case T_UCHAR:
      case T_SCHAR:
      case T_BOOL:
	get.add_local((char*)"Tcl_Obj *",(char*)"value");
	get.code << tab4 << "value = Tcl_NewIntObj((int) *addr);\n"
		 << tab4 << "Tcl_SetVar2(interp,name1,name2,Tcl_GetStringFromObj(value,NULL), flags);\n"
		 << tab4 << "Tcl_DecrRefCount(value);\n";
	break;
      case T_FLOAT:
      case T_DOUBLE:
	get.add_local((char*)"Tcl_Obj *",(char*)"value");
	get.code << tab4 << "value = Tcl_NewDoubleObj((double) *addr);\n"
		 << tab4 << "Tcl_SetVar2(interp,name1,name2,Tcl_GetStringFromObj(value,NULL), flags);\n"
		 << tab4 << "Tcl_DecrRefCount(value);\n";
	break;

      case T_CHAR:
	get.add_local((char*)"char",(char*)"temp[4]");
	get.code << tab4 << "temp[0] = *addr; temp[1] = 0;\n"
		 << tab4 << "Tcl_SetVar2(interp,name1,name2,temp,flags);\n";
	break;

      case T_USER:
	get.add_local((char*)"Tcl_Obj *",(char*)"value");
	t->is_pointer++;
	t->remember();
	get.code << tab4 << "value = SWIG_NewPointerObj(addr, SWIGTYPE" << t->print_mangle() << ");\n"
		 << tab4 << "Tcl_SetVar2(interp,name1,name2,Tcl_GetStringFromObj(value,NULL), flags);\n"
		 << tab4 << "Tcl_DecrRefCount(value);\n";
	t->is_pointer--;
	break;

      default:
	break;
      }
    } else {
      if ((t->is_pointer == 1) && (t->type == T_CHAR)) {
	get.code << tab4 << "Tcl_SetVar2(interp,name1,name2,*addr, flags);\n";
      } else {
	get.add_local((char*)"Tcl_Obj *",(char*)"value");
	t->remember();
	get.code << tab4 << "value = SWIG_NewPointerObj(*addr, SWIGTYPE" << t->print_mangle() << ");\n"
		 << tab4 << "Tcl_SetVar2(interp,name1,name2,Tcl_GetStringFromObj(value,NULL), flags);\n"
		 << tab4 << "Tcl_DecrRefCount(value);\n";
      }
    }

    get.code << tab4 << "return NULL;\n"
	     << "}\n";
    get.print(f_wrappers);
    set.print(f_wrappers);
  }
  var_info << tab4 << "{ SWIG_prefix \"" << iname << "\", (void *) &" << name << ", _swig_" << t->print_mangle() << "_get,";
  
  if (Status & STAT_READONLY) {
    static int readonly = 0;
    if (!readonly) {
      WrapperFunction ro;
      ro.def << "static char *_swig_readonly(ClientData clientData, Tcl_Interp *interp, char *name1, char *name2, int flags) {";
      ro.code << tab4 << "return \"Variable is read-only\";\n";
      ro.code << "}\n";
      ro.print(f_wrappers);
      readonly = 1;
    }
    var_info << "_swig_readonly},\n";
  } else {
    var_info << "_swig_" << t->print_mangle() << "_set},\n";
  }
}

// -----------------------------------------------------------------------
// TCL8::declare_const(char *name, char *iname, DataType *type, char *value)
//
// Makes a constant.  Really just creates a variable and links to it.
// Tcl variable linkage allows read-only variables so we'll use that
// instead of just creating a Tcl variable.
// ------------------------------------------------------------------------

void TCL8::declare_const(char *name, char *, DataType *type, char *value) {

  int OldStatus = Status;      // Save old status flags
  DataType *t; 
  char   var_name[256];
  char     *tm;
  String  rvalue;
  Status = STAT_READONLY;      // Enable readonly mode.

  // Make a static variable;

  sprintf(var_name,"_wrap_const_%s",name);
  
  // See if there's a typemap
  rvalue = value;
  if ((type->type == T_CHAR) && (type->is_pointer == 1)) {
    rvalue << "\""; 
    "\"" >> rvalue;
  } 
  if ((type->type == T_CHAR) && (type->is_pointer == 0)) {
    rvalue << "'";
    "'" >> rvalue;
  }
  if ((tm = typemap_lookup((char*)"const",(char*)"tcl8",type,name,rvalue.get(),name))) {
    // Yep.  Use it instead of the default
    fprintf(f_init,"%s\n",tm);
  } else {

    // Create variable and assign it a value
    
    if (type->is_pointer == 0) {
      switch(type->type) {
      case T_BOOL: case T_INT: case T_SINT: case T_DOUBLE:
	fprintf(f_header,"static %s %s = %s;\n", type->print_type(), var_name, value);
	link_variable(var_name,name,type);
	break;
      case T_SHORT:
      case T_LONG:
      case T_SSHORT:
      case T_SCHAR:
      case T_SLONG:
	fprintf(f_header,"static %s %s = %s;\n", type->print_type(), var_name, value);
	fprintf(f_header,"static char *%s_char;\n", var_name);
	if (CPlusPlus)
	  fprintf(f_init,"\t %s_char = new char[32];\n",var_name);
	else
	  fprintf(f_init,"\t %s_char = (char *) malloc(32);\n",var_name);
	
	fprintf(f_init,"\t sprintf(%s_char,\"%%ld\", (long) %s);\n", var_name, var_name);
	sprintf(var_name,"%s_char",var_name);
	t = new DataType(T_CHAR);
	t->is_pointer = 1;
	link_variable(var_name,name,t);
	delete t;
	break;
      case T_UINT:
      case T_USHORT:
      case T_ULONG:
      case T_UCHAR:
	fprintf(f_header,"static %s %s = %s;\n", type->print_type(), var_name, value);
	fprintf(f_header,"static char *%s_char;\n", var_name);
	if (CPlusPlus)
	  fprintf(f_init,"\t %s_char = new char[32];\n",var_name);
	else
	  fprintf(f_init,"\t %s_char = (char *) malloc(32);\n",var_name);
	
	fprintf(f_init,"\t sprintf(%s_char,\"%%lu\", (unsigned long) %s);\n", var_name, var_name);
	sprintf(var_name,"%s_char",var_name);
	t = new DataType(T_CHAR);
	t->is_pointer = 1;
	link_variable(var_name,name,t);
	delete t;
	break;
      case T_FLOAT:
	type->type = T_DOUBLE;
	strcpy(type->name,"double");
	fprintf(f_header,"static %s %s = %s (%s);\n", type->print_type(), var_name, type->print_cast(), value);
	link_variable(var_name,name,type);
	break;
	
      case T_CHAR:
	type->is_pointer++;
	fprintf(f_header,"static %s %s = \"%s\";\n", type->print_type(), var_name, value);
	link_variable(var_name,name,type);
	type->is_pointer--;
	break;
      default:
	fprintf(stderr,"%s : Line %d. Unsupported constant value.\n", input_file, line_number);
	break;
      }
    } else {
      // Have some sort of pointer value here
      if ((type->type == T_CHAR) && (type->is_pointer == 1)) {
	// Character string
	fprintf(f_header,"static %s %s = \"%s\";\n", type->print_type(), var_name, value);
	link_variable(var_name,name,type);
      } else {
	// Something else.   Some sort of pointer value
	fprintf(f_header,"static %s %s = %s;\n", type->print_type(), var_name, value);
	fprintf(f_header,"static char *%s_char;\n", var_name);
	if (CPlusPlus)
	  fprintf(f_init,"\t %s_char = new char[%d];\n",var_name,(int) strlen(type->print_mangle())+ 20);
	else
	  fprintf(f_init,"\t %s_char = (char *) malloc(%d);\n",var_name, (int) strlen(type->print_mangle())+ 20);
	
	t = new DataType(T_CHAR);
        t->is_pointer = 1;
        type->remember(); 
	fprintf(f_init,"\t SWIG_MakePtr(%s_char, (void *) %s, SWIGTYPE%s);\n",
		var_name, var_name, type->print_mangle());
	sprintf(var_name,"%s_char",var_name);
	link_variable(var_name,name,t);
	delete t;
      }
    }
  }
  Status = OldStatus;
}

// ----------------------------------------------------------------------
// TCL8::usage_var(char *iname, DataType *t, char **s)
//
// Produces a usage string for a tcl variable.  Stores it in s
// ----------------------------------------------------------------------

char *TCL8::usage_var(char *iname, DataType *t) {

  static char temp[1024];

  if (!nspace) {
    sprintf(temp,"$%s%s", prefix, iname);
  } else { 
    sprintf(temp,"%s::%s", ns_name, iname);
  }
  if (!(((t->type == T_INT) && (!t->is_pointer)) ||
	((t->type == T_UINT) && (!t->is_pointer)) ||
	((t->type == T_DOUBLE) && (!t->is_pointer)) ||
	((t->type == T_BOOL) && (!t->is_pointer)) ||
	((t->type == T_CHAR) && (t->is_pointer)))) {
    /* We emitted a pair of set/get functions instead.  Doc will be generated for that */
    return temp;
  }
  return temp;
}



// ---------------------------------------------------------------------------
// char *TCL8::usage_string(char *iname, DataType *t, ParmList *l),
// 
// Generates a generic usage string for a Tcl function.
// ---------------------------------------------------------------------------

char * TCL8::usage_string(char *iname, DataType *, ParmList *l) {

  static String temp;
  Parm  *p;
  int   i, numopt,pcount;

  temp = "";
  temp << iname << " ";
  
  /* Now go through and print parameters */
  i = 0;
  pcount = l->nparms;
  numopt = l->numopt();
  p = l->get_first();
  while (p != 0) {

    // Only print an argument if not ignored

    if (!typemap_check((char*)"ignore",(char*)"tcl8",p->t,p->name)) {
      if (i >= (pcount-numopt))
	temp << "?";

      /* If parameter has been named, use that.   Otherwise, just print a type  */

      if ((p->t->type != T_VOID) || (p->t->is_pointer)) {
	if (strlen(p->name) > 0) {
	  temp << p->name;
	}
	else {
	  temp << "{ " << p->t->print_type() << " }";
	}
      }
      if (i >= (pcount-numopt))
	temp << "?";
      temp << " ";
      i++;
    }
    p = l->get_next();
  }
  return temp;
}

// ---------------------------------------------------------------------------
// char *TCL8::usage_func(char *iname, DataType *t, ParmList *l),
// 
// Produces a usage string for a function in Tcl
// ---------------------------------------------------------------------------

char * TCL8::usage_func(char *iname, DataType *t, ParmList *l) {

  String temp;

  if (nspace) {
    temp << ns_name << "::" << iname;
  } else {
    temp << prefix << iname;
  }
  return usage_string(temp,t,l);
}

// -----------------------------------------------------------------
// TCL8::usage_const(char *name, DataType *type, char *value)
//                         char **s)
//
// Makes a usage string and returns it
// -----------------------------------------------------------------

char *TCL8::usage_const(char *name, DataType *, char *value) {
  static String temp;
  temp = "";
  if (nspace) {
    temp << ns_name << "::" << name << " = " << value;
  } else {
    temp << "$" << prefix << name << " = " << value;
  }
  return temp.get();
}
    
// -------------------------------------------------------------------
// TCL8::add_native(char *name, char *funcname, DataType *t, ParmList *l)
//
// This adds an already written Tcl wrapper function to our
// initialization function.
// -------------------------------------------------------------------


void TCL8::add_native(char *name, char *funcname, DataType *, ParmList *) {

  fprintf(f_init,"\t Tcl_CreateObjCommand(%s, SWIG_prefix \"%s\", %s, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);\n",interp_name, name, funcname);
    
}
 
// -------------------------------------------------------------------
// TCL8::pragma(char *lname, char *name, char *value)
//
// Handle pragmas.
// --------------------------------------------------------------------

void TCL8::pragma(char *, char *, char *) {

}

// ---------------------------------------------------------------------
// C++ Handling
//
// The following functions provide some support for C++ classes and
// C structs.   
// ---------------------------------------------------------------------

void TCL8::cpp_open_class(char *classname, char *rename, char *ctype, int strip) {

  this->Language::cpp_open_class(classname,rename,ctype,strip);

  if (shadow) {
    static int included_object = 0;
    if (!included_object) {
      if (Swig_insert_file("object.swg",f_header) == -1) {
	fprintf(stderr,"SWIG : Fatal error. Unable to locate 'object.swg' in SWIG library.\n");
	SWIG_exit(1);
      }
      included_object = 1;
    }
  
    attributes = "static _swig_attribute _swig_";
    attributes << classname << "_attributes[] = {\n";

    methods = "static _swig_method _swig_";
    methods << classname << "_methods[] = {\n";

    have_constructor = 0;
    have_destructor = 0;

    if (rename)
      class_name = copy_string(rename);
    else
      class_name = copy_string(classname);
  
    base_class = (char *) 0;
    if (!strip) {
      class_type = new char[strlen(ctype)+2];
      sprintf(class_type,"%s ", ctype);
    } else
      class_type = (char*)"";

    real_classname = copy_string(classname);
  }
}

void TCL8::cpp_close_class() {
  String code,temp;
  DataType *t;

  this->Language::cpp_close_class();
  if (shadow) {

    t = new DataType;
    sprintf(t->name,"%s%s", class_type, real_classname);
    t->type = T_USER;
    t->is_pointer = 1;

    if (have_destructor) {
      code << "static void _swig_delete_" << class_name << "(void *obj) {\n"
	      << tab4 << name_destroy(class_name) << "((" << t->print_type() << ") obj);\n"
	      << "}\n";
    }

    methods << "    {0,0}\n"
	    << "};\n";
    code << methods;

    attributes << "    {0,0,0}\n"
	    << "};\n";
    code << attributes;

    code << "static _swig_class _wrap_class_" << class_name << " = { \"" << class_name 
	 << "\", &SWIGTYPE" << t->print_mangle() << ",";

    if (have_constructor) {
      code << name_wrapper(name_construct(class_name),prefix);
    } else {
      code << "0";
    }
    if (have_destructor) {
      code << ", _swig_delete_" << class_name;
    } else {
      code << ",0";
    }
    code << ", _swig_" << class_name << "_methods, _swig_" << class_name << "_attributes };\n";
    fprintf(f_wrappers,"%s", code.get());

    cmd_info << tab4 << "{ SWIG_prefix \"" << class_name << "\", SwigObjectCmd, &_wrap_class_" << class_name << "},\n";
  }
}

void TCL8::cpp_member_func(char *name, char *iname, DataType *t, ParmList *l) {

  char *realname;
  String temp;
  char  *rname;

  this->Language::cpp_member_func(name,iname,t,l);

  if (shadow) {
    if (iname) 
      realname = iname;
    else
      realname = name;
    
    // Add stubs for this member to our class handler function

    temp = "";
    temp << name_member(realname,class_name);
    rname = GetChar(repeatcmd,temp);
    if (!rname)
      rname = name_wrapper(temp.get(),prefix);
    
    methods << tab4 << "{\"" << realname << "\", " << rname << "}, \n";
  }
}

void TCL8::cpp_variable(char *name, char *iname, DataType *t) {
  char *realname;
  String temp;
  char *rname;

  this->Language::cpp_variable(name, iname, t);

  if (shadow) {
    if (iname)
      realname = iname;
    else
      realname = name;
    
    char *bc = class_name;

    attributes << tab4 << "{ \"-" << realname << "\",";

    // Try to figure out if there is a wrapper for this function
    temp = "";
    temp << name_get(name_member(realname,bc));
    rname = GetChar(repeatcmd,temp);
    if (!rname) 
      rname = name_wrapper(temp.get(),prefix);
    attributes << rname << ", ";

    if (!(Status & STAT_READONLY)) {
      temp = "";
      temp << name_set(name_member(realname,bc));
      rname = GetChar(repeatcmd,temp);
      if (!rname) 
	rname = name_wrapper(temp.get(),prefix);
      attributes << rname << "},\n";
    } else {
      attributes << "0 },\n";
    }
  }
}

void TCL8::cpp_constructor(char *name, char *iname, ParmList *l) {
  this->Language::cpp_constructor(name,iname,l);

  if (shadow) {
    have_constructor = 1;
  }
}
void TCL8::cpp_destructor(char *name, char *newname) {
  this->Language::cpp_destructor(name,newname);
  if (shadow) {
    have_destructor = 1;
  }
}

void TCL8::cpp_inherit(char **baseclass, int) {
  this->Language::cpp_inherit(baseclass);
}

void TCL8::cpp_declare_const(char *name, char *iname, DataType *type, char *value) {
  this->Language::cpp_declare_const(name,iname,type,value);
}

// --------------------------------------------------------------------------------
// TCL8::add_typedef(DataType *t, char *name)
//
// This is called whenever a typedef is encountered.   When shadow classes are
// used, this function lets us discovered hidden uses of a class.  For example :
//
//     struct FooBar {
//            ...
//     }
//
// typedef FooBar *FooBarPtr;
//
// --------------------------------------------------------------------------------

void TCL8::add_typedef(DataType *t, char *name) {

  if (!shadow) return;

  // First check to see if there aren't too many pointers

  if (t->is_pointer > 1) return;
  if (Getattr(hash,name)) return;      // Already added

  // Now look up the datatype in our shadow class hash table

  if (Getattr(hash,t->name)) {

    // Yep.   This datatype is in the hash
    // Put this types 'new' name into the hash
    Setattr(hash,name,GetChar(hash,t->name));
  }
}

// -----------------------------------------------------------------------
// TCL8::cpp_class_decl(char *name, char *rename, char *type)
//
// Treatment of an empty class definition.    Used to handle
// shadow classes across modules.
// -----------------------------------------------------------------------

void TCL8::cpp_class_decl(char *name, char *rename, char *type) {
  char temp[256];
  this->Language::cpp_class_decl(name,rename, type);

  if (shadow) {
    Setattr(hash,name,rename);
    // Add full name of datatype to the hash table
    if (strlen(type) > 0) {
      sprintf(temp,"%s %s", type, name);
      Setattr(hash,temp,rename);
    }
  }
}
