/**
 \file
*/
#ifndef SPI_MASTER_C_HEADER_FILE
#define SPI_MASTER_C_HEADER_FILE

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void spi_start(){reg_spimaster_control = reg_spimaster_control | 0x1;}
void spi_stop(){reg_spimaster_control = reg_spimaster_control & 0x2;}
char spi_busy(){return reg_spimaster_status & 0x2;}
char spi_done(){return reg_spimaster_status & 0x1;}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/**
 * Write byte (8bits) through SPI master 
 * 
 * @param c byte to write range 0x0 to 0xFF
 * 
 */
void MSPI_write(char c){
    reg_spimaster_wdata = (unsigned long) c;
    #ifndef ARM
    reg_spimaster_control = 0x0801;
    #else
    spi_start();
    spi_stop();
    while(spi_busy());
    #endif
}
/**
 * Read byte (8bits) through SPI master 
 * 
 */
char MSPI_read(){
    MSPI_write(0x00);
    while (reg_spimaster_status != 1);
    return reg_spimaster_rdata;
}
/**
 * Enable or disable the master SPI
 *  
 * @param is_enable when 1 (true) master SPI is active, 0 (false) master SPI is disabled
 */
void MSPI_enable(bool is_enable){
    if(is_enable){
        #ifndef ARM
        reg_spi_enable = 1;
        #else
        reg_wb_enable = reg_wb_enable | 0x20;
        #endif
    }else{
        #ifndef ARM
        reg_spi_enable = 0;
        #else
        reg_wb_enable = reg_wb_enable & 0xFFDF;
        #endif  
    }
}
/**
 * assert or deassert chip select
 *  
 * @param is_enable when 1 (true) chip select is asserted, 0 (false) chip select is deasserted
 */
void MSPI_enableCS(bool is_enable){
    if (is_enable){
        #ifndef ARM
        reg_spimaster_cs = 0x10001; // select chip 0
        #else
        reg_spimaster_control = reg_spimaster_control | 0x2; //bit 1
        #endif
    }else{
        #ifndef ARM
        reg_spimaster_cs = 0;
        #else
        reg_spimaster_control = reg_spimaster_control & 0x1; //bit 1
        spi_stop();
        #endif
    }    
}

#endif // SPI_MASTER_C_HEADER_FILE
