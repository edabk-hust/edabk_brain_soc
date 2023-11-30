#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define AXONS 256
#define NEURONS_PER_CORE 32
#define NUM_CORES 5
#define FIFO_SIZE 256
#define SYNAPSES 32

typedef struct {
    int8_t current_membrane_potential;
    int8_t reset_posi_potential;
    int8_t reset_nega_potential;
    int8_t weights[4];
    int8_t leakage_value;
    int8_t positive_threshold;
    int8_t negative_threshold;
    uint8_t destination_axon;
} Neuron;

typedef struct {
    int front, rear, size;
    unsigned capacity;
    int* array;
} Queue;

typedef struct {
    Neuron neurons[NEURONS_PER_CORE];
    uint8_t synapse_connections[AXONS][NEURONS_PER_CORE];
    uint8_t large_synapse_connections[NEURONS_PER_CORE][AXONS];
    Queue spikeQueue;
    uint8_t output_axons[NEURONS_PER_CORE];
} SNNCore;

SNNCore cores[NUM_CORES];

Queue* createQueue(unsigned capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0; 
    queue->rear = capacity - 1;
    queue->array = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
}

int isFull(Queue* queue) { return (queue->size == queue->capacity); }
int isEmpty(Queue* queue) { return (queue->size == 0); }

void enqueue(Queue* queue, int item) {
    if (isFull(queue)) return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

int dequeue(Queue* queue) {
    if (isEmpty(queue)) return -1;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

int front(Queue* queue) {
    if (isEmpty(queue)) return -1;
    return queue->array[queue->front];
}

void readNeuronData(SNNCore* core, const char* line, int neuronIndex) {
    for (int i = 0; i < AXONS; i++) {
        core->synapse_connections[i][neuronIndex] = line[i] - '0';
    }

    uint8_t params[10];
    for (int i = 0, j = AXONS; i < 10; i++, j += 8) {
        uint8_t value = 0;
        for (int k = 0; k < 8; k++) {
            value = (value << 1) | (line[j + k] - '0');
        }
        params[i] = value;
    }

    Neuron* neuron = &core->neurons[neuronIndex];
    neuron->current_membrane_potential = params[0];
    neuron->reset_posi_potential = params[1];
    neuron->reset_nega_potential = neuron->reset_posi_potential;
    for (int i = 0; i < 4; i++) {
        neuron->weights[i] = params[2 + i];
    }
    neuron->leakage_value = params[6];
    neuron->positive_threshold = params[7];
    neuron->negative_threshold = params[8];
    neuron->destination_axon = params[9];
}

// Function to simulate sending data to a wishbone bus
void sendToWishboneBus(uint32_t* synapseData, uint32_t* neuronParameters, uint32_t* inputSpikes, uint32_t* outputSpikes) {
    // Dummy implementation for demonstration
    // In a real system, you would interface with the actual wishbone bus
    // and send/receive data accordingly.

    // For now, we print the addresses to demonstrate the concept
    printf("Sending synapse data to Wishbone Bus: %p\n", (void*)synapseData);
    printf("Sending neuron parameters to Wishbone Bus: %p\n", (void*)neuronParameters);
    printf("Sending input spikes to Wishbone Bus: %p\n", (void*)inputSpikes);

    // Send synapse data to wishbone bus
    for (int i = 0; i < AXONS * NEURONS_PER_CORE; i++) {
        reg_mprj_slave = i; // Set the wishbone address
        reg_mprj_datal = synapseData[i]; // Send synapse data
    }

    // Send neuron parameters to wishbone bus
    for (int i = 0; i < NUM_CORES; i++) {
        for (int j = 0; j < NEURONS_PER_CORE * 4; j++) {
            reg_mprj_slave = AXONS * NEURONS_PER_CORE + i * NEURONS_PER_CORE * 4 + j; // Set the wishbone address
            reg_mprj_datal = neuronParameters[i][j]; // Send neuron parameter data
        }
    }

    // Send input spikes to wishbone bus
    for (int i = 0; i < AXONS; i++) {
        reg_mprj_slave = AXONS * NEURONS_PER_CORE * 4 + i; // Set the wishbone address
        reg_mprj_datal = inputSpikes[i]; // Send input spike data
    }

    // Dummy data for output spikes (replace this with actual data after processing)
    *outputSpikes = reg_mprj_slave; // Receive processed output spikes

    printf("Received processed output spikes from Wishbone Bus: %p\n", (void*)outputSpikes);
}

int main() {
    const char* input_spikes[5] = {
        "1010010000110001000010101010111011100111010100000110101000111000100100100011101010101111101101001010101111101000100110101000111010101010101001011001001001101010001010100110111010101111101010100111011111000001100011111001100101011100101010000001011000011001",
        "1111111010001010010010100010101111110000101000001011000110100100011010110100101100001000101100101010010010100100111110110110101100001010111001110110101010110110101010111001011011111000000010001001001101010100101000101000100010100101100001111011000001011110",
        "1100101010100000101100101001000010001001011010110110101010100000011101101001010010101001000000010110100101101010111110100101010100011111101000010010110010010101010110101101001010000001010100111001111110010100100001101010110011001100100100001101010000000011",
        "1100100101010101101001001001011100100000100010100001000001110010000100001010000101010111011100000000101010000101111100110000011010010001001110010111110100101001011100000001111001010010110101110111001010100101001010001101000010010010000111010001110111101100",
        "1000101000000000100010000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    };

    for (int coreIndex = 0; coreIndex < NUM_CORES; coreIndex++) {
        printf("Processing Core %d:\n", coreIndex);

        // Read neuron data and calculate synaptic matrix and neuron parameters
        if (getNeuronData(&cores[coreIndex])) {
            return 1; // Error in reading data
        }

        // Send data to wishbone bus (simulated for now)
        uint32_t synapseData[AXONS * NEURONS_PER_CORE];
        uint32_t neuronParameters[NUM_CORES][NEURONS_PER_CORE][4];
        uint32_t inputSpikes[AXONS];

        // Pack synaptic connections into a 1D array for demonstration
        for (int axon = 0; axon < AXONS; axon++) {
            for (int neuron = 0; neuron < NEURONS_PER_CORE; neuron++) {
                synapseData[axon * NEURONS_PER_CORE + neuron] = cores[coreIndex].synapse_connections[axon][neuron];
            }
        }

        // Pack neuron parameters into a 3D array for demonstration
        for (int neuron = 0; neuron < NEURONS_PER_CORE; neuron++) {
            for (int param = 0; param < 4; param++) {
                neuronParameters[coreIndex][neuron][param] = binaryParameters[coreIndex][neuron][param];
            }
        }

        // Pack input spikes into a 1D array for demonstration
        for (int axon = 0; axon < AXONS; axon++) {
            inputSpikes[axon] = cores[coreIndex].spikeQueue.array[axon];
        }

        // Simulate sending data to wishbone bus
        sendToWishboneBus(synapseData, neuronParameters[coreIndex][0], inputSpikes, &cores[coreIndex].output_axons[0]);

        // Process the output spikes
        for (int axon = 0; axon < NEURONS_PER_CORE; axon++) {
            printf("%d", cores[coreIndex].output_axons[axon]);
        }
        printf("\n");
    }

    return 0;
}
