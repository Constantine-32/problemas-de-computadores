/*************************************************************
*   Problema: Problema de Examen (1 Conv.): boca artificial
*   Autor: Christian Callau Romero
*   Data: Gener 2018               Version: 1.0
*************************************************************/

#define MAX_CAR 16
#define MAX_FON 50

char palabra[MAX_CAR + 1];      //palabra a vocalizar
unsigned int fonemas[MAX_FON];  //fonemas a vocalizar
unsigned char fcs[MAX_FON];     //tiempo de cada fonema, en centésimas
unsigned char num_fon = 0;      //número de fonemas a vocalizar

unsigned char cur_fon = 0;      //fonema actual
unsigned char cur_fcs = 0;      //centesima actual

void main()
{
  inicializaciones();
  desactivar_timer0();
  do
  {
    tareas_independientes();
    if (num_fon == 0)
    {
      if (siguiente_palabra(palabra))
      {
        num_fon = pal2fon(palabra, fonemas, fcs);
        activar_timer0(100);
        swiWaitForVBlank();
        printf("%s\n", palabra);
      }
    }
  } while(1);
}


// RSI del timer 0 en C:
void RSI_timer0()
{
  if (cur_fcs == 0)
  {
    if (cur_fon == num_fon)
    {
      REG_MOUTH = 0;
      num_fon = 0;
      cur_fon = 0;
      desactivar_timer0();
      return;
    }
    REG_MOUTH = fonemas[cur_fon] | 0x80000000;
    cur_fcs == fcs[cur_fon];
    cur_fon++;
  }
  cur_fcs--;
}


@; RSI del timer 0 en ARM:
RSI_timer0:
    push {r0-r6, lr}
    ldr r0, =cur_fcs
    ldrb r1, [r0]
    cmp r1, #0
    bne .Lrsi_t0_if_1
    ldr r2, =cur_fon
    ldrb r3, [r2]
    ldr r4, =num_fon
    ldrb r5, [r4]
    cmp r3, r5
    ldr r6, =REG_MOUTH
    bne .Lrsi_t0_if_2
    ldr r4, =fonemas
    ldrb r5, [r4, r3]
    orr r5, #0x80000000
    str r5, [r6]
    ldr r4, =fcs
    ldrb r1, [r4, r3]
    add r3, #1
    strb r3, [r2]
    b .Lrsi_t0_if_1
.Lrsi_t0_if_2:
    mov r5, #0
    str r5, [r6]
    strb r5, [r4]
    strb r5, [r2]
    bl desactivar_timer0
    b .Lrsi_t0_return
.Lrsi_t0_if_1:
    sub r1, #1
    strb r1, [r0]
.Lrsi_t0_return:
    pop {r0-r6, pc}


// Funcion siguiente_palabra() en C:
int siguiente_palabra(char *string)
{
  if (REG_IPC_FIFO_CR & 0x100) return 0;
  int i = 0;
  do
  {
    palabra[i] = REG_IPC_FIFO_RX;
    i++;
  } while (!(REG_IPC_FIFO_CR & 0x100));
  palabra[i] = '\0';
  return i;
}


@; Funcion siguiente_palabra() en ARM:
@; Parametros:
@;  R0: char *string
@; Return:
@;  R0: string length
siguiente_palabra:
    push {r1-r5, lr}
    mov r1, #0            @; Valor de retorno
    ldr r2, =REG_IPC_FIFO_CR
    ldr r3, [r2]
    tst r3, #0x100
    bne .Lsp_return
    ldr r4, =REG_IPC_FIFO_RX
.Lsp_while:
    ldr r5, [r4]
    strb r5, [r0, r1]
    add r1, #1
    ldr r3, [r2]
    tst r3, #0x100
    bne .Lsp_while
    mov r5, #'\0'
    strb r5, [r0, r1]
.Lsp_return:
    mov r0, r1
    pop {r1-r5, pc}
