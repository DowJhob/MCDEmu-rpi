/* PI_SPI.c
 Yann Guidon whygee@f-cpu.org
 based on code from G.J. van Loo 15-Jan-2012
 20130508 : init
 20140904
 20140907 : stderr
 20161109 : HaD release

 Don't include this file directly.
 Include PI_SPI_config.h instead.
*/

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

#include "PI_GPIO.h"
#include "PI_SPI.h"

volatile unsigned *PI_spi0 = NULL;



// From Gert's example :
#define SPI0_CNTLSTAT *(PI_spi0 + 0)
#define SPI0_FIFO     *(PI_spi0 + 1)
#define SPI0_CLKSPEED *(PI_spi0 + 2)


// Backup of the expected controller state
int SPI_CTL_normal;

#ifndef GPIO_NO_ATEXIT
// parachute :
void parachute_SPI() {
  fputs("\nfermeture SPI", stderr);
  SPI0_CNTLSTAT = SPI_CTL_normal
                | SPI0_CS_CLRFIFOS
                | SPI0_CS_DONE;
}
#endif

void PI_setup_SPI(int pins, int speed, int spi_ctl) {

  // only enable the pins that you actually use:
  int pin_nr=7, pin_mask=1<<7;
  while(pins) {
    if(pins & pin_mask){
      PI_GPIO_config(pin_nr,BCM_GPIO_ALT0);
      pins -= pin_mask;
    }
    pin_mask+=pin_mask;
    pin_nr++;
  }

  // requests access to the SPI controller
  PI_spi0 = PI_IOmmap(SPI0_BASE);

  // speed = 250MHz/desired speed
  SPI0_CLKSPEED = speed;
  SPI0_CNTLSTAT = SPI0_CS_CLRFIFOS
                | SPI0_CS_DONE;

  SPI0_CNTLSTAT
    = SPI_CTL_normal
    = spi_ctl;
#ifndef GPIO_NO_ATEXIT
  atexit(parachute_SPI);
#endif
}

int SPI_status;
char SPI_dummy;

void SPI_wait_done(){
  // wait for SPI to be done and empty the FIFO
  do {
    SPI_status = SPI0_CNTLSTAT;
    if (SPI_status & SPI0_CS_RXFIFODATA)
      SPI_dummy = SPI0_FIFO; // Flush the read FIFO
  } while (( SPI_status & SPI0_CS_DONE) == 0);
}

void SPI_wait_send(char dat){
  // wait for SPI to be ready then send another byte
  do {
    SPI_status = SPI0_CNTLSTAT;
    if (SPI_status & SPI0_CS_RXFIFODATA)
      SPI_dummy = SPI0_FIFO; // Flush the read FIFO
  } while ((SPI_status & SPI0_CS_TXFIFOSPCE ) == 0);
  SPI0_FIFO = dat; // send the byte
}

// Send and receive a byte at the same time
unsigned int SPI_transmit_byte(unsigned int u) {
  SPI0_FIFO = u;  // send the byte
  // Wait until transmission is done:
  do {
    SPI_status = SPI0_CNTLSTAT;
  } while (( SPI_status & SPI0_CS_DONE) == 0);
  return SPI0_FIFO;  // read the data FIFO
}

