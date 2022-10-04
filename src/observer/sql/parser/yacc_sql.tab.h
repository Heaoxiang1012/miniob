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
    ASC = 266,
    SHOW = 267,
    SYNC = 268,
    INSERT = 269,
    DELETE = 270,
    UPDATE = 271,
    UNIQUE = 272,
    MAX = 273,
    MIN = 274,
    COUNT = 275,
    AVG = 276,
    LBRACE = 277,
    RBRACE = 278,
    COMMA = 279,
    TRX_BEGIN = 280,
    TRX_COMMIT = 281,
    TRX_ROLLBACK = 282,
    INT_T = 283,
    STRING_T = 284,
    FLOAT_T = 285,
    DATE_T = 286,
    TEXT_T = 287,
    HELP = 288,
    EXIT = 289,
    DOT = 290,
    INTO = 291,
    VALUES = 292,
    FROM = 293,
    WHERE = 294,
    AND = 295,
    SET = 296,
    ON = 297,
    LOAD = 298,
    DATA = 299,
    INFILE = 300,
    EQ = 301,
    LT = 302,
    GT = 303,
    LE = 304,
    GE = 305,
    NE = 306,
    ORDER = 307,
    BY = 308,
    INNER = 309,
    JOIN = 310,
    NUMBER = 311,
    FLOAT = 312,
    ID = 313,
    PATH = 314,
    SSS = 315,
    DATE = 316,
    STAR = 317,
    STRING_V = 318
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
#define ASC 266
#define SHOW 267
#define SYNC 268
#define INSERT 269
#define DELETE 270
#define UPDATE 271
#define UNIQUE 272
#define MAX 273
#define MIN 274
#define COUNT 275
#define AVG 276
#define LBRACE 277
#define RBRACE 278
#define COMMA 279
#define TRX_BEGIN 280
#define TRX_COMMIT 281
#define TRX_ROLLBACK 282
#define INT_T 283
#define STRING_T 284
#define FLOAT_T 285
#define DATE_T 286
#define TEXT_T 287
#define HELP 288
#define EXIT 289
#define DOT 290
#define INTO 291
#define VALUES 292
#define FROM 293
#define WHERE 294
#define AND 295
#define SET 296
#define ON 297
#define LOAD 298
#define DATA 299
#define INFILE 300
#define EQ 301
#define LT 302
#define GT 303
#define LE 304
#define GE 305
#define NE 306
#define ORDER 307
#define BY 308
#define INNER 309
#define JOIN 310
#define NUMBER 311
#define FLOAT 312
#define ID 313
#define PATH 314
#define SSS 315
#define DATE 316
#define STAR 317
#define STRING_V 318

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 119 "yacc_sql.y"

  struct _Attr *attr;
  struct _Condition *condition1;
  struct _Value *value1;
  char *string;
  int number;
  float floats;
	char *position;
  int date;

#line 194 "yacc_sql.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (void *scanner);

#endif /* !YY_YY_YACC_SQL_TAB_H_INCLUDED  */
