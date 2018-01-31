/*************************************************************
*   Problema 21: Emisor IR (Ex. 2ª Conv. 2016-17)
*   Autor: Christian Callau Romero
*   Fecha: Enero 2018               Versión: 1.0
*************************************************************/

unsigned short VCodes[10][34] =
{{0x0F07, 0x0101, 0x0101, 0x0101, 0x0104, 0x0101, …, 0x013B},
 {0x0F07, 0x0101, 0x0101, 0x0104, 0x0101, 0x0104, …, 0x013B},
 …
 {0x0F07, 0x0101, 0x0104, 0x0101, 0x0101, 0x0101, …, 0x013B}};
unsigned char current_code; // índice código actual
unsigned char current_pair; // índice par ON/OFF actual
unsigned char state; // estado actual (1=ON, 0=OFF)
unsigned char tics; // tics pendientes

void main()
{
  inicializaciones();
  desactivar_timer0();
  do
  {
    swiWaitForVBlank(); // relajar bucle principal
    // condicionando la frequencia del bucle principal
    // a 60 Hz (0.1666 seg) dando tiempo de sobra a una
    // sequencia (0.1141 seg) a transmitirse, por tanto
    // no hay que preocuparse por posibles solapaciones
    // de sequencias.
    scanKeys();
    key = keysDown();
    if (key & 0x3FF)
    {
      current_code = 0;
      while (((key >> current_code) & 1) != 1) current_code++;
      current_pair = -1;
      state = 0;
      tics = 0;
      activar_timer0(1700);
      printf("send code %d\n", current_code);
    }
  } while (1);
}


// RSI del timer0 (1.700 Hz) en C.
void RSI_timer0()
{
  if (tics <= 0)
  {
    state ^= 1; // "flip" el state 1 → 0, 0 → 1. 
    current_pair += state;  // si state pasa a 1, current_pair++;
    if (current_pair == 34)
    {
      desactivar_timer0();
      return;
    }
    tics = obtener_tics(VCodes, current_code, current_pair, state);
    REG_IR = state;
    return;
  }
  tics--;
}


@; RSI del timer0 (1.700 Hz) en ARM.
RSI_timer0:
    push {r0-r5, lr}
    ldr r5, =tics
    ldrb r4, [r5]
    cmp r4, #0
    bhi .Lrsi_t0_if1
    ldr r0, =state
    ldrb r3, [r0]
    eor r3, #1
    strb r3, [r0]
    ldr r0, =current_pair
    ldrb r2, [r0]
    add r2, r3
    cmp r2, #34
    bleq desactivar_timer0
    cmp r2, #34
    beq .Lrsi_t0_return
    strb r2, [r0]
    ldr r0, =current_code
    ldrb r1, [r0]
    ldr r0, =VCodes
    bl obtener_tics
    mov r4, r0
    ldr r0, =REG_IR
    str r3, [r0]
    b .Lrsi_t0_return
.Lrsi_t0_if1:
    sub r4, #1
    strb r4, [r5]
.Lrsi_t0_return:
    pop {r0-r5, pc}


// Funcion obtener_tics() en C.
unsigned char obtener_tics(unsigned short codes[][34],
  unsigned char ccode,
  unsigned char cpair,
  unsigned char cstate)
{
  return (unsigned char)(codes[ccode][cpair] >> (cstate * 8));
}


@; Funcion obtener_tics() en ARM.
@; Parámetros:
@;  R0 = unsigned short codes[][34]
@;  R1 = unsigned char ccode
@;  R2 = unsigned char cpair
@;  R3 = unsigned char cstate
@; Resultado:
@;  R0 = unsigned char tics
obtener_tics:
    push {r1-r4, lr}
    ldrb r1, [r1]
    ldrb r2, [r2]
    mov r4, #34
    mla r4, r1, r4, r2
    ldrh r4, [r0, r4 lsl #1]
    ldrb r3, [r3]
    mov r3, r3 lsl #3
    mov r0, r4 lsr r3
    and r0, r0, #0xFF
    pop {r1-r4, pc}
