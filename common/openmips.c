#include "includes.h"


#define BAYES_REG0_ADDR   0xbff00000   
#define BAYES_REG1_ADDR   0xbff00004   
#define BAYES_REG2_ADDR   0xbff00008   
#define BAYES_REG3_ADDR   0xbff0000C   

#define LED_ADDR          0xbfd0f000
#define LED_RGB0_ADDR     0xbfd0f100
#define LED_RGB1_ADDR     0xbfd0f104

#define TIMER_BASE        0xbfd30000
#define TIMER_TCSR0       0x0           // 计数器0的状态/控制寄存器
#define TIMER_TLR0        0x4           // 计数器0的加载寄存器
#define TIMER_TCR0        0x8           // 计数器0的计数寄存器
#define TIMER_TCSR1       0x10          // 计数器1的状态/控制寄存器
#define TIMER_TLR1        0x14          // 计数器1的加载寄存器
#define TIMER_TCR1        0x18          // 计数器1的计数寄存器

#define VGA_REG0_ADDR     0xbfd40000   



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
char Info[36] = {0x4f,0x53,0x20,0x52,0x75,0x6e,0x6e,0x69,0x6e,0x67,0x21,0x0d,0x0a,
                0x43,0x72,0x65,0x61,0x74,0x65,0x64,0x20,0x62,0x79,0x20,0x58,0x69,0x61,0x6f,0x20,0x4a,0x69,0x61,0x6e,0x2e,0x0d,0x0a}; 
/*OS Running!\r\nCreated by Xiao Jian.\r\n*/
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
    // REG32(NUM_ADDR) = number;
}

INT32U gpio_in()
{
	INT32U temp = 0;
	// temp = REG32(SWITCH_ADDR);
	return temp;
}

void timer_init()
{
    // SOC_TIMER_TLR0 = 0x7fffffff;
    // REG32(TIMER_BASE + TIMER_TCSR1) = 0;
    INT32U TCSR0 = REG32(TIMER_BASE + TIMER_TCSR0);
	TCSR0 |= 0x00000020; // 将某个控制/状态寄存器（TCSR0或TCSR1）中的LOAD位（第5位）置为1，把加载寄存器中的初始计数值存入到对应的内部计数器中
	REG32(TIMER_BASE + TIMER_TCSR0) = TCSR0;

	TCSR0 = REG32(TIMER_BASE + TIMER_TCSR0);
	// TCSR0 &= 0xffffffef; // 通过设置TCSR0或TCSR1中的ARHT位（第4位）来决定当定时器计时到期后，内部计数器是暂停还是重新加载初始计数值继续计数
	TCSR0 |= 0x00000010;
    REG32(TIMER_BASE + TIMER_TCSR0) = TCSR0;

	TCSR0 = REG32(TIMER_BASE + TIMER_TCSR0);
	TCSR0 |= 0x00000002; // 通过设置TCSR0或TCSR1中的UDT位（第1位），来决定计数器是递增计数还是递减计数
	REG32(TIMER_BASE + TIMER_TCSR0) = TCSR0;

	TCSR0 = REG32(TIMER_BASE + TIMER_TCSR0);
	// TCSR0 &= 0xffffffbf; // 通过设置TCSR0或TCSR1中的ENIT位（第6位）来决定是否使能中断
	TCSR0 |= 0x00000040;
    REG32(TIMER_BASE + TIMER_TCSR0) = TCSR0;
}

/*******************************************
********************************************/
void OSInitTick(void)
{
    INT32U compare = (INT32U)(IN_CLK / OS_TICKS_PER_SEC);
    
    // asm volatile("mtc0   %0,$9"   : :"r"(0x0)); 
    // asm volatile("mtc0   %0,$11"   : :"r"(compare));  
    asm volatile("mtc0   %0,$12"   : :"r"(0x10000401));

    REG32(TIMER_BASE + TIMER_TLR0) = compare;
    timer_init();
    INT32U TCSR0 = REG32(TIMER_BASE + TIMER_TCSR0);
	TCSR0 &= 0xffffffdf; 
	REG32(TIMER_BASE + TIMER_TCSR0) = TCSR0;

	TCSR0 = REG32(TIMER_BASE + TIMER_TCSR0);
	TCSR0 |= 0x00000080; 
	REG32(TIMER_BASE + TIMER_TCSR0) = TCSR0;
    // REG32(TIMER_BASE + TIMER_TCR0) = 0;
    // REG32(TIMER_BASE + TIMER_TCSR0) = 0x000000f2;
    
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
       if(count < 36)
        {
            // REG32(UART_BASE + UART_RT) = Info[count];
            uart_putc(Info[count]);
            // uart_putc(Info[count+1]);
        }
        count=count+1;
        // gpio_out(count);
        // count=count+2;
        OSTimeDly(2);      // Wait 10ms
    }
    
}

void main()
{
    OSInit();

    // uart_init();

    // gpio_init();	
	
	OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);

    OSStart();  
	return;
}
