void EnableInterrupts(void);
 void GPIOPortF_Handler(void);

#define GPIO_PORTF_DATA_R       (*((volatile signed long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile signed long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile signed long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile signed long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile signed long *)0x4002551C))
#define GPIO_PORTF_AMSEL_R      (*((volatile signed long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile signed long *)0x4002552C))
#define GPIO_PORTF_IS_R         (*((volatile unsigned long *)0x40025404))
#define GPIO_PORTF_IBE_R        (*((volatile unsigned long *)0x40025408))
#define GPIO_PORTF_IEV_R        (*((volatile unsigned long *)0x4002540C))
#define GPIO_PORTF_IM_R         (*((volatile unsigned long *)0x40025410))
#define GPIO_PORTF_ICR_R        (*((volatile unsigned long *)0x4002541C))
#define GPIO_PORTF_RIS_R        (*((volatile unsigned long *)0x40025414))
#define SYSCTL_RCGC2_R          (*((volatile signed long *)0x400FE108))

#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))
#define NVIC_PRI7_R             (*((volatile unsigned long *)0xE000E41C))
#define NVIC_ST_CTRL_R          (*((volatile unsigned long *)0xE000E010))			//Starts with E, this is an internal peripheral
#define NVIC_ST_RELOAD_R        (*((volatile unsigned long *)0xE000E014))
#define NVIC_ST_CURRENT_R       (*((volatile unsigned long *)0xE000E018))

#define SYSCTL_PRGPIO_R         (*((volatile unsigned long *)0x400FEA08))
#define SYSCTL_RCGCGPIO_R       (*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGC2_GPIOF      0x00000020  // port F Clock Gating Control



void systick_init()
{
	NVIC_ST_CTRL_R=0; //Disable systick
	NVIC_ST_RELOAD_R=0x00FFFFFF;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_CTRL_R=0x5;
}



void systic_waitms(unsigned int t)
{
	while((t--)!=0)
	{
		NVIC_ST_RELOAD_R=16000;
		NVIC_ST_CURRENT_R=0;
		while((NVIC_ST_CTRL_R&0x10000)==0);
	}
}


void init_portf()
{
	int d=3;
	SYSCTL_RCGCGPIO_R=0x20;
	//while((SYSCTL_PRGPIO_R&0x20)==0);
	while(d--);
	GPIO_PORTF_DIR_R|=0xE;
	GPIO_PORTF_AFSEL_R=0;
	GPIO_PORTF_AMSEL_R=0;
	GPIO_PORTF_DEN_R=0xE;
	GPIO_PORTF_DATA_R=0x8;
}
/*static inline void EnableInterrupts()
{
	asm("	CPSIE I/n" "	BX LR/n");
}*/
volatile unsigned int FallingEdges = 0;
void EdgeCounter_Init(void){
  //SYSCTL_RCGCGPIO_R |= 0x20; // (a) activate clock for port F
  FallingEdges = 0;
  // (b) initialize counter
  GPIO_PORTF_DIR_R &= ~0x10; // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10; // disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10; // enable digital I/O on PF4
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x10; // disable analog functionality on PF4
  GPIO_PORTF_PUR_R |= 0x10; // enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10; // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10; // PF4 is not both edges
  GPIO_PORTF_IEV_R &=~ 0x10; // PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;
  // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;
  // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000; // (h) enable interrupt 30 in NVIC
  EnableInterrupts();
  //__enable_irq();
  // (i) Program 9.3
}
void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;
  // acknowledge flag4
  FallingEdges = FallingEdges + 1;
	GPIO_PORTF_DATA_R &=~0x2;
		GPIO_PORTF_DATA_R |=0x4;
}

int main(void){
	init_portf();
	systick_init();
 // while(1){
		systic_waitms(1000);
		GPIO_PORTF_DATA_R=0x2;
	EdgeCounter_Init();
	 //GPIO_PORTF_RIS_R &=~0x10;
  //}
}
