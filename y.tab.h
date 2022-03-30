//
// Created on 2020/10/10.
//

#ifndef CPN_PNML_PARSE_Y_TAB_H
#define CPN_PNML_PARSE_Y_TAB_H




/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
    TYPE_NAME = 283,
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

//enum item1 {
//	PRIMARY_EXPRESSION = 400,
//	POSTFIX_EXPRESSION,
//	ARGUMENT_EXPRESSION_LIST,
//	UNARY_EXPRESSION,
//	UNARY_OPERATOR,
//	CAST_EXPRESSION,
//	MULTIPLICATIVE_EXPRESSION,
//	ADDITIVE_EXPRESSION,
//	SHIFT_EXPRESSION,
//	RELATIONAL_EXPRESSION,
//	EQUALITY_EXPRESSION,
//	AND_EXPRESSION,
//	EXCLUSIVE_OR_EXPRESSION,
//	INCLUSIVE_OR_EXPRESSION,
//	LOGICAL_AND_EXPRESSION,
//	LOGICAL_OR_EXPRESSION,
//	CONDITIONAL_EXPRESSION,
//	ASSIGNMENT_EXPRESSION,
//	ASSIGNMENT_OPERATOR,
//	EXPRESSION,
//	CONSTANT_EXPRESSION,
//	DECLARATION,
//	DECLARATION_SPECIFIERS,
//	INIT_DECLARATOR_LIST,
//	INIT_DECLARATOR,
//	STORAGE_CLASS_SPECIFIER,
//	TYPE_SPECIFIER,
//	STRUCT_OR_UNION_SPECIFIER,
//	STRUCT_OR_UNION,
//	STRUCT_DECLARATION_LIST,
//	STRUCT_DECLARATION,
//	SPECIFIER_QUALIFIER_LIST,
//	STRUCT_DECLARATOR_LIST,
//	STRUCT_DECLARATOR,
//	ENUM_SPECIFIER,
//	ENUMERATOR_LIST,
//	ENUMERATOR,
//	TYPE_QUALIFIER,
//	DECLARATOR,
//	DIRECT_DECLARATOR,
//	POINTER,
//	TYPE_QUALIFIER_LIST,
//	PARAMETER_TYPE_LIST,
//	PARAMETER_LIST,
//	PARAMETER_DECLARATION,
//	IDENTIFIER_LIST,
//	//TYPE_NAME,
//	ABSTRACT_DECLARATOR,
//	DIRECT_ABSTRACT_DECLARATOR,
//	INITIALIZER,
//	INITIALIZER_LIST,
//	STATEMENT,
//	LABELED_STATEMENT,
//	COMPOUND_STATEMENT,
//	DECLARATION_LIST,
//	STATEMENT_LIST,
//	EXPRESSION_STATEMENT,
//	SELECTION_STATEMENT,
//	ITERATION_STATEMENT,
//	JUMP_STATEMENT,
//	TRANSLATION_UNIT,
//	EXTERNAL_DECLARATION,
//	FUNCTION_DEFINITION
//};

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

extern YYSTYPE yylval;

#include<vector>
#include<string>
#include "AST.h"


class aka
{
public:
    string origin_name;
    string aka_name;
    int level;

};
extern std::vector<aka> aka_type_array;
extern int now_level;
gtree* newNode(std::string node_name, int type);
#endif //CPN_PNML_PARSE_Y_TAB_H