/* ----------------------------------------------------------------------------- 
 * doh.h
 *
 *     This file describes of the externally visible functions in DOH.
 * 
 * Author(s) : David Beazley (beazley@cs.uchicago.edu)
 *
 * Copyright (C) 1999-2000.  The University of Chicago
 * See the file LICENSE for information on usage and redistribution.	
 *
 * $Header$
 * ----------------------------------------------------------------------------- */

#ifndef _DOH_H
#define _DOH_H

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DOH_MAJOR_VERSION 0
#define DOH_MINOR_VERSION 1

typedef void DOH;                

#define DOH_BEGIN    -1
#define DOH_END      -2
#define DOH_CUR      -3
#define DOH_CURRENT  -3

/* -----------------------------------------------------------------------------
 * These structures define the interface to various categories of objects.
 * ----------------------------------------------------------------------------- */

/* Mapping Objects (i.e., hash tables) */
typedef struct {
  DOH    *(*doh_getattr)(DOH *obj, DOH *name);               /* Get attribute */
  int     (*doh_setattr)(DOH *obj, DOH *name, DOH *value);   /* Set attribute */
  int     (*doh_delattr)(DOH *obj, DOH *name);               /* Del attribute */
  DOH    *(*doh_firstkey)(DOH *obj);                         /* First key     */
  DOH    *(*doh_nextkey)(DOH *obj);                          /* Next key      */
} DohMappingMethods;

/* Sequence methods (i.e., lists) */
typedef struct {
  DOH      *(*doh_getitem)(DOH *obj, int index);             /* Get item      */
  int       (*doh_setitem)(DOH *obj, int index, DOH *value); /* Set item      */
  int       (*doh_delitem)(DOH *obj, int index);             /* Delete item   */
  int       (*doh_insitem)(DOH *obj, int index, DOH *value); /* Insert item   */
  DOH      *(*doh_firstitem)(DOH *obj);                      /* Iterators     */  
  DOH      *(*doh_nextitem)(DOH *obj);
} DohSequenceMethods;

/* File methods */
typedef struct {
  int       (*doh_read)(DOH *obj, void *buffer, int nbytes);
  int       (*doh_write)(DOH *obj, void *buffer, int nbytes);
  int       (*doh_putc)(DOH *obj, int ch);
  int       (*doh_getc)(DOH *obj);
  int       (*doh_ungetc)(DOH *obj, int ch);
  int       (*doh_seek)(DOH *obj, long offset, int whence);
  long      (*doh_tell)(DOH *obj);
  int       (*doh_close)(DOH *obj);
} DohFileMethods;

/* String methods */
typedef struct {
  int     (*doh_replace)(DOH *obj, DOH *old, DOH *rep, int flags);
  void    (*doh_chop)(DOH *obj);
} DohStringMethods;

/* Callable */
typedef struct {
  DOH    *(*doh_call)(DOH *obj, DOH *args);                 /* Callable */
} DohCallableMethods;

/* Positional */
typedef struct {
  void    (*doh_setfile)(DOH *obj, DOH *file);
  DOH    *(*doh_getfile)(DOH *obj);
  void    (*doh_setline)(DOH *obj, int line);
  int     (*doh_getline)(DOH *obj);
} DohPositionalMethods;

/* -----------------------------------------------------------------------------
 * DohObjInfo
 * 
 * A pointer to this structure is included in all DOH types and is used to
 * describe the properties of various objects.
 * ----------------------------------------------------------------------------- */

typedef struct DohObjInfo {
  char       *objname;                         /* Object name        */
  int         objsize;                         /* Object size        */

  /* Basic object methods */
  void      (*doh_del)(DOH *obj);              /* Delete object      */
  DOH      *(*doh_copy)(DOH *obj);             /* Copy and object    */
  void      (*doh_clear)(DOH *obj);            /* Clear an object    */
  void      (*doh_scope)(DOH *obj, int s);     /* Change the scope on an object */

  /* Output methods */
  DOH       *(*doh_str)(DOH *obj);             /* Make a full string */
  void      *(*doh_data)(DOH *obj);            /* Return raw data    */
  int        (*doh_dump)(DOH *obj, DOH *out);  /* Serialize on out   */
  DOH       *(*doh_load)(DOH *in);             /* Unserialize from in */

  /* Length and hash values */
  int       (*doh_len)(DOH *obj);          
  int       (*doh_hash)(DOH *obj); 

  /* Compare */ 
  int       (*doh_cmp)(DOH *obj1, DOH *obj2);

  DohMappingMethods  *doh_mapping;             /* Mapping methods    */
  DohSequenceMethods *doh_sequence;            /* Sequence methods   */
  DohFileMethods     *doh_file;                /* File methods       */
  DohStringMethods   *doh_string;              /* String methods     */
  DohCallableMethods *doh_callable;            /* Callable methods   */ 
  DohPositionalMethods *doh_position;          /* Positional methods */
  void               *reserved4;
  void               *reserved5;
  void               *reserved6;
  void               *user1;
  void               *user2;
  void               *user3;
  void               *user4;
} DohObjInfo;

/* Memory management */
  extern void   *DohMalloc(size_t size);             /* Allocate memory       */
  extern void   *DohRealloc(void *, size_t size);    /* Reallocate memory     */
  extern void    DohFree(DOH *ptr);                  /* Free memory           */
  extern void   *DohObjMalloc(size_t size);          /* Allocate a DOH object */
  extern void    DohObjFree(DOH *ptr);               /* Free a DOH object     */
  extern int     DohObjFreeCheck(DOH *ptr);          /* Check if already free */
  extern void    DohInit(DOH *obj);                  /* Initialize an object  */
  extern void    DohXInit(DOH *obj);                 /* Initialize extended object */
  extern int     DohCheck(DOH *ptr);                 /* Check if a DOH object */
  extern int     DohPoolSize(int);                   /* Set memory alloc size */
  extern int     DohNewScope();                      /* Create a new scope    */
  extern void    DohDelScope(int);                   /* Delete a scope        */
  extern void    DohGarbageCollect();                /* Invoke garbage collection */

  extern void    DohIntern(DOH *);                   /* Intern an object      */

  /* Basic object methods.  Common to most objects */

  extern void    DohDelete(DOH *obj);                /* Delete an object      */
  extern DOH    *DohCopy(DOH *obj);
  extern void    DohClear(DOH *obj);
  extern void    DohSetScope(DOH *, int scp);        /* Set scope of object   */
  extern DOH    *DohStr(DOH *obj);
  extern void   *DohData(DOH *obj);
  extern int     DohDump(DOH *obj, DOH *out);
  extern int     DohLen(DOH *obj);
  extern int     DohHashval(DOH *obj);
  extern int     DohCmp(DOH *obj1, DOH *obj2);

  /* Mapping methods */
  
  extern DOH    *DohGetattr(DOH *obj, DOH *name);
  extern int     DohSetattr(DOH *obj, DOH *name, DOH *value);
  extern void    DohDelattr(DOH *obj, DOH *name);
  extern DOH    *DohFirstkey(DOH *obj);
  extern DOH    *DohNextkey(DOH *obj);
  extern int     DohGetInt(DOH *obj, DOH *name);
  extern double  DohGetDouble(DOH *obj, DOH *name);
  extern char   *DohGetChar(DOH *obj, DOH *name);
  extern void    DohSetInt(DOH *obj, DOH *name, int);
  extern void    DohSetDouble(DOH *obj, DOH *name, double);

  /* Sequence methods */

  extern DOH    *DohGetitem(DOH *obj, int index);
  extern int     DohSetitem(DOH *obj, int index, DOH *value);
  extern int     DohDelitem(DOH *obj, int index);
  extern int     DohInsertitem(DOH *obj, int index, DOH *value);
  extern DOH    *DohFirstitem(DOH *obj);
  extern DOH    *DohNextitem(DOH *obj);

  /* File methods */

  extern int     DohWrite(DOH *obj, void *buffer, int length);
  extern int     DohRead(DOH *obj, void *buffer, int length);
  extern int     DohSeek(DOH *obj, long offset, int whence);
  extern long    DohTell(DOH *obj);
  extern int     DohGetc(DOH *obj);
  extern int     DohPutc(int ch, DOH *obj);
  extern int     DohUngetc(int ch, DOH *obj);

  /* Callable Methods */

  extern DOH    *DohCall(DOH *obj, DOH *args);

  /* Position Methods */

  extern int     DohGetline(DOH *obj);
  extern void    DohSetline(DOH *obj, int line);
  extern DOH    *DohGetfile(DOH *obj);
  extern void    DohSetfile(DOH *obj, DOH *file);

  /* String Methods */

  extern int     DohReplace(DOH *src, DOH *token, DOH *rep, int flags);
  extern void    DohChop(DOH *src);

  /* Utility functions */

  extern void    DohEncoding(char *name, DOH *(*fn)(DOH *s));
  extern int     DohPrintf(DOH *obj, char *format, ...);
  extern int     DohvPrintf(DOH *obj, char *format, va_list ap);
  extern DOH    *DohReadline(DOH *in);

  /* Miscellaneous */

  extern void    DohTrace(int level, char *fmt,...);
  extern void    DohDebug(int d);

  extern int     DohIsMapping(DOH *obj);
  extern int     DohIsSequence(DOH *obj);
  extern int     DohIsString(DOH *obj);
  extern int     DohIsFile(DOH *obj);
  extern int     DohIsCallable(DOH *obj);

#ifndef DOH_LONG_NAMES
/* Macros to invoke the above functions.  Includes the location of
   the caller to simplify debugging if something goes wrong */

#define Delete             DohDelete
#define Copy               DohCopy
#define Clear              DohClear
#define Str                DohStr
#define Dump               DohDump
#define Getattr            DohGetattr
#define Setattr            DohSetattr
#define Delattr            DohDelattr
#define Hashval            DohHashval
#define Getitem            DohGetitem
#define Setitem            DohSetitem
#define Delitem            DohDelitem
#define Insert             DohInsertitem
#define Append(s,x)        DohInsertitem(s,DOH_END,x)
#define Push(s,x)          DohInsertitem(s,DOH_BEGIN,x)
#define Len                DohLen
#define Firstkey           DohFirstkey
#define Nextkey            DohNextkey
#define Data               DohData
#define Char               (char *) Data
#define Cmp                DohCmp
#define Setline            DohSetline
#define Getline            DohGetline
#define Setfile            DohSetfile
#define Getfile            DohGetfile
#define Write              DohWrite
#define Read               DohRead
#define Seek               DohSeek
#define Tell               DohTell
#define Printf             DohPrintf
#define Getc               DohGetc
#define Putc               DohPutc
#define Ungetc             DohUngetc
#define vPrintf            DohvPrintf
#define GetInt             DohGetInt
#define GetDouble          DohGetDouble
#define GetChar            DohGetChar
#define SetInt             DohSetInt
#define SetDouble          DohSetDouble
#define Firstitem          DohFirstitem
#define Nextitem           DohNextitem
#define Readline           DohReadline
#define Replace            DohReplace
#define Chop               DohChop
#define NewScope           DohNewScope
#define DelScope           DohDelScope
#define Call               DohCall
#endif

/* -----------------------------------------------------------------------------
 * DohBase
 *
 * DohBase object type.  Attributes common to all objects.
 * ----------------------------------------------------------------------------- */

#define  DOHCOMMON      \
   DohObjInfo    *objinfo; \
   DOH           *nextptr; \
   int            refcount; \
   unsigned char  flags; \
   unsigned char  scope

typedef struct {
  DOHCOMMON;
} DohBase;

#define  DOHXCOMMON      \
  DOHCOMMON; \
  DOH    *file; \
  int     line

typedef struct {
  DOHXCOMMON;
} DohXBase;

/* Macros for decrefing and increfing (safe for null objects). */
#define Decref(a)        if (a) ((DohBase *) a)->refcount--
#define Incref(a)        if (a) ((DohBase *) a)->refcount++
#define Refcount(a)      ((DohBase *) a)->refcount

#define Getscope(a)      ((DohBase *) a)->scope
#define Setscope(a,s)    DohSetScope(a,s)
#define Objname(a)       ((DohBase *) a)->objinfo->objname

/* Flags for various internal operations */

#define DOH_FLAG_SETSCOPE    0x01
#define DOH_FLAG_PRINT       0x02
#define DOH_FLAG_DELSCOPE    0x04
#define DOH_FLAG_GC          0x08
#define DOH_FLAG_INTERN      0x10
  
/* -----------------------------------------------------------------------------
 * Strings.   
 * ----------------------------------------------------------------------------- */

extern DOH   *NewString(DOH *c);
extern DOH   *NewStringf(DOH *fmt, ...);

extern int    String_check(DOH *s);

/* String replacement flags */

#define   DOH_REPLACE_ANY         0x00
#define   DOH_REPLACE_NOQUOTE     0x01
#define   DOH_REPLACE_ID          0x02
#define   DOH_REPLACE_FIRST       0x04

/* -----------------------------------------------------------------------------
 * Files
 * ----------------------------------------------------------------------------- */

extern DOH *NewFile(DOH *file, char *mode);
extern DOH *NewFileFromFile(FILE *f);
extern DOH *NewFileFromFd(int fd);
extern int  DohCopyto(DOH *input, DOH *output);

#define Copyto DohCopyto

/* -----------------------------------------------------------------------------
 * List
 * ----------------------------------------------------------------------------- */

extern DOH  *NewList();
extern int  List_check(DOH *);
extern void List_sort(DOH *);

/* -----------------------------------------------------------------------------
 * Hash
 * ----------------------------------------------------------------------------- */

extern DOH   *NewHash();
extern int    Hash_check(DOH *h);
extern DOH   *Hash_keys(DOH *);

/* -----------------------------------------------------------------------------
 * Void
 * ----------------------------------------------------------------------------- */

extern DOH  *NewVoid(void *ptr, void (*del)(void *));

extern DOH *DohSplit(DOH *input, char *chs, int nsplits);
#define Split DohSplit

extern DOH *DohNone;

/* ----------------------------------------------------------------------------- 
 * Callable
 * ----------------------------------------------------------------------------- */

extern DOH *NewCallable(DOH *(*func)(DOH *, DOH *));

/* -----------------------------------------------------------------------------
 * Error handling levels.
 * ----------------------------------------------------------------------------- */

#define DOH_UNSUPPORTED 0x01
#define DOH_UNKNOWN     0x02
#define DOH_MEMORY      0x04
#define DOH_CONVERSION  0x08
#define DOH_CALLS       0x10

#ifdef __cplusplus
}
#endif

#endif /* DOH_H */
