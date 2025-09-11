# BandLang - Especificação EBNF

## Linguagem de Alto Nível para GarageBand VM

A linguagem BandLang permite criar composições musicais declarativas com suporte a:
- **Instrumentos**: bass, guitar, drums (✅ implementado)
- **Elementos musicais**: notas individuais, acordes, bateria (✅ implementado)
- **Estruturas de controle**: variáveis, loops (✅ implementado)
- **Configuração temporal**: BPM, fórmula de compasso (✅ implementado)

## 🎯 Status de Implementação

✅ **Totalmente implementado e funcionando**:
- Notas individuais com pitch, velocity, duração
- Acordes harmônicos com múltiplas notas
- Bateria completa (kick, snare, hihat)
- Loops com contadores (loop N { })
- Variáveis e expressões básicas
- Instrumentos com trilhas separadas
- Compilador Python robusto e funcional

🔧 **Implementação Real**:
- VM processa sons **sequencialmente** (um após outro)
- Compilador Python substitui Flex/Bison para maior robustez
- Foco em criar grooves musicais funcionais

## Gramática EBNF

```ebnf
program        = header , { decl | statement } , export_stmt ;

header         = "bpm" number ";" , "timesig" number "/" number ";" ;

decl           = instrument_decl | pattern_decl | var_decl | func_decl ;

instrument_decl= "instrument" ident ":" ("bass" | "guitar" | "drums") ";" ;

pattern_decl   = "pattern" ident "{" { statement } "}" ;

var_decl       = "let" ident "=" expr ";" ;

func_decl      = "fn" ident "(" [param_list] ")" "{" { statement } "}" ;

statement      = assign | if_stmt | while_stmt | play_stmt | drum_stmt
                 | chord_stmt | set_stmt | loop_stmt | section_stmt
                 | arrange_stmt | call_stmt | wait_stmt | comment ;

assign         = ident "=" expr ";" ;

if_stmt        = "if" "(" expr ")" "{" { statement } "}" 
                 [ "else" "{" { statement } "}" ] ;

while_stmt     = "while" "(" expr ")" "{" { statement } "}" ;

play_stmt      = "play" ident ":" "note" pitch "," velocity "," duration ";" ;

chord_stmt     = "play" ident ":" "chord" "[" pitch { "," pitch } "]" 
                 "," velocity "," duration ";" ;

drum_stmt      = "play" "drums" ":" ("kick"|"snare"|"hihat") 
                 "," velocity "," duration ";" ;

set_stmt       = "set" ("bpm" expr | "timesig" number "/" number) ";" ;

loop_stmt      = "loop" number "{" { statement } "}" ;

section_stmt   = "section" ident "{" { statement } "}" ;

arrange_stmt   = "arrange" ident { "," ident } ";" ;

call_stmt      = "call" ident "(" [arg_list] ")" ";" ;

wait_stmt      = "wait" duration ";" ;

export_stmt    = "export" string ["," ("wav"|"mp3")] ";" ;

expr           = logic_or ;
logic_or       = logic_and { "||" logic_and } ;
logic_and      = equality { "&&" equality } ;
equality       = comparison { ("==" | "!=") comparison } ;
comparison     = term { ("<" | ">" | "<=" | ">=") term } ;
term           = factor { ("+" | "-") factor } ;
factor         = unary { ("*" | "/" | "%") unary } ;
unary          = ("!" | "-") unary | primary ;
primary        = number | ident | string | "(" expr ")" ;

param_list     = ident { "," ident } ;
arg_list       = expr { "," expr } ;

pitch          = ident | string | number ;  (* ex: A4, "E2", 64 (MIDI) *)
velocity       = number ;                   (* 0..127 *)
duration       = number [ "ticks" | "ms" | "s" | "beat" | "beats" ] ;

ident          = letter { letter | digit | "_" } ;
number         = digit { digit } [ "." digit { digit } ] ;
string         = '"' { any_char } '"' ;
comment        = "/*" { any_char } "*/" | "//" { any_char } "\n" ;

letter         = "a" | "b" | ... | "z" | "A" | "B" | ... | "Z" ;
digit          = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
any_char       = (* qualquer caractere exceto delimitadores específicos *) ;
```

## Elementos da Linguagem

### 1. Header Obrigatório
```bandlang
bpm 120;           // Define BPM global
timesig 4/4;       // Define fórmula de compasso
```

### 2. Declarações

#### Instrumentos
```bandlang
instrument bass: bass;       // Declara instrumento de baixo
instrument gtr: guitar;      // Declara guitarra
instrument kit: drums;       // Declara bateria
```

#### Variáveis
```bandlang
let volume = 100;           // Variável inteira
let tempo = 120.5;          // Variável decimal
let name = "my_song";       // Variável string
```

#### Padrões Reutilizáveis
```bandlang
pattern groove {
    play drums: kick, 100, 120ms;
    wait 240ms;
    play drums: snare, 90, 120ms;
    wait 240ms;
}
```

#### Funções
```bandlang
fn bassline(note, vel) {
    play bass: note note, vel, 500ms;
    wait 500ms;
}
```

### 3. Comandos Musicais

#### Notas Individuais
```bandlang
play bass: note "E2", 100, 500ms;      // Nota com string
play gtr: note A4, 90, 250ms;          // Nota com identificador
play lead: note 64, 80, 125ms;         // Nota MIDI (0-127)
```

#### Acordes
```bandlang
play gtr: chord ["E3", "B3", "E4"], 90, 1000ms;
play piano: chord [60, 64, 67], 100, 500ms;
```

#### Bateria
```bandlang
play drums: kick, 100, 120ms;
play drums: snare, 90, 120ms;
play drums: hihat, 70, 60ms;
```

### 4. Estruturas de Controle

#### Condicionais
```bandlang
if (volume > 50) {
    play drums: kick, volume, 120ms;
} else {
    play drums: hihat, volume, 60ms;
}
```

#### Loops Condicionais
```bandlang
let i = 0;
while (i < 4) {
    call bassline("E2", 100);
    i = i + 1;
}
```

#### Loops de Repetição
```bandlang
loop 8 {
    call groove();
}
```

### 5. Estruturação Musical

#### Seções
```bandlang
section verse {
    loop 4 {
        call bassline("E2", 100);
        call groove();
    }
}

section chorus {
    play gtr: chord ["A3", "C4", "E4"], 100, 2000ms;
    call groove();
}
```

#### Arranjo
```bandlang
arrange verse, chorus, verse, chorus;
```

### 6. Controle Temporal

#### Configuração Dinâmica
```bandlang
set bpm 140;               // Muda BPM durante execução
set timesig 3/4;           // Muda fórmula de compasso
```

#### Pausas
```bandlang
wait 500ms;                // Pausa em milissegundos
wait 1s;                   // Pausa em segundos
wait 2beats;               // Pausa em beats
wait 480ticks;             // Pausa em ticks
```

### 7. Exportação
```bandlang
export "minha_musica.wav";              // WAV apenas
export "minha_musica.mp3", mp3;         // MP3 se disponível
export "output/song.wav", wav;          // WAV explícito
```

## Unidades de Duração

- **`ms`**: milissegundos
- **`s`**: segundos  
- **`ticks`**: ticks de resolução da VM (padrão: 480 por beat)
- **`beat`/`beats`**: beats baseados no BPM atual

## Notas sobre Expressões

A linguagem suporta expressões aritméticas e lógicas completas:
- **Aritméticas**: `+`, `-`, `*`, `/`, `%`
- **Comparação**: `<`, `>`, `<=`, `>=`, `==`, `!=`
- **Lógicas**: `&&`, `||`, `!`
- **Precedência**: seguindo convenções matemáticas padrão

## Exemplos de Uso

### Exemplo Simples
```bandlang
bpm 120; timesig 4/4;

instrument bass: bass;

play bass: note "E2", 100, 1000ms;
wait 1000ms;

export "simple.wav";
```

### Exemplo com Estruturas de Controle
```bandlang
bpm 140; timesig 4/4;

instrument drums: drums;

let bars = 4;
let i = 0;

while (i < bars) {
    if (i % 2 == 0) {
        play drums: kick, 100, 120ms;
    } else {
        play drums: snare, 90, 120ms;
    }
    wait 360ms;
    i = i + 1;
}

export "rhythm.wav";
```

---

**Nota**: Esta gramática atende aos requisitos acadêmicos de variáveis, condicionais e loops, fornecendo uma base sólida para compilação via Flex/Bison para o assembly GBASM da VM.
