/**
 \file
*/
#ifndef GPIO_C_HEADER_FILE
#define GPIO_C_HEADER_FILE
/**
 * Configure all GPIOs with the config
 *  
 * @param config is configuration of type gpio_mode
 * 
 * \note
 * These configurations will not be change the GPIOs modes until calling GPIOs_loadConfigs()
 * 
 */
void GPIOs_configureAll(enum gpio_mode config){
    #ifndef ARM
    reg_mprj_io_37 = config;
    reg_mprj_io_36 = config;
    reg_mprj_io_35 = config;
    #endif
    reg_mprj_io_34 = config;
    reg_mprj_io_33 = config;
    reg_mprj_io_32 = config;
    reg_mprj_io_31 = config;
    reg_mprj_io_30 = config;
    reg_mprj_io_29 = config;
    reg_mprj_io_28 = config;
    reg_mprj_io_27 = config;
    reg_mprj_io_26 = config;
    reg_mprj_io_25 = config;
    reg_mprj_io_24 = config;
    reg_mprj_io_23 = config;
    reg_mprj_io_22 = config;
    reg_mprj_io_21 = config;
    reg_mprj_io_20 = config;
    reg_mprj_io_19 = config;
    reg_mprj_io_18 = config;
    reg_mprj_io_17 = config;
    reg_mprj_io_16 = config;
    reg_mprj_io_15 = config;
    reg_mprj_io_14 = config;
    reg_mprj_io_13 = config;
    reg_mprj_io_12 = config;
    reg_mprj_io_11 = config;
    reg_mprj_io_10 = config;
    reg_mprj_io_9  = config;
    reg_mprj_io_8  = config;
    reg_mprj_io_7  = config;
    reg_mprj_io_6  = config;
    reg_mprj_io_5  = config;
    reg_mprj_io_4  = config;
    reg_mprj_io_3  = config;
    reg_mprj_io_2  = config;
    reg_mprj_io_1  = config;
    reg_mprj_io_0  = config;
    // GPIOs_loadConfigs();
}
/**
 * Load the configurations changes to the GPIOs 
 *  
 */
void GPIOs_loadConfigs(){
    reg_mprj_xfer = 1;
    while ((reg_mprj_xfer&0x1) == 1);

}
/**
 * Configure one GPIO with the input config
 *  
 * @param config is configuration of type gpio_mode
 * @param gpio_num is GPIO number it can have values from 0 to 37
 * 
 * \note
 * These configurations will not be change the GPIOs modes until calling GPIOs_loadConfigs()
 * 
 */
void GPIOs_configure(int gpio_num,enum gpio_mode config){
    switch(gpio_num){
        case 0 :
            reg_mprj_io_0   = config; break;
        case 1 :
            reg_mprj_io_1   = config; break;
        case 2 :
            reg_mprj_io_2   = config; break;
        case 3 :
            reg_mprj_io_3   = config; break;
        case 4 :
            reg_mprj_io_4   = config; break;
        case 5 :
            reg_mprj_io_5   = config; break;
        case 6 :
            reg_mprj_io_6   = config; break;
        case 7 :
            reg_mprj_io_7   = config; break;
        case 8 :
            reg_mprj_io_8   = config; break;
        case 9 :
            reg_mprj_io_9   = config; break;
        case 10:
            reg_mprj_io_10  = config; break;
        case 11:
            reg_mprj_io_11  = config; break;
        case 12:
            reg_mprj_io_12  = config; break;
        case 13:
            reg_mprj_io_13  = config; break;
        case 14:
            reg_mprj_io_14  = config; break;
        case 15:
            reg_mprj_io_15  = config; break;
        case 16:
            reg_mprj_io_16  = config; break;
        case 17:
            reg_mprj_io_17  = config; break;
        case 18:
            reg_mprj_io_18  = config; break;
        case 19:
            reg_mprj_io_19  = config; break;
        case 20:
            reg_mprj_io_20  = config; break;
        case 21:
            reg_mprj_io_21  = config; break;
        case 22:
            reg_mprj_io_22  = config; break;
        case 23:
            reg_mprj_io_23  = config; break;
        case 24:
            reg_mprj_io_24  = config; break;
        case 25:
            reg_mprj_io_25  = config; break;
        case 26:
            reg_mprj_io_26  = config; break;
        case 27:
            reg_mprj_io_27  = config; break;
        case 28:
            reg_mprj_io_28  = config; break;
        case 29:
            reg_mprj_io_29  = config; break;
        case 30:
            reg_mprj_io_30  = config; break;
        case 31:
            reg_mprj_io_31  = config; break;
        case 32:
            reg_mprj_io_32  = config; break;
        case 33:
            reg_mprj_io_33  = config; break;
        case 34:
            reg_mprj_io_34  = config; break;
        case 35:
            reg_mprj_io_35  = config; break;
        case 36:
            reg_mprj_io_36  = config; break;
        case 37:
            reg_mprj_io_37  = config; break;
        default:
            break;
    }
}

/**
 * Write to the low 32 GPIOs GPIOS[31:0]
 * \note
 * For writing by this function to be seen at the GPIO the GPIO has to be configured as management output
 *  
 * @param data is the data sent to the GPIOs 
 * 
 * Examples: 
 * \li \code GPIOs_writeLow(0x1); // write 1 to GPIO [0] and write 0 in the remaining 31 GPIOs \endcode
 * \li \code GPIOs_writeLow(0x5); // write 1 to GPIO [0] and GPIO [3] and write 0 in the remaining 30 GPIOs\endcode
 */
void GPIOs_writeLow(unsigned int data){reg_mprj_datal = data;}
/**
 * Write to the highest 6 GPIOs GPIOS[37:32]
 * \note
 * For writing by this function to be seen at the GPIO the GPIO has to be configured as management output
 *  
 * @param data is the data sent to the GPIOs 
 * 
 * Examples: 
 * \li \code GPIOs_writeHigh(0x1); // write 1 to GPIO [32] and write 0 in the remaining 5 GPIOs\endcode
 * \li \code GPIOs_writeHigh(0x5); // write 1 to GPIO [32] and 34 and write 0 in the remaining 4 GPIOs\endcode
 */
void GPIOs_writeHigh(unsigned int data){reg_mprj_datah = data;}
/**
 * Write to the 38 GPIOs GPIOS[37:0]
 * \note
 * For writing by this function to be seen at the GPIO the GPIO has to be configured as management output
 *  
 * @param data is the data sent to the GPIOs 
 * 
 * Examples: 
 * \li \code GPIOs_writeLowHigh(0x1); // write 1 to GPIO [0] and write 0 in the remaining 37 GPIOs \endcode
 * \li \code GPIOs_writeLowHigh(0x5); // write 1 to GPIO [0] and GPIO [3] and write 0 in the remaining 36 GPIOs\endcode
 * \li \code GPIOs_writeLowHigh(0x100000000); // write 1 to GPIO [32] and write 0 in the remaining 36 GPIOs\endcode
 * 
 * \todo verify this function
 */
void GPIOs_writeLowHigh(long data){
    reg_mprj_datal = data; 
    reg_mprj_datah = data <<32; 

}
/**
 * Read the highest 6 GPIOs GPIOS[37:32]
 * \note
 * For Reading value from the GPIOs, the GPIO should be configured as management input. otherwise 0 would be read
 * 
 */
unsigned int GPIOs_readHigh(){
    #ifdef ARM 
    return reg_mprj_datah & 0x7; // because with ARM the highest 3 GPIOsare not used by the design it is used by flashing
    #else 
    return reg_mprj_datah;
    #endif
}
/**
 * Read low 32 GPIOs GPIOS[31:0]
 * \note
 * For Reading value from the GPIOs, the GPIO should be configured as management input. otherwise 0 would be read
 * 
 */
unsigned int GPIOs_readLow(){return reg_mprj_datal;}
/**
 * wait over the lowest 32 GPIOsto equal the data passed
 * \note
 * For writing by this function to be seen at the GPIO the GPIO has to be configured as management output
 *  
 * @param data is the data that should wait until sent to the GPIOs 
 * 
 * Examples: 
 * \li \code GPIOs_waitLow(0x1); // function would return only when GPIO [0]==1 and rest of 31 GPIOs= 0  \endcode
 * \li \code GPIOs_waitLow(0x5); // function would return only when GPIO [0]==1 and GPIO [3]==1 and rest of 30 GPIOs = 0 \endcode
 */
void GPIOs_waitLow(unsigned int data){while (GPIOs_readLow()  != data);}
/**
 * wait over the highest 6 GPIOs to equal the data passed
 * \note
 * For writing by this function to be seen at the GPIO the GPIO has to be configured as management output
 *  
 * @param data is the data that should wait until sent to the GPIOs 
 * 
 * Examples: 
 * \li \code GPIOs_waitHigh(0x1); // function would return only when GPIO [32]==1 and rest of 5 GPIOs = 0  \endcode
 * \li \code GPIOs_waitHigh(0x5); // function would return only when GPIO [32]==1 and GPIO [34]==1 and rest of 4 GPIOs = 0 \endcode
 */
void GPIOs_waitHigh(unsigned int data){
    #ifdef ARM 
    data = data&0x7; // because with ARM the highest 3 GPIOs are not used by the design it is used by flashing
    #endif
    while (GPIOs_readHigh() != data);    
}
/**
 * wait over the masked lowest 32 GPIOs to equal the data passed
 * \note
 * For writing by this function to be seen at the GPIO the GPIO has to be configured as management output
 *  
 * @param data is the data that should wait until sent to the GPIOs 
 * @param mask mask over the each GPIO if the mask value is 0 the this GPIO  value are ignored
 * 
 * Examples: 
 * \li \code GPIOs_waitLowWithMask(0x1,0xF); // function would return only when GPIO [0]==1 and GPIO [3:1]==0 and don't care about the rest of GPIOs  \endcode
 * \li \code GPIOs_waitLowWithMask(0x5,0x7); // function would return only when GPIO [0]==1 and GPIO [3]==1 and GPIO [2]==0 and don't care about the rest of GPIOs \endcode
 */
void GPIOs_waitLowWithMask(unsigned int data,unsigned int mask){while (GPIOs_readLow()  & mask != data);}
/**
 * wait over the masked highest 6 GPIOs to equal the data passed
 * \note
 * For writing by this function to be seen at the GPIO the GPIO has to be configured as management output
 *  
 * @param data is the data that should wait until sent to the GPIOs 
 * @param mask mask over the each GPIO if the mask value is 0 the this GPIO  value are ignored
 * 
 * Examples: 
 * \li \code GPIOs_waitHighWithMask(0x1,0xF); // function would return only when GPIO [32]==1 and GPIO [35:33]==0 and don't care about the rest of GPIOs  \endcode
 * \li \code GPIOs_waitHighWithMask(0x5,0x7); // function would return only when GPIO [32]==1 and GPIO [34]==1 and GPIO [33]==0 and don't care about the rest of GPIOs \endcode
 */
void GPIOs_waitHighWithMask(unsigned int data,unsigned int mask){
    #ifdef ARM 
    data = data&0x7; // because with ARM the highest 3 GPIOs are not used by the design it is used by flashing
    #endif
    while (GPIOs_readHigh() != data);    
}
// 
#ifndef DOXYGEN_SHOULD_SKIP_THIS
unsigned int get_active_gpios_num(){
    #ifdef ARM 
    return 34;
    #else
    return 37;
    #endif
}
unsigned int get_gpio_num_bit(){
    #ifdef SKY 
    return 13;
    #elif GF
    return 10;
    #endif
}
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#ifdef DOXYGEN_DOCS_ONLY
/*! \enum gpio_mode
 * GPIOs possible modes
  <table>
    <caption id="multi_row"> Enumerator gpio_mode</caption>
    <tr><th >name<th>description
    <tr><td>GPIO_MODE_MGMT_STD_INPUT_NOPULL<td>Management input with no pull (floating is read as Z)
    <tr><td>GPIO_MODE_MGMT_STD_INPUT_PULLDOWN<td>Management input pull-down (floating is read as 0)
    <tr><td>GPIO_MODE_MGMT_STD_INPUT_PULLUP<td>Management input pull-up (floating is read as 1)
    <tr><td>GPIO_MODE_MGMT_STD_OUTPUT<td>Management output
    <tr><td>GPIO_MODE_MGMT_STD_BIDIRECTIONAL<td>Management bi-direction 
    <tr><td>GPIO_MODE_MGMT_STD_ANALOG<td>Management Analog  
    <tr><td>GPIO_MODE_USER_STD_INPUT_NOPULL<td>User input with no pull (floating is read as Z)
    <tr><td>GPIO_MODE_USER_STD_INPUT_PULLDOWN<td>User input pull-down (floating is read as 0)
    <tr><td>GPIO_MODE_USER_STD_INPUT_PULLUP<td>User input pull-up (floating is read as 1)
    <tr><td>GPIO_MODE_USER_STD_OUTPUT<td>User output
    <tr><td>GPIO_MODE_USER_STD_BIDIRECTIONAL<td>User bi-direction
    <tr><td>GPIO_MODE_USER_STD_OUT_MONITORED<td>User Monitor same as output
    <tr><td>GPIO_MODE_USER_STD_ANALOG<td>User Analog 
 */
enum gpio_mode {};

#endif
#endif // GPIO_C_HEADER_FILE
