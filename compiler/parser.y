%{
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include "ast.hpp"

// Forward declaration
struct SimpleNode;

extern int yylex();
extern int yylineno;
void yyerror(const char* s);

SimpleNode* root = nullptr;
%}

%union {
    double number;
    char* string;
    SimpleNode* node;
}

%token <number> NUMBER
%token <string> STRING IDENTIFIER

%token BPM TIMESIG INSTRUMENT PATTERN SECTION ARRANGE EXPORT
%token LET FN IF ELSE WHILE LOOP
%token PLAY CALL WAIT SET NOTE CHORD
%token BASS GUITAR DRUMS
%token KICK SNARE HIHAT
%token WAV MP3
%token MS S TICKS BEAT BEATS

%token PLUS MINUS MULTIPLY DIVIDE MODULO
%token ASSIGN EQUAL NOT_EQUAL LESS GREATER LESS_EQUAL GREATER_EQUAL
%token AND OR NOT

%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMICOLON COMMA COLON

%type <node> program header statement_list statement
%type <node> instrument_declaration play_statement wait_statement
%type <node> variable_declaration assignment export_statement
%type <node> while_statement loop_statement
%type <node> expression primary_expression
%type <node> pitch duration pitch_list

%left OR
%left AND
%left EQUAL NOT_EQUAL
%left LESS GREATER LESS_EQUAL GREATER_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT
%right UMINUS

%start program

%%

program:
    header statement_list export_statement {
        $$ = new SimpleNode("program");
        $$->addChild($1);
        $$->addChild($2);
        $$->addChild($3);
        root = $$;
    }
    ;

header:
    BPM NUMBER SEMICOLON TIMESIG NUMBER DIVIDE NUMBER SEMICOLON {
        $$ = new SimpleNode("header");
        $$->addChild(new SimpleNode("bpm", std::to_string(static_cast<int>($2))));
        $$->addChild(new SimpleNode("timesig", std::to_string(static_cast<int>($5)) + "/" + std::to_string(static_cast<int>($7))));
    }
    ;

statement_list:
    /* empty */ {
        $$ = new SimpleNode("statements");
    }
    | statement_list statement {
        $1->addChild($2);
        $$ = $1;
    }
    ;

statement:
    instrument_declaration { $$ = $1; }
    | play_statement { $$ = $1; }
    | wait_statement { $$ = $1; }
    | variable_declaration { $$ = $1; }
    | assignment { $$ = $1; }
    | while_statement { $$ = $1; }
    | loop_statement { $$ = $1; }
    ;

instrument_declaration:
    INSTRUMENT IDENTIFIER COLON BASS SEMICOLON {
        $$ = new SimpleNode("instrument_decl");
        $$->addChild(new SimpleNode("name", $2));
        $$->addChild(new SimpleNode("type", "bass"));
        free($2);
    }
    | INSTRUMENT IDENTIFIER COLON GUITAR SEMICOLON {
        $$ = new SimpleNode("instrument_decl");
        $$->addChild(new SimpleNode("name", $2));
        $$->addChild(new SimpleNode("type", "guitar"));
        free($2);
    }
    | INSTRUMENT IDENTIFIER COLON DRUMS SEMICOLON {
        $$ = new SimpleNode("instrument_decl");
        $$->addChild(new SimpleNode("name", $2));
        $$->addChild(new SimpleNode("type", "drums"));
        free($2);
    }
    ;

variable_declaration:
    LET IDENTIFIER ASSIGN expression SEMICOLON {
        $$ = new SimpleNode("var_decl");
        $$->addChild(new SimpleNode("name", $2));
        $$->addChild($4);
        free($2);
    }
    ;

assignment:
    IDENTIFIER ASSIGN expression SEMICOLON {
        $$ = new SimpleNode("assignment");
        $$->addChild(new SimpleNode("name", $1));
        $$->addChild($3);
        free($1);
    }
    ;

play_statement:
    PLAY IDENTIFIER COLON NOTE pitch COMMA expression COMMA duration SEMICOLON {
        $$ = new SimpleNode("play_note");
        $$->addChild(new SimpleNode("instrument", $2));
        $$->addChild($5);
        $$->addChild($7);
        $$->addChild($9);
        free($2);
    }
    | PLAY IDENTIFIER COLON CHORD LBRACKET pitch_list RBRACKET COMMA expression COMMA duration SEMICOLON {
        $$ = new SimpleNode("play_chord");
        $$->addChild(new SimpleNode("instrument", $2));
        $$->addChild($6);
        $$->addChild($9);
        $$->addChild($11);
        free($2);
    }
    | PLAY DRUMS COLON KICK COMMA expression COMMA duration SEMICOLON {
        $$ = new SimpleNode("play_drum");
        $$->addChild(new SimpleNode("instrument", "drums"));
        $$->addChild(new SimpleNode("drum_type", "kick"));
        $$->addChild($6);
        $$->addChild($8);
    }
    | PLAY DRUMS COLON SNARE COMMA expression COMMA duration SEMICOLON {
        $$ = new SimpleNode("play_drum");
        $$->addChild(new SimpleNode("instrument", "drums"));
        $$->addChild(new SimpleNode("drum_type", "snare"));
        $$->addChild($6);
        $$->addChild($8);
    }
    | PLAY DRUMS COLON HIHAT COMMA expression COMMA duration SEMICOLON {
        $$ = new SimpleNode("play_drum");
        $$->addChild(new SimpleNode("instrument", "drums"));
        $$->addChild(new SimpleNode("drum_type", "hihat"));
        $$->addChild($6);
        $$->addChild($8);
    }
    ;

wait_statement:
    WAIT duration SEMICOLON {
        $$ = new SimpleNode("wait");
        $$->addChild($2);
    }
    ;

while_statement:
    WHILE LPAREN expression RPAREN LBRACE statement_list RBRACE {
        $$ = new SimpleNode("while_stmt");
        $$->addChild($3);
        $$->addChild($6);
    }
    ;

loop_statement:
    LOOP expression LBRACE statement_list RBRACE {
        $$ = new SimpleNode("loop_stmt");
        $$->addChild($2);
        $$->addChild($4);
    }
    ;

export_statement:
    EXPORT STRING SEMICOLON {
        $$ = new SimpleNode("export");
        $$->addChild(new SimpleNode("filename", $2));
        $$->addChild(new SimpleNode("format", "wav"));
        free($2);
    }
    | EXPORT STRING COMMA WAV SEMICOLON {
        $$ = new SimpleNode("export");
        $$->addChild(new SimpleNode("filename", $2));
        $$->addChild(new SimpleNode("format", "wav"));
        free($2);
    }
    ;

pitch:
    STRING {
        $$ = new SimpleNode("pitch_string", $1);
        free($1);
    }
    | IDENTIFIER {
        $$ = new SimpleNode("pitch_identifier", $1);
        free($1);
    }
    | NUMBER {
        $$ = new SimpleNode("pitch_number", std::to_string(static_cast<int>($1)));
    }
    ;

pitch_list:
    pitch {
        $$ = new SimpleNode("pitch_list");
        $$->addChild($1);
    }
    | pitch_list COMMA pitch {
        $1->addChild($3);
        $$ = $1;
    }
    ;

duration:
    expression MS {
        $$ = new SimpleNode("duration");
        $$->addChild($1);
        $$->addChild(new SimpleNode("unit", "ms"));
    }
    | expression S {
        $$ = new SimpleNode("duration");
        $$->addChild($1);
        $$->addChild(new SimpleNode("unit", "s"));
    }
    | expression TICKS {
        $$ = new SimpleNode("duration");
        $$->addChild($1);
        $$->addChild(new SimpleNode("unit", "ticks"));
    }
    | expression {
        $$ = new SimpleNode("duration");
        $$->addChild($1);
        $$->addChild(new SimpleNode("unit", "ms"));
    }
    ;

expression:
    primary_expression { $$ = $1; }
    | expression PLUS expression {
        $$ = new SimpleNode("binary_expr", "+");
        $$->addChild($1);
        $$->addChild($3);
    }
    | expression MINUS expression {
        $$ = new SimpleNode("binary_expr", "-");
        $$->addChild($1);
        $$->addChild($3);
    }
    | expression MULTIPLY expression {
        $$ = new SimpleNode("binary_expr", "*");
        $$->addChild($1);
        $$->addChild($3);
    }
    | expression DIVIDE expression {
        $$ = new SimpleNode("binary_expr", "/");
        $$->addChild($1);
        $$->addChild($3);
    }
    | expression EQUAL expression {
        $$ = new SimpleNode("binary_expr", "==");
        $$->addChild($1);
        $$->addChild($3);
    }
    | expression NOT_EQUAL expression {
        $$ = new SimpleNode("binary_expr", "!=");
        $$->addChild($1);
        $$->addChild($3);
    }
    | expression LESS expression {
        $$ = new SimpleNode("binary_expr", "<");
        $$->addChild($1);
        $$->addChild($3);
    }
    | expression GREATER expression {
        $$ = new SimpleNode("binary_expr", ">");
        $$->addChild($1);
        $$->addChild($3);
    }
    ;

primary_expression:
    NUMBER {
        $$ = new SimpleNode("number", std::to_string($1));
    }
    | STRING {
        $$ = new SimpleNode("string", $1);
        free($1);
    }
    | IDENTIFIER {
        $$ = new SimpleNode("identifier", $1);
        free($1);
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    ;

%%

void yyerror(const char* s) {
    std::cerr << "Erro na linha " << yylineno << ": " << s << std::endl;
}
