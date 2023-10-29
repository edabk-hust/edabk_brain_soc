#include <queue>
#include <cmath>
#include <stdint.h>
#include <ap_int.h>

#define MAX_DIM 28
#define NUM_CLASSES 10

#include "Forward.h"
// 1-bit unsigned integer. You can use a bool or uint8_t for this purpose, as there's no direct 1-bit integer type in standard C++.
typedef uint8_t bit_t;

// Stream for 1-bit unsigned integer.
typedef std::queue<bit_t> bit_stream_t;

// Global variables
int16_t weights_conv1[36] = {(int16_t)0x3602,(int16_t)0x3720,(int16_t)0x3a4e,(int16_t)0x258e,(int16_t)0xacba,(int16_t)0x3783,(int16_t)0x3742,(int16_t)0x385a,(int16_t)0x346c,(int16_t)0x3850,(int16_t)0xba72,(int16_t)0xac05,(int16_t)0x2d55,(int16_t)0xb134,(int16_t)0xb2c5,(int16_t)0x36e4,(int16_t)0x25a5,(int16_t)0x347b,(int16_t)0x3725,(int16_t)0xb0c9,(int16_t)0x33cf,(int16_t)0xb1bf,(int16_t)0x3908,(int16_t)0xb4c9,(int16_t)0x38f1,(int16_t)0xb88c,(int16_t)0xb57b,(int16_t)0x374b,(int16_t)0x343a,(int16_t)0xb8f8,(int16_t)0xb579,(int16_t)0xb643,(int16_t)0x3b13,(int16_t)0xb8e5,(int16_t)0x3a03,(int16_t)0xacd2};
int16_t biases_conv1[4] = {(int16_t)0x67a,(int16_t)0x1dcf,(int16_t)0xbc2b,(int16_t)0xb6d4};
int16_t weights_conv2[288] = {(int16_t)0xb96e,(int16_t)0xa153,(int16_t)0x3acf,(int16_t)0xac8b,(int16_t)0xb520,(int16_t)0x311a,(int16_t)0x2db8,(int16_t)0x3723,(int16_t)0x36ac,(int16_t)0x356a,(int16_t)0x2acb,(int16_t)0xaeec,(int16_t)0xb91d,(int16_t)0xb716,(int16_t)0x3b20,(int16_t)0xb05f,(int16_t)0xb504,(int16_t)0xb613,(int16_t)0x3a7e,(int16_t)0xad2b,(int16_t)0x3104,(int16_t)0x3014,(int16_t)0xac4f,(int16_t)0xadf2,(int16_t)0xbfce,(int16_t)0x3028,(int16_t)0x3b24,(int16_t)0xb23c,(int16_t)0xc253,(int16_t)0x308a,(int16_t)0xb19b,(int16_t)0x3008,(int16_t)0xafa5,(int16_t)0xb4bc,(int16_t)0x3a71,(int16_t)0xb41f,(int16_t)0x30e6,(int16_t)0x3194,(int16_t)0x2c05,(int16_t)0xb40c,(int16_t)0x39e3,(int16_t)0xb151,(int16_t)0x34c7,(int16_t)0xb735,(int16_t)0xb80b,(int16_t)0x3344,(int16_t)0x30c7,(int16_t)0xacda,(int16_t)0xabf9,(int16_t)0xbc79,(int16_t)0x3d40,(int16_t)0xb6a8,(int16_t)0xbc74,(int16_t)0x2dbe,(int16_t)0x34b5,(int16_t)0xa197,(int16_t)0xbc8d,(int16_t)0x2380,(int16_t)0xb741,(int16_t)0x366b,(int16_t)0xac1b,(int16_t)0x2e1d,(int16_t)0xb7d4,(int16_t)0x30ed,(int16_t)0xb4c1,(int16_t)0x37cd,(int16_t)0x311a,(int16_t)0xb654,(int16_t)0x31e1,(int16_t)0xb456,(int16_t)0x3608,(int16_t)0xb554,(int16_t)0xa27f,(int16_t)0xa82e,(int16_t)0xb9f8,(int16_t)0x3bb8,(int16_t)0xb5bd,(int16_t)0xa969,(int16_t)0xa871,(int16_t)0xb75e,(int16_t)0x3460,(int16_t)0xb975,(int16_t)0x3ae2,(int16_t)0xb835,(int16_t)0x32dc,(int16_t)0xb6fd,(int16_t)0x38c3,(int16_t)0xb5e4,(int16_t)0xba6e,(int16_t)0x34be,(int16_t)0xba2a,(int16_t)0x397d,(int16_t)0x2fc0,(int16_t)0x31c7,(int16_t)0x33b2,(int16_t)0x9c58,(int16_t)0x2373,(int16_t)0xb6f6,(int16_t)0xb8c2,(int16_t)0x333d,(int16_t)0xb5c9,(int16_t)0x33f0,(int16_t)0x3306,(int16_t)0x3987,(int16_t)0x35e6,(int16_t)0x3928,(int16_t)0x2e32,(int16_t)0xb264,(int16_t)0xbc33,(int16_t)0x33f4,(int16_t)0xb239,(int16_t)0xb0c0,(int16_t)0x2e64,(int16_t)0xae85,(int16_t)0xb113,(int16_t)0xb5a9,(int16_t)0xbee7,(int16_t)0xad12,(int16_t)0x2918,(int16_t)0x2ce8,(int16_t)0xbb3d,(int16_t)0xb5ef,(int16_t)0xada4,(int16_t)0xb4fc,(int16_t)0x2e5f,(int16_t)0xb57d,(int16_t)0x3bc4,(int16_t)0x380d,(int16_t)0xae32,(int16_t)0xb876,(int16_t)0xb4bc,(int16_t)0x1623,(int16_t)0x2ea6,(int16_t)0xb57c,(int16_t)0x35ef,(int16_t)0x3428,(int16_t)0x3bba,(int16_t)0x3d36,(int16_t)0x3935,(int16_t)0x3874,(int16_t)0xbc29,(int16_t)0x372d,(int16_t)0x3618,(int16_t)0x2fe5,(int16_t)0x3b3d,(int16_t)0xb4d0,(int16_t)0x39ee,(int16_t)0x3c29,(int16_t)0xbdf5,(int16_t)0xae26,(int16_t)0x3174,(int16_t)0x3953,(int16_t)0xbde2,(int16_t)0xb7d4,(int16_t)0x3599,(int16_t)0x37c3,(int16_t)0xb900,(int16_t)0xba50,(int16_t)0x3c1c,(int16_t)0xbb17,(int16_t)0xb841,(int16_t)0xb438,(int16_t)0x311d,(int16_t)0xba0b,(int16_t)0x3146,(int16_t)0xb2f8,(int16_t)0xb56b,(int16_t)0x384d,(int16_t)0xa08b,(int16_t)0x3f95,(int16_t)0xb88e,(int16_t)0x38c0,(int16_t)0xb616,(int16_t)0xb36e,(int16_t)0x3518,(int16_t)0x3cb0,(int16_t)0x353f,(int16_t)0xb714,(int16_t)0x385d,(int16_t)0x3c8a,(int16_t)0x354a,(int16_t)0xac15,(int16_t)0x351f,(int16_t)0xb02c,(int16_t)0xb839,(int16_t)0x2ff4,(int16_t)0x3bdd,(int16_t)0xaf66,(int16_t)0x33a2,(int16_t)0x374f,(int16_t)0x9b0e,(int16_t)0xbc89,(int16_t)0x35a1,(int16_t)0x3139,(int16_t)0xb855,(int16_t)0x2108,(int16_t)0xb394,(int16_t)0x36f8,(int16_t)0xb4d2,(int16_t)0xb3e1,(int16_t)0x368f,(int16_t)0x302f,(int16_t)0xb7d8,(int16_t)0xae76,(int16_t)0xc049,(int16_t)0xa314,(int16_t)0xb637,(int16_t)0xb25f,(int16_t)0x383e,(int16_t)0xb5ca,(int16_t)0xb3dc,(int16_t)0x310b,(int16_t)0x39b2,(int16_t)0xb43d,(int16_t)0x3c3c,(int16_t)0x3684,(int16_t)0xb83e,(int16_t)0x30da,(int16_t)0x35e9,(int16_t)0xb5c4,(int16_t)0x3fc5,(int16_t)0x34a0,(int16_t)0xb7e1,(int16_t)0xb9db,(int16_t)0x3b96,(int16_t)0xab3e,(int16_t)0xac69,(int16_t)0x38ca,(int16_t)0xb437,(int16_t)0xada4,(int16_t)0xa603,(int16_t)0xaedd,(int16_t)0x2ecb,(int16_t)0xb645,(int16_t)0xb9fe,(int16_t)0x36f0,(int16_t)0xb856,(int16_t)0xb46e,(int16_t)0x2d38,(int16_t)0x38a8,(int16_t)0xb3a6,(int16_t)0xab74,(int16_t)0xba30,(int16_t)0x3c13,(int16_t)0xb8d4,(int16_t)0x355d,(int16_t)0x3840,(int16_t)0x35cf,(int16_t)0xb40f,(int16_t)0xb8df,(int16_t)0x3db2,(int16_t)0xb935,(int16_t)0x89c0,(int16_t)0xb1f9,(int16_t)0xb891,(int16_t)0xbc6f,(int16_t)0x322a,(int16_t)0x3131,(int16_t)0x18e9,(int16_t)0x39bd,(int16_t)0x3294,(int16_t)0xb8f8,(int16_t)0xb7e1,(int16_t)0xb8d4,(int16_t)0x34ff,(int16_t)0xb4d6,(int16_t)0xb412,(int16_t)0x2241,(int16_t)0xb753,(int16_t)0xbbfe,(int16_t)0x3a69,(int16_t)0x3af7,(int16_t)0xb28e,(int16_t)0xb4e5,(int16_t)0xb2a4,(int16_t)0x38c0,(int16_t)0x29dd,(int16_t)0xa8be,(int16_t)0x37f2,(int16_t)0x2997,(int16_t)0x9c46,(int16_t)0xba4e,(int16_t)0x3bc5,(int16_t)0x2f60,(int16_t)0x32a4,(int16_t)0x376f,(int16_t)0x3876,(int16_t)0xbca0};
int16_t biases_conv2[8] = {(int16_t)0x26c7,(int16_t)0x3c8e,(int16_t)0xb0da,(int16_t)0xb820,(int16_t)0x1ee2,(int16_t)0xb6da,(int16_t)0xbc55,(int16_t)0x3ab6};
int16_t weights_dense2[320] = {(int16_t)0x39d4,(int16_t)0x3a57,(int16_t)0x39ce,(int16_t)0x39b6,(int16_t)0x2cf8,(int16_t)0x2caf,(int16_t)0xbe59,(int16_t)0x3c4d,(int16_t)0xb7b9,(int16_t)0xb9b6,(int16_t)0xbccb,(int16_t)0x2b28,(int16_t)0x3895,(int16_t)0x3a1e,(int16_t)0xbcbd,(int16_t)0xbd30,(int16_t)0xbc0c,(int16_t)0x2cec,(int16_t)0xb558,(int16_t)0xb322,(int16_t)0xb7d5,(int16_t)0x1836,(int16_t)0x2c3b,(int16_t)0x385a,(int16_t)0xa818,(int16_t)0xb781,(int16_t)0xb8bd,(int16_t)0x34a0,(int16_t)0x9112,(int16_t)0xb804,(int16_t)0xb8e5,(int16_t)0xb8b0,(int16_t)0x3021,(int16_t)0xb556,(int16_t)0x36fc,(int16_t)0x3464,(int16_t)0x3512,(int16_t)0xb801,(int16_t)0x3bdd,(int16_t)0x3684,(int16_t)0x2c7c,(int16_t)0x3c16,(int16_t)0xb704,(int16_t)0xb985,(int16_t)0x3e21,(int16_t)0x3f58,(int16_t)0x3a6d,(int16_t)0xb5b5,(int16_t)0x409f,(int16_t)0x2b4d,(int16_t)0x47a2,(int16_t)0x4318,(int16_t)0xc4f6,(int16_t)0xc65b,(int16_t)0x414e,(int16_t)0x4a26,(int16_t)0xc190,(int16_t)0x3b99,(int16_t)0xbfe0,(int16_t)0xbeb0,(int16_t)0x315a,(int16_t)0xb8d7,(int16_t)0xb7d7,(int16_t)0x357d,(int16_t)0xb7ea,(int16_t)0x3914,(int16_t)0xb599,(int16_t)0xb9bd,(int16_t)0x3715,(int16_t)0x3c03,(int16_t)0xb7c8,(int16_t)0x334b,(int16_t)0xb152,(int16_t)0xb70d,(int16_t)0xb45a,(int16_t)0xb7a1,(int16_t)0xbb01,(int16_t)0x3c00,(int16_t)0xb432,(int16_t)0xb84e,(int16_t)0x2f0d,(int16_t)0xb574,(int16_t)0xaea7,(int16_t)0x3933,(int16_t)0xbb4c,(int16_t)0x3181,(int16_t)0xb80c,(int16_t)0x37ec,(int16_t)0x34bb,(int16_t)0x3d8d,(int16_t)0xb351,(int16_t)0xb996,(int16_t)0xba06,(int16_t)0xbc70,(int16_t)0xbbc7,(int16_t)0x3c43,(int16_t)0x36dc,(int16_t)0xb44c,(int16_t)0xb7b2,(int16_t)0xb7f0,(int16_t)0x3b22,(int16_t)0xb163,(int16_t)0x393a,(int16_t)0xb90f,(int16_t)0xbb10,(int16_t)0xbae7,(int16_t)0xbbdf,(int16_t)0x3a1f,(int16_t)0xb623,(int16_t)0x1e48,(int16_t)0x3499,(int16_t)0xbb57,(int16_t)0x2b28,(int16_t)0x3989,(int16_t)0xb49d,(int16_t)0x34e7,(int16_t)0x3dc4,(int16_t)0xb08f,(int16_t)0x392e,(int16_t)0x3716,(int16_t)0xb0ba,(int16_t)0x3957,(int16_t)0x390f,(int16_t)0xcc2b,(int16_t)0x3d1e,(int16_t)0xb788,(int16_t)0xb7fe,(int16_t)0xb919,(int16_t)0xc96e,(int16_t)0xbdb4,(int16_t)0xb9fb,(int16_t)0x3fab,(int16_t)0xbb0c,(int16_t)0x2b40,(int16_t)0xbff8,(int16_t)0x400a,(int16_t)0x3bad,(int16_t)0xbe6b,(int16_t)0xba91,(int16_t)0x30c4,(int16_t)0xb74a,(int16_t)0xbace,(int16_t)0xbb6a,(int16_t)0x3caf,(int16_t)0x24d9,(int16_t)0x3909,(int16_t)0xbb85,(int16_t)0xb9d1,(int16_t)0x3c1c,(int16_t)0x3138,(int16_t)0x3817,(int16_t)0x30d4,(int16_t)0x2cd6,(int16_t)0xb922,(int16_t)0xb800,(int16_t)0x3d1d,(int16_t)0x353c,(int16_t)0xb722,(int16_t)0x2837,(int16_t)0xb7ad,(int16_t)0xbc13,(int16_t)0x3df1,(int16_t)0x3985,(int16_t)0x3cd5,(int16_t)0xb836,(int16_t)0x3c08,(int16_t)0xbf36,(int16_t)0xb743,(int16_t)0xb5ff,(int16_t)0xb6c7,(int16_t)0xbc0a,(int16_t)0x2d21,(int16_t)0xb585,(int16_t)0x3c9a,(int16_t)0x3085,(int16_t)0x32f1,(int16_t)0xbd93,(int16_t)0x39ff,(int16_t)0xbbaa,(int16_t)0x38fd,(int16_t)0xb7f9,(int16_t)0xade2,(int16_t)0xb7c0,(int16_t)0xb960,(int16_t)0x3c15,(int16_t)0xb309,(int16_t)0xae96,(int16_t)0x3243,(int16_t)0x2f4f,(int16_t)0x38a8,(int16_t)0x3a28,(int16_t)0xbd15,(int16_t)0xb623,(int16_t)0xb8be,(int16_t)0x30b4,(int16_t)0xac81,(int16_t)0x3cbe,(int16_t)0xbc63,(int16_t)0xb31e,(int16_t)0x2c8c,(int16_t)0x4004,(int16_t)0x3157,(int16_t)0xb918,(int16_t)0xb91b,(int16_t)0xafc9,(int16_t)0xb862,(int16_t)0x3c59,(int16_t)0xba6f,(int16_t)0xb8d2,(int16_t)0x355f,(int16_t)0xc07b,(int16_t)0x3c82,(int16_t)0x3937,(int16_t)0xad2d,(int16_t)0x3c56,(int16_t)0xb51b,(int16_t)0xbddc,(int16_t)0x4419,(int16_t)0xbfab,(int16_t)0xbe86,(int16_t)0xb4dd,(int16_t)0xbc00,(int16_t)0x3860,(int16_t)0xb5c2,(int16_t)0xbe8c,(int16_t)0xbca1,(int16_t)0xbaa6,(int16_t)0xac1b,(int16_t)0x3c16,(int16_t)0xbcbe,(int16_t)0x3317,(int16_t)0xb79c,(int16_t)0xb91b,(int16_t)0xb43d,(int16_t)0x3a2b,(int16_t)0x212e,(int16_t)0x3900,(int16_t)0xb890,(int16_t)0xb9f3,(int16_t)0x3a7d,(int16_t)0x2e68,(int16_t)0xaf0b,(int16_t)0xb439,(int16_t)0x3579,(int16_t)0xbcfa,(int16_t)0x3560,(int16_t)0xb47b,(int16_t)0x3106,(int16_t)0xb8d7,(int16_t)0xb529,(int16_t)0x2ed1,(int16_t)0xb806,(int16_t)0x301a,(int16_t)0xb8d5,(int16_t)0x3604,(int16_t)0xb93c,(int16_t)0xb172,(int16_t)0xb4f2,(int16_t)0x2e8e,(int16_t)0xb885,(int16_t)0x38f7,(int16_t)0x3939,(int16_t)0x286d,(int16_t)0x2c50,(int16_t)0x38c4,(int16_t)0x3403,(int16_t)0xb1d7,(int16_t)0x31f1,(int16_t)0xb589,(int16_t)0x2dfe,(int16_t)0xbcdf,(int16_t)0x3046,(int16_t)0x3905,(int16_t)0x3a65,(int16_t)0xb478,(int16_t)0x9e97,(int16_t)0x2908,(int16_t)0xbc3e,(int16_t)0x3832,(int16_t)0xbb90,(int16_t)0xb852,(int16_t)0xb42e,(int16_t)0xc07f,(int16_t)0xb965,(int16_t)0xb4be,(int16_t)0xbdbb,(int16_t)0xb419,(int16_t)0xc421,(int16_t)0xb3dd,(int16_t)0xc856,(int16_t)0xbdd0,(int16_t)0x3fdf,(int16_t)0xbbf6,(int16_t)0x2c7f,(int16_t)0x3ac3,(int16_t)0xbca5,(int16_t)0xbced,(int16_t)0x3e2f,(int16_t)0x3600,(int16_t)0x38a1,(int16_t)0xb842,(int16_t)0xba54,(int16_t)0xad26,(int16_t)0xb00c,(int16_t)0x39ca,(int16_t)0xb479,(int16_t)0x397e,(int16_t)0x3061,(int16_t)0x3316,(int16_t)0xb5f5,(int16_t)0x38da,(int16_t)0x3d2d,(int16_t)0x3ac3,(int16_t)0xbb26,(int16_t)0x3437,(int16_t)0xb675,(int16_t)0x3936,(int16_t)0x32f8,(int16_t)0xb8f3,(int16_t)0xb86d};
int16_t biases_dense2[10] = {(int16_t)0x3a42,(int16_t)0x3f77,(int16_t)0xb9f8,(int16_t)0xb821,(int16_t)0x3888,(int16_t)0xb878,(int16_t)0x39ae,(int16_t)0xb760,(int16_t)0x328e,(int16_t)0xb56b};


ap_uint<4> forward_pass(volatile int16_t image[785]) {
    #pragma HLS INTERFACE bram port=image
	//#pragma HLS INTERFACE ap_vld port=dense_final
    //#pragma HLS INTERFACE bram port=onehot_output
	int16_t dense[10];
    // First Conv2D layer: input is 28x28x1, filter is 3x3x1x4
    // It results in a 26x26x4 output (we are assuming VALID padding)
    int16_t conv1[26][26][4]= {};
    
    int16_t aux_mult;


    for(int i = 0; i < 26; i++) //1
        for(int j = 0; j < 26; j++) //2
            for(int k = 0; k < 4; k++) //k = 0
            {
                conv1[i][j][k] = 0;
                // Convolution operation
                for(int di = 0; di < 3; di++)//
                    for(int dj = 0; dj < 3; dj++){//
                        aux_mult = multiply_custom_float(image[(i+di)*28 + (j+dj)],weights_conv1[(di*3 + dj)*4 + k]);
                        conv1[i][j][k] = add(conv1[i][j][k],aux_mult);
                    }
                conv1[i][j][k] = add(conv1[i][j][k],biases_conv1[k]);
                // ReLU activation
                conv1[i][j][k] = relu_custom_float(conv1[i][j][k]);
            }

    // First MaxPooling layer: input is 26x26x4, pool size is 4
    // It results in a 6x6x4 output
    int16_t maxpool1[6][6][4]= {};
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 6; j++)
            for(int k = 0; k < 4; k++) {
                maxpool1[i][j][k] = 0;
                for(int di = 0; di < 4; di++)
                    for(int dj = 0; dj < 4; dj++)
                        maxpool1[i][j][k] = max_custom_float(maxpool1[i][j][k], conv1[i*4+di][j*4+dj][k]);
            }

    // Second Conv2D layer: input is 6x6x4, filter is 3x3x4x8
    // It results in a 4x4x8 output
    int16_t conv2[4][4][8]= {};
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 4; j++)
            for(int k = 0; k < 8; k++) {
                conv2[i][j][k] = 0;
                for(int di = 0; di < 3; di++)
                    for(int dj = 0; dj < 3; dj++)
                        for(int dk = 0; dk < 4; dk++){
                            aux_mult = multiply_custom_float(maxpool1[i+di][j+dj][dk],weights_conv2[((di*3+dj)*4+dk)*8+k]);
                            conv2[i][j][k] = add(conv2[i][j][k],aux_mult);}
                conv2[i][j][k] = add(conv2[i][j][k],biases_conv2[k]);
                conv2[i][j][k] = relu_custom_float(conv2[i][j][k]);
            }

    // Second MaxPooling layer: input is 4x4x8, pool size is 2
    // It results in a 2x2x8 output
    int16_t maxpool2[2][2][8]= {};
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 2; j++)
            for(int k = 0; k < 8; k++) {
                maxpool2[i][j][k] = 0;
                for(int di = 0; di < 2; di++)
                    for(int dj = 0; dj < 2; dj++)
                        maxpool2[i][j][k] = max_custom_float(maxpool2[i][j][k], conv2[i*2+di][j*2+dj][k]);
            }

    // Flatten layer: input is 2x2x8
    // It results in a 32-length 1D array
    int16_t flatten[32]= {};
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 2; j++)
            for(int k = 0; k < 8; k++)
                flatten[i*2*8 + j*8 + k] = maxpool2[i][j][k];

    // Dense layer: input is a 32-length 1D array, weights are 32x10
    // It results in a 10-length 1D array
    int16_t aux_summ_pinter;

    for(int i = 0; i < 10; i++) {
        dense[i] = 0;
        for(int j = 0; j < 32; j++){
            aux_mult = multiply_custom_float(flatten[j], weights_dense2[j*10+i]);
            dense[i] = add(dense[i],aux_mult);
        }
        dense[i] = add(dense[i],biases_dense2[i]);
    }
/*
    // Softmax activation
    softmax(dense);
*/
    // ... fill dense_output with values from your dense layer ...

    int16_t max_value = dense[0];
    ap_uint<4> max_index=0;

    for(int i = 0; i < NUM_CLASSES; i++) {
        if(greater_than_custom_float(dense[i], max_value)) {
            max_value = dense[i];
            max_index = i;
        }
    }
    return max_index;


}


