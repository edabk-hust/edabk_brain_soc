/**
 \file
*/
#ifndef UART_API_C_HEADER_FILE
#define UART_API_C_HEADER_FILE

#ifndef DOXYGEN_SHOULD_SKIP_THIS
void arm_mgmt_uart_enable(){reg_wb_enable = reg_wb_enable | 0x40;}
void arm_mgmt_uart_disable(){reg_wb_enable = reg_wb_enable  & 0xFFBF;}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
// UART 
/**
 * Enable or disable TX of UART
 * 
 *  
 * @param is_enable when 1(true) UART TX enable, 0 (false) UART TX disable
 * 
 * \note
 * Some caravel CPU enable and disable UART TX and RX together
 * 
 */
void UART_enableTX(bool is_enable){
    if (is_enable){
        #ifdef ARM 
        // 0x08 RW    CTRL[3:0]   TxIntEn, RxIntEn, TxEn, RxEn
        //              [6] High speed test mode Enable
        //              [5] RX overrun interrupt enable
        //              [4] TX overrun interrupt enable
        //              [3] RX Interrupt Enable
        //              [2] TX Interrupt Enable
        //              [1] RX Enable
        //              [0] TX Enable
        arm_mgmt_uart_enable();
        reg_uart_ctrl = reg_uart_ctrl | 0x1;
        reg_uart_clkdiv=0x3C0; // set default to 9600
        #else
        reg_uart_enable = 1;
        #endif
    }else{
        #ifdef ARM 
        arm_mgmt_uart_disable();
        reg_uart_ctrl = reg_uart_ctrl & 0xFFFFE;
        reg_uart_clkdiv=0x3C0; // set default to 9600
        #else
        reg_uart_enable = 0;
        #endif  
    }

} 
// UART 
/**
 * Enable or disable RX of UART
 * 
 *  
 * @param is_enable when 1(true) UART RX enable, 0 (false) UART RX disable
 * 
 * \note
 * Some caravel CPU enable and disable UART TX and RX together
 * 
 */
void UART_enableRX(bool is_enable){
    if (is_enable){
       #ifdef ARM 
        arm_mgmt_uart_enable();
        reg_uart_ctrl = reg_uart_ctrl | 0x2;
        reg_uart_clkdiv=0x3C0; // set default to 9600
        #else
        reg_uart_enable = 1;
        #endif
    }else{
        #ifdef ARM 
        arm_mgmt_uart_disable();
        reg_uart_ctrl = reg_uart_ctrl & 0xFFFFD;
        reg_uart_clkdiv=0x3C0; // set default to 9600
        #else
        reg_uart_enable = 0;
        #endif

    }
}
/**
 * Wait receiving ASCII symbol and return it. 
 * 
 * Return the first ASCII symbol of the UART received queue
 * 
 * RX mode have to be enabled
 * 
 */
char UART_readChar(){
    #ifdef ARM 
    while ((reg_uart_stat &2) == 0); // RX is empty
    #else 
    while (uart_rxempty_read() == 1);
    #endif
    return reg_uart_data;
}
/**
 * Pop the first ASCII symbol of the UART received queue
 * 
 * UART_readChar() function would keeping reading the same symbol unless this function is called
 */
void UART_popChar(){
    #ifndef ARM
    uart_ev_pending_write(UART_EV_RX);
    #endif
    return;
}

/**
 * read full line msg and return it
 * 
 */
char* UART_readLine(){
    char* received_array =0;
    char received_char;
    int count = 0;
    while ((received_char = UART_readChar()) != '\n'){
        received_array[count++] = received_char;
        UART_popChar();
    }
    received_array[count++] = received_char;
    UART_popChar();
    return received_array;
}


#ifdef DOXYGEN_DOCS_ONLY
/**
 * Send ASCII symbol or symbols through UART 
 * 
 * TX mode have to be enabled
 */
void print(const char *p){}
// real function defined at caravel repo 
#endif // DOXYGEN_DOCS_ONLY
/**
 * Send ASCII char through UART
 * @param c ASCII char to send
 * 
 * TX mode have to be enabled
 */
void UART_sendChar(char c){
    while (reg_uart_txfull == 1);
	reg_uart_data = c;
}

/**
 * Send int through UART 
 * the int would be sent as 8 hex characters
 * @param c int to send
 * 
 * TX mode have to be enabled
 */
void UART_sendInt(int data){
 for (int i = 0; i < 8; i++) {
        // Extract the current 4-bit chunk
        int chunk = (data >> (i * 4)); 
        if (chunk == 0) {
            break;
        }
        chunk = chunk & 0x0F;
        char ch; 
        if (chunk >= 0 && chunk <= 9) {
            ch = '0' + chunk;  // Convert to corresponding decimal digit character
        } else {
            ch = 'A' + (chunk - 10);  // Convert to corresponding hex character A-F
        }
        UART_sendChar(ch);
    }
    UART_sendChar('\n');
}
#endif // UART_API_C_HEADER_FILE
