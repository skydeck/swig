/* ----------------------------------------------------------------------------- 
 * stype.c
 *
 *     This file provides general support for datatypes that are encoded in
 *     the form of simple strings.
 * 
 * Author(s) : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (C) 1999-2000.  The University of Chicago
 * See the file LICENSE for information on usage and redistribution.	
 * ----------------------------------------------------------------------------- */

#include "swig.h"

/* -----------------------------------------------------------------------------
 * Synopsis
 *
 * One of the goals of SWIG2.0 is to provide a standardized representation
 * of parse trees using XML.   However, it order to do this, it is necessary
 * to provide a textual representation of all parsing elements including datatypes.
 * Unfortunately, type systems are relatively complicated--making a
 * low-level or direct XML representation unnecessarily complicated to work
 * with.  For example, you probably wouldn't want to do this:
 *
 *       <type><pointer><array size="500"><pointer><pointer>int
 *             </pointer></pointer></array></pointer></type>
 *        
 * The purpose of this module is to provide a general purpose type system
 * in which types are represented as simple text strings.   The goal of
 * this representation is to make it extremely easy to specify types in
 * XML documents, reduce memory overhead, and to allow fairly easy type
 * manipulation by anyone who wants to play with the type system.
 *
 * General idea:
 *
 * Types are represented by a base type (e.g., "int") and a collection of
 * type elements applied to the base (e.g., pointers, arrays, etc...).
 * 
 * Encoding:
 *
 * Types are encoded as strings of type constructors separated by '.' delimeters.
 *
 *            String Encoding             C Example
 *            ---------------             ---------
 *            *.*.int                     int **
 *            [300].[400].int             int [300][400]
 *            *.+const.char               char const *
 * 
 * '*'       = Pointer
 * '[...]'   = Array
 * '(...)'   = Function
 * '{...}'   = Structure
 * '&'       = Reference
 * '+str'    = Qualifier
 *
 * The encoding follows the order that you might describe a type in words.
 * For example "*.[200].int" is "A pointer to array of integers" and "*.+const.char"
 * is "a pointer to a const char".
 *
 * This representation is particularly convenient because string operations
 * can be used to combine and manipulate types.  For example, a type could be
 * formed by combining two strings such as the following:
 *
 *        "*.*." + "[400].int" = "*.*.[400].int"
 *
 * Similarly, one could strip a 'const' declaration from a type doing something
 * like this:
 *
 *        Replace(t,"+const.","",DOH_REPLACE_ANY)
 *
 * The string representation of types also provides a convenient way to 
 * build typemaps and other pattern matching rules against type information.
 * ----------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
 * SwigType_add_pointer()
 *
 * Adds a pointer constructor to a type
 * ----------------------------------------------------------------------------- */

void 
SwigType_add_pointer(DOH *t) {
  assert(DohIsString(t));
  Insert(t,0,"*.");
}

/* -----------------------------------------------------------------------------
 * SwigType_add_array()
 *
 * Adds an array constructor to a type
 * ----------------------------------------------------------------------------- */

void 
SwigType_add_array(DOH *t, DOH *size) {
  assert(DohIsString(t));
  Insert(t,0,"].");
  Insert(t,0,size);
  Insert(t,0,"[");
}

/* -----------------------------------------------------------------------------
 * SwigType_add_reference()
 *
 * Adds a reference constructor to a type.
 * ----------------------------------------------------------------------------- */

void 
SwigType_add_reference(DOH *t) {
  assert(DohIsString(t));
  Insert(t,0,"&.");
}

/* -----------------------------------------------------------------------------
 * SwigType_add_qualifier()
 *
 * Adds a qualifier to a type
 * ----------------------------------------------------------------------------- */

void 
SwigType_add_qualifier(DOH *t, DOH *qual) {
  assert(DohIsString(t));
  Insert(t,0,".");
  Insert(t,0,qual);
  Insert(t,0,"+");
}

/* -----------------------------------------------------------------------------
 * SwigType_add_function()
 *
 * Adds a function to a type. Accepts a list of abstract types as parameters.
 * These abstract types should be passed as a list of type-strings.
 * ----------------------------------------------------------------------------- */

void 
SwigType_add_function(DOH *t, DOH *parms) {
  DOH *pstr;
  int i,l;
  assert(DohIsString(t));
  Insert(t,0,").");
  pstr = NewString("");
  l = Len(parms);
  Putc('(',pstr);
  for (i = 0; i < l; i++) {
    Printf(pstr,"%s",Getitem(parms,i));
    if (i < (l-1))
      Putc(',',pstr);
  }
  Insert(t,0,pstr);
  Delete(pstr);
}

/* -----------------------------------------------------------------------------
 * static isolate_element()
 *
 * Isolate a single element of a type string (delimeted by periods)
 * ----------------------------------------------------------------------------- */
static DOH *
isolate_element(char *c) {
  DOH *result = NewString("");
  while (*c) {
    if (*c == '.') return result;
    else if (*c == '(') {
      int nparen = 1;
      Putc(*c,result);
      c++;
      while(*c) {
	Putc(*c,result);
	if (*c == '(') nparen++;
	if (*c == ')') {
	  nparen--;
	  if (nparen == 0) break;
	}
	c++;
      }
    } else if (*c == '{') {
      int nbrace = 1;
      Putc(*c,result);
      c++;
      while(*c) {
	Putc(*c,result);
	if (*c == '{') nbrace++;
	if (*c == '}') {
	  nbrace--;
	  if (nbrace == 0) break;
	}
	c++;
      }
    } else {
      Putc(*c,result);
    }
    if (*c) c++;
  }
  return result;
}
/* -----------------------------------------------------------------------------
 * SwigType_split(DOH *t)
 *
 * Splits a type into it's component parts and returns a list.
 * ----------------------------------------------------------------------------- */

DOH *SwigType_split(DOH *t) {
  DOH *item, *list;
  char *c;
  int len;
  assert(DohIsString(t));
  c = Char(t);
  
  list = NewList();
  while (*c) {
    item = isolate_element(c);
    len = Len(item);
    if (len) {
      Append(list,item);
      Delete(item);
    } else {
      Delete(item);
      break;
    }
    c = c + len;
    if (*c == '.') c++;
  }
  return list;
}

/* -----------------------------------------------------------------------------
 * SwigType_pop()
 *
 * Pop off the first type-constructor object and update the type
 * ----------------------------------------------------------------------------- */

DOH *SwigType_pop(DOH *t)
{
  DOH *result;
  char *c;
  assert(DohIsString(t));
  if (Len(t) == 0) return 0;
  c = Char(t);
  result = isolate_element(c);
  Replace(t,result,"",DOH_REPLACE_ANY | DOH_REPLACE_FIRST);
  c = Char(t);
  if (*c == '.') {
    Delitem(t,0);
  }
  return result;
}

/* -----------------------------------------------------------------------------
 * SwigType_push()
 *
 * Push a type constructor onto the type
 * ----------------------------------------------------------------------------- */

void SwigType_push(DOH *t, DOH *cons)
{
  assert(DohIsString(t));
  if (Len(t)) {
    int len;
    char *c = Char(t);
    if (c[strlen(c)-1] != '.')
      Insert(t,0,".");
  }
  Insert(t,0,cons);
}

/* -----------------------------------------------------------------------------
 * SwigType_split_parms()
 *
 * Splits a comma separated list of components into strings.
 * ----------------------------------------------------------------------------- */

DOH *SwigType_split_parms(DOH *p) {
  DOH *item, *list;
  char *c;
  assert(DohIsString(p));
  c = Char(p);
  assert(*c == '(');
  c++;
  list = NewList();
  item = NewString("");
  while (*c) {
    if (*c == ',') {
      Append(list,item);
      Delete(item);
      item = NewString("");
    } else if (*c == '(') {
      int nparens = 1;
      Putc(*c,item);
      c++;
      while (*c) {
	Putc(*c,item);
	if (*c == '(') nparens++;
	if (*c == ')') {
	  nparens--;
	  if (nparens == 0) break;
	}
	c++;
      }
    } else if (*c == ')') {
      break;
    } else if (*c == '{') {
      int nbraces = 1;
      Putc(*c,item);
      c++;
      while (*c) {
	Putc(*c,item);
	if (*c == '{') nbraces++;
	if (*c == '}') {
	  nbraces--;
	  if (nbraces == 0) break;
	}
	c++;
      }
    } else {
      Putc(*c,item);
    }
    if (*c) 
      c++;
  }
  Append(list,item);
  Delete(item);
  return list;
}


/* -----------------------------------------------------------------------------
 * SwigType_split_struct()
 *
 * Splits a comma separated list of structure components
 * ----------------------------------------------------------------------------- */

DOH *SwigType_split_struct(DOH *p) {
  DOH *item, *list;
  char *c;
  assert(DohIsString(p));
  c = Char(p);
  assert(*c == '{');
  c++;
  list = NewList();
  item = NewString("");
  while (*c) {
    if (*c == ',') {
      Append(list,item);
      Delete(item);
      item = NewString("");
    } else if (*c == '{') {
      int nbrace = 1;
      Putc(*c,item);
      c++;
      while (*c) {
	Putc(*c,item);
	if (*c == '{') nbrace++;
	if (*c == '}') {
	  nbrace--;
	  if (nbrace == 0) break;
	}
	c++;
      }
    } else if (*c == '}') {
      break;
    } else if (*c == '(') {
      int nparen = 1;
      Putc(*c,item);
      c++;
      while (*c) {
	Putc(*c,item);
	if (*c == '(') nparen++;
	if (*c == ')') {
	  nparen--;
	  if (nparen == 0) break;
	}
	c++;
      }
    } else {
      Putc(*c,item);
    }
    if (*c) 
      c++;
  }
  Append(list,item);
  Delete(item);
  return list;
}

/* -----------------------------------------------------------------------------
 * SwigType_ispointer()
 * SwigType_isarray()
 * SwigType_isreference()
 * SwigType_isfunction()
 * SwigType_isstruct()
 * SwigType_isqualifier()
 *
 * Testing functions for querying a datatype
 * ----------------------------------------------------------------------------- */

int SwigType_ispointer(DOH *t) {
  char *c;
  assert(DohIsString(t));
  c = Char(t);
  if (*c == '*') return 1;
  return 0;
}

int SwigType_isreference(DOH *t) {
  char *c;
  assert(DohIsString(t));
  c = Char(t);
  if (*c == '&') return 1;
  return 0;
}

int SwigType_isarray(DOH *t) {
  char *c;
  assert(DohIsString(t));
  c = Char(t);
  if (*c == '[') return 1;
  return 0;
}

int SwigType_isfunction(DOH *t) {
  char *c;
  assert(DohIsString(t));
  c = Char(t);
  if (*c == '(') return 1;
  return 0;
}

int SwigType_isstruct(DOH *t) {
  char *c;
  assert(DohIsString(t));
  c = Char(t);
  if (*c == '{') return 1;
  return 0;
}

int SwigType_isqualifier(DOH *t) {
  char *c;
  assert(DohIsString(t));
  c = Char(t);
  if (*c == '+') return 1;
  return 0;
}

/* -----------------------------------------------------------------------------
 * SwigType_base()
 * 
 * Returns the base of a datatype.
 * ----------------------------------------------------------------------------- */

DOH *SwigType_base(DOH *t) {
  char *c, *d;
  assert(DohIsString(t));
  c = Char(t);
  d = c + strlen(c);
  while (d > c) {
    d--;
    if (*d == '.') return NewString(d+1);
  }
  return NewString(c);
}

/* -----------------------------------------------------------------------------
 * SwigType_cstr(DOH *s, DOH *id)
 *
 * Create a C string representation of a datatype.
 * ----------------------------------------------------------------------------- */

DOH *
SwigType_cstr(DOH *s, DOH *id)
{
  DOH *result;
  DOH *element, *nextelement;
  DOH *elements;
  int nelements, i;
  char *c;

  if (id) {
    result = NewString(Char(id));
  } else {
    result = NewString("");
  }

  elements = SwigType_split(s);
  Printf(stdout,"%s\n",elements);
  nelements = Len(elements);

  if (nelements > 0) {
    element = Getitem(elements,0);
  }
  /* Now, walk the type list and start emitting */
  for (i = 0; i < nelements; i++) {
    if (i < (nelements - 1)) {
      nextelement = Getitem(elements,i+1);
    } else {
      nextelement = 0;
    }
    if (SwigType_ispointer(element)) {
      Insert(result,0,"*");
      if ((nextelement) && ((SwigType_isfunction(nextelement) || (SwigType_isarray(nextelement))))) {
	Insert(result,0,"(");
	Append(result,")");
      }
    }
    else if (SwigType_isreference(element)) Insert(result,0,"&");
    else if (SwigType_isarray(element)) Append(result,element);
    else if (SwigType_isfunction(element)) {
      DOH *parms, *p;
      int j, plen;
      Append(result,"(");
      parms = SwigType_split_parms(element);
      plen = Len(parms);
      for (j = 0; j < plen; j++) {
	p = SwigType_cstr(Getitem(parms,j),0);
	Append(result,p);
	if (j < (plen-1)) Append(result,",");
	Delete(p);
      }
      Append(result,")");
      Delete(parms);
    } else if (SwigType_isstruct(element)) {
      DOH *members, *m;
      int j, mlen;
      Append(result,"{");
      members = SwigType_split_struct(element);
      mlen = Len(members);
      for (j = 0; j < mlen; j++) {
	m = SwigType_cstr(Getitem(members,j),0);
	Append(result,m);
	if (j < (mlen-1)) Append(result,";");
	Delete(m);
      }
      Append(result,"}");
      Delete(members);
    } else if (SwigType_isqualifier(element)) {
      Insert(result,0, " ");
      Insert(result,0,Char(element)+1);

    } else {
      Insert(result,0," ");
      Insert(result,0,element);
    }
    element = nextelement;
  }
  return result;
}
