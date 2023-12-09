#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define NUM_CORES 5
#define NEURONS_PER_CORE 32
#define AXONS_PER_CORE 256

#define BITS_PER_NEURON 336
#define BITS_PER_PACKET 30
#define MAX_PACKETS 1024  // Assuming a maximum of 1024 packets for simplicity

////////////////////////////////////////////// Neuron data structure
typedef struct {
    int8_t membrane_potential;          // Current membrane potential
    int8_t reset_posi_potential;
    int8_t reset_nega_potential;
    int8_t weights[4];                  // Weights for each input axon
    int8_t leakage_value;
    int8_t positive_threshold;
    int8_t negative_threshold;
    uint8_t axon_dest;                  // Index of the destination axon (of destination core) if Neuron creates spike
    // uint8_t core_dest;               // Don't need since we fixed the destination core of each neuron in C code
} Neuron;

////////////////////////////////////////////// Core data structure
typedef struct {
    Neuron neurons[NEURONS_PER_CORE];                   // Neurons in the core
    uint32_t synapse_connection[AXONS_PER_CORE];        // A (256 x 32) binary matrix to represent synaptic connection
} Core;

////////////////////////////////////////////// Packet data structure
typedef struct {
    int8_t dx;
    int8_t dy;
    uint8_t axon_dest;
} Packet;

// // Function to convert a string of bits to a signed integer
// int bitStringToSignedInt(char *str, int start, int end) {
//     int value = 0;
//     int sign = str[start] == '1' ? -1 : 1;
//     for (int i = start; i < end; i++) {
//         value = value * 2 + (str[i] - '0');
//     }
//     return sign == -1 ? -(value & ~(1 << (end - start - 1))) : value;
// }

// // Function to convert a string of bits to an unsigned integer
// unsigned int bitStringToUnsignedInt(char *str, int start, int end) {
//     unsigned int value = 0;
//     for (int i = start; i < end; i++) {
//         value = value * 2 + (str[i] - '0');
//     }
//     return value;
// }

// Function to convert a string of bits to an integer
int bitStringToInt(char *str, int start, int end) {
    int value = 0;
    for (int i = start; i < end; i++) {
        value = value * 2 + (str[i] - '0');
    }
    return value;
}

int main() {
    FILE *inputFile1, *inputFile2, *outputFile;
    char bitString[BITS_PER_NEURON + 1];
    Core cores[NUM_CORES];
    Packet packets[MAX_PACKETS];
    int packetCount = 0;
    char transposed_synap_connection[NEURONS_PER_CORE][AXONS_PER_CORE];

    // Open input and output files
    inputFile1 = fopen("input_neuron_data.txt", "r");
    inputFile2 = fopen("input_packets.txt", "r");
    outputFile = fopen("SNN_data.c", "w");

    if (!inputFile1 || !inputFile2 || !outputFile) {
        printf("Error in file opening\n");
        return 1;
    }

    ///////////////////////////////  Read input_neuron_data.txt
    for (int core = 0; core < NUM_CORES; core++) {
        for (int neuron = 0; neuron < NEURONS_PER_CORE; neuron++) {
            if (fscanf(inputFile1, "%s", bitString) != 1) {
                printf("Error in reading neuron data\n");
                return 1;
            }

            // Read and transpose synapse connection
            for (int i = 0; i < AXONS_PER_CORE; i++) {
                transposed_synap_connection[neuron][i] = bitString[i];
            }

            // Parse neuron data
            int baseIndex = AXONS_PER_CORE;
            cores[core].neurons[neuron].membrane_potential = bitStringToInt(bitString, baseIndex, baseIndex + 8);
            int8_t reset_potential = bitStringToInt(bitString, baseIndex + 8, baseIndex + 16);
            cores[core].neurons[neuron].reset_posi_potential = reset_potential;
            cores[core].neurons[neuron].reset_nega_potential = reset_potential;

            baseIndex += 16;
            for (int i = 0; i < 4; i++) {
                cores[core].neurons[neuron].weights[i] = bitStringToInt(bitString, baseIndex + i * 8, baseIndex + 8 + i * 8);
            }

            baseIndex += 32;
            cores[core].neurons[neuron].leakage_value = bitStringToInt(bitString, baseIndex, baseIndex + 8);
            cores[core].neurons[neuron].positive_threshold = bitStringToInt(bitString, baseIndex + 8, baseIndex + 16);
            cores[core].neurons[neuron].negative_threshold = bitStringToInt(bitString, baseIndex + 16, baseIndex + 24);
            baseIndex += 24;
            cores[core].neurons[neuron].axon_dest = bitStringToInt(bitString, baseIndex, baseIndex + 8);

            //Debug
            if (neuron == 1 && core ==0) {
                printf("membrane_potential: %d", cores[core].neurons[neuron].membrane_potential); printf("\n");
                printf("reset_posi_potential: %d", cores[core].neurons[neuron].reset_posi_potential);printf("\n");
                printf("reset_nega_potential: %d", cores[core].neurons[neuron].reset_nega_potential);printf("\n");
                printf("weights[0]: %d", cores[core].neurons[neuron].weights[0]);printf("\n");
                printf("weights[1]: %d", cores[core].neurons[neuron].weights[1]);printf("\n");
                printf("weights[2]: %d", cores[core].neurons[neuron].weights[2]);printf("\n");
                printf("weights[3]: %d", cores[core].neurons[neuron].weights[3]);printf("\n");
                printf("leakage_value: %d", cores[core].neurons[neuron].leakage_value);printf("\n");
                printf("positive_threshold: %d", cores[core].neurons[neuron].positive_threshold);printf("\n");
                printf("negative_threshold: %d", cores[core].neurons[neuron].negative_threshold);printf("\n");
                printf("axon_dest: %u", cores[core].neurons[neuron].axon_dest);printf("\n\n");
            }
            
        }

        // Convert and store synapse connections for the core
        for (int i = 0; i < AXONS_PER_CORE; i++) {
            uint32_t synapse_value = 0;
            for (int j = 0; j < NEURONS_PER_CORE; j++) {
                synapse_value |= (transposed_synap_connection[j][i] - '0') << j;
            }
            cores[core].synapse_connection[i] = synapse_value;
        }
    }

    

    ///////////////////////////////  Read input_packets.txt
    while (fscanf(inputFile2, "%s", bitString) == 1 && packetCount < MAX_PACKETS) {
    // while (fscanf(inputFile2, "%s", bitString) == 1) {
        // Parse packet data
        packets[packetCount].dx = bitStringToInt(bitString, 0, 9);
        // printf("dx: %d; ", packets[packetCount].dx);
        packets[packetCount].dy = bitStringToInt(bitString, 9, 18);
        packets[packetCount].axon_dest = bitStringToInt(bitString, 18, 26);
        packetCount++;
    }

    ///////////////////////////////  Write received data to SNN_data.c
    // Write #include, #define directives and data struct definition
    fprintf(outputFile, "#include <stdint.h>\n\n");
    fprintf(outputFile, "#include \"SNN_data.h\"\n\n");
    fprintf(outputFile, "#define NUM_CORES %d\n", NUM_CORES);
    fprintf(outputFile, "#define NEURONS_PER_CORE %d\n", NEURONS_PER_CORE);
    fprintf(outputFile, "#define AXONS_PER_CORE %d\n\n", AXONS_PER_CORE);
    fprintf(outputFile, "typedef struct {\n");
    fprintf(outputFile, "    int8_t membrane_potential;\n");
    fprintf(outputFile, "    int8_t reset_posi_potential;\n");
    fprintf(outputFile, "    int8_t reset_nega_potential;\n");
    fprintf(outputFile, "    int8_t weights[4];\n");
    fprintf(outputFile, "    int8_t leakage_value;\n");
    fprintf(outputFile, "    int8_t positive_threshold;\n");
    fprintf(outputFile, "    int8_t negative_threshold;\n");
    fprintf(outputFile, "    uint8_t axon_dest;\n");
    fprintf(outputFile, "} Neuron;\n\n");
    fprintf(outputFile, "typedef struct {\n");
    fprintf(outputFile, "    Neuron neurons[NEURONS_PER_CORE];\n");
    fprintf(outputFile, "    uint32_t synapse_connection[AXONS_PER_CORE];\n");
    fprintf(outputFile, "} Core;\n\n");
    fprintf(outputFile, "typedef struct {\n");
    fprintf(outputFile, "    int8_t dx;\n");
    fprintf(outputFile, "    int8_t dy;\n");
    fprintf(outputFile, "    uint8_t axon_dest;\n");
    fprintf(outputFile, "} Packet;\n\n");

    // Write SNN data
    fprintf(outputFile, "Core cores[NUM_CORES] = {\n");
    for (int core = 0; core < NUM_CORES; core++) {
        fprintf(outputFile, "    { // Core %d\n", core);
        fprintf(outputFile, "        .neurons = {\n");
        for (int neuron = 0; neuron < NEURONS_PER_CORE; neuron++) {

            

            fprintf(outputFile, "            { %d, %d, %d, {%d, %d, %d, %d}, %d, %d, %d, %u },\n",
                    cores[core].neurons[neuron].membrane_potential, cores[core].neurons[neuron].reset_posi_potential, cores[core].neurons[neuron].reset_nega_potential,
                    cores[core].neurons[neuron].weights[0], cores[core].neurons[neuron].weights[1], cores[core].neurons[neuron].weights[2], cores[core].neurons[neuron].weights[3],
                    cores[core].neurons[neuron].leakage_value, cores[core].neurons[neuron].positive_threshold, cores[core].neurons[neuron].negative_threshold, cores[core].neurons[neuron].axon_dest);
        }
        fprintf(outputFile, "        },\n");
        fprintf(outputFile, "        .synapse_connection = {\n");
        for (int axon = 0; axon < AXONS_PER_CORE; axon++) {
            fprintf(outputFile, "            0x%08X,\n", cores[core].synapse_connection[axon]);
        }
        fprintf(outputFile, "        }\n");
        fprintf(outputFile, "    },\n");
    }
    fprintf(outputFile, "};\n\n");

    fprintf(outputFile, "Packet packets[%d] = {\n", packetCount);
    for (int i = 0; i < packetCount; i++) {
        fprintf(outputFile, "    { %u, %d, %d },\n", packets[i].dx, packets[i].dy, packets[i].axon_dest);
    }
    fprintf(outputFile, "};\n");

    // Close files
    fclose(inputFile1);
    fclose(inputFile2);
    fclose(outputFile);

    return 0;
}