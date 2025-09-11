/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUMBER = 258,                  /* NUMBER  */
    STRING = 259,                  /* STRING  */
    IDENTIFIER = 260,              /* IDENTIFIER  */
    BPM = 261,                     /* BPM  */
    TIMESIG = 262,                 /* TIMESIG  */
    INSTRUMENT = 263,              /* INSTRUMENT  */
    PATTERN = 264,                 /* PATTERN  */
    SECTION = 265,                 /* SECTION  */
    ARRANGE = 266,                 /* ARRANGE  */
    EXPORT = 267,                  /* EXPORT  */
    LET = 268,                     /* LET  */
    FN = 269,                      /* FN  */
    IF = 270,                      /* IF  */
    ELSE = 271,                    /* ELSE  */
    WHILE = 272,                   /* WHILE  */
    LOOP = 273,                    /* LOOP  */
    PLAY = 274,                    /* PLAY  */
    CALL = 275,                    /* CALL  */
    WAIT = 276,                    /* WAIT  */
    SET = 277,                     /* SET  */
    NOTE = 278,                    /* NOTE  */
    CHORD = 279,                   /* CHORD  */
    BASS = 280,                    /* BASS  */
    GUITAR = 281,                  /* GUITAR  */
    DRUMS = 282,                   /* DRUMS  */
    KICK = 283,                    /* KICK  */
    SNARE = 284,                   /* SNARE  */
    HIHAT = 285,                   /* HIHAT  */
    WAV = 286,                     /* WAV  */
    MP3 = 287,                     /* MP3  */
    MS = 288,                      /* MS  */
    S = 289,                       /* S  */
    TICKS = 290,                   /* TICKS  */
    BEAT = 291,                    /* BEAT  */
    BEATS = 292,                   /* BEATS  */
    PLUS = 293,                    /* PLUS  */
    MINUS = 294,                   /* MINUS  */
    MULTIPLY = 295,                /* MULTIPLY  */
    DIVIDE = 296,                  /* DIVIDE  */
    MODULO = 297,                  /* MODULO  */
    ASSIGN = 298,                  /* ASSIGN  */
    EQUAL = 299,                   /* EQUAL  */
    NOT_EQUAL = 300,               /* NOT_EQUAL  */
    LESS = 301,                    /* LESS  */
    GREATER = 302,                 /* GREATER  */
    LESS_EQUAL = 303,              /* LESS_EQUAL  */
    GREATER_EQUAL = 304,           /* GREATER_EQUAL  */
    AND = 305,                     /* AND  */
    OR = 306,                      /* OR  */
    NOT = 307,                     /* NOT  */
    LPAREN = 308,                  /* LPAREN  */
    RPAREN = 309,                  /* RPAREN  */
    LBRACE = 310,                  /* LBRACE  */
    RBRACE = 311,                  /* RBRACE  */
    LBRACKET = 312,                /* LBRACKET  */
    RBRACKET = 313,                /* RBRACKET  */
    SEMICOLON = 314,               /* SEMICOLON  */
    COMMA = 315,                   /* COMMA  */
    COLON = 316,                   /* COLON  */
    UMINUS = 317                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "parser.y"

    double number;
    char* string;
    SimpleNode* node;

#line 132 "parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_HPP_INCLUDED  */
