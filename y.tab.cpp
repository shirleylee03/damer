//
// Created on 2020/10/10.
//


#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

	  Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
//#line 1 "hello.y"

#include<iostream>
#define YYSTYPE class gtree*
#include "AST.h"
#include <stdio.h>
#include <string.h>

gtree *p, *head;
void insert(gtree *parent, gtree *child);
gtree* newNode(std::string s, int type);
extern int yylex();//这行yylex是改为C++之后加上的
int yyerror(char *s);
void type_add(gtree *p);
char* strdup (const char* s)
{
    size_t slen = strlen(s);
    char* result = (char *)malloc(slen + 1);
    if(result == NULL)
    {
        return NULL;
    }

    memcpy(result, s, slen+1);
    return result;
}

/* Line 189 of yacc.c  */
//#line 85 "y.tab.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
/* Put the tokens into the symbol table, so that GDB and other debuggers
   know about them.  */
enum yytokentype {
    IDENTIFIER = 258,
    CONSTANT = 259,
    STRING_LITERAL = 260,
    SIZEOF = 261,
    PTR_OP = 262,
    INC_OP = 263,
    DEC_OP = 264,
    LEFT_OP = 265,
    RIGHT_OP = 266,
    LE_OP = 267,
    GE_OP = 268,
    EQ_OP = 269,
    NE_OP = 270,
    AND_OP = 271,
    OR_OP = 272,
    MUL_ASSIGN = 273,
    DIV_ASSIGN = 274,
    MOD_ASSIGN = 275,
    ADD_ASSIGN = 276,
    SUB_ASSIGN = 277,
    LEFT_ASSIGN = 278,
    RIGHT_ASSIGN = 279,
    AND_ASSIGN = 280,
    XOR_ASSIGN = 281,
    OR_ASSIGN = 282,
    //TYPE_NAME = 283,
    TYPEDEF = 284,
    EXTERN = 285,
    STATIC = 286,
    AUTO = 287,
    REGISTER = 288,
    CHAR = 289,
    SHORT = 290,
    INT = 291,
    LONG = 292,
    SIGNED = 293,
    UNSIGNED = 294,
    FLOAT = 295,
    DOUBLE = 296,
    CONST = 297,
    VOLATILE = 298,
    VOID = 299,
    STRUCT = 300,
    UNION = 301,
    ENUM = 302,
    ELLIPSIS = 303,
    CASE = 304,
    DEFAULT = 305,
    IF = 306,
    SWITCH = 307,
    WHILE = 308,
    DO = 309,
    FOR = 310,
    GOTO = 311,
    CONTINUE = 312,
    BREAK = 313,
    RETURN = 314,
    ELSE = 315,
    REMAIN = 316
};
#endif
/* Tokens.  */
#define IDENTIFIER 258
#define CONSTANT 259
#define STRING_LITERAL 260
#define SIZEOF 261
#define PTR_OP 262
#define INC_OP 263
#define DEC_OP 264
#define LEFT_OP 265
#define RIGHT_OP 266
#define LE_OP 267
#define GE_OP 268
#define EQ_OP 269
#define NE_OP 270
#define AND_OP 271
#define OR_OP 272
#define MUL_ASSIGN 273
#define DIV_ASSIGN 274
#define MOD_ASSIGN 275
#define ADD_ASSIGN 276
#define SUB_ASSIGN 277
#define LEFT_ASSIGN 278
#define RIGHT_ASSIGN 279
#define AND_ASSIGN 280
#define XOR_ASSIGN 281
#define OR_ASSIGN 282
#define TYPE_NAME 283
#define TYPEDEF 284
#define EXTERN 285
#define STATIC 286
#define AUTO 287
#define REGISTER 288
#define CHAR 289
#define SHORT 290
#define INT 291
#define LONG 292
#define SIGNED 293
#define UNSIGNED 294
#define FLOAT 295
#define DOUBLE 296
#define CONST 297
#define VOLATILE 298
#define VOID 299
#define STRUCT 300
#define UNION 301
#define ENUM 302
#define ELLIPSIS 303
#define CASE 304
#define DEFAULT 305
#define IF 306
#define SWITCH 307
#define WHILE 308
#define DO 309
#define FOR 310
#define GOTO 311
#define CONTINUE 312
#define BREAK 313
#define RETURN 314
#define ELSE 315
#define REMAIN 316


#define PRIMARY_EXPRESSION 400
#define POSTFIX_EXPRESSION 401
#define ARGUMENT_EXPRESSION_LIST 402
#define UNARY_EXPRESSION 403
#define UNARY_OPERATOR 404
#define CAST_EXPRESSION 405
#define MULTIPLICATIVE_EXPRESSION 406
#define ADDITIVE_EXPRESSION 407
#define SHIFT_EXPRESSION 408
#define RELATIONAL_EXPRESSION 409
#define EQUALITY_EXPRESSION 410
#define AND_EXPRESSION 411
#define EXCLUSIVE_OR_EXPRESSION 412
#define INCLUSIVE_OR_EXPRESSION 413
#define LOGICAL_AND_EXPRESSION 414
#define LOGICAL_OR_EXPRESSION 415
#define CONDITIONAL_EXPRESSION 416
#define ASSIGNMENT_EXPRESSION 417
#define ASSIGNMENT_OPERATOR 418
#define EXPRESSION 419
#define CONSTANT_EXPRESSION 420
#define DECLARATION 421
#define DECLARATION_SPECIFIERS 422
#define INIT_DECLARATOR_LIST 423
#define INIT_DECLARATOR 424
#define STORAGE_CLASS_SPECIFIER 425
#define TYPE_SPECIFIER 426
#define STRUCT_OR_UNION_SPECIFIER 427
#define STRUCT_OR_UNION 428
#define STRUCT_DECLARATION_LIST 429
#define STRUCT_DECLARATION 430
#define SPECIFIER_QUALIFIER_LIST 431
#define STRUCT_DECLARATOR_LIST 432
#define STRUCT_DECLARATOR 433
#define ENUM_SPECIFIER 434
#define ENUMERATOR_LIST 435
#define ENUMERATOR 436
#define TYPE_QUALIFIER 437
#define DECLARATOR 438
#define DIRECT_DECLARATOR 439
#define POINTER 440
#define TYPE_QUALIFIER_LIST 441
#define PARAMETER_TYPE_LIST 442
#define PARAMETER_LIST 443
#define PARAMETER_DECLARATION 444
#define IDENTIFIER_LIST 445
#define type_name 446
#define ABSTRACT_DECLARATOR 447
#define DIRECT_ABSTRACT_DECLARATOR 448
#define INITIALIZER 449
#define INITIALIZER_LIST 450
#define STATEMENT 451
#define LABELED_STATEMENT 452
#define COMPOUND_STATEMENT 453
#define DECLARATION_LIST 454
#define STATEMENT_LIST 455
#define EXPRESSION_STATEMENT 456
#define SELECTION_STATEMENT 457
#define ITERATION_STATEMENT 458
#define JUMP_STATEMENT 459
#define TRANSLATION_UNIT 460
#define EXTERNAL_DECLARATION 461
#define FUNCTION_DEFINITION 462

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
//#line 247 "y.tab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID(int yyi)
#else
static int
YYID(yyi)
int yyi;
#endif
{
	return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
/* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
	/* The OS might guarantee only one guard page at the bottom of the stack,
	   and a page size can be as small as 4096 bytes.  So we cannot safely
	   invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
	   to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc(YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free(void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
	yytype_int16 yyss_alloc;
	YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

   /* Copy COUNT objects from FROM to TO.  The source and destination do
	  not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

	  /* Relocate STACK from its old location to the new one.  The
		 local variables YYSIZE and YYSTACKSIZE give the old and new number of
		 elements in the stack, and YYPTR gives the new location of the
		 stack.  Advance YYPTR to a properly aligned location for the next
		 stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  61
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1299

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  85
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  64
/* YYNRULES -- Number of rules.  */
#define YYNRULES  212
/* YYNRULES -- Number of states.  */
#define YYNSTATES  350

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   315

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
        {
                0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,    72,     2,     2,     2,    74,    67,     2,
                62,    60,    68,    69,    66,    70,    65,    73,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,    80,    82,
                75,    81,    76,    79,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,    63,     2,    64,    77,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,    83,    78,    84,    71,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
                2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
                5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
                15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
                25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
                35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
                45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
                55,    56,    57,    58,    59,    61
        };

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
	   0,     0,     3,     5,     7,     9,    13,    15,    20,    24,
	  29,    33,    37,    40,    43,    45,    49,    51,    54,    57,
	  60,    63,    68,    70,    72,    74,    76,    78,    80,    82,
	  87,    89,    93,    97,   101,   103,   107,   111,   113,   117,
	 121,   123,   127,   131,   135,   139,   141,   145,   149,   151,
	 155,   157,   161,   163,   167,   169,   173,   175,   179,   181,
	 187,   189,   193,   195,   197,   199,   201,   203,   205,   207,
	 209,   211,   213,   215,   217,   221,   223,   226,   230,   232,
	 235,   237,   240,   242,   245,   247,   251,   253,   257,   259,
	 261,   263,   265,   267,   269,   271,   273,   275,   277,   279,
	 281,   283,   285,   287,   289,   291,   297,   302,   305,   307,
	 309,   311,   314,   318,   321,   323,   326,   328,   330,   334,
	 336,   339,   343,   348,   354,   357,   359,   363,   365,   369,
	 371,   373,   376,   378,   380,   384,   389,   393,   398,   403,
	 407,   409,   412,   415,   419,   421,   424,   426,   430,   432,
	 436,   439,   442,   444,   446,   450,   452,   455,   457,   459,
	 462,   466,   469,   473,   477,   482,   485,   489,   493,   498,
	 500,   504,   509,   511,   515,   517,   519,   521,   523,   525,
	 527,   531,   536,   540,   543,   547,   551,   556,   558,   561,
	 563,   566,   568,   571,   577,   585,   591,   597,   605,   612,
	 620,   624,   627,   630,   633,   637,   639,   642,   644,   646,
	 651,   655,   659
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
	 146,     0,    -1,     3,    -1,     4,    -1,     5,    -1,    62,
	 105,    60,    -1,    86,    -1,    87,    63,   105,    64,    -1,
	  87,    62,    60,    -1,    87,    62,    88,    60,    -1,    87,
	  65,     3,    -1,    87,     7,     3,    -1,    87,     8,    -1,
	  87,     9,    -1,   103,    -1,    88,    66,   103,    -1,    87,
	  -1,     8,    89,    -1,     9,    89,    -1,    90,    91,    -1,
	   6,    89,    -1,     6,    62,   132,    60,    -1,    67,    -1,
	  68,    -1,    69,    -1,    70,    -1,    71,    -1,    72,    -1,
	  89,    -1,    62,   132,    60,    91,    -1,    91,    -1,    92,
	  68,    91,    -1,    92,    73,    91,    -1,    92,    74,    91,
	  -1,    92,    -1,    93,    69,    92,    -1,    93,    70,    92,
	  -1,    93,    -1,    94,    10,    93,    -1,    94,    11,    93,
	  -1,    94,    -1,    95,    75,    94,    -1,    95,    76,    94,
	  -1,    95,    12,    94,    -1,    95,    13,    94,    -1,    95,
	  -1,    96,    14,    95,    -1,    96,    15,    95,    -1,    96,
	  -1,    97,    67,    96,    -1,    97,    -1,    98,    77,    97,
	  -1,    98,    -1,    99,    78,    98,    -1,    99,    -1,   100,
	  16,    99,    -1,   100,    -1,   101,    17,   100,    -1,   101,
	  -1,   101,    79,   105,    80,   102,    -1,   102,    -1,    89,
	 104,   103,    -1,    81,    -1,    18,    -1,    19,    -1,    20,
	  -1,    21,    -1,    22,    -1,    23,    -1,    24,    -1,    25,
	  -1,    26,    -1,    27,    -1,   103,    -1,   105,    66,   103,
	  -1,   102,    -1,   108,    82,    -1,   108,   109,    82,    -1,
	 111,    -1,   111,   108,    -1,   112,    -1,   112,   108,    -1,
	 123,    -1,   123,   108,    -1,   110,    -1,   109,    66,   110,
	  -1,   124,    -1,   124,    81,   135,    -1,    29,    -1,    30,
	  -1,    31,    -1,    32,    -1,    33,    -1,    44,    -1,    34,
	  -1,    35,    -1,    36,    -1,    37,    -1,    40,    -1,    41,
	  -1,    38,    -1,    39,    -1,   113,    -1,   120,    -1,    28,
	  -1,   114,     3,    83,   115,    84,    -1,   114,    83,   115,
	  84,    -1,   114,     3,    -1,    45,    -1,    46,    -1,   116,
	  -1,   115,   116,    -1,   117,   118,    82,    -1,   112,   117,
	  -1,   112,    -1,   123,   117,    -1,   123,    -1,   119,    -1,
	 118,    66,   119,    -1,   124,    -1,    80,   106,    -1,   124,
	  80,   106,    -1,    47,    83,   121,    84,    -1,    47,     3,
	  83,   121,    84,    -1,    47,     3,    -1,   122,    -1,   121,
	  66,   122,    -1,     3,    -1,     3,    81,   106,    -1,    42,
	  -1,    43,    -1,   126,   125,    -1,   125,    -1,     3,    -1,
	  62,   124,    60,    -1,   125,    63,   106,    64,    -1,   125,
	  63,    64,    -1,   125,    62,   128,    60,    -1,   125,    62,
	 131,    60,    -1,   125,    62,    60,    -1,    68,    -1,    68,
	 127,    -1,    68,   126,    -1,    68,   127,   126,    -1,   123,
	  -1,   127,   123,    -1,   129,    -1,   129,    66,    48,    -1,
	 130,    -1,   129,    66,   130,    -1,   108,   124,    -1,   108,
	 133,    -1,   108,    -1,     3,    -1,   131,    66,     3,    -1,
	 117,    -1,   117,   133,    -1,   126,    -1,   134,    -1,   126,
	 134,    -1,    62,   133,    60,    -1,    63,    64,    -1,    63,
	 106,    64,    -1,   134,    63,    64,    -1,   134,    63,   106,
	  64,    -1,    62,    60,    -1,    62,   128,    60,    -1,   134,
	  62,    60,    -1,   134,    62,   128,    60,    -1,   103,    -1,
	  83,   136,    84,    -1,    83,   136,    66,    84,    -1,   135,
	  -1,   136,    66,   135,    -1,   138,    -1,   139,    -1,   142,
	  -1,   143,    -1,   144,    -1,   145,    -1,     3,    80,   137,
	  -1,    49,   106,    80,   137,    -1,    50,    80,   137,    -1,
	  83,    84,    -1,    83,   141,    84,    -1,    83,   140,    84,
	  -1,    83,   140,   141,    84,    -1,   107,    -1,   140,   107,
	  -1,   137,    -1,   141,   137,    -1,    82,    -1,   105,    82,
	  -1,    51,    62,   105,    60,   137,    -1,    51,    62,   105,
	  60,   137,    61,   137,    -1,    52,    62,   105,    60,   137,
	  -1,    53,    62,   105,    60,   137,    -1,    54,   137,    53,
	  62,   105,    60,    82,    -1,    55,    62,   142,   142,    60,
	 137,    -1,    55,    62,   142,   142,   105,    60,   137,    -1,
	  56,     3,    82,    -1,    57,    82,    -1,    58,    82,    -1,
	  59,    82,    -1,    59,   105,    82,    -1,   147,    -1,   146,
	 147,    -1,   148,    -1,   107,    -1,   108,   124,   140,   139,
	  -1,   108,   124,   139,    -1,   124,   140,   139,    -1,   124,
	 139,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
	   0,    32,    32,    35,    38,    41,    49,    52,    58,    63,
	  69,    74,    79,    83,    90,    93,   101,   104,   108,   112,
	 116,   120,   129,   132,   135,   138,   141,   144,   150,   153,
	 162,   165,   170,   175,   183,   186,   191,   198,   201,   206,
	 214,   217,   222,   227,   232,   240,   243,   248,   256,   259,
	 267,   270,   278,   281,   289,   292,   300,   303,   311,   314,
	 324,   327,   335,   338,   341,   344,   347,   350,   353,   356,
	 359,   362,   365,   371,   374,   382,   388,   392,   400,   403,
	 407,   410,   414,   417,   424,   427,   435,   438,   446,   449,
	 452,   455,   458,   464,   467,   470,   473,   476,   479,   482,
	 485,   488,   491,   494,   497,   503,   510,   516,   523,   526,
	 532,   535,   542,   550,   554,   557,   561,   567,   570,   578,
	 581,   585,   593,   599,   606,   613,   616,   624,   627,   635,
	 638,   644,   648,   654,   657,   662,   668,   673,   679,   685,
	 693,   696,   700,   704,   712,   715,   723,   726,   734,   737,
	 745,   749,   753,   759,   762,   770,   773,   780,   783,   786,
	 793,   798,   802,   807,   812,   818,   822,   827,   832,   841,
	 844,   849,   858,   861,   869,   872,   875,   878,   881,   884,
	 890,   895,   901,   909,   913,   918,   923,   932,   935,   942,
	 945,   952,   955,   962,   969,   978,   988,   995,  1004,  1012,
	1024,  1029,  1033,  1037,  1041,  1049,  1053,  1061,  1064,  1070,
	1075,  1080,  1085
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "CONSTANT",
  "STRING_LITERAL", "SIZEOF", "PTR_OP", "INC_OP", "DEC_OP", "LEFT_OP",
  "RIGHT_OP", "LE_OP", "GE_OP", "EQ_OP", "NE_OP", "AND_OP", "OR_OP",
  "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN", "ADD_ASSIGN", "SUB_ASSIGN",
  "LEFT_ASSIGN", "RIGHT_ASSIGN", "AND_ASSIGN", "XOR_ASSIGN", "OR_ASSIGN",
  "TYPE_NAME", "TYPEDEF", "EXTERN", "STATIC", "AUTO", "REGISTER", "CHAR",
  "SHORT", "INT", "LONG", "SIGNED", "UNSIGNED", "FLOAT", "DOUBLE", "CONST",
  "VOLATILE", "VOID", "STRUCT", "UNION", "ENUM", "ELLIPSIS", "CASE",
  "DEFAULT", "IF", "SWITCH", "WHILE", "DO", "FOR", "GOTO", "CONTINUE",
  "BREAK", "RETURN", "')'", "ELSE", "'('", "'['", "']'", "'.'", "','",
  "'&'", "'*'", "'+'", "'-'", "'~'", "'!'", "'/'", "'%'", "'<'", "'>'",
  "'^'", "'|'", "'?'", "':'", "'='", "';'", "'{'", "'}'", "$accept",
  "primary_expression", "postfix_expression", "argument_expression_list",
  "unary_expression", "unary_operator", "cast_expression",
  "multiplicative_expression", "additive_expression", "shift_expression",
  "relational_expression", "equality_expression", "and_expression",
  "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration",
  "declaration_specifiers", "init_declarator_list", "init_declarator",
  "storage_class_specifier", "type_specifier", "struct_or_union_specifier",
  "struct_or_union", "struct_declaration_list", "struct_declaration",
  "specifier_qualifier_list", "struct_declarator_list",
  "struct_declarator", "enum_specifier", "enumerator_list", "enumerator",
  "type_qualifier", "declarator", "direct_declarator", "pointer",
  "type_qualifier_list", "parameter_type_list", "parameter_list",
  "parameter_declaration", "identifier_list", "type_name",
  "abstract_declarator", "direct_abstract_declarator", "initializer",
  "initializer_list", "statement", "labeled_statement",
  "compound_statement", "declaration_list", "statement_list",
  "expression_statement", "selection_statement", "iteration_statement",
  "jump_statement", "translation_unit", "external_declaration",
  "function_definition", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
	   0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
	 265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
	 275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
	 285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
	 295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
	 305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
	  41,   315,    40,    91,    93,    46,    44,    38,    42,    43,
	  45,   126,    33,    47,    37,    60,    62,    94,   124,    63,
	  58,    61,    59,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
        {
                0,    85,    86,    86,    86,    86,    87,    87,    87,    87,
                87,    87,    87,    87,    88,    88,    89,    89,    89,    89,
                89,    89,    90,    90,    90,    90,    90,    90,    91,    91,
                92,    92,    92,    92,    93,    93,    93,    94,    94,    94,
                95,    95,    95,    95,    95,    96,    96,    96,    97,    97,
                98,    98,    99,    99,   100,   100,   101,   101,   102,   102,
                103,   103,   104,   104,   104,   104,   104,   104,   104,   104,
                104,   104,   104,   105,   105,   106,   107,   107,   108,   108,
                108,   108,   108,   108,   109,   109,   110,   110,   111,   111,
                111,   111,   111,   112,   112,   112,   112,   112,   112,   112,
                112,   112,   112,   112,   112,   113,   113,   113,   114,   114,
                115,   115,   116,   117,   117,   117,   117,   118,   118,   119,
                119,   119,   120,   120,   120,   121,   121,   122,   122,   123,
                123,   124,   124,   125,   125,   125,   125,   125,   125,   125,
                126,   126,   126,   126,   127,   127,   128,   128,   129,   129,
                130,   130,   130,   131,   131,   132,   132,   133,   133,   133,
                134,   134,   134,   134,   134,   134,   134,   134,   134,   135,
                135,   135,   136,   136,   137,   137,   137,   137,   137,   137,
                138,   138,   138,   139,   139,   139,   139,   140,   140,   141,
                141,   142,   142,   143,   143,   143,   144,   144,   144,   144,
                145,   145,   145,   145,   145,   146,   146,   147,   147,   148,
                148,   148,   148
        };

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
        {
                0,     2,     1,     1,     1,     3,     1,     4,     3,     4,
                3,     3,     2,     2,     1,     3,     1,     2,     2,     2,
                2,     4,     1,     1,     1,     1,     1,     1,     1,     4,
                1,     3,     3,     3,     1,     3,     3,     1,     3,     3,
                1,     3,     3,     3,     3,     1,     3,     3,     1,     3,
                1,     3,     1,     3,     1,     3,     1,     3,     1,     5,
                1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
                1,     1,     1,     1,     3,     1,     2,     3,     1,     2,
                1,     2,     1,     2,     1,     3,     1,     3,     1,     1,
                1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
                1,     1,     1,     1,     1,     5,     4,     2,     1,     1,
                1,     2,     3,     2,     1,     2,     1,     1,     3,     1,
                2,     3,     4,     5,     2,     1,     3,     1,     3,     1,
                1,     2,     1,     1,     3,     4,     3,     4,     4,     3,
                1,     2,     2,     3,     1,     2,     1,     3,     1,     3,
                2,     2,     1,     1,     3,     1,     2,     1,     1,     2,
                3,     2,     3,     3,     4,     2,     3,     3,     4,     1,
                3,     4,     1,     3,     1,     1,     1,     1,     1,     1,
                3,     4,     3,     2,     3,     3,     4,     1,     2,     1,
                2,     1,     2,     5,     7,     5,     5,     7,     6,     7,
                3,     2,     2,     2,     3,     1,     2,     1,     1,     4,
                3,     3,     2
        };

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
        {
                0,   133,   104,    88,    89,    90,    91,    92,    94,    95,
                96,    97,   100,   101,    98,    99,   129,   130,    93,   108,
                109,     0,     0,   140,   208,     0,    78,    80,   102,     0,
                103,    82,     0,   132,     0,     0,   205,   207,   124,     0,
                0,   144,   142,   141,    76,     0,    84,    86,    79,    81,
                107,     0,    83,     0,   187,     0,   212,     0,     0,     0,
                131,     1,   206,     0,   127,     0,   125,   134,   145,   143,
                0,    77,     0,   210,     0,     0,   114,     0,   110,     0,
                116,     2,     3,     4,     0,     0,     0,     0,     0,     0,
                0,     0,     0,     0,     0,     0,     0,     0,     0,    22,
                23,    24,    25,    26,    27,   191,   183,     6,    16,    28,
                0,    30,    34,    37,    40,    45,    48,    50,    52,    54,
                56,    58,    60,    73,     0,   189,   174,   175,     0,     0,
                176,   177,   178,   179,    86,   188,   211,   153,   139,   152,
                0,   146,   148,     0,     2,   136,    28,    75,     0,     0,
                0,     0,   122,    85,     0,   169,    87,   209,     0,   113,
                106,   111,     0,     0,   117,   119,   115,     0,     0,    20,
                0,    17,    18,     0,     0,     0,     0,     0,     0,     0,
                0,   201,   202,   203,     0,     0,   155,     0,     0,    12,
                13,     0,     0,     0,    63,    64,    65,    66,    67,    68,
                69,    70,    71,    72,    62,     0,    19,     0,     0,     0,
                0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,     0,     0,     0,     0,     0,   192,   185,     0,
                184,   190,     0,     0,   150,   157,   151,   158,   137,     0,
                138,     0,   135,   123,   128,   126,   172,     0,   105,   120,
                0,   112,     0,   180,     0,     0,   182,     0,     0,     0,
                0,     0,   200,   204,     5,     0,   157,   156,     0,    11,
                8,     0,    14,     0,    10,    61,    31,    32,    33,    35,
                36,    38,    39,    43,    44,    41,    42,    46,    47,    49,
                51,    53,    55,    57,     0,    74,   186,   165,     0,     0,
                161,     0,   159,     0,     0,   147,   149,   154,     0,   170,
                118,   121,    21,   181,     0,     0,     0,     0,     0,    29,
                9,     0,     7,     0,   166,   160,   162,   167,     0,   163,
                0,   171,   173,   193,   195,   196,     0,     0,     0,    15,
                59,   168,   164,     0,     0,   198,     0,   194,   197,   199
        };

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
        {
                -1,   107,   108,   271,   109,   110,   111,   112,   113,   114,
                115,   116,   117,   118,   119,   120,   121,   122,   123,   205,
                124,   148,    54,    55,    45,    46,    26,    27,    28,    29,
                77,    78,    79,   163,   164,    30,    65,    66,    31,    32,
                33,    34,    43,   298,   141,   142,   143,   187,   299,   237,
                156,   247,   125,   126,   127,    57,   129,   130,   131,   132,
                133,    35,    36,    37
        };

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -221
static const yytype_int16 yypact[] =
        {
                932,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
                -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,
                -221,     5,    41,   150,  -221,    10,  1252,  1252,  -221,    11,
                -221,  1252,  1101,    48,    13,   842,  -221,  -221,   -60,    27,
                8,  -221,  -221,   150,  -221,    31,  -221,  1081,  -221,  -221,
                -19,  1055,  -221,   278,  -221,    10,  -221,  1101,   977,   646,
                48,  -221,  -221,    27,    -8,   -41,  -221,  -221,  -221,  -221,
                41,  -221,   524,  -221,  1101,  1055,  1055,  1004,  -221,    19,
                1055,    26,  -221,  -221,   296,   786,   786,   797,    35,    67,
                71,    81,   490,    88,   137,   104,   121,   560,   631,  -221,
                -221,  -221,  -221,  -221,  -221,  -221,  -221,  -221,   182,   579,
                797,  -221,    78,   158,   220,    73,   219,   144,   140,   165,
                209,     4,  -221,  -221,    46,  -221,  -221,  -221,   348,   418,
                -221,  -221,  -221,  -221,   167,  -221,  -221,  -221,  -221,    14,
                199,   194,  -221,   -32,  -221,  -221,  -221,  -221,   197,   -37,
                797,    27,  -221,  -221,   524,  -221,  -221,  -221,  1024,  -221,
                -221,  -221,   797,    76,  -221,   184,  -221,   490,   631,  -221,
                797,  -221,  -221,   186,   490,   797,   797,   797,   214,   576,
                191,  -221,  -221,  -221,   114,     1,    57,   216,   271,  -221,
                -221,   676,   797,   274,  -221,  -221,  -221,  -221,  -221,  -221,
                -221,  -221,  -221,  -221,  -221,   797,  -221,   797,   797,   797,
                797,   797,   797,   797,   797,   797,   797,   797,   797,   797,
                797,   797,   797,   797,   797,   797,   797,  -221,  -221,   454,
                -221,  -221,   887,   716,  -221,    42,  -221,   173,  -221,  1231,
                -221,   275,  -221,  -221,  -221,  -221,  -221,   -15,  -221,  -221,
                19,  -221,   797,  -221,   225,   490,  -221,    87,   122,   134,
                226,   576,  -221,  -221,  -221,  1157,   190,  -221,   797,  -221,
                -221,   135,  -221,   148,  -221,  -221,  -221,  -221,  -221,    78,
                78,   158,   158,   220,   220,   220,   220,    73,    73,   219,
                144,   140,   165,   209,    -1,  -221,  -221,  -221,   229,   230,
                -221,   227,   173,  1198,   746,  -221,  -221,  -221,   366,  -221,
                -221,  -221,  -221,  -221,   490,   490,   490,   797,   759,  -221,
                -221,   797,  -221,   797,  -221,  -221,  -221,  -221,   232,  -221,
                231,  -221,  -221,   233,  -221,  -221,   142,   490,   147,  -221,
                -221,  -221,  -221,   490,   211,  -221,   490,  -221,  -221,  -221
        };

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
        {
                -221,  -221,  -221,  -221,   -48,  -221,   -91,    44,    45,     7,
                52,    77,    75,   116,   103,    74,  -221,   -55,   -70,  -221,
                -38,   -54,     6,     0,  -221,   269,  -221,   -27,  -221,  -221,
                266,   -67,   -24,  -221,    92,  -221,   280,   193,    47,   -13,
                -29,    -3,  -221,   -57,  -221,   120,  -221,   187,   -85,  -220,
                -151,  -221,   -74,  -221,   152,   163,   245,  -172,  -221,  -221,
                -221,  -221,   361,  -221
        };

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint16 yytable[] =
        {
                25,   140,   155,   246,   147,    60,    24,   261,    38,    40,
                161,   146,    47,     1,    50,   302,     1,     1,   178,   206,
                42,   224,     1,    63,    76,   151,    48,    49,   240,   151,
                64,    52,   147,   173,   241,    25,   169,   171,   172,   146,
                69,    24,   134,   152,     1,     1,   302,   243,    76,    76,
                76,   308,   159,    76,   236,   231,   166,   134,   139,   184,
                185,   264,   146,   135,    75,   226,   165,   226,    67,   309,
                41,    76,    22,   150,   186,    22,   232,   233,    23,   323,
                135,    22,    23,   225,   155,   214,   215,    23,    39,   318,
                68,   161,    44,   253,    51,   147,   244,    70,    80,   162,
                256,   267,   146,    22,   232,   233,   167,   147,   249,    23,
                58,    59,   226,    71,   146,   174,   276,   277,   278,   265,
                233,   272,    80,    80,    80,    23,   234,    80,   227,   175,
                185,    76,   185,   176,   135,   275,   235,   257,   258,   259,
                180,    76,   250,   177,   186,    80,   207,   314,   216,   217,
                179,   208,   209,   226,   273,   231,   295,   332,   251,   146,
                146,   146,   146,   146,   146,   146,   146,   146,   146,   146,
                146,   146,   146,   146,   146,   146,   146,   319,   147,   301,
                226,   313,   315,   266,    56,   146,   181,   294,   226,   188,
                189,   190,    16,    17,   316,   320,   263,   147,   311,    73,
                226,   321,   344,   182,   146,    80,    60,   346,   226,   136,
                74,   220,   322,   226,   226,    80,   128,   221,    23,    40,
                146,   283,   284,   285,   286,   223,   157,   210,   211,   235,
                212,   213,   139,   218,   219,   303,   304,   165,   155,   139,
                333,   334,   335,   222,   191,   192,   328,   193,    72,   147,
                330,   339,   265,   233,   279,   280,   146,   281,   282,   238,
                239,   242,   266,   345,   252,   139,   255,   260,   340,   347,
                287,   288,   349,   262,   269,   146,   268,   274,   307,   336,
                338,    81,    82,    83,    84,   312,    85,    86,   317,   324,
                325,   326,   341,   348,   343,   342,   290,   289,   293,   144,
                82,    83,    84,   139,    85,    86,     2,     3,     4,     5,
                6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
                16,    17,    18,    19,    20,    21,   292,    87,    88,    89,
                90,    91,    92,    93,    94,    95,    96,    97,   291,   153,
                98,   158,   310,   149,   245,    99,   100,   101,   102,   103,
                104,    81,    82,    83,    84,   254,    85,    86,   168,   306,
                105,    53,   106,    99,   100,   101,   102,   103,   104,   144,
                82,    83,    84,   229,    85,    86,     2,     3,     4,     5,
                6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
                16,    17,    18,    19,    20,    21,    62,    87,    88,    89,
                90,    91,    92,    93,    94,    95,    96,    97,     0,     0,
                98,     0,     0,     0,     0,    99,   100,   101,   102,   103,
                104,    81,    82,    83,    84,     0,    85,    86,    98,     0,
                105,    53,   228,    99,   100,   101,   102,   103,   104,     0,
                0,     0,     0,     0,     0,     0,     0,     0,     0,   154,
                331,     0,     0,     0,     0,     0,     0,    81,    82,    83,
                84,     0,    85,    86,     0,     0,     0,    87,    88,    89,
                90,    91,    92,    93,    94,    95,    96,    97,     0,     0,
                98,     0,     0,     0,     0,    99,   100,   101,   102,   103,
                104,     0,     0,    81,    82,    83,    84,     0,    85,    86,
                105,    53,   230,    87,    88,    89,    90,    91,    92,    93,
                94,    95,    96,    97,     0,     0,    98,     0,     0,     0,
                0,    99,   100,   101,   102,   103,   104,   144,    82,    83,
                84,     0,    85,    86,     0,     0,   105,    53,   296,    87,
                88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
                0,     0,    98,     0,     0,     0,     0,    99,   100,   101,
                102,   103,   104,   144,    82,    83,    84,     0,    85,    86,
                0,     0,   105,    53,     0,     0,     0,     0,     0,   144,
                82,    83,    84,     0,    85,    86,    98,     0,     0,     0,
                0,    99,   100,   101,   102,   103,   104,   194,   195,   196,
                197,   198,   199,   200,   201,   202,   203,   154,     0,     0,
                0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,    98,     0,     0,     0,     0,    99,   100,   101,
                102,   103,   104,     0,   144,    82,    83,    84,    98,    85,
                86,     0,   183,    99,   100,   101,   102,   103,   104,   144,
                82,    83,    84,     0,    85,    86,     0,     0,   105,     2,
                204,     0,     0,     0,     0,     8,     9,    10,    11,    12,
                13,    14,    15,    16,    17,    18,    19,    20,    21,   144,
                82,    83,    84,     0,    85,    86,     0,     0,     0,     0,
                0,     0,     0,    98,     0,     0,     0,     0,    99,   100,
                101,   102,   103,   104,     0,     0,     0,     0,    98,     0,
                145,     0,     0,    99,   100,   101,   102,   103,   104,   144,
                82,    83,    84,     0,    85,    86,     0,     0,     0,     0,
                0,     0,     0,     0,     0,     0,   270,     0,    98,     0,
                0,     0,     0,    99,   100,   101,   102,   103,   104,   144,
                82,    83,    84,     0,    85,    86,     0,     0,     0,     0,
                0,     0,   144,    82,    83,    84,     0,    85,    86,     0,
                0,     0,     0,     0,     0,     0,     0,     0,    98,     0,
                300,     0,     0,    99,   100,   101,   102,   103,   104,   144,
                82,    83,    84,     0,    85,    86,     0,     0,     0,     0,
                144,    82,    83,    84,     0,    85,    86,     0,    98,     0,
                329,     0,     0,    99,   100,   101,   102,   103,   104,   337,
                0,    98,     0,     0,     0,     0,    99,   100,   101,   102,
                103,   104,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,    61,     0,     0,     1,     0,     0,   170,     0,
                0,     0,     0,    99,   100,   101,   102,   103,   104,    98,
                0,     0,     0,     0,    99,   100,   101,   102,   103,   104,
                2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
                12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
                1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,     0,     0,    22,     0,     0,     0,     0,     0,
                23,     0,     0,     0,     0,     2,     3,     4,     5,     6,
                7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
                17,    18,    19,    20,    21,     1,     0,     0,     0,     0,
                0,     0,     0,     0,     0,     0,     0,   297,     0,   232,
                233,     0,     0,     0,     0,    23,     0,     0,     0,     0,
                2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
                12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
                137,     0,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,     0,     0,    22,     0,     0,     0,     0,     0,
                23,     0,     0,     0,     0,     2,     3,     4,     5,     6,
                7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
                17,    18,    19,    20,    21,     0,     0,     0,     0,     0,
                0,     0,     2,     0,     0,     0,     0,   138,     8,     9,
                10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
                20,    21,     2,     0,     0,     0,     0,     0,     8,     9,
                10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
                20,    21,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,     0,     2,     0,     0,     0,     0,   160,     8,
                9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
                19,    20,    21,     0,     0,     0,     0,     0,   248,     2,
                3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
                13,    14,    15,    16,    17,    18,    19,    20,    21,     2,
                3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
                13,    14,    15,    16,    17,    18,    19,    20,    21,     0,
                0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,    72,     0,    53,     0,     0,     0,     0,     0,
                0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
                0,     0,     0,     0,    53,     2,     3,     4,     5,     6,
                7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
                17,    18,    19,    20,    21,     0,     0,     0,     0,     0,
                0,     0,     0,     0,     0,     0,     0,   297,     0,   265,
                233,     0,     0,     0,     0,    23,     2,     3,     4,     5,
                6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
                16,    17,    18,    19,    20,    21,     0,     0,     0,     0,
                0,     0,     0,     0,     0,     0,     0,     0,   327,     2,
                3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
                13,    14,    15,    16,    17,    18,    19,    20,    21,   305,
                2,     3,     4,     5,     6,     7,     8,     9,    10,    11,
                12,    13,    14,    15,    16,    17,    18,    19,    20,    21
        };

static const yytype_int16 yycheck[] =
        {
                0,    58,    72,   154,    59,    34,     0,   179,     3,    22,
                77,    59,    25,     3,     3,   235,     3,     3,    92,   110,
                23,    17,     3,    83,    51,    66,    26,    27,    60,    66,
                3,    31,    87,    87,    66,    35,    84,    85,    86,    87,
                43,    35,    55,    84,     3,     3,   266,    84,    75,    76,
                77,    66,    76,    80,   139,   129,    80,    70,    58,    97,
                98,    60,   110,    57,    83,    66,    79,    66,    60,    84,
                23,    98,    62,    81,    98,    62,    62,    63,    68,    80,
                74,    62,    68,    79,   154,    12,    13,    68,    83,   261,
                43,   158,    82,   167,    83,   150,   150,    66,    51,    80,
                174,   186,   150,    62,    62,    63,    80,   162,   162,    68,
                62,    63,    66,    82,   162,    80,   207,   208,   209,    62,
                63,   191,    75,    76,    77,    68,   139,    80,    82,    62,
                168,   158,   170,    62,   128,   205,   139,   175,   176,   177,
                3,   168,    66,    62,   168,    98,    68,    60,    75,    76,
                62,    73,    74,    66,   192,   229,   226,   308,    82,   207,
                208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
                218,   219,   220,   221,   222,   223,   224,   268,   233,   233,
                66,   255,    60,   186,    32,   233,    82,   225,    66,     7,
                8,     9,    42,    43,    60,    60,    82,   252,   252,    47,
                66,    66,    60,    82,   252,   158,   235,    60,    66,    57,
                47,    67,    64,    66,    66,   168,    53,    77,    68,   232,
                268,   214,   215,   216,   217,    16,    74,    69,    70,   232,
                10,    11,   232,    14,    15,    62,    63,   250,   308,   239,
                314,   315,   316,    78,    62,    63,   303,    65,    81,   304,
                304,   321,    62,    63,   210,   211,   304,   212,   213,    60,
                66,    64,   265,   337,    80,   265,    80,    53,   323,   343,
                218,   219,   346,    82,     3,   323,    60,     3,     3,   317,
                318,     3,     4,     5,     6,    60,     8,     9,    62,    60,
                60,    64,    60,    82,    61,    64,   221,   220,   224,     3,
                4,     5,     6,   303,     8,     9,    28,    29,    30,    31,
                32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
                42,    43,    44,    45,    46,    47,   223,    49,    50,    51,
                52,    53,    54,    55,    56,    57,    58,    59,   222,    70,
                62,    75,   250,    63,   151,    67,    68,    69,    70,    71,
                72,     3,     4,     5,     6,   168,     8,     9,    62,   239,
                82,    83,    84,    67,    68,    69,    70,    71,    72,     3,
                4,     5,     6,   128,     8,     9,    28,    29,    30,    31,
                32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
                42,    43,    44,    45,    46,    47,    35,    49,    50,    51,
                52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
                62,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
                72,     3,     4,     5,     6,    -1,     8,     9,    62,    -1,
                82,    83,    84,    67,    68,    69,    70,    71,    72,    -1,
                -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,
                84,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,
                6,    -1,     8,     9,    -1,    -1,    -1,    49,    50,    51,
                52,    53,    54,    55,    56,    57,    58,    59,    -1,    -1,
                62,    -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,
                72,    -1,    -1,     3,     4,     5,     6,    -1,     8,     9,
                82,    83,    84,    49,    50,    51,    52,    53,    54,    55,
                56,    57,    58,    59,    -1,    -1,    62,    -1,    -1,    -1,
                -1,    67,    68,    69,    70,    71,    72,     3,     4,     5,
                6,    -1,     8,     9,    -1,    -1,    82,    83,    84,    49,
                50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
                -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68,    69,
                70,    71,    72,     3,     4,     5,     6,    -1,     8,     9,
                -1,    -1,    82,    83,    -1,    -1,    -1,    -1,    -1,     3,
                4,     5,     6,    -1,     8,     9,    62,    -1,    -1,    -1,
                -1,    67,    68,    69,    70,    71,    72,    18,    19,    20,
                21,    22,    23,    24,    25,    26,    27,    83,    -1,    -1,
                -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68,    69,
                70,    71,    72,    -1,     3,     4,     5,     6,    62,     8,
                9,    -1,    82,    67,    68,    69,    70,    71,    72,     3,
                4,     5,     6,    -1,     8,     9,    -1,    -1,    82,    28,
                81,    -1,    -1,    -1,    -1,    34,    35,    36,    37,    38,
                39,    40,    41,    42,    43,    44,    45,    46,    47,     3,
                4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    62,    -1,    -1,    -1,    -1,    67,    68,
                69,    70,    71,    72,    -1,    -1,    -1,    -1,    62,    -1,
                64,    -1,    -1,    67,    68,    69,    70,    71,    72,     3,
                4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    62,    -1,
                -1,    -1,    -1,    67,    68,    69,    70,    71,    72,     3,
                4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
                -1,    -1,     3,     4,     5,     6,    -1,     8,     9,    -1,
                -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    62,    -1,
                64,    -1,    -1,    67,    68,    69,    70,    71,    72,     3,
                4,     5,     6,    -1,     8,     9,    -1,    -1,    -1,    -1,
                3,     4,     5,     6,    -1,     8,     9,    -1,    62,    -1,
                64,    -1,    -1,    67,    68,    69,    70,    71,    72,    60,
                -1,    62,    -1,    -1,    -1,    -1,    67,    68,    69,    70,
                71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,     0,    -1,    -1,     3,    -1,    -1,    62,    -1,
                -1,    -1,    -1,    67,    68,    69,    70,    71,    72,    62,
                -1,    -1,    -1,    -1,    67,    68,    69,    70,    71,    72,
                28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
                38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
                3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    -1,    62,    -1,    -1,    -1,    -1,    -1,
                68,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    32,
                33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
                43,    44,    45,    46,    47,     3,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    62,
                63,    -1,    -1,    -1,    -1,    68,    -1,    -1,    -1,    -1,
                28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
                38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
                3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    -1,    62,    -1,    -1,    -1,    -1,    -1,
                68,    -1,    -1,    -1,    -1,    28,    29,    30,    31,    32,
                33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
                43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,    28,    -1,    -1,    -1,    -1,    60,    34,    35,
                36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
                46,    47,    28,    -1,    -1,    -1,    -1,    -1,    34,    35,
                36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
                46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    28,    -1,    -1,    -1,    -1,    84,    34,
                35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
                45,    46,    47,    -1,    -1,    -1,    -1,    -1,    84,    28,
                29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
                39,    40,    41,    42,    43,    44,    45,    46,    47,    28,
                29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
                39,    40,    41,    42,    43,    44,    45,    46,    47,    -1,
                -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,    81,    -1,    83,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    -1,    83,    28,    29,    30,    31,    32,
                33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
                43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    -1,    62,
                63,    -1,    -1,    -1,    -1,    68,    28,    29,    30,    31,
                32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
                42,    43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,
                -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    60,    28,
                29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
                39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
                28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
                38,    39,    40,    41,    42,    43,    44,    45,    46,    47
        };

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
        {
                0,     3,    28,    29,    30,    31,    32,    33,    34,    35,
                36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
                46,    47,    62,    68,   107,   108,   111,   112,   113,   114,
                120,   123,   124,   125,   126,   146,   147,   148,     3,    83,
                124,   123,   126,   127,    82,   109,   110,   124,   108,   108,
                3,    83,   108,    83,   107,   108,   139,   140,    62,    63,
                125,     0,   147,    83,     3,   121,   122,    60,   123,   126,
                66,    82,    81,   139,   140,    83,   112,   115,   116,   117,
                123,     3,     4,     5,     6,     8,     9,    49,    50,    51,
                52,    53,    54,    55,    56,    57,    58,    59,    62,    67,
                68,    69,    70,    71,    72,    82,    84,    86,    87,    89,
                90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
                100,   101,   102,   103,   105,   137,   138,   139,   140,   141,
                142,   143,   144,   145,   124,   107,   139,     3,    60,   108,
                128,   129,   130,   131,     3,    64,    89,   102,   106,   121,
                81,    66,    84,   110,    83,   103,   135,   139,   115,   117,
                84,   116,    80,   118,   119,   124,   117,    80,    62,    89,
                62,    89,    89,   106,    80,    62,    62,    62,   137,    62,
                3,    82,    82,    82,   105,   105,   117,   132,     7,     8,
                9,    62,    63,    65,    18,    19,    20,    21,    22,    23,
                24,    25,    26,    27,    81,   104,    91,    68,    73,    74,
                69,    70,    10,    11,    12,    13,    75,    76,    14,    15,
                67,    77,    78,    16,    17,    79,    66,    82,    84,   141,
                84,   137,    62,    63,   124,   126,   133,   134,    60,    66,
                60,    66,    64,    84,   106,   122,   135,   136,    84,   106,
                66,    82,    80,   137,   132,    80,   137,   105,   105,   105,
                53,   142,    82,    82,    60,    62,   126,   133,    60,     3,
                60,    88,   103,   105,     3,   103,    91,    91,    91,    92,
                92,    93,    93,    94,    94,    94,    94,    95,    95,    96,
                97,    98,    99,   100,   105,   103,    84,    60,   128,   133,
                64,   106,   134,    62,    63,    48,   130,     3,    66,    84,
                119,   106,    60,   137,    60,    60,    60,    62,   142,    91,
                60,    66,    64,    80,    60,    60,    64,    60,   128,    64,
                106,    84,   135,   137,   137,   137,   105,    60,   105,   103,
                102,    60,    64,    61,    60,   137,    60,   137,    82,   137
        };

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print(FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print(yyoutput, yytype, yyvaluep)
FILE *yyoutput;
int yytype;
YYSTYPE const * const yyvaluep;
#endif
{
	if (!yyvaluep)
		return;
# ifdef YYPRINT
	if (yytype < YYNTOKENS)
		YYPRINT(yyoutput, yytoknum[yytype], *yyvaluep);
# else
	YYUSE(yyoutput);
# endif
	switch (yytype)
	{
	default:
		break;
	}
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print(FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print(yyoutput, yytype, yyvaluep)
FILE *yyoutput;
int yytype;
YYSTYPE const * const yyvaluep;
#endif
{
	if (yytype < YYNTOKENS)
		YYFPRINTF(yyoutput, "token %s (", yytname[yytype]);
	else
		YYFPRINTF(yyoutput, "nterm %s (", yytname[yytype]);

	yy_symbol_value_print(yyoutput, yytype, yyvaluep);
	YYFPRINTF(yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print(yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print(yybottom, yytop)
yytype_int16 *yybottom;
yytype_int16 *yytop;
#endif
{
	YYFPRINTF(stderr, "Stack now");
	for (; yybottom <= yytop; yybottom++)
	{
		int yybot = *yybottom;
		YYFPRINTF(stderr, " %d", yybot);
	}
	YYFPRINTF(stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print(YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print(yyvsp, yyrule)
YYSTYPE *yyvsp;
int yyrule;
#endif
{
	int yynrhs = yyr2[yyrule];
	int yyi;
	unsigned long int yylno = yyrline[yyrule];
	YYFPRINTF(stderr, "Reducing stack by rule %d (line %lu):\n",
		yyrule - 1, yylno);
	/* The symbols being reduced.  */
	for (yyi = 0; yyi < yynrhs; yyi++)
	{
		YYFPRINTF(stderr, "   $%d = ", yyi + 1);
		yy_symbol_print(stderr, yyrhs[yyprhs[yyrule] + yyi],
			&(yyvsp[(yyi + 1) - (yynrhs)])
		);
		YYFPRINTF(stderr, "\n");
	}
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
   /* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen(const char *yystr)
#else
static YYSIZE_T
yystrlen(yystr)
const char *yystr;
#endif
{
	YYSIZE_T yylen;
	for (yylen = 0; yystr[yylen]; yylen++)
		continue;
	return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy(char *yydest, const char *yysrc)
#else
static char *
yystpcpy(yydest, yysrc)
char *yydest;
const char *yysrc;
#endif
{
	char *yyd = yydest;
	const char *yys = yysrc;

	while ((*yyd++ = *yys++) != '\0')
		continue;

	return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr(char *yyres, const char *yystr)
{
	if (*yystr == '"')
	{
		YYSIZE_T yyn = 0;
		char const *yyp = yystr;

		for (;;)
			switch (*++yyp)
			{
			case '\'':
			case ',':
				goto do_not_strip_quotes;

			case '\\':
				if (*++yyp != '\\')
					goto do_not_strip_quotes;
				/* Fall through.  */
			default:
				if (yyres)
					yyres[yyn] = *yyp;
				yyn++;
				break;

			case '"':
				if (yyres)
					yyres[yyn] = '\0';
				return yyn;
			}
	do_not_strip_quotes:;
	}

	if (!yyres)
		return yystrlen(yystr);

	return yystpcpy(yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error(char *yyresult, int yystate, int yychar)
{
	int yyn = yypact[yystate];

	if (!(YYPACT_NINF < yyn && yyn <= YYLAST))
		return 0;
	else
	{
		int yytype = YYTRANSLATE(yychar);
		YYSIZE_T yysize0 = yytnamerr(0, yytname[yytype]);
		YYSIZE_T yysize = yysize0;
		YYSIZE_T yysize1;
		int yysize_overflow = 0;
		enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
		char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
		int yyx;

# if 0
		/* This is so xgettext sees the translatable formats that are
	   constructed on the fly.  */
		YY_("syntax error, unexpected %s");
		YY_("syntax error, unexpected %s, expecting %s");
		YY_("syntax error, unexpected %s, expecting %s or %s");
		YY_("syntax error, unexpected %s, expecting %s or %s or %s");
		YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
		char *yyfmt;
		char const *yyf;
		static char const yyunexpected[] = "syntax error, unexpected %s";
		static char const yyexpecting[] = ", expecting %s";
		static char const yyor[] = " or %s";
		char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
				* (sizeof yyor - 1))];
		char const *yyprefix = yyexpecting;

		/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
		int yyxbegin = yyn < 0 ? -yyn : 0;

		/* Stay within bounds of both yycheck and yytname.  */
		int yychecklim = YYLAST - yyn + 1;
		int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
		int yycount = 1;

		yyarg[0] = yytname[yytype];
		yyfmt = yystpcpy(yyformat, yyunexpected);

		for (yyx = yyxbegin; yyx < yyxend; ++yyx)
			if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
			{
				if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
				{
					yycount = 1;
					yysize = yysize0;
					yyformat[sizeof yyunexpected - 1] = '\0';
					break;
				}
				yyarg[yycount++] = yytname[yyx];
				yysize1 = yysize + yytnamerr(0, yytname[yyx]);
				yysize_overflow |= (yysize1 < yysize);
				yysize = yysize1;
				yyfmt = yystpcpy(yyfmt, yyprefix);
				yyprefix = yyor;
			}

		yyf = YY_(yyformat);
		yysize1 = yysize + yystrlen(yyf);
		yysize_overflow |= (yysize1 < yysize);
		yysize = yysize1;

		if (yysize_overflow)
			return YYSIZE_MAXIMUM;

		if (yyresult)
		{
			/* Avoid sprintf, as that infringes on the user's name space.
			   Don't have undefined behavior even if the translation
			   produced a string with the wrong number of "%s"s.  */
			char *yyp = yyresult;
			int yyi = 0;
			while ((*yyp = *yyf) != '\0')
			{
				if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
				{
					yyp += yytnamerr(yyp, yyarg[yyi++]);
					yyf += 2;
				}
				else
				{
					yyp++;
					yyf++;
				}
			}
		}
		return yysize;
	}
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct(const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct(yymsg, yytype, yyvaluep)
const char *yymsg;
int yytype;
YYSTYPE *yyvaluep;
#endif
{
    YYUSE(yyvaluep);

    if (!yymsg)
        yymsg = "Deleting";
    YY_SYMBOL_PRINT(yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
    {
        case 1:
        default:
            break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse(void *YYPARSE_PARAM);
#else
int yyparse();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse(void);
#else
int yyparse();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse(void *YYPARSE_PARAM)
#else
int
yyparse(YYPARSE_PARAM)
void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse(void)
#else
int
yyparse()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

    int yyn;
    int yyresult;
    /* Lookahead token as an internal (translated) token number.  */
    int yytoken;
    /* The variables used to return semantic value and location from the
       action routines.  */
    YYSTYPE yyval;

#if YYERROR_VERBOSE
    /* Buffer for error messages, and its allocated size.  */
	char yymsgbuf[128];
	char *yymsg = yymsgbuf;
	YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

    /* The number of symbols on the RHS of the reduced rule.
       Keep to zero when no symbol should be popped.  */
    int yylen = 0;

    yytoken = 0;
    yyss = yyssa;
    yyvs = yyvsa;
    yystacksize = YYINITDEPTH;

    YYDPRINTF((stderr, "Starting parse\n"));

    yystate = 0;
    yyerrstatus = 0;
    yynerrs = 0;
    yychar = YYEMPTY; /* Cause a token to be read.  */

    /* Initialize stack pointers.
       Waste one element of value and location stack
       so that they stay on the same level as the state stack.
       The wasted elements are never initialized.  */
    yyssp = yyss;
    yyvsp = yyvs;

    goto yysetstate;

    /*------------------------------------------------------------.
    | yynewstate -- Push a new state, which is found in yystate.  |
    `------------------------------------------------------------*/
    yynewstate:
    /* In all cases, when you get here, the value and location stacks
       have just been pushed.  So pushing a state here evens the stacks.  */
    yyssp++;

    yysetstate:
    *yyssp = yystate;

    if (yyss + yystacksize - 1 <= yyssp)
    {
        /* Get the current used size of the three stacks, in elements.  */
        YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
        {
			/* Give user a chance to reallocate the stack.  Use copies of
			   these so that the &'s don't force the real ones into
			   memory.  */
			YYSTYPE *yyvs1 = yyvs;
			yytype_int16 *yyss1 = yyss;

			/* Each stack pointer address is followed by the size of the
			   data in use in that stack, in bytes.  This used to be a
			   conditional around just the two extra args, but that might
			   be undefined if yyoverflow is a macro.  */
			yyoverflow(YY_("memory exhausted"),
				&yyss1, yysize * sizeof(*yyssp),
				&yyvs1, yysize * sizeof(*yyvsp),
				&yystacksize);

			yyss = yyss1;
			yyvs = yyvs1;
		}
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
        goto yyexhaustedlab;
# else
        /* Extend the stack our own way.  */
		if (YYMAXDEPTH <= yystacksize)
			goto yyexhaustedlab;
		yystacksize *= 2;
		if (YYMAXDEPTH < yystacksize)
			yystacksize = YYMAXDEPTH;

		{
			yytype_int16 *yyss1 = yyss;
			union yyalloc *yyptr =
				(union yyalloc *) YYSTACK_ALLOC(YYSTACK_BYTES(yystacksize));
			if (!yyptr)
				goto yyexhaustedlab;
			YYSTACK_RELOCATE(yyss_alloc, yyss);
			YYSTACK_RELOCATE(yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
			if (yyss1 != yyssa)
				YYSTACK_FREE(yyss1);
		}
# endif
#endif /* no yyoverflow */

        yyssp = yyss + yysize - 1;
        yyvsp = yyvs + yysize - 1;

        YYDPRINTF((stderr, "Stack size increased to %lu\n",
                (unsigned long int) yystacksize));

        if (yyss + yystacksize - 1 <= yyssp)
            YYABORT;
    }

    YYDPRINTF((stderr, "Entering state %d\n", yystate));

    if (yystate == YYFINAL)
        YYACCEPT;

    goto yybackup;

    /*-----------.
    | yybackup.  |
    `-----------*/
    yybackup:

    /* Do appropriate processing given the current state.  Read a
       lookahead token if we need one and don't already have one.  */

    /* First try to decide what to do without reference to lookahead token.  */
    yyn = yypact[yystate];
    if (yyn == YYPACT_NINF)
        goto yydefault;

    /* Not known => get a lookahead token if don't already have one.  */

    /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
    if (yychar == YYEMPTY)
    {
        YYDPRINTF((stderr, "Reading a token: "));
        yychar = YYLEX;
    }

    if (yychar <= YYEOF)
    {
        yychar = yytoken = YYEOF;
        YYDPRINTF((stderr, "Now at end of input.\n"));
    }
    else
    {
        yytoken = YYTRANSLATE(yychar);
        YY_SYMBOL_PRINT("Next token is", yytoken, &yylval, &yylloc);
    }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
        goto yydefault;
    yyn = yytable[yyn];
    if (yyn <= 0)
    {
        if (yyn == 0 || yyn == YYTABLE_NINF)
            goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
    }

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus)
        yyerrstatus--;

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the shifted token.  */
    yychar = YYEMPTY;

    yystate = yyn;
    *++yyvsp = yylval;

    goto yynewstate;


    /*-----------------------------------------------------------.
    | yydefault -- do the default action for the current state.  |
    `-----------------------------------------------------------*/
    yydefault:
    yyn = yydefact[yystate];
    if (yyn == 0)
        goto yyerrlab;
    goto yyreduce;


    /*-----------------------------.
    | yyreduce -- Do a reduction.  |
    `-----------------------------*/
    yyreduce:
    /* yyn is the number of a rule to reduce with.  */
    yylen = yyr2[yyn];

    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  Assigning to YYVAL
       unconditionally makes the parser a bit smaller, and it avoids a
       GCC warning that YYVAL may be used uninitialized.  */
    yyval = yyvsp[1 - yylen];


    YY_REDUCE_PRINT(yyn);
    switch (yyn)
    {
        case 2:

            /* Line 1455 of yacc.c  */
//#line 32 "hello.y"
        { p = newNode("primary_expression", PRIMARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 3:

            /* Line 1455 of yacc.c  */
//#line 35 "hello.y"
        {p = newNode("primary_expression", PRIMARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 4:

            /* Line 1455 of yacc.c  */
//#line 38 "hello.y"
        {p = newNode("primary_expression", PRIMARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 5:

            /* Line 1455 of yacc.c  */
//#line 41 "hello.y"
        {p = newNode("primary_expression", PRIMARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 6:

            /* Line 1455 of yacc.c  */
//#line 49 "hello.y"
        {p = newNode("postfix_expression", POSTFIX_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 7:

            /* Line 1455 of yacc.c  */
//#line 52 "hello.y"
        {p = newNode("postfix_expression", POSTFIX_EXPRESSION);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 8:

            /* Line 1455 of yacc.c  */
//#line 58 "hello.y"
        {p = newNode("postfix_expression", POSTFIX_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 9:

            /* Line 1455 of yacc.c  */
//#line 63 "hello.y"
        {p = newNode("postfix_expression", POSTFIX_EXPRESSION);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 10:

            /* Line 1455 of yacc.c  */
//#line 69 "hello.y"
        {p = newNode("postfix_expression", POSTFIX_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 11:

            /* Line 1455 of yacc.c  */
//#line 74 "hello.y"
        {p = newNode("postfix_expression", POSTFIX_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 12:

            /* Line 1455 of yacc.c  */
//#line 79 "hello.y"
        {p = newNode("postfix_expression", POSTFIX_EXPRESSION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 13:

            /* Line 1455 of yacc.c  */
//#line 83 "hello.y"
        {p = newNode("postfix_expression", POSTFIX_EXPRESSION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 14:

            /* Line 1455 of yacc.c  */
//#line 90 "hello.y"
        {p = newNode("argument_expression_list", ARGUMENT_EXPRESSION_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 15:

            /* Line 1455 of yacc.c  */
//#line 93 "hello.y"
        {p = newNode("argument_expression_list", ARGUMENT_EXPRESSION_LIST);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 16:

            /* Line 1455 of yacc.c  */
//#line 101 "hello.y"
        {p = newNode("unary_expression", UNARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 17:

            /* Line 1455 of yacc.c  */
//#line 104 "hello.y"
        {p = newNode("unary_expression", UNARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 18:

            /* Line 1455 of yacc.c  */
//#line 108 "hello.y"
        {p = newNode("unary_expression", UNARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 19:

            /* Line 1455 of yacc.c  */
//#line 112 "hello.y"
        {p = newNode("unary_expression", UNARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 20:

            /* Line 1455 of yacc.c  */
//#line 116 "hello.y"
        {p = newNode("unary_expression", UNARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 21:

            /* Line 1455 of yacc.c  */
//#line 120 "hello.y"
        {p = newNode("unary_expression", UNARY_EXPRESSION);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 22:

            /* Line 1455 of yacc.c  */
//#line 129 "hello.y"
        {p = newNode("unary_operator", UNARY_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 23:

            /* Line 1455 of yacc.c  */
//#line 132 "hello.y"
        {p = newNode("unary_operator", UNARY_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 24:

            /* Line 1455 of yacc.c  */
//#line 135 "hello.y"
        {p = newNode("unary_operator", UNARY_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 25:

            /* Line 1455 of yacc.c  */
//#line 138 "hello.y"
        {p = newNode("unary_operator", UNARY_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 26:

            /* Line 1455 of yacc.c  */
//#line 141 "hello.y"
        {p = newNode("unary_operator", UNARY_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 27:

            /* Line 1455 of yacc.c  */
//#line 144 "hello.y"
        {p = newNode("unary_operator", UNARY_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 28:

            /* Line 1455 of yacc.c  */
//#line 150 "hello.y"
        {p = newNode("cast_expression", CAST_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 29:

            /* Line 1455 of yacc.c  */
//#line 153 "hello.y"
        {p = newNode("cast_expression", CAST_EXPRESSION);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 30:

            /* Line 1455 of yacc.c  */
//#line 162 "hello.y"
        {p = newNode("multiplicative_expression", MULTIPLICATIVE_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 31:

            /* Line 1455 of yacc.c  */
//#line 165 "hello.y"
        {p = newNode("multiplicative_expression", MULTIPLICATIVE_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 32:

            /* Line 1455 of yacc.c  */
//#line 170 "hello.y"
        {p = newNode("multiplicative_expression", MULTIPLICATIVE_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 33:

            /* Line 1455 of yacc.c  */
//#line 175 "hello.y"
        {p = newNode("multiplicative_expression", MULTIPLICATIVE_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 34:

            /* Line 1455 of yacc.c  */
//#line 183 "hello.y"
        {p = newNode("additive_expression", ADDITIVE_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 35:

            /* Line 1455 of yacc.c  */
//#line 186 "hello.y"
        {p = newNode("additive_expression", ADDITIVE_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 36:

            /* Line 1455 of yacc.c  */
//#line 191 "hello.y"
        {p = newNode("additive_expression", ADDITIVE_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 37:

            /* Line 1455 of yacc.c  */
//#line 198 "hello.y"
        {p = newNode("shift_expression", SHIFT_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 38:

            /* Line 1455 of yacc.c  */
//#line 201 "hello.y"
        {p = newNode("shift_expression", SHIFT_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 39:

            /* Line 1455 of yacc.c  */
//#line 206 "hello.y"
        {p = newNode("shift_expression", SHIFT_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 40:

            /* Line 1455 of yacc.c  */
//#line 214 "hello.y"
        {p = newNode("relational_expression", RELATIONAL_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 41:

            /* Line 1455 of yacc.c  */
//#line 217 "hello.y"
        {p = newNode("relational_expression", RELATIONAL_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 42:

            /* Line 1455 of yacc.c  */
//#line 222 "hello.y"
        {p = newNode("relational_expression", RELATIONAL_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 43:

            /* Line 1455 of yacc.c  */
//#line 227 "hello.y"
        {p = newNode("relational_expression", RELATIONAL_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 44:

            /* Line 1455 of yacc.c  */
//#line 232 "hello.y"
        {p = newNode("relational_expression", RELATIONAL_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 45:

            /* Line 1455 of yacc.c  */
//#line 240 "hello.y"
        {p = newNode("equality_expression", EQUALITY_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 46:

            /* Line 1455 of yacc.c  */
//#line 243 "hello.y"
        {p = newNode("equality_expression", EQUALITY_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 47:

            /* Line 1455 of yacc.c  */
//#line 248 "hello.y"
        {p = newNode("equality_expression", EQUALITY_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 48:

            /* Line 1455 of yacc.c  */
//#line 256 "hello.y"
        {p = newNode("and_expression", AND_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 49:

            /* Line 1455 of yacc.c  */
//#line 259 "hello.y"
        {p = newNode("and_expression", AND_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 50:

            /* Line 1455 of yacc.c  */
//#line 267 "hello.y"
        {p = newNode("exclusive_or_expression", EXCLUSIVE_OR_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 51:

            /* Line 1455 of yacc.c  */
//#line 270 "hello.y"
        {p = newNode("exclusive_or_expression", EXCLUSIVE_OR_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 52:

            /* Line 1455 of yacc.c  */
//#line 278 "hello.y"
        {p = newNode("inclusive_or_expression", INCLUSIVE_OR_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 53:

            /* Line 1455 of yacc.c  */
//#line 281 "hello.y"
        {p = newNode("inclusive_or_expression", INCLUSIVE_OR_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 54:

            /* Line 1455 of yacc.c  */
//#line 289 "hello.y"
        {p = newNode("logical_and_expression", LOGICAL_AND_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 55:

            /* Line 1455 of yacc.c  */
//#line 292 "hello.y"
        {p = newNode("logical_and_expression", LOGICAL_AND_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 56:

            /* Line 1455 of yacc.c  */
//#line 300 "hello.y"
        {p = newNode("logical_or_expression", LOGICAL_OR_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 57:

            /* Line 1455 of yacc.c  */
//#line 303 "hello.y"
        {p = newNode("logical_or_expression", LOGICAL_OR_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 58:

            /* Line 1455 of yacc.c  */
//#line 311 "hello.y"
        {p = newNode("conditional_expression", CONDITIONAL_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 59:

            /* Line 1455 of yacc.c  */
//#line 314 "hello.y"
        {p = newNode("conditional_expression", CONDITIONAL_EXPRESSION);
            insert(p, (yyvsp[(1) - (5)]));
            insert(p, (yyvsp[(2) - (5)]));
            insert(p, (yyvsp[(3) - (5)]));
            insert(p, (yyvsp[(4) - (5)]));
            insert(p, (yyvsp[(5) - (5)]));
            (yyval) = p; }
            break;

        case 60:

            /* Line 1455 of yacc.c  */
//#line 324 "hello.y"
        {p = newNode("assignment_expression", ASSIGNMENT_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 61:

            /* Line 1455 of yacc.c  */
//#line 327 "hello.y"
        {p = newNode("assignment_expression", ASSIGNMENT_EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 62:

            /* Line 1455 of yacc.c  */
//#line 335 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 63:

            /* Line 1455 of yacc.c  */
//#line 338 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 64:

            /* Line 1455 of yacc.c  */
//#line 341 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 65:

            /* Line 1455 of yacc.c  */
//#line 344 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 66:

            /* Line 1455 of yacc.c  */
//#line 347 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 67:

            /* Line 1455 of yacc.c  */
//#line 350 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 68:

            /* Line 1455 of yacc.c  */
//#line 353 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 69:

            /* Line 1455 of yacc.c  */
//#line 356 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 70:

            /* Line 1455 of yacc.c  */
//#line 359 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 71:

            /* Line 1455 of yacc.c  */
//#line 362 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 72:

            /* Line 1455 of yacc.c  */
//#line 365 "hello.y"
        {p = newNode("assignment_operator", ASSIGNMENT_OPERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 73:

            /* Line 1455 of yacc.c  */
//#line 371 "hello.y"
        {p = newNode("expression", EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 74:

            /* Line 1455 of yacc.c  */
//#line 374 "hello.y"
        {p = newNode("expression", EXPRESSION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 75:

            /* Line 1455 of yacc.c  */
//#line 382 "hello.y"
        {p = newNode("constant_expression", CONSTANT_EXPRESSION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 76:

            /* Line 1455 of yacc.c  */
//#line 388 "hello.y"
        {p = newNode("declaration", DECLARATION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 77:

            /* Line 1455 of yacc.c  */
//#line 392 "hello.y"
        {p = newNode("declaration", DECLARATION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p;
            type_add(p); }
            break;

        case 78:

            /* Line 1455 of yacc.c  */
//#line 400 "hello.y"
        {p = newNode("declaration_specifiers", DECLARATION_SPECIFIERS);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 79:

            /* Line 1455 of yacc.c  */
//#line 403 "hello.y"
        {p = newNode("declaration_specifiers", DECLARATION_SPECIFIERS);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 80:

            /* Line 1455 of yacc.c  */
//#line 407 "hello.y"
        {p = newNode("declaration_specifiers", DECLARATION_SPECIFIERS);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 81:

            /* Line 1455 of yacc.c  */
//#line 410 "hello.y"
        {p = newNode("declaration_specifiers", DECLARATION_SPECIFIERS);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 82:

            /* Line 1455 of yacc.c  */
//#line 414 "hello.y"
        {p = newNode("declaration_specifiers", DECLARATION_SPECIFIERS);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 83:

            /* Line 1455 of yacc.c  */
//#line 417 "hello.y"
        {p = newNode("declaration_specifiers", DECLARATION_SPECIFIERS);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 84:

            /* Line 1455 of yacc.c  */
//#line 424 "hello.y"
        {p = newNode("init_declarator_list", INIT_DECLARATOR_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 85:

            /* Line 1455 of yacc.c  */
//#line 427 "hello.y"
        {p = newNode("init_declarator_list", INIT_DECLARATOR_LIST);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 86:

            /* Line 1455 of yacc.c  */
//#line 435 "hello.y"
        {p = newNode("init_declarator", INIT_DECLARATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 87:

            /* Line 1455 of yacc.c  */
//#line 438 "hello.y"
        {p = newNode("init_declarator", INIT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 88:

            /* Line 1455 of yacc.c  */
//#line 446 "hello.y"
        {p = newNode("storage_class_specifier", STORAGE_CLASS_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 89:

            /* Line 1455 of yacc.c  */
//#line 449 "hello.y"
        {p = newNode("storage_class_specifier", STORAGE_CLASS_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 90:

            /* Line 1455 of yacc.c  */
//#line 452 "hello.y"
        {p = newNode("storage_class_specifier", STORAGE_CLASS_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 91:

            /* Line 1455 of yacc.c  */
//#line 455 "hello.y"
        {p = newNode("storage_class_specifier", STORAGE_CLASS_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 92:

            /* Line 1455 of yacc.c  */
//#line 458 "hello.y"
        {p = newNode("storage_class_specifier", STORAGE_CLASS_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 93:

            /* Line 1455 of yacc.c  */
//#line 464 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 94:

            /* Line 1455 of yacc.c  */
//#line 467 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 95:

            /* Line 1455 of yacc.c  */
//#line 470 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 96:

            /* Line 1455 of yacc.c  */
//#line 473 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 97:

            /* Line 1455 of yacc.c  */
//#line 476 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 98:

            /* Line 1455 of yacc.c  */
//#line 479 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 99:

            /* Line 1455 of yacc.c  */
//#line 482 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 100:

            /* Line 1455 of yacc.c  */
//#line 485 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 101:

            /* Line 1455 of yacc.c  */
//#line 488 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 102:

            /* Line 1455 of yacc.c  */
//#line 491 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 103:

            /* Line 1455 of yacc.c  */
//#line 494 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 104:

            /* Line 1455 of yacc.c  */
//#line 497 "hello.y"
        {p = newNode("type_specifier", TYPE_SPECIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 105:

            /* Line 1455 of yacc.c  */
//#line 503 "hello.y"
        {p = newNode("struct_or_union_specifier", STRUCT_OR_UNION_SPECIFIER);
            insert(p, (yyvsp[(1) - (5)]));
            insert(p, (yyvsp[(2) - (5)]));
            insert(p, (yyvsp[(3) - (5)]));
            insert(p, (yyvsp[(4) - (5)]));
            insert(p, (yyvsp[(5) - (5)]));
            (yyval) = p; }
            break;

        case 106:

            /* Line 1455 of yacc.c  */
//#line 510 "hello.y"
        {p = newNode("struct_or_union_specifier", STRUCT_OR_UNION_SPECIFIER);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 107:

            /* Line 1455 of yacc.c  */
//#line 516 "hello.y"
        {p = newNode("struct_or_union_specifier", STRUCT_OR_UNION_SPECIFIER);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 108:

            /* Line 1455 of yacc.c  */
//#line 523 "hello.y"
        {p = newNode("struct_or_union", STRUCT_OR_UNION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 109:

            /* Line 1455 of yacc.c  */
//#line 526 "hello.y"
        {p = newNode("struct_or_union", STRUCT_OR_UNION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 110:

            /* Line 1455 of yacc.c  */
//#line 532 "hello.y"
        {p = newNode("struct_declaration_list", STRUCT_DECLARATION_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 111:

            /* Line 1455 of yacc.c  */
//#line 535 "hello.y"
        {p = newNode("struct_declaration_list", STRUCT_DECLARATION_LIST);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 112:

            /* Line 1455 of yacc.c  */
//#line 542 "hello.y"
        {p = newNode("struct_declaration", STRUCT_DECLARATION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 113:

            /* Line 1455 of yacc.c  */
//#line 550 "hello.y"
        {p = newNode("specifier_qualifier_list", SPECIFIER_QUALIFIER_LIST);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 114:

            /* Line 1455 of yacc.c  */
//#line 554 "hello.y"
        {p = newNode("specifier_qualifier_list", SPECIFIER_QUALIFIER_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 115:

            /* Line 1455 of yacc.c  */
//#line 557 "hello.y"
        {p = newNode("specifier_qualifier_list", SPECIFIER_QUALIFIER_LIST);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 116:

            /* Line 1455 of yacc.c  */
//#line 561 "hello.y"
        {p = newNode("specifier_qualifier_list", SPECIFIER_QUALIFIER_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 117:

            /* Line 1455 of yacc.c  */
//#line 567 "hello.y"
        {p = newNode("struct_declarator_list", STRUCT_DECLARATOR_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 118:

            /* Line 1455 of yacc.c  */
//#line 570 "hello.y"
        {p = newNode("struct_declarator_list", STRUCT_DECLARATOR_LIST);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 119:

            /* Line 1455 of yacc.c  */
//#line 578 "hello.y"
        {p = newNode("struct_declarator", STRUCT_DECLARATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 120:

            /* Line 1455 of yacc.c  */
//#line 581 "hello.y"
        {p = newNode("struct_declarator", STRUCT_DECLARATOR);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 121:

            /* Line 1455 of yacc.c  */
//#line 585 "hello.y"
        {p = newNode("struct_declarator", STRUCT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 122:

            /* Line 1455 of yacc.c  */
//#line 593 "hello.y"
        {p = newNode("enum_specifier", ENUM_SPECIFIER);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 123:

            /* Line 1455 of yacc.c  */
//#line 599 "hello.y"
        {p = newNode("enum_specifier", ENUM_SPECIFIER);
            insert(p, (yyvsp[(1) - (5)]));
            insert(p, (yyvsp[(2) - (5)]));
            insert(p, (yyvsp[(3) - (5)]));
            insert(p, (yyvsp[(4) - (5)]));
            insert(p, (yyvsp[(5) - (5)]));
            (yyval) = p; }
            break;

        case 124:

            /* Line 1455 of yacc.c  */
//#line 606 "hello.y"
        {p = newNode("enum_specifier", ENUM_SPECIFIER);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 125:

            /* Line 1455 of yacc.c  */
//#line 613 "hello.y"
        {p = newNode("enumerator_list", ENUMERATOR_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 126:

            /* Line 1455 of yacc.c  */
//#line 616 "hello.y"
        {p = newNode("enumerator_list", ENUMERATOR_LIST);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 127:

            /* Line 1455 of yacc.c  */
//#line 624 "hello.y"
        {p = newNode("enumerator", ENUMERATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 128:

            /* Line 1455 of yacc.c  */
//#line 627 "hello.y"
        {p = newNode("enumerator", ENUMERATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 129:

            /* Line 1455 of yacc.c  */
//#line 635 "hello.y"
        {p = newNode("type_qualifier", TYPE_QUALIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 130:

            /* Line 1455 of yacc.c  */
//#line 638 "hello.y"
        {p = newNode("type_qualifier", TYPE_QUALIFIER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 131:

            /* Line 1455 of yacc.c  */
//#line 644 "hello.y"
        {p = newNode("declarator", DECLARATOR);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 132:

            /* Line 1455 of yacc.c  */
//#line 648 "hello.y"
        {p = newNode("declarator", DECLARATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 133:

            /* Line 1455 of yacc.c  */
//#line 654 "hello.y"
        {p = newNode("direct_declarator", DIRECT_DECLARATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 134:

            /* Line 1455 of yacc.c  */
//#line 657 "hello.y"
        {p = newNode("direct_declarator", DIRECT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 135:

            /* Line 1455 of yacc.c  */
//#line 662 "hello.y"
        {p = newNode("direct_declarator", DIRECT_DECLARATOR);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 136:

            /* Line 1455 of yacc.c  */
//#line 668 "hello.y"
        {p = newNode("direct_declarator", DIRECT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 137:

            /* Line 1455 of yacc.c  */
//#line 673 "hello.y"
        {p = newNode("direct_declarator", DIRECT_DECLARATOR);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 138:

            /* Line 1455 of yacc.c  */
//#line 679 "hello.y"
        {p = newNode("direct_declarator", DIRECT_DECLARATOR);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 139:

            /* Line 1455 of yacc.c  */
//#line 685 "hello.y"
        {p = newNode("direct_declarator", DIRECT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 140:

            /* Line 1455 of yacc.c  */
//#line 693 "hello.y"
        {p = newNode("pointer", POINTER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 141:

            /* Line 1455 of yacc.c  */
//#line 696 "hello.y"
        {p = newNode("pointer", POINTER);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 142:

            /* Line 1455 of yacc.c  */
//#line 700 "hello.y"
        {p = newNode("pointer", POINTER);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 143:

            /* Line 1455 of yacc.c  */
//#line 704 "hello.y"
        {p = newNode("pointer", POINTER);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 144:

            /* Line 1455 of yacc.c  */
//#line 712 "hello.y"
        {p = newNode("type_qualifier_list", TYPE_QUALIFIER_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 145:

            /* Line 1455 of yacc.c  */
//#line 715 "hello.y"
        {p = newNode("type_qualifier_list", TYPE_QUALIFIER_LIST);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 146:

            /* Line 1455 of yacc.c  */
//#line 723 "hello.y"
        {p = newNode("parameter_type_list", PARAMETER_TYPE_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 147:

            /* Line 1455 of yacc.c  */
//#line 726 "hello.y"
        {p = newNode("parameter_type_list", PARAMETER_TYPE_LIST);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 148:

            /* Line 1455 of yacc.c  */
//#line 734 "hello.y"
        {p = newNode("parameter_list", PARAMETER_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 149:

            /* Line 1455 of yacc.c  */
//#line 737 "hello.y"
        {p = newNode("parameter_list", PARAMETER_LIST);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 150:

            /* Line 1455 of yacc.c  */
//#line 745 "hello.y"
        {p = newNode("parameter_declaration", PARAMETER_DECLARATION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 151:

            /* Line 1455 of yacc.c  */
//#line 749 "hello.y"
        {p = newNode("parameter_declaration", PARAMETER_DECLARATION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 152:

            /* Line 1455 of yacc.c  */
//#line 753 "hello.y"
        {p = newNode("parameter_declaration", PARAMETER_DECLARATION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 153:

            /* Line 1455 of yacc.c  */
//#line 759 "hello.y"
        {p = newNode("identifier_list", IDENTIFIER_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 154:

            /* Line 1455 of yacc.c  */
//#line 762 "hello.y"
        {p = newNode("identifier_list", IDENTIFIER_LIST);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 155:

            /* Line 1455 of yacc.c  */
//#line 770 "hello.y"
        {p = newNode("type_name", type_name);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 156:

            /* Line 1455 of yacc.c  */
//#line 773 "hello.y"
        {p = newNode("type_name", type_name);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p;}
            break;

        case 157:

            /* Line 1455 of yacc.c  */
//#line 780 "hello.y"
        {p = newNode("abstract_declarator", ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 158:

            /* Line 1455 of yacc.c  */
//#line 783 "hello.y"
        {p = newNode("abstract_declarator", ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 159:

            /* Line 1455 of yacc.c  */
//#line 786 "hello.y"
        {p = newNode("abstract_declarator", ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 160:

            /* Line 1455 of yacc.c  */
//#line 793 "hello.y"
        {p = newNode("direct_abstract_declarator", DIRECT_ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 161:

            /* Line 1455 of yacc.c  */
//#line 798 "hello.y"
        {p = newNode("direct_abstract_declarator", DIRECT_ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 162:

            /* Line 1455 of yacc.c  */
//#line 802 "hello.y"
        {p = newNode("direct_abstract_declarator", DIRECT_ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 163:

            /* Line 1455 of yacc.c  */
//#line 807 "hello.y"
        {p = newNode("direct_abstract_declarator", DIRECT_ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 164:

            /* Line 1455 of yacc.c  */
//#line 812 "hello.y"
        {p = newNode("direct_abstract_declarator", DIRECT_ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 165:

            /* Line 1455 of yacc.c  */
//#line 818 "hello.y"
        {p = newNode("direct_abstract_declarator", DIRECT_ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 166:

            /* Line 1455 of yacc.c  */
//#line 822 "hello.y"
        {p = newNode("direct_abstract_declarator", DIRECT_ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 167:

            /* Line 1455 of yacc.c  */
//#line 827 "hello.y"
        {p = newNode("direct_abstract_declarator", DIRECT_ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 168:

            /* Line 1455 of yacc.c  */
//#line 832 "hello.y"
        {p = newNode("direct_abstract_declarator", DIRECT_ABSTRACT_DECLARATOR);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 169:

            /* Line 1455 of yacc.c  */
//#line 841 "hello.y"
        {p = newNode("initializer", INITIALIZER);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 170:

            /* Line 1455 of yacc.c  */
//#line 844 "hello.y"
        {p = newNode("initializer", INITIALIZER);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 171:

            /* Line 1455 of yacc.c  */
//#line 849 "hello.y"
        {p = newNode("initializer", INITIALIZER);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 172:

            /* Line 1455 of yacc.c  */
//#line 858 "hello.y"
        {p = newNode("initializer_list", INITIALIZER_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 173:

            /* Line 1455 of yacc.c  */
//#line 861 "hello.y"
        {p = newNode("initializer_list", INITIALIZER_LIST);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 174:

            /* Line 1455 of yacc.c  */
//#line 869 "hello.y"
        {p = newNode("statement", STATEMENT);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 175:

            /* Line 1455 of yacc.c  */
//#line 872 "hello.y"
        {p = newNode("statement", STATEMENT);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 176:

            /* Line 1455 of yacc.c  */
//#line 875 "hello.y"
        {p = newNode("statement", STATEMENT);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 177:

            /* Line 1455 of yacc.c  */
//#line 878 "hello.y"
        {p = newNode("statement", STATEMENT);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 178:

            /* Line 1455 of yacc.c  */
//#line 881 "hello.y"
        {p = newNode("statement", STATEMENT);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 179:

            /* Line 1455 of yacc.c  */
//#line 884 "hello.y"
        {p = newNode("statement", STATEMENT);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 180:

            /* Line 1455 of yacc.c  */
//#line 890 "hello.y"
        {p = newNode("labeled_statement", LABELED_STATEMENT);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 181:

            /* Line 1455 of yacc.c  */
//#line 895 "hello.y"
        {p = newNode("labeled_statement", LABELED_STATEMENT);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 182:

            /* Line 1455 of yacc.c  */
//#line 901 "hello.y"
        {p = newNode("labeled_statement", LABELED_STATEMENT);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 183:

            /* Line 1455 of yacc.c  */
//#line 909 "hello.y"
        {p = newNode("compound_statement", COMPOUND_STATEMENT);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 184:

            /* Line 1455 of yacc.c  */
//#line 913 "hello.y"
        {p = newNode("compound_statement", COMPOUND_STATEMENT);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 185:

            /* Line 1455 of yacc.c  */
//#line 918 "hello.y"
        {p = newNode("compound_statement", COMPOUND_STATEMENT);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 186:

            /* Line 1455 of yacc.c  */
//#line 923 "hello.y"
        {p = newNode("compound_statement", COMPOUND_STATEMENT);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            insert(p, (yyvsp[(4) - (4)]));
            (yyval) = p; }
            break;

        case 187:

            /* Line 1455 of yacc.c  */
//#line 932 "hello.y"
        {p = newNode("declaration_list", DECLARATION_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 188:

            /* Line 1455 of yacc.c  */
//#line 935 "hello.y"
        {p = newNode("declaration_list", DECLARATION_LIST);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 189:

            /* Line 1455 of yacc.c  */
//#line 942 "hello.y"
        {p = newNode("statement_list", STATEMENT_LIST);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 190:

            /* Line 1455 of yacc.c  */
//#line 945 "hello.y"
        {p = newNode("statement_list", STATEMENT_LIST);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 191:

            /* Line 1455 of yacc.c  */
//#line 952 "hello.y"
        {p = newNode("expression_statement", EXPRESSION_STATEMENT);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 192:

            /* Line 1455 of yacc.c  */
//#line 955 "hello.y"
        {p = newNode("expression_statement", EXPRESSION_STATEMENT);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 193:

            /* Line 1455 of yacc.c  */
//#line 962 "hello.y"
        {p = newNode("selection_statement", SELECTION_STATEMENT);
            insert(p, (yyvsp[(1) - (5)]));
            insert(p, (yyvsp[(2) - (5)]));
            insert(p, (yyvsp[(3) - (5)]));
            insert(p, (yyvsp[(4) - (5)]));
            insert(p, (yyvsp[(5) - (5)]));
            (yyval) = p; }
            break;

        case 194:

            /* Line 1455 of yacc.c  */
//#line 969 "hello.y"
        {p = newNode("selection_statement", SELECTION_STATEMENT);
            insert(p, (yyvsp[(1) - (7)]));
            insert(p, (yyvsp[(2) - (7)]));
            insert(p, (yyvsp[(3) - (7)]));
            insert(p, (yyvsp[(4) - (7)]));
            insert(p, (yyvsp[(5) - (7)]));
            insert(p, (yyvsp[(6) - (7)]));
            insert(p, (yyvsp[(7) - (7)]));
            (yyval) = p; }
            break;

        case 195:

            /* Line 1455 of yacc.c  */
//#line 978 "hello.y"
        {p = newNode("selection_statement", SELECTION_STATEMENT);
            insert(p, (yyvsp[(1) - (5)]));
            insert(p, (yyvsp[(2) - (5)]));
            insert(p, (yyvsp[(3) - (5)]));
            insert(p, (yyvsp[(4) - (5)]));
            insert(p, (yyvsp[(5) - (5)]));
            (yyval) = p; }
            break;

        case 196:

            /* Line 1455 of yacc.c  */
//#line 988 "hello.y"
        {p = newNode("iteration_statement", ITERATION_STATEMENT);
            insert(p, (yyvsp[(1) - (5)]));
            insert(p, (yyvsp[(2) - (5)]));
            insert(p, (yyvsp[(3) - (5)]));
            insert(p, (yyvsp[(4) - (5)]));
            insert(p, (yyvsp[(5) - (5)]));
            (yyval) = p; }
            break;

        case 197:

            /* Line 1455 of yacc.c  */
//#line 995 "hello.y"
        {p = newNode("iteration_statement", ITERATION_STATEMENT);
            insert(p, (yyvsp[(1) - (7)]));
            insert(p, (yyvsp[(2) - (7)]));
            insert(p, (yyvsp[(3) - (7)]));
            insert(p, (yyvsp[(4) - (7)]));
            insert(p, (yyvsp[(5) - (7)]));
            insert(p, (yyvsp[(6) - (7)]));
            insert(p, (yyvsp[(7) - (7)]));
            (yyval) = p; }
            break;

        case 198:

            /* Line 1455 of yacc.c  */
//#line 1004 "hello.y"
        {p = newNode("iteration_statement", ITERATION_STATEMENT);
            insert(p, (yyvsp[(1) - (6)]));
            insert(p, (yyvsp[(2) - (6)]));
            insert(p, (yyvsp[(3) - (6)]));
            insert(p, (yyvsp[(4) - (6)]));
            insert(p, (yyvsp[(5) - (6)]));
            insert(p, (yyvsp[(6) - (6)]));
            (yyval) = p; }
            break;

        case 199:

            /* Line 1455 of yacc.c  */
//#line 1012 "hello.y"
        {p = newNode("iteration_statement", ITERATION_STATEMENT);
            insert(p, (yyvsp[(1) - (7)]));
            insert(p, (yyvsp[(2) - (7)]));
            insert(p, (yyvsp[(3) - (7)]));
            insert(p, (yyvsp[(4) - (7)]));
            insert(p, (yyvsp[(5) - (7)]));
            insert(p, (yyvsp[(6) - (7)]));
            insert(p, (yyvsp[(7) - (7)]));
            (yyval) = p; }
            break;

        case 200:

            /* Line 1455 of yacc.c  */
//#line 1024 "hello.y"
        {p = newNode("jump_statement", JUMP_STATEMENT);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 201:

            /* Line 1455 of yacc.c  */
//#line 1029 "hello.y"
        {p = newNode("jump_statement", JUMP_STATEMENT);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 202:

            /* Line 1455 of yacc.c  */
//#line 1033 "hello.y"
        {p = newNode("jump_statement", JUMP_STATEMENT);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 203:

            /* Line 1455 of yacc.c  */
//#line 1037 "hello.y"
        {p = newNode("jump_statement", JUMP_STATEMENT);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;

        case 204:

            /* Line 1455 of yacc.c  */
//#line 1041 "hello.y"
        {p = newNode("jump_statement", JUMP_STATEMENT);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 205:

            /* Line 1455 of yacc.c  */
//#line 1049 "hello.y"
        {p = newNode("translation_unit", TRANSLATION_UNIT);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p;
            head = p; }
            break;

        case 206:

            /* Line 1455 of yacc.c  */
//#line 1053 "hello.y"
        {p = newNode("translation_unit", TRANSLATION_UNIT);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p;
            head = p; }
            break;

        case 207:

            /* Line 1455 of yacc.c  */
//#line 1061 "hello.y"
        {p = newNode("external_declaration", EXTERNAL_DECLARATION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 208:

            /* Line 1455 of yacc.c  */
//#line 1064 "hello.y"
        {p = newNode("external_declaration", EXTERNAL_DECLARATION);
            insert(p, (yyvsp[(1) - (1)]));
            (yyval) = p; }
            break;

        case 209:

            /* Line 1455 of yacc.c  */
//#line 1070 "hello.y"
        {p = newNode("function_definition", FUNCTION_DEFINITION);
            insert(p, (yyvsp[(1) - (4)]));
            insert(p, (yyvsp[(2) - (4)]));
            insert(p, (yyvsp[(3) - (4)]));
            (yyval) = p; }
            break;

        case 210:

            /* Line 1455 of yacc.c  */
//#line 1075 "hello.y"
        {p = newNode("function_definition", FUNCTION_DEFINITION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 211:

            /* Line 1455 of yacc.c  */
//#line 1080 "hello.y"
        {p = newNode("function_definition", FUNCTION_DEFINITION);
            insert(p, (yyvsp[(1) - (3)]));
            insert(p, (yyvsp[(2) - (3)]));
            insert(p, (yyvsp[(3) - (3)]));
            (yyval) = p; }
            break;

        case 212:

            /* Line 1455 of yacc.c  */
//#line 1085 "hello.y"
        {p = newNode("function_definition", FUNCTION_DEFINITION);
            insert(p, (yyvsp[(1) - (2)]));
            insert(p, (yyvsp[(2) - (2)]));
            (yyval) = p; }
            break;



            /* Line 1455 of yacc.c  */
//#line 4129 "y.tab.cpp"
        default: break;
    }
    YY_SYMBOL_PRINT("-> $$ =", yyr1[yyn], &yyval, &yyloc);

    YYPOPSTACK(yylen);
    yylen = 0;
    YY_STACK_PRINT(yyss, yyssp);

    *++yyvsp = yyval;

    /* Now `shift' the result of the reduction.  Determine what state
       that goes to, based on the state we popped back to and the rule
       number reduced by.  */

    yyn = yyr1[yyn];

    yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
    if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
        yystate = yytable[yystate];
    else
        yystate = yydefgoto[yyn - YYNTOKENS];

    goto yynewstate;


    /*------------------------------------.
    | yyerrlab -- here on detecting error |
    `------------------------------------*/
    yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus)
    {
        ++yynerrs;
#if ! YYERROR_VERBOSE
        yyerror(YY_(strdup("syntax error")));
#else
        {
			YYSIZE_T yysize = yysyntax_error(0, yystate, yychar);
			if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
			{
				YYSIZE_T yyalloc = 2 * yysize;
				if (!(yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
					yyalloc = YYSTACK_ALLOC_MAXIMUM;
				if (yymsg != yymsgbuf)
					YYSTACK_FREE(yymsg);
				yymsg = (char *)YYSTACK_ALLOC(yyalloc);
				if (yymsg)
					yymsg_alloc = yyalloc;
				else
				{
					yymsg = yymsgbuf;
					yymsg_alloc = sizeof yymsgbuf;
				}
			}

			if (0 < yysize && yysize <= yymsg_alloc)
			{
				(void)yysyntax_error(yymsg, yystate, yychar);
				yyerror(yymsg);
			}
			else
			{
				yyerror(YY_("syntax error"));
				if (yysize != 0)
					goto yyexhaustedlab;
			}
		}
#endif
    }



    if (yyerrstatus == 3)
    {
        /* If just tried and failed to reuse lookahead token after an
       error, discard it.  */

        if (yychar <= YYEOF)
        {
            /* Return failure if at end of input.  */
            if (yychar == YYEOF)
                YYABORT;
        }
        else
        {
            yydestruct("Error: discarding",
                       yytoken, &yylval);
            yychar = YYEMPTY;
        }
    }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
    goto yyerrlab1;


    /*---------------------------------------------------.
    | yyerrorlab -- error raised explicitly by YYERROR.  |
    `---------------------------------------------------*/
    yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (/*CONSTCOND*/ 0)
        goto yyerrorlab;

    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    YYPOPSTACK(yylen);
    yylen = 0;
    YY_STACK_PRINT(yyss, yyssp);
    yystate = *yyssp;
    goto yyerrlab1;


    /*-------------------------------------------------------------.
    | yyerrlab1 -- common code for both syntax error and YYERROR.  |
    `-------------------------------------------------------------*/
    yyerrlab1:
    yyerrstatus = 3;	/* Each real token shifted decrements this.  */

    for (;;)
    {
        yyn = yypact[yystate];
        if (yyn != YYPACT_NINF)
        {
            yyn += YYTERROR;
            if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
                yyn = yytable[yyn];
                if (0 < yyn)
                    break;
            }
        }

        /* Pop the current state because it cannot handle the error token.  */
        if (yyssp == yyss)
            YYABORT;


        yydestruct("Error: popping",
                   yystos[yystate], yyvsp);
        YYPOPSTACK(1);
        yystate = *yyssp;
        YY_STACK_PRINT(yyss, yyssp);
    }

    *++yyvsp = yylval;


    /* Shift the error token.  */
    YY_SYMBOL_PRINT("Shifting", yystos[yyn], yyvsp, yylsp);

    yystate = yyn;
    goto yynewstate;


    /*-------------------------------------.
    | yyacceptlab -- YYACCEPT comes here.  |
    `-------------------------------------*/
    yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /*-----------------------------------.
    | yyabortlab -- YYABORT comes here.  |
    `-----------------------------------*/
    yyabortlab:
    yyresult = 1;
    goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
    /*-------------------------------------------------.
    | yyexhaustedlab -- memory exhaustion comes here.  |
    `-------------------------------------------------*/
    yyexhaustedlab :
    yyerror(YY_(strdup("memory exhausted")));
    yyresult = 2;
    /* Fall through.  */
#endif

    yyreturn:
    if (yychar != YYEMPTY)
        yydestruct("Cleanup: discarding lookahead",
                   yytoken, &yylval);
    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    YYPOPSTACK(yylen);
    YY_STACK_PRINT(yyss, yyssp);
    while (yyssp != yyss)
    {
        yydestruct("Cleanup: popping",
                   yystos[*yyssp], yyvsp);
        YYPOPSTACK(1);
    }
#ifndef yyoverflow
    if (yyss != yyssa)
        YYSTACK_FREE(yyss);
#endif
#if YYERROR_VERBOSE
    if (yymsg != yymsgbuf)
					   YYSTACK_FREE(yymsg);
#endif
    /* Make sure YYID is used.  */
    return YYID(yyresult);
}



/* Line 1675 of yacc.c  */
//#line 1091 "hello.y"



extern char yytext[];
extern int column;

int yyerror(char *s)
{
    cout<<s<<endl;
    throw "syntax error when parsing program!";
    fflush(stdout);

    exit(-1);
    return 1;
}

extern FILE *yyin;
int struct_ptr = 0;
#include<vector>
#include"y.tab.h"

std::vector<aka> aka_type_array;

void insert(gtree *parent, gtree *child)
{
    gtree *p;
    if (child == NULL)
        return;

    if (parent->No_Child == 0)
    {
        parent->child = child;
        //child->IsBegin=1;
        parent->No_Child = 1;
        child->parent = parent;
        parent->row = child->row;
    }
    else
    {
        p = parent->child;
        while (p->next != NULL)
            p = p->next;
        p->next = child;
        child->parent = parent;
        //child->IsBegin=0;
        parent->No_Child++;
    }
}

void printtree(gtree* t)
{
    if (t == NULL)
        return;
    std::cout << "current node :" << t->place << std::endl;
    printtree(t->child);
    printtree(t->next);
}


//****use for 'typedef' keyword*****//
void type_add(gtree *p)
{
    gtree *type_specifier = p->child->child;

    if (type_specifier->type != STORAGE_CLASS_SPECIFIER)
        return;
    if (type_specifier->child->type != TYPEDEF)
        return;

    gtree *init_declarator_list = p->child->next;
    if (init_declarator_list->type != INIT_DECLARATOR_LIST)
        return;

    string origin_name;

    type_specifier = type_specifier->next->child;
    while (type_specifier->type != TYPE_SPECIFIER)
        type_specifier = type_specifier->next->child;

    if (type_specifier->child->type != STRUCT_OR_UNION_SPECIFIER && type_specifier->child->type != ENUM_SPECIFIER)
        origin_name = type_specifier->child->place;
    else
    {
        if (type_specifier->child->child->next->type == IDENTIFIER)
            origin_name = type_specifier->child->child->next->place;
        else {
            origin_name = "struct" + to_string(struct_ptr++);
        }
        type_specifier->child->place = origin_name;
    }
    gtree *init_declarator;
    aka temp;
    while (init_declarator_list->type == INIT_DECLARATOR_LIST)
    {
        if (init_declarator_list->child->type == INIT_DECLARATOR)
            init_declarator = init_declarator_list->child;
        else
            init_declarator = init_declarator_list->child->next->next;

        while (init_declarator->type != IDENTIFIER)
            init_declarator = init_declarator->child;
        temp.origin_name = origin_name;
        temp.aka_name = init_declarator->place;
        temp.level = now_level;
        aka_type_array.push_back(temp);

        init_declarator_list = init_declarator_list->child;
    }
}