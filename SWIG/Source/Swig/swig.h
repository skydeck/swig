/* ----------------------------------------------------------------------------- 
 * swig.h
 *
 *     Header file for the SWIG core.
 * 
 * Author(s) : David Beazley (beazley@cs.uchicago.edu)
 *             Dustin Mitchell (djmitche@cs.uchicago.edu)
 *
 * Copyright (C) 1999-2000.  The University of Chicago
 * See the file LICENSE for information on usage and redistribution.	
 *
 * $Header$
 * ----------------------------------------------------------------------------- */

#ifndef _SWIGCORE_H
#define _SWIGCORE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "doh.h"

/* --- File interface --- */

extern void        Swig_add_directory(const DOHString_or_char *dirname);
extern DOHString  *Swig_last_file();
extern DOHList    *Swig_search_path();
extern FILE       *Swig_open(const DOHString_or_char *name);
extern DOHString  *Swig_read_file(FILE *f);
extern DOHString  *Swig_include(const DOHString_or_char *name);
extern int         Swig_insert_file(const DOHString_or_char *name, DOHFile *outfile);
extern int         Swig_bytes_read();

#define  SWIG_FILE_DELIMETER   "/"

/* --- Super Strings --- */

extern DOH *NewSuperString(char *s, DOH *filename, int firstline);
extern int SuperString_check(DOH *s);

/* --- Command line parsing --- */

extern void  Swig_init_args(int argc, char **argv);
extern void  Swig_mark_arg(int n);
extern void  Swig_check_options();
extern void  Swig_arg_error();

/* --- Scanner Interface --- */

typedef struct SwigScanner SwigScanner;

extern SwigScanner *NewSwigScanner();
extern void         DelSwigScanner(SwigScanner *);
extern void         SwigScanner_clear(SwigScanner *);
extern void         SwigScanner_push(SwigScanner *, DOHString *);
extern void         SwigScanner_pushtoken(SwigScanner *, int);
extern int          SwigScanner_token(SwigScanner *);
extern DOHString   *SwigScanner_text(SwigScanner *);
extern void         SwigScanner_skip_line(SwigScanner *);
extern int          SwigScanner_skip_balanced(SwigScanner *, int startchar, int endchar);
extern void         SwigScanner_set_location(SwigScanner *, DOHString *file, int line);
extern DOHString   *SwigScanner_get_file(SwigScanner *);
extern int          SwigScanner_get_line(SwigScanner *);
extern void         SwigScanner_idstart(SwigScanner *, char *idchar);

#define   SWIG_MAXTOKENS          512
#define   SWIG_TOKEN_LPAREN        1  
#define   SWIG_TOKEN_RPAREN        2
#define   SWIG_TOKEN_SEMI          3
#define   SWIG_TOKEN_COMMA         4
#define   SWIG_TOKEN_STAR          5
#define   SWIG_TOKEN_LBRACE        6
#define   SWIG_TOKEN_RBRACE        7
#define   SWIG_TOKEN_EQUAL         8
#define   SWIG_TOKEN_EQUALTO       9
#define   SWIG_TOKEN_NOTEQUAL     10
#define   SWIG_TOKEN_PLUS         11
#define   SWIG_TOKEN_MINUS        12
#define   SWIG_TOKEN_AND          13
#define   SWIG_TOKEN_LAND         14
#define   SWIG_TOKEN_OR           15
#define   SWIG_TOKEN_LOR          16
#define   SWIG_TOKEN_XOR          17
#define   SWIG_TOKEN_LESSTHAN     18
#define   SWIG_TOKEN_GREATERTHAN  19
#define   SWIG_TOKEN_LTEQUAL      20
#define   SWIG_TOKEN_GTEQUAL      21
#define   SWIG_TOKEN_NOT          22
#define   SWIG_TOKEN_LNOT         23
#define   SWIG_TOKEN_LBRACKET     24
#define   SWIG_TOKEN_RBRACKET     25
#define   SWIG_TOKEN_SLASH        26
#define   SWIG_TOKEN_BACKSLASH    27
#define   SWIG_TOKEN_ENDLINE      28
#define   SWIG_TOKEN_STRING       29
#define   SWIG_TOKEN_POUND        30
#define   SWIG_TOKEN_PERCENT      31
#define   SWIG_TOKEN_COLON        32
#define   SWIG_TOKEN_DCOLON       33
#define   SWIG_TOKEN_LSHIFT       34
#define   SWIG_TOKEN_RSHIFT       35
#define   SWIG_TOKEN_ID           36
#define   SWIG_TOKEN_FLOAT        37
#define   SWIG_TOKEN_DOUBLE       38
#define   SWIG_TOKEN_INT          39
#define   SWIG_TOKEN_UINT         40
#define   SWIG_TOKEN_LONG         41
#define   SWIG_TOKEN_ULONG        42
#define   SWIG_TOKEN_CHAR         43
#define   SWIG_TOKEN_PERIOD       44
#define   SWIG_TOKEN_AT           45
#define   SWIG_TOKEN_DOLLAR       46
#define   SWIG_TOKEN_CODEBLOCK    47
#define   SWIG_TOKEN_ILLEGAL      98
#define   SWIG_TOKEN_LAST         99 

/* --- Functions for manipulating the string-based type encoding --- */

extern void        SwigType_add_pointer(DOHString *t);
extern void        SwigType_add_array(DOHString *t, DOHString_or_char *size);
extern void        SwigType_add_reference(DOHString *t);
extern void        SwigType_add_qualifier(DOHString *t, DOHString_or_char *qual);
extern void        SwigType_add_function(DOHString *t, DOHList *parms);
extern DOHList    *SwigType_split(DOHString *t);
extern DOHString  *SwigType_pop(DOHString *t);
extern void        SwigType_push(DOHString *t, DOHString *s);
extern DOHList    *SwigType_parmlist(DOHString *p);
extern DOHString  *SwigType_parm(DOHString *p);
extern DOHString  *SwigType_cstr(DOHString *s, DOHString_or_char *id);
extern int         SwigType_ispointer(DOHString_or_char *t);
extern int         SwigType_isreference(DOHString_or_char *t);
extern int         SwigType_isarray(DOHString_or_char *t);
extern int         SwigType_isfunction(DOHString_or_char *t);
extern int         SwigType_isqualifier(DOHString_or_char *t);
extern DOHString  *SwigType_base(DOHString_or_char *t);
extern DOHString  *SwigType_prefix(DOHString_or_char *t);

extern int         SwigType_typedef(DOHString_or_char *type, DOHString_or_char *name);
extern void        SwigType_new_scope();
extern void        SwigType_reset_scopes();
extern void        SwigType_set_scope_name(DOHString_or_char *name);
extern void        SwigType_merge_scope(DOHHash *scope, DOHString_or_char *prefix);
extern DOHHash    *SwigType_pop_scope();
extern DOHString  *SwigType_typedef_resolve(DOHString_or_char *t);
extern int         SwigType_istypedef(DOHString_or_char *t);
extern int         SwigType_cmp(DOHString_or_char *pat, DOHString_or_char *t);
extern int         SwigType_array_ndim(DOHString_or_char *t);
extern DOHString  *SwigType_array_getdim(DOHString_or_char *t, int n);
extern void        SwigType_array_setdim(DOHString_or_char *t, int n, DOHString_or_char *rep);
extern DOHString  *SwigType_default(DOHString_or_char *t);

/* --- Parse tree support --- */

typedef struct {
   char *name;
   int  (*action)(DOH *obj, void *clientdata);
} SwigRule;

extern void Swig_dump_tags(DOH *obj, DOH *root);
extern void Swig_add_rule(DOHString_or_char *, int (*action)(DOH *, void *));
extern void Swig_add_rules(SwigRule ruleset[]);
extern void Swig_clear_rules();
extern int  Swig_emit(DOH *obj, void *clientdata);
extern void Swig_cut_node(DOH *obj);
extern DOH *Swig_next(DOH *obj);
extern DOH *Swig_prev(DOH *obj);

/* -- Wrapper function Object */

typedef struct {
  DOHHash   *localh;
  DOHString *def;
  DOHString *locals;
  DOHString *code;
} Wrapper;

extern Wrapper *NewWrapper();
extern void     DelWrapper(Wrapper *w);
extern void     Wrapper_print(Wrapper *w, DOHFile *f);
extern int      Wrapper_add_local(Wrapper *w, const DOHString_or_char *name, const DOHString_or_char *decl);
extern int      Wrapper_add_localv(Wrapper *w, const DOHString_or_char *name, ...);
extern int      Wrapper_check_local(Wrapper *w, const DOHString_or_char *name);
extern char    *Wrapper_new_local(Wrapper *w, const DOHString_or_char *name, const DOHString_or_char *decl);
extern char    *Wrapper_new_localv(Wrapper *w, const DOHString_or_char *name, ...);

/* --- Naming functions --- */

extern void        Swig_name_register(DOHString_or_char *method, DOHString_or_char *format);
extern char       *Swig_name_mangle(DOHString_or_char *s);
extern char       *Swig_name_wrapper(DOHString_or_char *fname);
extern char       *Swig_name_member(DOHString_or_char *classname, DOHString_or_char *mname);
extern char       *Swig_name_get(DOHString_or_char *vname);
extern char       *Swig_name_set(DOHString_or_char *vname);
extern char       *Swig_name_construct(DOHString_or_char *classname);
extern char       *Swig_name_destroy(DOHString_or_char *classname);

/* --- Mapping interface --- */

extern void        Swig_map_add(DOHHash *ruleset, DOHString_or_char *rulename, DOHHash *parms, DOH *obj);
extern DOH        *Swig_map_match(DOHHash *ruleset, DOHString_or_char *rulename, DOHHash *parms, int *nmatch);

/* --- Misc --- */
extern char *Swig_copy_string(const char *c);
extern void  Swig_banner(DOHFile *f);

/* --- Legacy DataType interface.  This is being replaced --- */

#define    T_INT       1
#define    T_SHORT     2
#define    T_LONG      3
#define    T_UINT      4
#define    T_USHORT    5
#define    T_ULONG     6
#define    T_UCHAR     7
#define    T_SCHAR     8
#define    T_BOOL      9
#define    T_DOUBLE    10
#define    T_FLOAT     11
#define    T_CHAR      12
#define    T_USER      13
#define    T_VOID      14
#define    T_SYMBOL    98
#define    T_ERROR     99

/* These types are now obsolete, but defined for backwards compatibility */

#define    T_SINT      90
#define    T_SSHORT    91
#define    T_SLONG     92

#define MAX_NAME 96

typedef struct DataType {
  int         type;            /* SWIG Type code */
  char        name[MAX_NAME];  /* Name of type   */
  int         is_pointer;      /* Is this a pointer */
  int         implicit_ptr;    /* Implicit ptr */
  int         is_reference;    /* A C++ reference type */
  int         status;          /* Is this datatype read-only? */
  char        *_qualifier;     /* A qualifier string (ie. const). */
  char        *_arraystr;      /* String containing array part */
  int         id;              /* type identifier (unique for every type). */
} DataType;

extern DataType *NewDataType(int type);
extern DataType *CopyDataType(DataType *type);
extern void      DelDataType(DataType *type);

/* -- New type interface -- */

extern char     *DataType_str(DataType *, char *name);  /* Exact datatype */

/* -- Old type interface -- */

extern char     *DataType_qualifier(DataType *);
extern void      DataType_set_qualifier(DataType *, char *q);
extern char     *DataType_arraystr(DataType *);
extern void      DataType_set_arraystr(DataType *, char *a);

extern void      DataType_primitive(DataType *);
extern char     *DataType_print_type(DataType *);
extern char     *DataType_print_full(DataType *);
extern char     *DataType_print_cast(DataType *);
extern char     *DataType_print_mangle(DataType *);
extern char     *DataType_print_arraycast(DataType *);
extern char     *DataType_print_mangle_default(DataType *);
extern void      DataType_set_mangle(char *(*m)(DataType *));
extern int       DataType_array_dimensions(DataType *);
extern char     *DataType_get_dimension(DataType *, int);

/* Typedef support */
extern int       DataType_typedef_add(DataType *, char *name, int mode);
extern void      DataType_typedef_resolve(DataType *, int level);
extern void      DataType_typedef_replace(DataType *);
extern int       DataType_is_typedef(char *name);
extern void      DataType_updatestatus(DataType *, int newstatus);
extern void      DataType_init_typedef();
extern void      DataType_merge_scope(DOHHash *h);
extern void      DataType_new_scope(DOHHash *h);
extern void     *DataType_collapse_scope(char *name);
extern void      DataType_remember(DataType *);
extern void      DataType_record_base(char *derived, char *base);

extern int       type_id;
extern void      emit_ptr_equivalence(DOHFile *tablef, DOHFile *initf);
extern void      emit_type_table(DOHFile *out);
extern void      typeeq_derived(char *n1, char *n2, char *cast);
extern void      typeeq_addtypedef(char *name, char *eqname, DataType *t);

#define STAT_REPLACETYPE   2

/* --- Deprecated parameter list structure */

typedef struct Parm {
  DataType   *_type;            /* Datatype of this parameter */
  char       *_name;            /* Name of parameter (optional) */
  char       *_defvalue;        /* Default value (as a string) */
  int        ignore;            /* Ignore flag */
} Parm;

extern Parm     *NewParm(DataType *type, char *n);
extern Parm     *CopyParm(Parm *p);
extern void      DelParm(Parm *p);
extern void      Parm_Settype(Parm *p, DataType *t);
extern DataType *Parm_Gettype(Parm *p);
extern void      Parm_Setname(Parm *p, char *name);
extern char     *Parm_Getname(Parm *p);
extern void      Parm_Setvalue(Parm *p, char *value);
extern char     *Parm_Getvalue(Parm *p);

typedef struct ParmList {
  int     maxparms;               /* Max parms possible in current list  */
  Parm  **parms;                  /* Pointer to parms array */
  int     current_parm;           /* Internal state for get_first,get_next */
  int     nparms;                 /* Number of parms in list */
} ParmList;

extern ParmList *NewParmList();
extern ParmList *CopyParmList(ParmList *);
extern void      DelParmList(ParmList *);
extern Parm     *ParmList_get(ParmList *l, int pos);
extern void      ParmList_append(ParmList *, Parm *);
extern void      ParmList_insert(ParmList *, Parm *, int);
extern void      ParmList_del(ParmList *, int);
extern int       ParmList_numarg(ParmList *);
extern Parm     *ParmList_first(ParmList *);
extern Parm     *ParmList_next(ParmList *);
extern void      ParmList_print_types(ParmList*,DOHFile *f);
extern void      ParmList_print_args(ParmList *, DOHFile *f);

#endif




