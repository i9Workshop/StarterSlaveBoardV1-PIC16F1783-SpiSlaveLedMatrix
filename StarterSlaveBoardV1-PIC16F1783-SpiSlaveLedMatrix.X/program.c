#include "program.h"


// Delay x1.5us
void delay_x1o5us(uint8_t delay) {
    for(uint8_t i=0; i<delay; i++) NOP();
}

// Delay x24.25us
void delay_x24o25us(uint16_t delay) {
    for(uint16_t i=0; i<delay; i++) delay_x1o5us(15);
}

// Delay x1ms
void delay_ms(uint32_t delay) {
    for(uint32_t i=0; i<delay; i++) delay_x24o25us(41);
}


uint8_t spi_Data = 0; // Variable to store received data


void programInitialize(void) {
    TRISA = 0;
    ANSELA = 0;
    LATA = 0;
    
    // SCk pin
    TRISCbits.TRISC3 = 1;
    APFCONbits.SCKSEL = 0;
    
    // SDI pin
    TRISCbits.TRISC4 = 1;
    APFCONbits.SDISEL = 0;
    
    // SDO pin
    TRISCbits.TRISC5 = 0;
    APFCONbits.SDOSEL = 0;
    
    // SS pin
    TRISCbits.TRISC2 = 1;
    
    spi_Initialize();
}

void programLoop(void) {
    ledMatrix_SetDisplay(spi_Data); // Display LEDs matrix from variable spi_Data
}


void ledMatrix_DelayScanInput(uint16_t delay) {
    for(uint16_t i=0; i<delay; i++) {
        spi_ScanMaster(); // Call SPI polling function
    }
}

void ledMatrix_AllOff(void) { // Turn off all LEDs
    led_VccColumn1 = 0;
    led_VccColumn2 = 0;
    led_VccColumn3 = 0;
    led_GndRow1 = 0;
    led_GndRow2 = 0;
    led_GndRow3 = 0;
}

void ledMatrix_SetDisplay(uint16_t ledWord) { // Set LED matrix using 9bit binary word
    uint16_t delay = 1000;
    
    // First row
    
    ledMatrix_AllOff();
    
    led_VccColumn1 = (bool)(ledWord & 0b100000000);
    led_VccColumn2 = (bool)(ledWord & 0b010000000);
    led_VccColumn3 = (bool)(ledWord & 0b001000000);
    led_GndRow1 = 0;
    led_GndRow2 = 1;
    led_GndRow3 = 1;
    
    ledMatrix_DelayScanInput(delay);
    
    // Second row
    
    ledMatrix_AllOff();
    
    led_VccColumn1 = (bool)(ledWord & 0b000100000);
    led_VccColumn2 = (bool)(ledWord & 0b000010000);
    led_VccColumn3 = (bool)(ledWord & 0b000001000);
    led_GndRow1 = 1;
    led_GndRow2 = 0;
    led_GndRow3 = 1;
    
    ledMatrix_DelayScanInput(delay);
    
    // Third row
    
    ledMatrix_AllOff();
    
    led_VccColumn1 = (bool)(ledWord & 0b000000100);
    led_VccColumn2 = (bool)(ledWord & 0b000000010);
    led_VccColumn3 = (bool)(ledWord & 0b000000001);
    led_GndRow1 = 1;
    led_GndRow2 = 1;
    led_GndRow3 = 0;
    
    ledMatrix_DelayScanInput(delay);
}

void spi_Initialize(void) {
    SSPCON1bits.SSPM = 5; // Set MSSP module for SPI slave mode - Page 306
	
    SSPCON1bits.SSPEN = 1; // Set enable MSSP module - Page 306
    
    delay_x1o5us(2); // Wait for MSSP module configuration - Page 373 from I/O pin timing
                     // Tioz = 2us
}

void spi_ScanMaster(void) {
    // Single byte data transmit and receive - Page 263
    if(!spi_SS) { // Polling to wait for master communication
        spi_Data = SSPBUF; // Read data from MSSP module buffer - Page 266
        
        NOP();
        
        SSPBUF = spi_Data; // Transmit data received back to master
        
        while(!spi_SS); // Wait for master to release slave device
    }
}
