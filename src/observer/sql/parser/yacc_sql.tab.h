/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_YACC_SQL_TAB_H_INCLUDED
# define YY_YY_YACC_SQL_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    SEMICOLON = 258,
    CREATE = 259,
    DROP = 260,
    TABLE = 261,
    TABLES = 262,
    INDEX = 263,
    SELECT = 264,
    DESC = 265,
    SHOW = 266,
    SYNC = 267,
    INSERT = 268,
    DELETE = 269,
    UPDATE = 270,
    UNIQUE = 271,
    MAX = 272,
    MIN = 273,
    COUNT = 274,
    AVG = 275,
    LBRACE = 276,
    RBRACE = 277,
    COMMA = 278,
    TRX_BEGIN = 279,
    TRX_COMMIT = 280,
    TRX_ROLLBACK = 281,
    INT_T = 282,
    STRING_T = 283,
    FLOAT_T = 284,
    DATE_T = 285,
    HELP = 286,
    EXIT = 287,
    DOT = 288,
    INTO = 289,
    VALUES = 290,
    FROM = 291,
    WHERE = 292,
    AND = 293,
    SET = 294,
    ON = 295,
    LOAD = 296,
    DATA = 297,
    INFILE = 298,
    EQ = 299,
    LT = 300,
    GT = 301,
    LE = 302,
    GE = 303,
    NE = 304,
    NUMBER = 305,
    FLOAT = 306,
    ID = 307,
    PATH = 308,
    SSS = 309,
    DATE = 310,
    STAR = 311,
    STRING_V = 312
  };
#endif
/* Tokens.  */
#define SEMICOLON 258
#define CREATE 259
#define DROP 260
#define TABLE 261
#define TABLES 262
#define INDEX 263
#define SELECT 264
#define DESC 265
#define SHOW 266
#define SYNC 267
#define INSERT 268
#define DELETE 269
#define UPDATE 270
#define UNIQUE 271
#define MAX 272
#define MIN 273
#define COUNT 274
#define AVG 275
#define LBRACE 276
#define RBRACE 277
#define COMMA 278
#define TRX_BEGIN 279
#define TRX_COMMIT 280
#define TRX_ROLLBACK 281
#define INT_T 282
#define STRING_T 283
#define FLOAT_T 284
#define DATE_T 285
#define HELP 286
#define EXIT 287
#define DOT 288
#define INTO 289
#define VALUES 290
#define FROM 291
#define WHERE 292
#define AND 293
#define SET 294
#define ON 295
#define LOAD 296
#define DATA 297
#define INFILE 298
#define EQ 299
#define LT 300
#define GT 301
#define LE 302
#define GE 303
#define NE 304
#define NUMBER 305
#define FLOAT 306
#define ID 307
#define PATH 308
#define SSS 309
#define DATE 310
#define STAR 311
#define STRING_V 312

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 112 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
	char *position;
  int date;

#line 182 "yacc_sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner);

#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */
