/*
 * Test reserved keyword renaming
 */

%module keyword_rename

#pragma SWIG nowarn=SWIGWARN_PARSE_KEYWORD

%inline %{

#define KW(x, y) int x (int y) { return y;} 

/* Python keywords */
KW(in, except)
KW(except, in)
KW(raise, in)

/* Perl keywords */
KW(tie, die)
KW(use, next)

/* Java keywords */
KW(implements, native)
KW(byte, final)

/* C# Keywords */
KW(string, out)
struct sealed {};

%}


