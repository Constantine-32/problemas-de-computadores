/*************************************************************
*   Problema 15: Display de LEDs (Ex. 2ª Conv. 2014-15)
*   Autor: Christian Callau Romero
*   Fecha: Enero 2018               Versión: 1.0
*************************************************************/

char string[] = "C'est la vie"
char currentChar;
char canviar_char = 0;
char num_col = 0;
char state = 0;

int main()
{
  int i = 0;
  currentchar = string[i++];
  inicializaciones();
  swiWaitForVBlank();
  printf("%s\n", string);
  do
  {
    tareas_independientes();
    if (canviar_char != 0)
    {
      canviar_char = 0;
      currentChar = string[i++];
      if (currentChar == '\0')
      {
        i = 0;
        currentChar = string[i++];
      }
    }
  } while (1);
}


@; RSI del timer0 en ARM.
RSI_timer0:
    push {r0-r6, lr}
    ldr r1, =currentChar
    ldrb r0, [r1]
    ldr r2, =num_col
    ldrb r1, [r2]
    bl obtener_puntos
    ldr r4, =state
    ldrb r3, [r4]
    cmp r3, #0
    beq .Lset_to_1
    and r0, 0x7f
    mov r3, #0
    b .Lfin
.Lset_to_1:
    orr r0, 0x80
    mov r3, #1
    ldr r6, =canviar_char
    ldrb r5, [r6]
    add r1, #1
    cmp r1, #6
    moveq r1, #0
    moveq r5, #1
    strb r1, [r2]
    strb r5, [r6]
.Lfin:
    ldr r1, =REG_DISP
    strb r0, [r1]
    strb r3, [r4]
    pop {r0-r6, pc}


@; Funcion obtener_puntos() en ARM.
@; Parámetros:
@;  R0 = char caracter
@;  R1 = char num_columna
@; Resultado:
@;  R0 = char puntos
obtener_puntos:
    push {r1-r2, lr}
    sub r0, #32
    mov r2, #6
    mla r2, r0, r2, r1
    ldr r1, =base_ASCII
    ldrb r0, [r1, r2]
    pop {r1-r2, pc}
