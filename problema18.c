/*************************************************************
*   Problema 18: Luz LED regulada por PWM
*   Autor: Christian Callau Romero
*   Fecha: Enero 2018               Versión: 1.0
*************************************************************/

#define NUM_PATTERNS = 5        // número de patrones de brillo
#define NUM_VALUES = 16         // número valores de brillo por patrón

unsigned char patterns[NUM_PATTERNS][NUM_VALUES] = {  //patrones
  {100,100,100,100,100,100,100,100,0,0,0,0,0,0,0,0},  //cuadrado
  {100,88,75,63,50,38,25,13,0,13,25,38,50,63,75,88},  //triangular
  {0,6,13,19,25,31,37,44,50,56,63,69,75,81,88,94},    //rampa
  {100,96,85,69,50,31,15,4,0,4,15,31,50,69,85,96},    //coseno
  {0,50,80,100,70,40,21,15,37,48,25,13,8,4,2,1}       //latido
};
unsigned char curr_pattern = 0; // índice del patrón actual
unsigned char curr_value = 0;   // índice del valor actual del pat.
unsigned char brightness = 0;   // valor de brillo actual
unsigned char brig_counter = 0; // numero de activaciones del timer0

void main()
{
  inicializaciones();
  do
  {
    tareas_independientes();
    scanKeys();
    if (keysDown() & KEY_A)
    {
      if (curr_pattern < (NUM_PATTERNS - 1)) curr_pattern++;
      else curr_pattern = 0;
      swiWaitForVBlank();
      printf("patrón: %d\n", curr_pattern);
    }
  } while (1);
}


// RSI del timer0 en C.
void RSI_timer0()
{
  unsigned short *pointer = REG_DOUT;
  if (brig_counter < brightness) *pointer |= 0x20;  // bit 5 de REG_DOUT a 1.
  else *pointer &= 0xFFDF;  // bit 5 de REG_DOUT a 0.
  brig_counter++;
  if (brig_counter == 100) brig_counter = 0;
}


@; RSI del timer0 en ARM.
RSI_timer0:
    push {r0-r4, lr}
    ldr r0, =brig_counter
    ldrb r1, [r0]
    ldr r2, =brightness
    ldrb r3, [r2]
    ldr r2, =REG_DOUT
    ldrh r4, [r2]
    cmp r1, r3
    orrlo r4, 0x20
    bichs r4, 0x20
    strh r4, [r2]
    add r1, #1
    cmp r1, #100
    moveq r1, #0
    strb r1, [r0]
    pop {r0-r4, pc}


// RSI del timer1 en C.
void RSI_timer1()
{
  brightness = patterns[curr_pattern][curr_value];
  curr_value++;
  if (curr_value == NUM_VALUES) curr_value = 0;
}


@; RSI del timer1 en ARM.
RSI_timer1:
    push {r0-r3, lr}
    ldr r0, =curr_pattern
    ldrb r1, [r0]
    ldr r2, =curr_value
    ldrb r3, [r2]
    mov r0, #NUM_VALUES
    mla r1, r1, r0, r3
    ldr r0, =patterns
    ldrb r0, [r0, r1]
    ldr r1, =brightness
    strb r0, [r1]
    add r3, #1
    cmp r3, #NUM_VALUES
    moveq r3, #0
    strb r3, [r2]
    pop {r0-r3, pc}
