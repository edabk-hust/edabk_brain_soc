
#ifndef BITBANG_C_HEADER_FILE
#define BITBANG_C_HEADER_FILE
/*
reg_mprj_xfer contain 
bit 0 : busy 
bit 1 : bitbang enable
bit 2 : bitbang reset active low
bit 3 : bitbang load registers
bit 4 : bitbang clock
bit 5 : serial data 1
bit 6 : serial data 2
*/

void bb_clock11(){
    reg_mprj_xfer = 0x66;
    reg_mprj_xfer = 0x76;
}

void bb_clock00(){
    reg_mprj_xfer = 0x06;
    reg_mprj_xfer = 0x16;
}

void bb_clock10(){
    reg_mprj_xfer = 0x46; 
    reg_mprj_xfer = 0x56;
}

void bb_clock01(){
    reg_mprj_xfer = 0x26; 
    reg_mprj_xfer = 0x36;
}

void bb_load(){
    reg_mprj_xfer = 0x06;
    reg_mprj_xfer = 0x0e; 
    reg_mprj_xfer = 0x06;	
}

void bb_reset(){
    reg_mprj_xfer = 0x04;
    reg_mprj_xfer = 0x06;			

}

// configure the GPIO  in the left chain with configL and the GPIO  in 
// the right chain with configR
// left | right
// 18	& 19
// 17	& 20
// 16	& 21
// 15	& 22
// 14	& 23
// 13	& 24
// 12	& 25
// 11	& 26
// 10	& 27
// 9	& 28
// 8	& 29
// 7	& 30
// 6	& 31
// 5	& 32
// 4	& 33
// 3	& 34
// 2	& 35
// 1	& 36
// 0	& 37
void bb_configure2Gpios(unsigned int configL,unsigned int configR){
    int num_bits,mask;
    int left,right;
    num_bits = get_gpio_num_bit();
    mask = 0x1 << num_bits-1;
    for (int i = num_bits-1; i >= 0; i--){
        left = (configL & mask ) >> i;
        right= (configR & mask ) >> i;
        mask = mask >> 1;
        if (left){
            if (right)
                bb_clock11();
            else
                bb_clock10();

        }else{
            if(right) 
                bb_clock01();
            else
                bb_clock00();
        }
    }   
}

void bb_configureAllGpios(unsigned int config){
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

    bb_reset();
    bb_configure2Gpios(config,config);// 18	& 19
    bb_configure2Gpios(config,config);// 17	& 20
    bb_configure2Gpios(config,config);// 16	& 21
    bb_configure2Gpios(config,config);// 15	& 22
    bb_configure2Gpios(config,config);// 14	& 23
    bb_configure2Gpios(config,config);// 13	& 24
    bb_configure2Gpios(config,config);// 12	& 25
    bb_configure2Gpios(config,config);// 11	& 26
    bb_configure2Gpios(config,config);// 10	& 27
    bb_configure2Gpios(config,config);// 9	& 28
    bb_configure2Gpios(config,config);// 8	& 29
    bb_configure2Gpios(config,config);// 7	& 30
    bb_configure2Gpios(config,config);// 6	& 31
    bb_configure2Gpios(config,config);// 5	& 32
    bb_configure2Gpios(config,config);// 4	& 33
    bb_configure2Gpios(config,config);// 3	& 34
    bb_configure2Gpios(config,config);// 2	& 35
    bb_configure2Gpios(config,config);// 1	& 36
    bb_configure2Gpios(config,config);// 0	& 37
    bb_load();
}

#endif // BITBANG_C_HEADER_FILE