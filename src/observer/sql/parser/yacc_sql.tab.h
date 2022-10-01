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
    TEXT_T = 286,
    HELP = 287,
    EXIT = 288,
    DOT = 289,
    INTO = 290,
    VALUES = 291,
    FROM = 292,
    WHERE = 293,
    AND = 294,
    SET = 295,
    ON = 296,
    LOAD = 297,
    DATA = 298,
    INFILE = 299,
    EQ = 300,
    LT = 301,
    GT = 302,
    LE = 303,
    GE = 304,
    NE = 305,
    NUMBER = 306,
    FLOAT = 307,
    ID = 308,
    PATH = 309,
    SSS = 310,
    DATE = 311,
    STAR = 312,
    STRING_V = 313
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
#define TEXT_T 286
#define HELP 287
#define EXIT 288
#define DOT 289
#define INTO 290
#define VALUES 291
#define FROM 292
#define WHERE 293
#define AND 294
#define SET 295
#define ON 296
#define LOAD 297
#define DATA 298
#define INFILE 299
#define EQ 300
#define LT 301
#define GT 302
#define LE 303
#define GE 304
#define NE 305
#define NUMBER 306
#define FLOAT 307
#define ID 308
#define PATH 309
#define SSS 310
#define DATE 311
#define STAR 312
#define STRING_V 313

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 113 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
	char *position;
  int date;

#line 184 "yacc_sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner);

#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */
