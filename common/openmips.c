#include "includes.h"


#define BAYES_REG0_ADDR   0xbff00000   // �?31位表示start，第30位表示valid
#define BAYES_REG1_ADDR   0xbff00004   // 分类结果
#define BAYES_REG2_ADDR   0xbff00008   // 接收从AXI传递的数据，表示第几幅图片
#define BAYES_REG3_ADDR   0xbff0000C   // 结果有效

#define LED_ADDR          0xbfd0f000
#define LED_RGB0_ADDR     0xbfd0f100
#define LED_RGB1_ADDR     0xbfd0f104
#define NUM_ADDR          0xbfd0f200
#define SWITCH_ADDR       0xbfd0f300

/*
#define BAYES_REG0   (* (volatile unsigned *)  BAYES_REG0_ADDR  )
#define BAYES_REG1   (* (volatile unsigned *)  BAYES_REG1_ADDR  )
#define BAYES_REG2   (* (volatile unsigned *)  BAYES_REG2_ADDR  )
#define BAYES_REG3   (* (volatile unsigned *)  BAYES_REG2_ADDR  )
*/
#define VGA_REG0_ADDR   0xbfb10000   // 1为开�?

//#define VGA_REG0   (* (volatile unsigned *)  VGA_REG0_ADDR  )


#define BOTH_EMPTY (UART_LS_TEMT | UART_LS_THRE)

#define WAIT_FOR_XMITR \
        do { \
                lsr = REG8(UART_BASE + UART_LS_REG); \
        } while ((lsr & BOTH_EMPTY) != BOTH_EMPTY)

#define WAIT_FOR_THRE \
        do { \
                lsr = REG8(UART_BASE + UART_LS_REG); \
        } while ((lsr & UART_LS_THRE) != UART_LS_THRE)

#define TASK_STK_SIZE 256
#define UART_BASE               0xbfd10000
#define UART_RX                 0x00000000	// UART RX
#define UART_RT                 0x00000004	// UART RT
#define UART_STAT               0x00000008	// UART status reg
#define UART_CTRL               0x0000000c	// UART control reg
OS_STK TaskStartStk[TASK_STK_SIZE];

// char Info[103]={0xC9,0xCF,0xB5,0xDB,0xCB,0xB5,0xD2,0xAA,0xD3,0xD0,0xB9,0xE2,0xA3,0xAC,0xD3,0xDA,0xCA,0xC7,0xBE,0xCD,0xD3,0xD0,0xC1,0xCB,0xB9,0xE2,0x0D,0x0A,0xC9,0xCF,0xB5,0xDB,0xCB,0xB5,0xD2,0xAA,0xD3,0xD0,0xCC,0xEC,0xBF,0xD5,0xA3,0xAC,0xD3,0xDA,0xCA,0xC7,0xBE,0xCD,0xD3,0xD0,0xC1,0xCB,0xCC,0xEC,0xBF,0xD5,0x0D,0x0A,0xC9,0xCF,0xB5,0xDB,0xCB,0xB5,0xD2,0xAA,0xD3,0xD0,0xC2,0xBD,0xB5,0xD8,0xBA,0xCD,0xBA,0xA3,0xD1,0xF3,0xA3,0xAC,0xD3,0xDA,0xCA,0xC7,0xBE,0xCD,0xD3,0xD0,0xC1,0xCB,0xC2,0xBD,0xB5,0xD8,0xBA,0xCD,0xBA,0xA3,0xD1,0xF3,0x0D};
//char Info[8] = {0xbb,0xd2,0xcb,0xb0,0xc5,0xbe,0xe5,0xce}; // 一八九�?
char Info[44] = {0x54,0x4a,0x55,0x20,0x77,0x69,0x6e,0x73,0x21,0x0d,
                0x0a,0x43,0x72,0x65,0x61,0x74,0x65,0x64,0x20,0x62,
                0x79,0x20,0x53,0x68,0x69,0x20,0x53,0x69,0x79,0x75,
                0x20,0x61,0x6e,0x64,0x20,0x43,0x68,0x75,0x20,0x58,
                0x75,0x2e,0x0d,0x0a}; 
                /*
                TJU wins!
                Created by Shi Siyu and Chu Xu.
                */
void uart_init(void)
{
	REG32(UART_BASE + UART_CTRL) =  (INT32U)0;

    uart_print_str("UART initialize done ! \r\n", 25);
	return;
}

void uart_putc(char c)
{
    INT32U StatusRegister = REG32(UART_BASE + UART_STAT);
	while ((StatusRegister & 0x08) == 0x08) {
		StatusRegister = REG32(UART_BASE + UART_STAT);
	}
	REG32(UART_BASE + UART_RT) = c;
}

void uart_print_str(char* str, int count)
{
    INT32U i = 0;
    OS_CPU_SR cpu_sr;
    OS_ENTER_CRITICAL()
    
    while(i < count)
    {
        uart_putc(str[i]);
        i++;
    }
    
    OS_EXIT_CRITICAL()
}

void gpio_init()
{
	REG32(LED_ADDR) = 0xffffffff;
	REG32(LED_RGB0_ADDR) = 0x1;
	REG32(LED_RGB1_ADDR) = 0x2;
	gpio_out(0x0f0f0f0f);
	uart_print_str("GPIO initialize done ! \n", 24);
        return;
}

void gpio_out(INT32U number)
{
    REG32(NUM_ADDR) = number;
}

INT32U gpio_in()
{
	INT32U temp = 0;
	temp = REG32(SWITCH_ADDR);
	return temp;
}

/*******************************************
********************************************/
void OSInitTick(void)
{
    INT32U compare = (INT32U)(IN_CLK / OS_TICKS_PER_SEC);
    
    asm volatile("mtc0   %0,$9"   : :"r"(0x0)); 
    asm volatile("mtc0   %0,$11"   : :"r"(compare));  
    asm volatile("mtc0   %0,$12"   : :"r"(0x10000401));
    
    return; 
}

/*
void wait(INT32U cnt)
{
   while (cnt)
   {
	   cnt--;  
   }
}
*/


void  TaskStart (void *pdata)
{	
    INT32U count = 0;
    pdata = pdata;          // Prevent compiler warning
    OSInitTick();	        // don't put this function in main()       
    for (;;) {
       if(count <= 43)
        {
            // REG32(UART_BASE + UART_RT) = Info[count];
            uart_putc(Info[count]);
            // uart_putc(Info[count+1]);
        }
        count=count+1;
        // gpio_out(count);
        // count=count+2;
        OSTimeDly(10);      // Wait 10ms
    }
    
}

void main()
{
    OSInit();

    // uart_init();

    // gpio_init();	

    // OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
	
	OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);

    OSStart();  
	return;
}
