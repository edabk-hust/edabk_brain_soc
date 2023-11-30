#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define NUM_CORES 5
#define NEURONS_PER_CORE 32
#define BITS_PER_NEURON 336
#define SYNAPSE_COLS 256

typedef struct {
    uint8_t synapse_connection[NEURONS_PER_CORE][32]; // since each uint_8 already can store 8-bit
    uint8_t current_membrane_potential;
    uint8_t reset_posi_potential;
    uint8_t reset_nega_potential;
    uint8_t weights[4];
    uint8_t leakage_value;
    uint8_t positive_threshold;
    uint8_t negative_threshold;
    uint8_t destination_axon;
} NeuronData;

// Function to convert neuron_data.txt to C file
void convertNeuronData(const char* inputFileName, const char* outputFileName) {
    FILE* inputFile = fopen(inputFileName, "r");
    if (!inputFile) {
        printf("Error opening input file %s\n", inputFileName);
        return;
    }

    FILE* outputFile = fopen(outputFileName, "w");
    if (!outputFile) {
        printf("Error opening output file %s\n", outputFileName);
        fclose(inputFile);
        return;
    }

    fprintf(outputFile, "#include <stdint.h>\n\n");
    fprintf(outputFile, "NeuronData neuronDataArray[NUM_CORES][NEURONS_PER_CORE] = {\n");

    for (int coreIndex = 0; coreIndex < NUM_CORES; coreIndex++) {
        fprintf(outputFile, "    { // Core %d\n", coreIndex);

        for (int neuronIndex = 0; neuronIndex < NEURONS_PER_CORE; neuronIndex++) {
            NeuronData neuron = {0};  // Initialize with zeros

            // Read and write synapse connection data (256 bits)
            for (int col = 0; col < SYNAPSE_COLS; col++) {
                for (int batch = 0; batch < 32; batch++) {
                    int synapseBit;
                    fscanf(inputFile, "%1d", &synapseBit);
                    neuron.synapse_connection[neuronIndex][batch] |=
                        (synapseBit << (7 - (col % 8)));
                }
            }

            // Read and write other neuron parameters
            fscanf(inputFile, "%" SCNu8 "%" SCNu8 "%" SCNu8 "%" SCNu8 "%" SCNu8
                              "%" SCNu8 "%" SCNu8 "%" SCNu8 "%" SCNu8 "%" SCNu8,
                   &neuron.current_membrane_potential, &neuron.reset_posi_potential,
                   &neuron.reset_nega_potential, &neuron.weights[0], &neuron.weights[1],
                   &neuron.weights[2], &neuron.weights[3], &neuron.leakage_value,
                   &neuron.positive_threshold, &neuron.negative_threshold,
                   &neuron.destination_axon);

            fprintf(outputFile, "        {");

            // Write synapse connection data to the C file
            for (int col = 0; col < SYNAPSE_COLS / 8; col++) {
                fprintf(outputFile, "0x%02X, ", neuron.synapse_connection[neuronIndex][col]);
            }

            // Write other neuron parameters to the C file
            fprintf(outputFile, "%" PRIu8 ", %" PRIu8 ", %" PRIu8 ", %" PRIu8 ", %" PRIu8
                                ", %" PRIu8 ", %" PRIu8 ", %" PRIu8 ", %" PRIu8 ", %" PRIu8
                                ", %" PRIu8 "},\n",
                    neuron.current_membrane_potential, neuron.reset_posi_potential,
                    neuron.reset_nega_potential, neuron.weights[0], neuron.weights[1],
                    neuron.weights[2], neuron.weights[3], neuron.leakage_value,
                    neuron.positive_threshold, neuron.negative_threshold,
                    neuron.destination_axon);
        }

        fprintf(outputFile, "    },\n");
    }

    fprintf(outputFile, "};\n");

    fclose(inputFile);
    fclose(outputFile);

    printf("Conversion successful!\n");
}

int main() {
    convertNeuronData("neuron_data.txt", "neuron_data_array.c");
    return 0;
}
