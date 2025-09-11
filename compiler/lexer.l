%{
#include <string>
#include <cstdlib>
#include "ast.hpp"
#include "parser.hpp"

void yyerror(const char* s);
%}

%option noyywrap
%option yylineno

%%

    /* Comentários */
"//".*                  { /* linha de comentário - ignora */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* bloco de comentário - ignora */ }

    /* Palavras-chave */
"bpm"                   { return BPM; }
"timesig"               { return TIMESIG; }
"instrument"            { return INSTRUMENT; }
"pattern"               { return PATTERN; }
"section"               { return SECTION; }
"arrange"               { return ARRANGE; }
"export"                { return EXPORT; }
"let"                   { return LET; }
"fn"                    { return FN; }
"if"                    { return IF; }
"else"                  { return ELSE; }
"while"                 { return WHILE; }
"loop"                  { return LOOP; }
"play"                  { return PLAY; }
"call"                  { return CALL; }
"wait"                  { return WAIT; }
"set"                   { return SET; }
"note"                  { return NOTE; }
"chord"                 { return CHORD; }

    /* Tipos de instrumentos */
"bass"                  { return BASS; }
"guitar"                { return GUITAR; }
"drums"                 { return DRUMS; }

    /* Tipos de bateria */
"kick"                  { return KICK; }
"snare"                 { return SNARE; }
"hihat"                 { return HIHAT; }

    /* Formatos de arquivo */
"wav"                   { return WAV; }
"mp3"                   { return MP3; }

    /* Unidades de tempo */
"ms"                    { return MS; }
"s"                     { return S; }
"ticks"                 { return TICKS; }
"beat"                  { return BEAT; }
"beats"                 { return BEATS; }

    /* Operadores e símbolos */
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return MULTIPLY; }
"/"                     { return DIVIDE; }
"%"                     { return MODULO; }
"="                     { return ASSIGN; }
"=="                    { return EQUAL; }
"!="                    { return NOT_EQUAL; }
"<"                     { return LESS; }
">"                     { return GREATER; }
"<="                    { return LESS_EQUAL; }
">="                    { return GREATER_EQUAL; }
"&&"                    { return AND; }
"||"                    { return OR; }
"!"                     { return NOT; }

    /* Delimitadores */
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
"["                     { return LBRACKET; }
"]"                     { return RBRACKET; }
";"                     { return SEMICOLON; }
","                     { return COMMA; }
":"                     { return COLON; }

    /* Números */
[0-9]+\.[0-9]+          { 
                          yylval.number = std::stod(yytext); 
                          return NUMBER; 
                        }
[0-9]+                  { 
                          yylval.number = std::stod(yytext); 
                          return NUMBER; 
                        }

    /* Strings */
\"([^\"\\]|\\.)*\"      { 
                          yylval.string = strdup(yytext + 1);
                          yylval.string[strlen(yylval.string) - 1] = '\0'; 
                          return STRING; 
                        }

    /* Identificadores */
[a-zA-Z_][a-zA-Z0-9_]*  { 
                          yylval.string = strdup(yytext); 
                          return IDENTIFIER; 
                        }

    /* Whitespace */
[ \t\r\n]+              { /* ignora whitespace */ }

    /* Qualquer outro caractere */
.                       { 
                          printf("Caractere não reconhecido: %c\n", yytext[0]); 
                          return yytext[0];
                        }

%%
