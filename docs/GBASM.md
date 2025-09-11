# GBASM - Assembly da GarageBand VM

## Visão Geral

O GBASM (GarageBand Assembly) é a linguagem assembly de baixo nível interpretada pela GarageBand VM. Cada linha representa uma instrução, com suporte a comentários e rótulos para controle de fluxo.

## 🎯 Status de Implementação

✅ **Totalmente implementado e testado**:
- Configuração temporal (SET_TEMPO, SET_TS)
- Seleção de trilhas (TRACK 0/1/2)
- Notas individuais (NOTE)
- Acordes harmônicos (CHORD)
- Bateria completa (DRUM 0/1/2)
- Loops funcionais (LOAD, DECJNZ)
- Pausas e timing (WAIT)
- Rótulos e saltos (JMP, labels)

## Formato Geral

```gbasm
; Comentários começam com ponto e vírgula
; Rótulos terminam com dois pontos
:LABEL_NAME
INSTRUCTION operand1 operand2 ...
```

## Registradores

A VM possui pelo menos 2 registradores de propósito geral:
- **R0**: Registrador principal
- **R1**: Registrador auxiliar
- **R2**: Registrador adicional (opcional)
- **R3**: Registrador adicional (opcional)

## Sensores (Somente Leitura)

Sensores fornecem informações do estado atual da VM:
- **S0**: `TEMPO_ATUAL` - BPM atual
- **S1**: `CURRENT_TICK` - Tick atual na timeline  
- **S2**: `RANDOM_INT` - Número aleatório (0-65535)
- **S3**: `CURRENT_BAR` - Compasso atual
- **S4**: `BEAT_IN_BAR` - Beat atual dentro do compasso
- **S5**: `TRACK_ID` - ID da trilha ativa (0=bass, 1=guitar, 2=drums)

## Conjunto de Instruções

### Configuração Temporal

#### SET_TEMPO
Define o BPM da composição.
```gbasm
SET_TEMPO 120        ; Define BPM para 120
SET_TEMPO R0         ; Define BPM usando valor do registrador R0
```

#### SET_TS
Define a fórmula de compasso.
```gbasm
SET_TS 4 4           ; Define compasso 4/4
SET_TS 3 4           ; Define compasso 3/4
SET_TS 6 8           ; Define compasso 6/8
```

### Seleção de Trilha e Instrumento

#### TRACK
Seleciona a trilha ativa para próximas operações musicais.
```gbasm
TRACK 0              ; Seleciona trilha bass (0)
TRACK 1              ; Seleciona trilha guitar (1)  
TRACK 2              ; Seleciona trilha drums (2)
TRACK R0             ; Seleciona trilha usando registrador
```

#### SET_INSTR
Configura timbre/instrumento dentro da trilha (opcional).
```gbasm
SET_INSTR 0          ; Instrumento padrão
SET_INSTR 1          ; Instrumento alternativo 1
SET_INSTR R1         ; Instrumento via registrador
```

### Comandos Musicais

#### NOTE
Toca uma nota individual.
```gbasm
NOTE 60 100 480      ; Nota MIDI 60, velocity 100, duração 480 ticks
NOTE R0 R1 R2        ; Parâmetros via registradores
```
- **Parâmetro 1**: Pitch MIDI (0-127)
- **Parâmetro 2**: Velocity (0-127)  
- **Parâmetro 3**: Duração em ticks

#### CHORD
Toca múltiplas notas simultaneamente.
```gbasm
CHORD 3 60 64 67 100 480    ; Acorde de 3 notas: C4-E4-G4, vel 100, dur 480
CHORD R0 R1 R2 R3 90 240    ; Primeiros registradores definem as notas
```
- **Parâmetro 1**: Número de notas (1-8)
- **Parâmetros 2-N**: Pitches MIDI das notas
- **Penúltimo**: Velocity (0-127)
- **Último**: Duração em ticks

#### DRUM
Toca elementos de bateria.
```gbasm
DRUM 0 100 240       ; Kick (0), velocity 100, duração 240 ticks
DRUM 1 90 120        ; Snare (1), velocity 90, duração 120 ticks  
DRUM 2 70 60         ; Hi-hat (2), velocity 70, duração 60 ticks
DRUM R0 R1 R2        ; Parâmetros via registradores
```
- **Hit IDs**: 0=kick, 1=snare, 2=hihat, 3=crash, 4=ride

### Controle Temporal

#### WAIT
Pausa a execução por um período especificado.
```gbasm
WAIT 480             ; Espera 480 ticks
WAIT R0              ; Espera tempo definido no registrador R0
```

### Instruções de Registrador e Memória

#### LOAD
Carrega valor imediato no registrador.
```gbasm
LOAD R0 100          ; R0 = 100
LOAD R1 0            ; R1 = 0
LOAD R2 -50          ; R2 = -50
```

#### MOV  
Copia valor entre registradores.
```gbasm
MOV R0 R1            ; R0 = R1
MOV R2 R0            ; R2 = R0
```

#### READ
Lê valor de sensor para registrador.
```gbasm
READ R0 S0           ; R0 = TEMPO_ATUAL
READ R1 S1           ; R1 = CURRENT_TICK
READ R2 S2           ; R2 = RANDOM_INT
READ R3 S3           ; R3 = CURRENT_BAR
```

### Operações Aritméticas

#### INC
Incrementa registrador em 1.
```gbasm
INC R0               ; R0 = R0 + 1
INC R1               ; R1 = R1 + 1
```

#### DEC
Decrementa registrador em 1.
```gbasm
DEC R0               ; R0 = R0 - 1
DEC R1               ; R1 = R1 - 1
```

#### ADD
Soma dois registradores.
```gbasm
ADD R0 R1            ; R0 = R0 + R1
ADD R2 R3            ; R2 = R2 + R3
```

#### SUB
Subtrai dois registradores.
```gbasm
SUB R0 R1            ; R0 = R0 - R1
SUB R2 R3            ; R2 = R2 - R3
```

### Controle de Fluxo (Turing-Complete)

#### JMP
Pulo incondicional para rótulo.
```gbasm
JMP LOOP_START       ; Pula para :LOOP_START
JMP END              ; Pula para :END
```

#### DECJNZ
Decrementa registrador e pula se não for zero (Minsky machine style).
```gbasm
DECJNZ R0 LOOP       ; R0--; if (R0 != 0) goto LOOP
DECJNZ R1 REPEAT     ; R1--; if (R1 != 0) goto REPEAT
```

#### JZ / JNZ
Pulos condicionais baseados em zero.
```gbasm
JZ R0 ZERO_CASE      ; if (R0 == 0) goto ZERO_CASE
JNZ R1 NON_ZERO      ; if (R1 != 0) goto NON_ZERO
```

#### JEQ / JNE / JLT / JGT
Comparações entre registradores.
```gbasm
JEQ R0 R1 EQUAL      ; if (R0 == R1) goto EQUAL
JNE R0 R1 DIFFERENT  ; if (R0 != R1) goto DIFFERENT
JLT R0 R1 LESS       ; if (R0 < R1) goto LESS
JGT R0 R1 GREATER    ; if (R0 > R1) goto GREATER
```

### Gerenciamento de Pilha

#### PUSH
Empilha registrador na pilha.
```gbasm
PUSH R0              ; Empilha valor de R0
PUSH R1              ; Empilha valor de R1
```

#### POP
Desempilha valor para registrador.
```gbasm
POP R0               ; R0 = valor do topo da pilha
POP R1               ; R1 = próximo valor da pilha
```

### Controle de Execução

#### HALT
Finaliza execução da VM.
```gbasm
HALT                 ; Para a execução
```

#### NOP
Instrução sem operação (para debugging).
```gbasm
NOP                  ; Não faz nada, usa 1 ciclo
```

## Exemplo Completo

```gbasm
; Configuração inicial
SET_TEMPO 120
SET_TS 4 4

; Padrão de bateria com loop
TRACK 2              ; Seleciona drums

LOAD R0 4            ; Contador de repetições
:DRUM_LOOP
    DRUM 0 100 240   ; Kick
    WAIT 240
    DRUM 2 70 120    ; Hi-hat
    WAIT 120  
    DRUM 1 90 240    ; Snare
    WAIT 240
    DRUM 2 70 120    ; Hi-hat
    WAIT 120
    
    DECJNZ R0 DRUM_LOOP  ; Decrementa e repete se R0 > 0

; Linha de baixo
TRACK 0              ; Seleciona bass
LOAD R1 2            ; 2 repetições
:BASS_LOOP
    NOTE 40 100 960  ; E2 por 2 beats
    WAIT 960
    NOTE 47 100 480  ; B2 por 1 beat  
    WAIT 480
    NOTE 40 100 480  ; E2 por 1 beat
    WAIT 480
    
    DECJNZ R1 BASS_LOOP

; Acorde de guitarra
TRACK 1              ; Seleciona guitar
CHORD 3 52 59 64 90 1920  ; Acorde Em (E3-B3-E4) por 4 beats
WAIT 1920

HALT                 ; Fim da música
```

## Considerações de Implementação

### Timeline e Ticks
- A VM usa uma timeline baseada em **ticks** como unidade mínima de tempo
- Resolução padrão: **480 ticks por beat** (quarter note)
- Conversão: `ticks = (duração_ms * bpm * 480) / (60 * 1000)`

### Concurrent Events
- Múltiplas trilhas podem ter eventos simultâneos
- A VM mixa todas as trilhas ativas no buffer de áudio final
- Eventos com timestamp idêntico são processados em paralelo

### Limitações de Recursos
- Máximo 16 registradores (R0-R15)
- Pilha limitada a 256 entries
- Máximo 1024 rótulos por programa
- Máximo 65536 instruções por programa

### Validação
- Pitches MIDI devem estar no range 0-127
- Velocities devem estar no range 0-127  
- Durações devem ser positivas
- Rótulos devem ser únicos
- Referências de rótulos devem existir

---

**Nota**: Este conjunto de instruções garante Turing-completude através das operações `INC`, `DECJNZ`, `JMP` e manipulação de registradores, permitindo implementar qualquer algoritmo computável dentro do domínio musical.
