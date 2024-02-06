// Written by Todd Doucet.

#include <io.h>
#include <signal.h>
#include <iomacros.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fll.h"



void sensor_setup()
{
    P1DIR = ~BIT1;  // Configure P1.1 (pin 13) as an input, rest as outputs.
    P1SEL = BIT1;   // That pin selected selected for timer_A.

    P2DIR = ~BIT2;  // Configure P2.2 (pin 22) as an input, rest as outputs.
    P2SEL = BIT2;   // That pin selected selected for timer_A.

    TACTL = TASSEL_2 + MC_2;   // timer_A: SMCLK + Continuous Mode

/*
    P2SEL = BIT0;   // So I can measure the watch xtal freq conveniently.
    P2DIR = BIT0;   
    BCSCTL1 &= ~(BIT4 | BIT5);  // Don't divide xtal for output at P2.0.
*/

}

/* We use the built-in uart for sending and receiving characters.
 * On the MS430F149, serial out is pin 32, and serial in is pin 33.
 */

void usart0_setup()
{
    UCTL0 = SWRST;  // reset the USART
    U0ME |= UTXE0 + URXE0;  // enable USART0 module

      // For usart0, we use P3.4 (pin 32) for transmit, and
      // P3.5 (pin 33) for receive.
    P3SEL |= BV(4) + BV(5); // set Port 3 pins for USART0

    UCTL0 |= CHAR;      // 8 bits, no parity, 1 stop
      // select the baudrate generator clock
    UTCTL0 |= SSEL1 + SSEL0; 
    UMCTL0 = 0;         // no modulation
    UBR10 = 0;
    UBR00 = 0xa0;       // 9600: divide by 160 for our freq
    URCTL0 = 0;         // init receiver contol register
    UCTL0 &= ~SWRST;    // enable usart
    U0IE |= URXIE0;     // enable receiver interrupts
}


    // Allow time for the watch crystal to become stable at startup.
void xtal_warmup() {
   volatile int i;  // Don't optimize away.
   for (i = 0; i < 30000; i++){
      nop();
   }
}


void hardware_setup()
{
    WDTCTL = WDTPW | WDTHOLD;   // Disable watchdog timer.

    xtal_warmup();


        // Set the frequency-locked loop to  the desired frequency,
        // using the 32k768 crystal as reference.
    fllInit();

    usart0_setup();
    sensor_setup();
    eint();         //enable interrupts
}

void serial_putc(char c)
{
    while ( !(IFG1 & UTXIFG0) )  // wait for TX buffer to empty
        continue;
    TXBUF0 = c;
}


#define putchar            serial_putc
#define printf(args...) uprintf(putchar, args)



   // Ring buffer for serial input.
#define CHARBUFSIZE 32
volatile char charbuf[CHARBUFSIZE];
volatile int firstch, nextch;


    // Called by isr to enqueue a character.
void recv_char(char rxdata)
{
   charbuf[nextch] = rxdata;
   nextch = (nextch + 1) % CHARBUFSIZE;
}


interrupt (UART0RX_VECTOR) wakeup usart0_recv_vec(void)
{
  volatile char dummy;

  if (URCTL0 & RXERR) {  // check status register for receive errors
      dummy = RXBUF0;    // clear error flags by forcing a dummy read
      return;
    }
    recv_char(RXBUF0);
}


char getchar()
{
    char r;

    while(1){
        if (nextch != firstch) {
            r = charbuf[firstch];
            firstch = (firstch + 1) % CHARBUFSIZE;
            return r;
        }

        LPM0;   // Sleep.  Awakened by irq.
    }
}

int is_eol(int c)
{
    return (c == '\r') || (c == '\n') ;
}



    // Get a line of input, with simple line editing.
void gets(char *buf, int bufsize)
{
    int pos = 0;
    char c;

    while ( !is_eol(c = getchar())  ){
        if ( (c == '\b') || (c == 127) ){
            if (pos > 0){
                pos--;
                putchar('\b');  // go back
                putchar(' ');   // erase on screen
                putchar('\b');  // go back              
            }
        } else if (pos + 1 < bufsize){
            putchar(c);       // echo it
            buf[pos++] = c;
        }
    }
    buf[pos] = 0;
    putchar('\r');  // echo crlf
    putchar('\n');  
    return;
}



    // Convert time ticks to microseconds, avoiding overflow.
int ticks_to_microsecs(int ticks)
{
    long int r = ticks;
    r *= 125;
    r /= 192;
    return r;
}


volatile int tstamp;  // Set by timerA_vec;

interrupt (TIMERA0_VECTOR) wakeup timerA_vec(void)
{
    tstamp = TACCR0;  // The timer_A clock value at the time of the capture.
}

/*
 * Wait for a positive edge transition on the specified input pin, then
 * return the timer counter that was present when that event occurred.
 */

int edge_trigger(int source)
{
            // rising edge, capture mode, interrupt
    TACCTL0 = CM_1 + SCS + source + CAP + CCIE; 
    LPM0;               // Sleep.  Awakened by edge transition interrupt.
    TACCTL0 &= ~CCIE;   // Disable further interrupts.

    return tstamp;
}

int edgeA_timestamp()
{
    return edge_trigger(CCIS_0);        // CCI0A (P1.1, pin 13)
}

int edgeB_timestamp()
{
    return edge_trigger(CCIS_1);        // CCI0B (P2.2, pin 22)
}

/*
 * Measure the time elapsed during an arbitrary interval.  The
 * functions func1() and func2() return the timestamp of the desired
 * event, and are supposed to use timer capture, like the above.  This
 * allows accurate timings and clear, nonduplicated code.
 */

int interval_time( int (*func1)(), int (*func2)() )
{
    int bgn, end;

    U0IE &= ~URXIE0;    // Disable uart interrupts during measurment.

    TAR = 0;  // So the timestamps do not overflow.
    bgn = (*func1)();
    end = (*func2)();

    U0IE |= URXIE0;     // Re-enable uart interrupts.

    return end - bgn;
}


// The implementations of the various user commands.

void cmd_time()
{
    int period = interval_time(&edgeA_timestamp, &edgeA_timestamp);
    printf("  period of A: %d ticks, %d microsecs\r\n", period, ticks_to_microsecs(period));
}


void cmd_timeAB()
{
    int elapsed = interval_time(&edgeA_timestamp, &edgeB_timestamp);
    printf("  A/B interval: %d ticks, %d microsecs\r\n", elapsed, ticks_to_microsecs(elapsed));
}


void cmd_freq()
{
    int ticks = interval_time(&edgeA_timestamp, &edgeA_timestamp);    
    int period = ticks_to_microsecs(ticks);
    long f = 1000000L / period;
    printf ("  frequency of A: %ld Hz\r\n", f);
}

void cmd_bullet()
{
    int elapsed = interval_time(&edgeA_timestamp, &edgeB_timestamp);
    int period = ticks_to_microsecs(elapsed);
    unsigned long mph = 2237414 / period;  // conversion factor computed elsewhere.
    printf ("  bullet speed: %ld mph\r\n", mph);    
}

void banner()
{
    printf("\r\nAE3K interval timer for MSP430\r\n");
    printf("    time -- measures and reports time elapsed between leading edges\r\n");
    printf("    freq -- frequency, determined from measuring one cycle\r\n");
    printf("    timeAB -- A/B interval timer, two different inputs\r\n");
    printf("    bullet -- A/B interval, interpreted as velocity across 1 meter\r\n");
}

int main(void)
{
    char buf[100];

    hardware_setup();

    banner();
    while(1){
        printf("\r\nCommand> ");
        gets(buf, sizeof(buf));
        if (!strcmp(buf, "time")){

            cmd_time();

        } else if (!strcmp(buf, "freq")){

            cmd_freq();

        } else  if (!strcmp(buf, "timeAB")){

            cmd_timeAB();

        } else  if (!strcmp(buf, "bullet")){

            cmd_bullet();

        } else if (!buf[0]){

            banner();

        } else {

            printf("unknown command: %s\r\n", buf);
            banner();

        }
    }
    printf("\r\n");
}


