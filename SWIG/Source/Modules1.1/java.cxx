/*******************************************************************************
 * SWIG Java module
 * Author: Harco de Hilster
 *	   AT Consultancy
 *	   Toernooiveld 104
 *	   P.O. Box 1428
 *	   6501 BK Nijmegen
 *	   +31 24 3527282
 *	   harcoh@ATConsultancy.nl
 *
 * thanks to the following persons for submitting bug-reports:
 *
 *	James Hicks <jamey@crl.dec.com>
 *    	Lars Schmidt-Thieme <lschmidt@ix.urz.uni-heidelberg.de>
 *	Per OEyvind Hvidsten Per-Oyvind.Hvidsten@ffi.no
 *	Geoff Dillon <gdillon@pervasive.com>
 *    	Michael Haller <haller@lionbio.co.uk>
 *    	"Simon J. Julier" <sjulier@erols.com>
 *      "Pritam Kamat" <pritam@alier.com>
 *      Marc Hadley <marc_hadley@chrystal.co.uk>
 *******************************************************************************
*/

#include <ctype.h>

#include "mod11.h"
#include "java.h"

char bigbuf[1024];

static char *usage = (char*)"\
Java Options\n\
     -jnic            - use c syntax for jni calls\n\
     -jnicpp          - use c++ syntax for jni calls\n\
     -module name     - set name of module\n\
     -package name    - set name of package\n\
     -shadow          - enable shadow classes\n\
     -finalize        - generate finalize methods\n\
     -rn              - generate register natives code\n\n";

static char         *module = 0;          // Name of the module
static  char         *java_path = (char*)"java";
static  char         *package = 0;         // Name of the package
static  char         *c_pkgstr;         // Name of the package
static  char         *jni_pkgstr;         // Name of the package
static  char         *shadow_classname;
static  char         *jimport = 0;
static  char         *method_modifiers = (char*)"public final static";
static  FILE         *f_java = 0;
static  FILE         *f_shadow = 0;
static  int          shadow = 0;
static  DOHHash      *shadow_classes;
static  DOHString    *shadow_classdef;
static  char         *shadow_name = 0;
static  char         *shadow_baseclass = 0;
static  int          classdef_emitted = 0;
static  int          shadow_classdef_emitted = 0;
static  int          have_default_constructor = 0;
static  int          native_func = 0;           // Set to 1 when wrapping a native function
static  int          member_func = 0;           // Set to 1 when wrapping a member function
static  int          jnic = -1;          // 1: use c syntax jni; 0: use c++ syntax jni
static  int          finalize = 0;          // generate finalize methods
static  int          useRegisterNatives = 0;        // Set to 1 when doing stuff with register natives
static  DOHString   *registerNativesList = 0;

char *JAVA::SwigTcToJniType(DataType *t, int ret) {
  if(t->is_pointer == 1) {
	  switch(t->type) {
	    case T_INT:		return (char*)"jintArray";
	    case T_SHORT:	return (char*)"jshortArray";
	    case T_LONG:	return (char*)"jlongArray";
	    case T_CHAR:	return (char*)"jstring";
	    case T_FLOAT:	return (char*)"jfloatArray";
	    case T_DOUBLE:	return (char*)"jdoubleArray";
	    case T_UINT:	return (char*)"jintArray";
	    case T_USHORT:	return (char*)"jshortArray";
	    case T_ULONG:	return (char*)"jlongArray";
	    case T_UCHAR:	return (char*)"jbyteArray";
	    case T_SCHAR:	return (char*)"jbyteArray";
	    case T_BOOL:	return (char*)"jbooleanArray";
	    case T_VOID:	
	    case T_USER:	return (char*)"jlong";
	  }
  } else if(t->is_pointer > 1) {
    if(ret)
	return (char*)"jlong";
    else return (char*)"jlongArray";
  } else {
	  switch(t->type) {
	    case T_INT: return (char*)"jint";
	    case T_SHORT: return (char*)"jshort";
	    case T_LONG: return (char*)"jlong";
	    case T_CHAR: return (char*)"jbyte";
	    case T_FLOAT: return (char*)"jfloat";
	    case T_DOUBLE: return (char*)"jdouble";
	    case T_UINT: return (char*)"jint";
	    case T_USHORT: return (char*)"jshort";
	    case T_ULONG: return (char*)"jlong";
	    case T_UCHAR: return (char*)"jbyte";
	    case T_SCHAR: return (char*)"jbyte";
	    case T_BOOL: return (char*)"jboolean";
	    case T_VOID: return (char*)"void";
	    case T_USER: return (char*)"jlong";
	  }
  }
  fprintf(stderr, "SwigTcToJniType: unhandled SWIG type %d, %s\n", t->type, (char *) t->name);
  return NULL;
}

char *JAVA::SwigTcToJavaType(DataType *t, int ret, int inShadow) {
  if(t->is_pointer == 1) {
	  switch(t->type) {
	    case T_INT:    return (char*)"int []";
	    case T_SHORT:  return (char*)"short []";
	    case T_LONG:   return (char*)"long []";
	    case T_CHAR:   return (char*)"String";
	    case T_FLOAT:  return (char*)"float []";
	    case T_DOUBLE: return (char*)"double []";
	    case T_UINT:   return (char*)"int []";
	    case T_USHORT: return (char*)"short []";
	    case T_ULONG:  return (char*)"long []";
	    case T_UCHAR:  return (char*)"byte []";
	    case T_SCHAR:  return (char*)"byte []";
	    case T_BOOL:   return (char*)"boolean []";
	    case T_VOID:
        case T_USER:   if(inShadow && Getattr(shadow_classes,t->name))
                         return GetChar(shadow_classes,t->name);
                       else return (char*)"long";
	  }
  } else if(t->is_pointer > 1) {
    if(ret)
	  return (char*)"long";
    else return (char*)"long []";
  } else {
	  switch(t->type) {
	    case T_INT: return (char*)"int";
	    case T_SHORT: return (char*)"short";
	    case T_LONG: return (char*)"long";
	    case T_CHAR: return (char*)"byte";
	    case T_FLOAT: return (char*)"float";
	    case T_DOUBLE: return (char*)"double";
	    case T_UINT: return (char*)"int";
	    case T_USHORT: return (char*)"short";
	    case T_ULONG: return (char*)"long";
	    case T_UCHAR: return (char*)"byte";
	    case T_SCHAR: return (char*)"byte";
	    case T_BOOL: return (char*)"boolean";
	    case T_VOID: return (char*)"void";
	    case T_USER: return (char*)"long";
	  }
  }
  fprintf(stderr, "SwigTcToJavaType: unhandled SWIG type %d, %s\n", t->type, (char *) t->name);
  return NULL;
}

char *JAVA::SwigTcToJniScalarType(DataType *t) {
  if(t->is_pointer == 1) {
	  switch(t->type) {
	    case T_INT: return (char*)"Int";
	    case T_SHORT: return (char*)"Short";
	    case T_LONG: return (char*)"Long";
	    case T_CHAR: return (char*)"Byte";
	    case T_FLOAT: return (char*)"Float";
	    case T_DOUBLE: return (char*)"Double";
	    case T_UINT: return (char*)"Int";
	    case T_USHORT: return (char*)"Short";
	    case T_ULONG: return (char*)"Long";
	    case T_UCHAR: return (char*)"Byte";
	    case T_SCHAR: return (char*)"Byte";
	    case T_BOOL: return (char*)"Boolean";
            case T_VOID:
	    case T_USER: return (char*)"Long";
	  }
  } else {
    return (char*)"Long";
  }

  fprintf(stderr, "SwigTcToJniScalarType: unhandled SWIG type %d, %s\n", t->type, (char *) t->name);
  return NULL;
}

char *JAVA::JavaMethodSignature(DataType *t, int ret, int inShadow) {
  if(t->is_pointer == 1) {
	  switch(t->type) {
	   case T_INT:    return (char*)"[I";
	    case T_SHORT:  return (char*)"[S";
	    case T_LONG:   return (char*)"[J";
	    case T_CHAR:   return (char*)"Ljava/lang/String;";
	    case T_FLOAT:  return (char*)"[F";
	    case T_DOUBLE: return (char*)"[D";
	    case T_UINT:   return (char*)"[I";
	    case T_USHORT: return (char*)"[S";
	    case T_ULONG:  return (char*)"[J";
	    case T_UCHAR:  return (char*)"[B";
	    case T_SCHAR:  return (char*)"[B";
	    case T_BOOL:   return (char*)"[Z";
	    case T_VOID:
        case T_USER:   if(inShadow && Getattr(shadow_classes,t->name))
                         return GetChar(shadow_classes,t->name);
                       else return (char*)"J";
	  }
  } else if(t->is_pointer > 1) {
    if(ret) return (char*)"J";
    else return (char*)"[J";
  } else {
	  switch(t->type) {
	    case T_INT: return (char*)"I";
	    case T_SHORT: return (char*)"S";
	    case T_LONG: return (char*)"J";
	    case T_CHAR: return (char*)"B";
	    case T_FLOAT: return (char*)"F";
	    case T_DOUBLE: return (char*)"D";
	    case T_UINT: return (char*)"I";
	    case T_USHORT: return (char*)"S";
	    case T_ULONG: return (char*)"J";
	    case T_UCHAR: return (char*)"B";
	    case T_SCHAR: return (char*)"B";
	    case T_BOOL: return (char*)"Z";
	    case T_VOID: return (char*)"V";
	    case T_USER: return (char*)"J";
	  }
  }
  fprintf(stderr, "JavaMethodSignature: unhandled SWIG type %d, %s\n", t->type, (char *) t->name);
  return NULL;
}

char *JAVA::JavaTypeFromTypemap(char *op, char *lang, DataType *t, char *pname) {
  char *tm;
  char *c = bigbuf;
  if(!(tm = typemap_lookup(op, lang, t, pname, (char*)"", (char*)""))) return NULL;
  while(*tm && (isspace(*tm) || *tm == '{')) tm++;
  while(*tm && *tm != '}') *c++ = *tm++;
  *c='\0';

  return strdup(bigbuf);
}

char *JAVA::makeValidJniName(char *name) {
  char *c = name;
  char *b = bigbuf;

  while(*c) {
    *b++ = *c;
    if(*c == '_') *b++ = '1';
    c++;
  }
  *b = '\0';

  return strdup(bigbuf);
}

// !! this approach fails for functions without arguments
char *JAVA::JNICALL(DOHString_or_char *func) {
  if(jnic)
	sprintf(bigbuf, "(*jenv)->%s(jenv, ", Char(func));
  else
	sprintf(bigbuf, "jenv->%s(", Char(func));

  return strdup(bigbuf);
}

void JAVA::writeRegisterNatives()
{
  if(Len(registerNativesList) == 0)
    return;

  Printf(f_wrappers,"\n");
  Printf(f_wrappers,"JNINativeMethod nativeMethods[] = {\n");
  Printv(f_wrappers, registerNativesList, 0);
  Printf(f_wrappers, "};\n");
  
  Printf(f_wrappers,"\nint numberOfNativeMethods=sizeof(nativeMethods)/sizeof(JNINativeMethod);\n\n");

  // The registerNatives function

  Printv(f_wrappers,
	 "jint registerNatives(JNIEnv *jenv) {", "\n",
	 tab4, "jclass nativeClass = ", JNICALL((char*)"FindClass"),
	 "\"", jni_pkgstr, module, "\");","\n",
	 0);

  Printv(f_wrappers,
	 tab4, "if (nativeClass == 0)", "\n", tab8, "return -1;", "\n",
	 tab4, "return ", JNICALL((char*)"RegisterNatives"), "nativeClass, nativeMethods, ", "numberOfNativeMethods);", "\n",
	 "}", "\n", "\n",
	 0);

  // The unregisterNatives function

  Printv(f_wrappers,
	 "jint unregisterNatives(JNIEnv *jenv) {", "\n",
	 tab4, "jclass nativeClass = ", JNICALL((char*)"FindClass"),
	 "\"", jni_pkgstr, module, "\");","\n",
	 0);

  Printv(f_wrappers,
	 tab4, "if (nativeClass == 0)", "\n", tab8, "return -1;", "\n",
	 tab4, "// Sun documentation suggests that this method should not be invoked in ",
	 "\"normal native code\".", "\n",
	 tab4, "// return ", JNICALL((char*)"UnregisterNatives"), "nativeClass);", "\n",
	 tab4, "return 0;", "\n",
	 "}", "\n",
	 0);
}

// ---------------------------------------------------------------------
// JAVA::parse_args(int argc, char *argv[])
//
// Parse my command line options and initialize by variables.
// ---------------------------------------------------------------------

void JAVA::parse_args(int argc, char *argv[]) {

  // file::set_library(java_path);
  sprintf(LibDir,"%s", "java");


  // Look for certain command line options
  for (int i = 1; i < argc; i++) {
    if (argv[i]) {
      if (strcmp(argv[i],"-module") == 0) {
	if (argv[i+1]) {
	  set_module(argv[i+1],0);
	  Swig_mark_arg(i);
	  Swig_mark_arg(i+1);
	  i++;
	} else {
	  Swig_arg_error();
	}
      } else if (strcmp(argv[i],"-package") == 0) {
	if (argv[i+1]) {
	  package = new char[strlen(argv[i+1])+1];
          strcpy(package, argv[i+1]);
	  Swig_mark_arg(i);
	  Swig_mark_arg(i+1);
	  i++;
	} else {
	  Swig_arg_error();
	}
      } else if (strcmp(argv[i],"-shadow") == 0) {
	    Swig_mark_arg(i);
        shadow = 1;
      } else if (strcmp(argv[i],"-jnic") == 0) {
	    Swig_mark_arg(i);
        jnic = 1;
      } else if (strcmp(argv[i],"-finalize") == 0) {
	    Swig_mark_arg(i);
        finalize = 1;
      } else if (strcmp(argv[i],"-rn") == 0) {
	    Swig_mark_arg(i);
        useRegisterNatives = 1;
      } else if (strcmp(argv[i],"-jnicpp") == 0) {
        Swig_mark_arg(i);
	    jnic = 0;
      } else if (strcmp(argv[i],"-help") == 0) {
	    fprintf(stderr,"%s\n", usage);
      }
    }
  }

  if(jnic == -1) {
    if(CPlusPlus)
	jnic = 0;
    else jnic = 1;
  }

  // Add a symbol to the parser for conditional compilation
  // cpp::define("SWIGJAVA");
  Preprocessor_define((void *) "SWIGJAVA 1",0);

  // Add typemap definitions
  typemap_lang = (char*)"java";
}

// ---------------------------------------------------------------------
// void JAVA::parse()
//
// Start parsing an interface file for JAVA.
// ---------------------------------------------------------------------

void JAVA::parse() {

  fprintf(stderr,"Generating wrappers for Java\n");

  shadow_classes = NewHash();
  shadow_classdef = NewString("");
  registerNativesList = NewString("");

  headers();       // Emit header files and other supporting code
  yyparse();       // Run the SWIG parser
}

// ---------------------------------------------------------------------
// JAVA::set_module(char *mod_name,char **mod_list)
//
// Sets the module name.  Does nothing if it's already set (so it can
// be overriddent as a command line option).
//
// mod_list is a NULL-terminated list of additional modules.  This
// is really only useful when building static executables.
//----------------------------------------------------------------------

void JAVA::set_module(char *mod_name, char **mod_list) {
  if (module) return;
  module = new char[strlen(mod_name)+1];
  strcpy(module,mod_name);
}

// ---------------------------------------------------------------------
// JAVA::headers(void)
//
// Generate the appropriate header files for JAVA interface.
// ----------------------------------------------------------------------

void JAVA::headers(void)
{
  Swig_banner(f_header);               // Print the SWIG banner message
  fprintf(f_header,"/* Implementation : Java */\n\n");

  // Include header file code fragment into the output
  // if (file::include("java.swg",f_header) == -1) {
  if (Swig_insert_file("java.swg",f_header) == -1) {
    fprintf(stderr,"Fatal Error. Unable to locate 'java.swg'.\n");
    SWIG_exit(1);
  }
}

// --------------------------------------------------------------------
// JAVA::initialize(void)
//
// Produces an initialization function.   Assumes that the init function
// name has already been specified.
// ---------------------------------------------------------------------

void JAVA::initialize() 
{
  if (!module) {
    fprintf(stderr,"*** Error. No module name specified.\n");
    SWIG_exit(1);
  }

  if(package) {
    DOHString *s = NewString(package);
    Replace(s,".","_", DOH_REPLACE_ANY);
    Append(s,"_");
    c_pkgstr = copy_string(Char(s));
    Delete(s);

    DOHString *s2 = NewString(package);
    Replace(s2,".","/", DOH_REPLACE_ANY);
    Append(s2,"/");
    jni_pkgstr = copy_string(Char(s2));
    Delete(s2);
  } else {
    package = c_pkgstr = jni_pkgstr = (char*)"";
  }
    
  sprintf(bigbuf, "Java_%s%s", c_pkgstr, module);
  c_pkgstr = copy_string(bigbuf);
  sprintf(bigbuf, "%s_%%f", c_pkgstr);
  Swig_name_register((char*)"wrapper", copy_string(bigbuf));
  Swig_name_register((char*)"set", (char*)"set_%v");
  Swig_name_register((char*)"get", (char*)"get_%v");
  Swig_name_register((char*)"member", (char*)"%c_%m"); 
 
  // Generate the java class
  sprintf(bigbuf, "%s.java", module);
  if((f_java = fopen(bigbuf, "w")) == 0) {
    fprintf(stderr,"Unable to open %s\n", bigbuf);
    SWIG_exit(1);
  }

  fprintf(f_header, "#define J_CLASSNAME %s\n", module);
  if(package && *package) {
    fprintf(f_java, "package %s;\n\n", package);
    fprintf(f_header, "#define J_PACKAGE %s\n", package);
  } else {
    fprintf(f_header, "#define J_PACKAGE\n");
  }
}

void JAVA::emit_classdef() {
  if(!classdef_emitted)
    fprintf(f_java, "public class %s {\n", module);
  classdef_emitted = 1;
}

// ---------------------------------------------------------------------
// JAVA::close(void)
//
// Wrap things up.  Close initialization function.
// ---------------------------------------------------------------------

void JAVA::close(void)
{
  if(!classdef_emitted) emit_classdef();

  // Finish off the java class
  fprintf(f_java, "}\n");
  fclose(f_java);

  if(useRegisterNatives)
	writeRegisterNatives();
}

// ----------------------------------------------------------------------
// JAVA::create_command(char *cname, char *iname)
//
// Creates a JAVA command from a C function.
// ----------------------------------------------------------------------

void JAVA::create_command(char *cname, char *iname) {
}

void JAVA::add_native(char *name, char *iname, DataType *t, ParmList *l) {
  native_func = 1;
  create_function(name, iname, t, l);
  native_func = 0;
}

// ----------------------------------------------------------------------
// JAVA::create_function(char *name, char *iname, DataType *d, ParmList *l)
//
// Create a function declaration and register it with the interpreter.
// ----------------------------------------------------------------------

void JAVA::create_function(char *name, char *iname, DataType *t, ParmList *l)
{
  char           source[256], target[256];
  char	 	*tm;
  DOHString     *cleanup, *outarg, *body;
  char		*javaReturnSignature;
  DOHString     *javaParameterSignature;

  cleanup = NewString("");
  outarg = NewString("");
  body = NewString("");
  javaParameterSignature = NewString("");

  // A new wrapper function object
  WrapperFunction  f;

  if(!classdef_emitted) emit_classdef();

  // Make a wrapper name for this function
  
  char *jniname = makeValidJniName(iname);
  char *wname = Swig_name_wrapper(jniname);
  free(jniname);

  char *jnirettype = JavaTypeFromTypemap((char*)"jni", typemap_lang, t, iname);
  if(!jnirettype) jnirettype = SwigTcToJniType(t, 1);
  char *javarettype = JavaTypeFromTypemap((char*)"jtype", typemap_lang, t, iname);
  if(!javarettype) javarettype = SwigTcToJavaType(t, 1, 0);

  // If dumping the registerNative outputs, store the method return type
  // signature
  if (useRegisterNatives) {
      javaReturnSignature = JavaMethodSignature(t, 1, 0);
  }

  if(t->type != T_VOID || t->is_pointer) {
	 f.add_local(jnirettype, (char*)"_jresult", (char*)"0");
  }

  fprintf(f_java, "  %s ", method_modifiers);
  fprintf(f_java, "native %s %s(", javarettype, iname);
  if(shadow && member_func) {
    DOHString *member_name = NewString("");
    if(strcmp(iname, Swig_name_set(Swig_name_member(shadow_classname, shadow_name))) == 0)
      Printf(member_name,"set");
    else Printf(member_name,"get");
    Putc(toupper((int) *shadow_name), member_name);
    Printf(member_name, "%s", shadow_name+1);
    Printf(f_shadow, "  public %s %s(", javarettype, member_name);
    Printv(body, tab4, "return ", module, ".", iname, "(_self", 0);
    Delete(member_name);
  }

  if(!jnic) Printf(f._def,"extern \"C\"\n");
  Printv(f._def, "JNIEXPORT ", jnirettype, " JNICALL ", wname, "(JNIEnv *jenv, jclass jcls", 0);

  // Emit all of the local variables for holding arguments.
  int pcount = emit_args(t,l,f);

  int gencomma = 0;

  // Now walk the function parameter list and generate code to get arguments
  for (int i = 0; i < pcount ; i++) {
    Parm *p = l->get(i);         // Get the ith argument
    char *target_copy = NULL;
    char *target_length = NULL;
    char *local_i = NULL;

    // Produce string representation of source and target arguments
    sprintf(source,"jarg%d",i);
    sprintf(target,"_arg%d",i);

    char *jnitype = JavaTypeFromTypemap((char*)"jni", typemap_lang, p->t, p->name);
    if(!jnitype) jnitype = SwigTcToJniType(p->t, 0);
    char *jtype = JavaTypeFromTypemap((char*)"jtype", typemap_lang, p->t, p->name);
    if(!jtype) jtype = SwigTcToJavaType(p->t, 0, 0);
    if (useRegisterNatives) {
      Printv(javaParameterSignature, JavaMethodSignature(p->t, 0, 0), 0);
    }

    if(p->ignore) continue;

      // Add to java function header
      if(shadow && member_func) {
        if(i > 0) {
          if(i>1) fprintf(f_shadow, ", ");
          fprintf(f_shadow, "%s %s", jtype, source);
	  Printv(body,", ", source, 0);
        }
      }

      if(gencomma) fprintf(f_java, ", ");
      fprintf(f_java, "%s %s", jtype, source);

      gencomma = 1;

      // Add to Jni function header
      Printv(f._def, ", ", jnitype, " ", source, 0);
  
      // Get typemap for this argument
      tm = typemap_lookup((char*)"in",typemap_lang,p->t,p->name,source,target,&f);
      if (tm) {
	Printf(f._code,"%s\n", tm);
	Replace(f._code,"$arg",source, DOH_REPLACE_ANY);
      } else {
        if(!p->t->is_pointer)
	  Printv(f._code, tab4, target, " = ", p->t->print_cast(), source, ";\n", 0);
        else if((p->t->type == T_VOID && p->t->is_pointer == 1) ||
	        (p->t->type == T_USER && p->t->is_pointer == 1)) {
            p->t->is_pointer++;
	    Printv(f._code, tab4, target, " = *", p->t->print_cast(), "&", source, ";\n", 0);
            p->t->is_pointer--;
        } else {
          if(p->t->type == T_CHAR && p->t->is_pointer == 1) {
	    Printv(f._code, tab4, target, " = (", source, ") ? (char *)", JNICALL((char*)"GetStringUTFChars"), source, ", 0) : NULL;\n", 0);
          } else {
            char *scalarType = SwigTcToJniScalarType(p->t);
            char *cptrtype = p->t->print_type();
            p->t->is_pointer--;
            const char *basic_jnitype = (p->t->is_pointer > 0) ? "jlong" : SwigTcToJniType(p->t, 0);
            char *ctype = p->t->print_type();
	    if(scalarType == NULL || basic_jnitype == NULL) {
	      fprintf(stderr, "\'%s\' does not have a in/jni typemap, and is not a basic type.\n", ctype);
    	      SWIG_exit(1);
	    };
            p->t->is_pointer++;

	    DOHString *basic_jniptrtype = NewStringf("%s*",basic_jnitype);
            DOHString *source_length = NewStringf("%s%s)", JNICALL((char*)"GetArrayLength"), source);

            target_copy = copy_string(f.new_local(Char(basic_jniptrtype), target, NULL));
            target_length = copy_string(f.new_local((char*)"jsize", target, Char(source_length)));
            if(local_i == NULL) local_i = copy_string(f.new_local((char*)"int", (char*)"i", NULL));
	    
	    DOHString *scalarFunc = NewStringf("Get%sArrayElements",scalarType);

	    Printv(f._code, tab4, target_copy, " = ", JNICALL(scalarFunc), source, ", 0);\n", 0);
	    Printv(f._code, tab4, target, " = ", p->t->print_cast(), " malloc(", target_length, " * sizeof(", ctype, "));\n", 0);
	    Printv(f._code, tab4, "for(i=0; i<", target_length, "; i++)\n", 0);
	    if(p->t->is_pointer > 1) {
	      Printv(f._code, tab8, target, "[i] = *", p->t->print_cast(), "&", target_copy, "[i];\n", 0);
	    } else {
              p->t->is_pointer--;
	      Printv(f._code, tab8, target, "[i] = ", p->t->print_cast(), target_copy, "[i];\n", 0); 
	      p->t->is_pointer++;
	    }
	    Delete(scalarFunc);
	    Delete(source_length);
	    Delete(basic_jniptrtype);
          }
        }
      }

    // Check to see if there was any sort of a constaint typemap
    if ((tm = typemap_lookup((char*)"check",typemap_lang,p->t,p->name,source,target))) {
      // Yep.  Use it instead of the default
      Printf(f._code,"%s\n", tm);
      Replace(f._code,"$arg",source, DOH_REPLACE_ANY);
    }

    // Check if there was any cleanup code (save it for later)
    if ((tm = typemap_lookup((char*)"freearg",typemap_lang,p->t,p->name,source,target))) {
      // Yep.  Use it instead of the default
      Printf(cleanup,"%s\n", tm);
      Replace(cleanup,"$arg",source, DOH_REPLACE_ANY);
    }

    if ((tm = typemap_lookup((char*)"argout",typemap_lang,p->t,p->name,source,target))) {
      // Yep.  Use it instead of the default
      Printf(outarg,"%s\n", tm);
      Replace(outarg,"$arg",source, DOH_REPLACE_ANY);
    } else {
       // if(p->t->is_pointer && p->t->type != T_USER &&  p->t->type != T_VOID) {
       if(p->t->is_pointer) {
         if(p->t->type == T_CHAR && p->t->is_pointer == 1) {
	   Printv(outarg, tab4, "if(", target,") ", JNICALL((char*)"ReleaseStringUTFChars"), source, ", ", target, ");\n", 0);
         } else if((p->t->type == T_VOID && p->t->is_pointer == 1) ||
                (p->t->type == T_USER && p->t->is_pointer == 1)) {
	   // nothing to do
         } else {
            char *scalarType = SwigTcToJniScalarType(p->t);
            char *cptrtype = p->t->print_type();
            p->t->is_pointer--;
            const char *basic_jnitype = (p->t->is_pointer > 0) ? "jlong" : SwigTcToJniType(p->t, 0);
            char *ctype = p->t->print_type();
	    if(scalarType == NULL || basic_jnitype == NULL) {
	      fprintf(stderr, "\'%s\' does not have a argout/jni typemap, and is not a basic type.\n", ctype);
    	      SWIG_exit(1);
	    };
            p->t->is_pointer++;
	    Printf(outarg, "    for(i=0; i< %d; i++)\n", target_length);
	    if(p->t->is_pointer > 1) {
	      Printv(outarg, tab8, "*", p->t->print_cast(), "&", target_copy, "[i] = ",  target, "[i];\n", 0);
	    } else {
	      Printv(outarg, tab8, target_copy, "[i] = (", basic_jnitype, ") ", target, "[i];\n", 0); 
	    }
	    DOHString *scalarFunc = NewStringf("Release%sArrayElements",scalarType);
	    Printv(outarg, tab4, JNICALL(scalarFunc), source, ", ", target_copy, ", 0);\n", 0);
	    Printv(outarg, tab4, "free(", target, ");\n", 0);
	    Delete(scalarFunc);
            free(target_copy);
            free(target_length);
            free(local_i);
         }
       }
    }
  }

  fprintf(f_java, ");\n");
  if(shadow && member_func) {
    fprintf(f_shadow, ") {\n");
    Printf(body,")");
    Printf(f_shadow, "%s;\n  }\n\n",body);
  }
  Printf(f._def,") {");

  // Now write code to make the function call

  if(!native_func)
	emit_func_call(name,t,l,f);

  // Return value if necessary 

  if((t->type != T_VOID || t->is_pointer) && !native_func) {
    if ((tm = typemap_lookup((char*)"out",typemap_lang,t,iname,(char*)"_result",(char*)"_jresult"))) {
      Printf(f._code,"%s\n", tm);
    } else {
      if(t->is_pointer == 0 && t->type == T_USER) { /* return by value */
	    t->is_pointer=2;
	    Printv(f._code, tab4, "*", t->print_cast(), "&_jresult = _result;\n", 0);
	    t->is_pointer=0;
      } else if(t->is_pointer == 0 && t->type != T_USER) {
	Printv(f._code, tab4, "_jresult = (", jnirettype, ") _result;\n", 0);
      } else if((t->type == T_VOID && t->is_pointer == 1) ||
	        (t->type == T_USER && t->is_pointer == 1)) {
	    t->is_pointer++;
	    Printv(f._code, tab4, "*", t->print_cast(), "&_jresult = _result;\n", 0);
	    t->is_pointer--;
      } else {
        if(t->type == T_CHAR && t->is_pointer == 1) {
	  Printv(f._code, tab4, "if(_result != NULL)\n", 0);
          Printv(f._code, tab8, "_jresult = (jstring)", JNICALL((char*)"NewStringUTF"),  "_result);\n", 0);
        } else {
	  fprintf(stderr,"%s : Line %d. Warning: no return typemap for datatype %s\n", input_file,line_number,t->print_type());
	    t->is_pointer++;
	    Printv(f._code, tab4, "*", t->print_cast(), "&_jresult = _result;\n", 0);
	    t->is_pointer--;
        }
      }
    }
  }

  // Dump argument output code;
  Printv(f._code, outarg, 0);

  // Dump the argument cleanup code
  Printv(f._code,cleanup, 0);

  // Look for any remaining cleanup

  if (NewObject) {
    if ((tm = typemap_lookup((char*)"newfree",typemap_lang,t,iname,(char*)"_result",(char*)""))) {
      Printf(f._code,"%s\n", tm);
    }
  }

  if((t->type != T_VOID || t->is_pointer) && !native_func) {
    if ((tm = typemap_lookup((char*)"ret",typemap_lang,t,iname,(char*)"_result",(char*)"_jresult", NULL))) {
      Printf(f._code,"%s\n", tm);
    }
  }

  // Wrap things up (in a manner of speaking)
  if(t->type != T_VOID || t->is_pointer)
    Printv(f._code, tab4, "return _jresult;\n", 0);
  Printf(f._code, "}\n");

  // Substitute the cleanup code (some exception handlers like to have this)
  Replace(f._code,"$cleanup",cleanup, DOH_REPLACE_ANY);
 
  // Emit the function
  
  if(!native_func)
	f.print(f_wrappers);
  
  
 // If registerNatives is active, store the table entry for this method
  if (useRegisterNatives) {
    Printv(registerNativesList,
	   tab4, "{",
	   "\"", name, "\", \"(", javaParameterSignature, ")", javaReturnSignature, "\", ", wname,
	   "},\n",
	   0);
	   
  }
}

// -----------------------------------------------------------------------
// JAVA::link_variable(char *name, char *iname, DataType *t)
//
// Create a JAVA link to a C variable.
// -----------------------------------------------------------------------

void JAVA::link_variable(char *name, char *iname, DataType *t)
{
  emit_set_get(name,iname, t);
}

// -----------------------------------------------------------------------
// JAVA::declare_const(char *name, char *iname, DataType *type, char *value)
// ------------------------------------------------------------------------

void JAVA::declare_const(char *name, char *iname, DataType *type, char *value) {
  char *tm;
  FILE *jfile;
  char *jname;

  if(!classdef_emitted) emit_classdef();

  if(shadow && member_func) {
    jfile = f_shadow;
    jname = shadow_name;
  } else {
    jfile = f_java;
    jname = name;
  }

  if ((tm = typemap_lookup((char*)"const",typemap_lang,type,name,name,iname))) {
    DOHString *str = NewString(tm);
    Replace(str,"$value",value, DOH_REPLACE_ANY);
    Printf(jfile,"  %s\n\n", str);
    Delete(str);
  } else {
    if((type->is_pointer == 0)) {
      char *jtype = typemap_lookup((char*)"jtype", typemap_lang, type, name, name, iname);
      if(!jtype) jtype = SwigTcToJavaType(type, 0, 0);
      if(strcmp(jname, value) == 0 || strstr(value,"::") != NULL) {
        fprintf(stderr, "ignoring enum constant: %s\n", jname);
      } else 
        fprintf(jfile, "  public final static %s %s = %s;\n\n", jtype, jname, value);
    } else {
      if(type->type == T_CHAR && type->is_pointer == 1) {
        fprintf(jfile, "  public final static String %s = \"%s\";\n\n", jname, value);
      } else {
        emit_set_get(name,iname, type);
      }
    }
  }
}

void emit_shadow_banner(FILE *f) {
  fprintf(f, "/*\n");
  fprintf(f, " *\n");
  fprintf(f, " * This file was automatically generated by :\n");
  fprintf(f, " * Simplified Wrapper and Interface Generator (SWIG)\n");
  fprintf(f, " * Version 1.1  (Final)\n");
  fprintf(f, " *\n");
  fprintf(f, " * Portions Copyright (c) 1995-1997\n");
  fprintf(f, " * The University of Utah and The Regents of the University of California.\n");
  fprintf(f, " * Permission is granted to distribute this file in any manner provided\n");
  fprintf(f, " * this notice remains intact.\n");
  fprintf(f, " *\n");
  fprintf(f, " * Portions Copyright (c) 1997-1999\n");
  fprintf(f, " * Harco de Hilster, Harco.de.Hilster@ATConsultancy.nl\n");
  fprintf(f, " *\n");
  fprintf(f, " * Do not make changes to this file--changes will be lost!\n");
  fprintf(f, " *\n");
  fprintf(f, " */\n\n\n");
}

void JAVA::pragma(char *lang, char *code, char *value) {
  if(strcmp(lang, "java") != 0) return;

  DOHString *s = NewString(value);
  Replace(s,"\\\"", "\"", DOH_REPLACE_ANY);
  if(strcmp(code, "import") == 0) {
    jimport = copy_string(Char(s));
    Printf(f_java, "// pragma\nimport %s;\n\n", jimport);
  } else if(strcmp(code, "module") == 0) {
    if(!classdef_emitted) emit_classdef();
    Printf(f_java, "// pragma\n");
    Printf(f_java, "%s", s);
    Printf(f_java, "\n\n");
  } else if(strcmp(code, "shadow") == 0) {
    if(shadow && f_shadow) {
      Printf(f_shadow, "// pragma\n");
      Printf(f_shadow, "%s", s);
      Printf(f_shadow, "\n\n");
    }
  } else if(strcmp(code, "modifiers") == 0) {
    method_modifiers = copy_string(value);
  } else {
    fprintf(stderr,"%s : Line %d. Unrecognized pragma.\n", input_file,line_number);
  }
  Delete(s);
}

// ---------------------------------------------------------------------
// C++ Handling
//
// The following functions provide some support for C++ classes and
// C structs.
// ---------------------------------------------------------------------

void JAVA::add_typedef(DataType *t, char *name) {
  if(!shadow) return;
  
  // First check to see if there aren't too many pointers

  if (t->is_pointer > 1) return;

  if(Getattr(shadow_classes,name)) return;      // Already added

  // Now look up the datatype in our shadow class hash table

  if (Getattr(shadow_classes,t->name)) {

    // Yep.   This datatype is in the hash
    // Put this types 'new' name into the hash
    Setattr(shadow_classes,name,GetChar(shadow_classes,t->name));
  }
}

void JAVA::cpp_open_class(char *classname, char *rename, char *ctype, int strip) {

  this->Language::cpp_open_class(classname,rename,ctype,strip);

  if(!shadow) return;

  if(rename)
    shadow_classname = copy_string(rename);
  else shadow_classname = copy_string(classname);

  if (strcmp(shadow_classname, module) == 0) {
    fprintf(stderr, "class name cannot be equal to module name: %s\n", shadow_classname);
    SWIG_exit(1);
  }

  Setattr(shadow_classes,classname, shadow_classname);
  if(ctype && strcmp(ctype, "struct") == 0) {
    sprintf(bigbuf, "struct %s", classname);
    Setattr(shadow_classes, bigbuf, shadow_classname);
  }

  sprintf(bigbuf, "%s.java", shadow_classname);
  if(!(f_shadow = fopen(bigbuf, "w"))) {
    fprintf(stderr, "Unable to create shadow class file: %s\n", bigbuf);
  }

  emit_shadow_banner(f_shadow);

  if(*package)
	fprintf(f_shadow, "package %s;\n\n", package);
  else fprintf(f_shadow, "import %s;\n\n", module);
  if(jimport != NULL)
	fprintf(f_shadow, "import %s;\n\n", jimport);

  Clear(shadow_classdef);
  Printv(shadow_classdef, "public class ", shadow_classname, " %BASECLASS% ", "{\n", 0);

  shadow_baseclass = (char*) "";
  shadow_classdef_emitted = 0;
  have_default_constructor = 0;
}

void JAVA::emit_shadow_classdef() {
  if(*shadow_baseclass) {
    sprintf(bigbuf, "extends %s", shadow_baseclass);
    Replace(shadow_classdef,"%BASECLASS%", bigbuf, DOH_REPLACE_ANY);
    Printv(shadow_classdef, "  public ", shadow_classname, "(java.lang.Long obj) {\n", tab4, "_self = obj.longValue();\n  }\n\n", 0);
  } else {
    Replace(shadow_classdef,"%BASECLASS%", "",DOH_REPLACE_ANY);

    Printv(shadow_classdef,
	   "  public long _self = 0;\n",
	   "  public boolean _selfown = false;\n\n",
           "  public static Object newInstance(long p) {\n",
	   "    return new ", shadow_classname, "(new Long(p));\n",
	   "  };\n\n",

	   "  public ", shadow_classname, "(java.lang.Long obj) {\n", tab4, "_self = obj.longValue();\n  }\n\n",
	   0);
  }
  Printv(shadow_classdef, "  public Class _selfClass() {\n", tab4, "return ", shadow_classname, ".class;\n", "  };\n\n", 0);

  Printv(f_shadow, shadow_classdef,0);
  shadow_classdef_emitted = 1;
}

void JAVA::cpp_close_class() {
  this->Language::cpp_close_class();
  if(!shadow) return;

  if(!shadow_classdef_emitted) emit_shadow_classdef();

  if(have_default_constructor == 0) {
    fprintf(f_shadow, "  public %s() {}\n\n", shadow_classname);
  }

  fprintf(f_shadow, "}\n");
  fclose(f_shadow);
  f_shadow = NULL;

  free(shadow_classname);
  shadow_classname = NULL;
}

void JAVA::cpp_member_func(char *name, char *iname, DataType *t, ParmList *l) {
  char                arg[256];
  DOHString           *nativecall;

  nativecall = NewString("");

  this->Language::cpp_member_func(name,iname,t,l);

  if(!shadow) return;
  if(!shadow_classdef_emitted) emit_shadow_classdef();

  char *javarettype = JavaTypeFromTypemap((char*)"jtype", typemap_lang, t, iname);
  if(!javarettype) javarettype = SwigTcToJavaType(t, 1, 0);
  char *shadowrettype = JavaTypeFromTypemap((char*)"jstype", typemap_lang, t, iname);
  if(!shadowrettype && t->type == T_USER && t->is_pointer <= 1) {
    shadowrettype = GetChar(shadow_classes,t->name);
  }

  fprintf(f_shadow, "  public %s %s(", (shadowrettype) ? shadowrettype : javarettype, iname);

  if((t->type != T_VOID || t->is_pointer)) {
    Printf(nativecall,"return ");
    if(shadowrettype) {
      Printv(nativecall, "new ", shadowrettype, "(new Long(", 0);
    }
  }
  Printv(nativecall, module, ".", Swig_name_member(shadow_classname,iname), "(_self", 0);

  int pcount = l->nparms;

  for (int i = 0; i < pcount ; i++) {
    Parm *p = l->get(i);         // Get the ith argument
    // Produce string representation of source and target arguments
    if(p->name && *(p->name))
      strcpy(arg,p->name);
    else {
      sprintf(arg,"arg%d",i);
    }

      if(p->t->type == T_USER && p->t->is_pointer <= 1 && Getattr(shadow_classes,p->t->name)) {
	Printv(nativecall, ", ", arg, "._self", 0);
      } else Printv(nativecall, ", ", arg, 0);

      char *jtype = JavaTypeFromTypemap((char*)"jtype", typemap_lang, p->t, p->name);
      if(!jtype) jtype = SwigTcToJavaType(p->t, 0, 0);

      char *jstype = JavaTypeFromTypemap((char*)"jstype", typemap_lang, p->t, p->name);
      if(!jstype && p->t->type == T_USER && p->t->is_pointer <= 1) {
	    jstype = GetChar(shadow_classes,p->t->name);
      }

      // Add to java function header
      fprintf(f_shadow, "%s %s", (jstype) ? jstype : jtype, arg);
      if(i != pcount-1) {
        fprintf(f_shadow, ", ");
      }
  }

  if((t->type != T_VOID) && shadowrettype) 
    Printf(nativecall, "))");
  
  Printf(nativecall,");\n");

  Printf(f_shadow, ") {\n");
  Printf(f_shadow, "\t%s\n", nativecall);
  Printf(f_shadow, "  }\n\n");
  Delete(nativecall);

}

void JAVA::cpp_static_func(char *name, char *iname, DataType *t, ParmList *l) {
  char             arg[256];
  DOHString       *nativecall;

  this->Language::cpp_static_func(name,iname,t,l);

  if(!shadow) return;
  nativecall = NewString("");
  if(!shadow_classdef_emitted) emit_shadow_classdef();

  char *javarettype = JavaTypeFromTypemap((char*)"jtype", typemap_lang, t, iname);
  if(!javarettype) javarettype = SwigTcToJavaType(t, 1, 0);
  char *shadowrettype = JavaTypeFromTypemap((char*)"jstype", typemap_lang, t, iname);
  if(!shadowrettype && t->type == T_USER && t->is_pointer <= 1) {
    shadowrettype = GetChar(shadow_classes,t->name);
  }

  fprintf(f_shadow, "  public static %s %s(", (shadowrettype) ? shadowrettype : javarettype, iname);

  if((t->type != T_VOID || t->is_pointer)) {
    Printf(nativecall, "return ");
    if(shadowrettype) {
      Printv(nativecall, "new ", shadowrettype, "(new Long(", 0);
    }
  }
  Printv(nativecall, module, ".", Swig_name_member(shadow_classname,iname), "(", 0);

  int pcount = l->nparms;
  int gencomma = 0;

  for (int i = 0; i < pcount ; i++) {
    Parm *p = l->get(i);         // Get the ith argument
    // Produce string representation of source and target arguments
    if(p->name && *(p->name))
      strcpy(arg,p->name);
    else {
      sprintf(arg,"arg%d",i);
    }

    if(gencomma) Printf(nativecall,", ");

    if(p->t->type == T_USER && p->t->is_pointer <= 1 && Getattr(shadow_classes,p->t->name)) {
      Printv(nativecall, arg, "._self", 0);
    } else Printv(nativecall,arg,0);

    gencomma = 1;

    char *jtype = JavaTypeFromTypemap((char*)"jtype", typemap_lang, p->t, p->name);
    if(!jtype) jtype = SwigTcToJavaType(p->t, 0, 0);

    char *jstype = JavaTypeFromTypemap((char*)"jstype", typemap_lang, p->t, p->name);
    if(!jstype && p->t->type == T_USER && p->t->is_pointer <= 1) {
	  jstype = GetChar(shadow_classes, p->t->name);
    }

    // Add to java function header
    Printf(f_shadow, "%s %s", (jstype) ? jstype : jtype, arg);
    if(i != pcount-1) {
      Printf(f_shadow, ", ");
    }
  }


  if((t->type != T_VOID || t->is_pointer) && shadowrettype) 
    Printf(nativecall,"))");

  Printf(nativecall,");\n");

  Printf(f_shadow, ") {\n");
  Printf(f_shadow, "\t%s\n", nativecall);
  Printf(f_shadow, "  }\n\n");
  Delete(nativecall);
}

void JAVA::cpp_constructor(char *name, char *iname, ParmList *l) {
  this->Language::cpp_constructor(name,iname,l);

  if(!shadow) return;
  if(!shadow_classdef_emitted) emit_shadow_classdef();

  DOHString *nativecall = NewString("");
  char arg[256];

  fprintf(f_shadow, "  public %s(", shadow_classname);

  Printv(nativecall, "    if(_self == 0 && ", shadow_classname, ".class == _selfClass()) {\n", 0);
  if (iname != NULL)
    Printv(nativecall, tab8, " _self = ", module, ".", Swig_name_construct(iname), "(", 0);
  else
    Printv(nativecall, tab8, " _self = ", module, ".", Swig_name_construct(shadow_classname), "(", 0);

  int pcount = l->nparms;
  if(pcount == 0)  // We must have a default constructor
    have_default_constructor = 1;

  for (int i = 0; i < pcount ; i++) {
    Parm *p = l->get(i);         // Get the ith argument
    // Produce string representation of source and target arguments
    if(p->name && *(p->name))
      strcpy(arg,p->name);
    else {
      sprintf(arg,"arg%d",i);
    }

      char *jtype = JavaTypeFromTypemap((char*)"jtype", typemap_lang, p->t, p->name);
      if(!jtype) jtype = SwigTcToJavaType(p->t, 0, 0);
      char *jstype = JavaTypeFromTypemap((char*)"jstype", typemap_lang, p->t, p->name);
      if(!jstype) jstype = SwigTcToJavaType(p->t, 0, 1);
      if(strcmp(jtype, jstype) == 0) jstype = NULL;

      // Add to java function header
      Printf(f_shadow, "%s %s", (jstype) ? jstype : jtype, arg);

      if(p->t->type == T_USER && p->t->is_pointer <= 1 && Getattr(shadow_classes,p->t->name)) {
	Printv(nativecall,arg, "._self", 0);
      } else Printv(nativecall, arg, 0);

      if(i != pcount-1) {
        Printf(nativecall, ", ");
        Printf(f_shadow, ", ");
      }
  }


  Printf(f_shadow, ") {\n");
  Printv(nativecall,
	 ");\n",
	 tab8, " _selfown = true;\n",
	 "    }\n",
	 0);

  Printf(f_shadow, "%s", nativecall);
  Printf(f_shadow, "  }\n\n");
  Delete(nativecall);
}

void JAVA::cpp_destructor(char *name, char *newname) {
  this->Language::cpp_destructor(name,newname);

  if(!shadow) return;
  if(!shadow_classdef_emitted) emit_shadow_classdef();

  char *realname = (newname) ? newname : name;

  if(finalize) {
    fprintf(f_shadow, "  protected void finalize() {\n");
    fprintf(f_shadow, "    if(_selfown) {\n");
    fprintf(f_shadow, "      _delete();\n");
    fprintf(f_shadow, "    }\n");
    fprintf(f_shadow, "  };\n\n");
  }

  fprintf(f_shadow, "  public void _delete() {\n");
  fprintf(f_shadow, "    if(_self != 0 && %s.class == _selfClass()) {\n", shadow_classname);
  fprintf(f_shadow, "\t%s.%s(_self);\n", module, Swig_name_destroy(realname));
  fprintf(f_shadow, "\t_self = 0;\n");
  fprintf(f_shadow, "    }\n");
  fprintf(f_shadow, "  }\n\n");
}

void JAVA::cpp_class_decl(char *name, char *rename, char *type) {
  this->Language::cpp_class_decl(name,rename, type);

  if(!shadow) return;

  char *realname = (rename) ? rename : name;

  Setattr(shadow_classes,name, realname);
  if(type && strcmp(type, "struct") == 0) {
    sprintf(bigbuf, "struct %s", name);
    Setattr(shadow_classes, bigbuf, rename);
  }
}

void JAVA::cpp_inherit(char **baseclass, int) {
  this->Language::cpp_inherit(baseclass, 0);

  if(!shadow) return;

  int cnt = 0;
  char **bc = baseclass;
  while(*bc++) cnt++;

  if(cnt > 1)
    fprintf(stderr, "Warning: %s inherits from multiple base classes. Multiple inheritance is not supported.\n", shadow_classname);
 
  shadow_baseclass = copy_string(*baseclass);
}

void JAVA::cpp_variable(char *name, char *iname, DataType *t) {
  if(shadow && !shadow_classdef_emitted) emit_shadow_classdef();

  if(shadow) member_func = 1;
  shadow_name = copy_string((iname) ? iname : name);
  this->Language::cpp_variable(name, iname, t);
  member_func = 0;
}

void JAVA::cpp_static_var(char *name, char *iname, DataType *t) {
  if(shadow) member_func = 1;
  shadow_name = copy_string((iname) ? iname : name);
  this->Language::cpp_static_var(name, iname, t);
  member_func = 0;
}

void JAVA::cpp_declare_const(char *name, char *iname, DataType *type, char *value) {
  if(shadow && !shadow_classdef_emitted) emit_shadow_classdef();

  if(shadow) member_func = 1;
  shadow_name = copy_string((iname) ? iname : name);
  this->Language::cpp_declare_const(name, iname, type, value);
  member_func = 0;
}

