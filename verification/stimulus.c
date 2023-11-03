#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define AXONS 256
#define NEURONS_PER_CORE 32
#define NUM_CORES 5
#define FIFO_SIZE 256

typedef struct {
    uint8_t current_membrane_potential;
    uint8_t reset_posi_potential;
    uint8_t reset_nega_potential;
    uint8_t weights[4];
    uint8_t leakage_value;
    uint8_t positive_threshold;
    uint8_t negative_threshold;
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

int getNeuronData(SNNCore cores[]) {
    FILE* file = fopen("neuron_data.txt", "r");
    if (file == NULL) {
        printf("Unable to open file.\n");
        return 1;
    }
    char line[AXONS + 84];
    int coreIndex = 0, neuronIndex = 0;
    while (fgets(line, sizeof(line), file)) {
        readNeuronData(&cores[coreIndex], line, neuronIndex);
        neuronIndex++;
        if (neuronIndex >= NEURONS_PER_CORE) {
            neuronIndex = 0;
            coreIndex++;
            if (coreIndex >= NUM_CORES) break;
        }
    }
    fclose(file);
    return 0;
}
void processSpikeEvent(SNNCore* core,int axonIndex) {
    if (!isEmpty(&core->spikeQueue)) {
        int axon_index = dequeue(&core->spikeQueue);
        for (int i = 0; i < NEURONS_PER_CORE; i++) {
            if (core->synapse_connections[axon_index][i]) {
                Neuron *neuron = &core->neurons[i];

                // Increase membrane potential by synaptic weight
                neuron->current_membrane_potential += neuron->weights[i % 4];
                // Subtract leakage value from membrane potential
                neuron->current_membrane_potential -= neuron->leakage_value;

                // Check against thresholds
                if (neuron->current_membrane_potential > neuron->positive_threshold) {
                    neuron->current_membrane_potential = neuron->reset_posi_potential;
                    core->output_axons[i] = 1;
                    printf("Neuron %d fired spike at axon index %d\n", i, neuron->destination_axon);
                    enqueue(&core->spikeQueue, neuron->destination_axon);
                } else if (neuron->current_membrane_potential < neuron->negative_threshold) {
                    neuron->current_membrane_potential = neuron->reset_nega_potential;
                }
            }
        }
    }
}


void initializeCoreQueues() {
    for (int i = 0; i < NUM_CORES; i++) {
        cores[i].spikeQueue = *createQueue(FIFO_SIZE);
    }
}

void loadSpikesToQueue(const char* input_spike[NUM_CORES]) {
    for (int core = 0; core < NUM_CORES; core++) {
        for (int axon = 0; axon < AXONS; axon++) {
            if (input_spike[core][axon] == '1') {
                enqueue(&cores[core].spikeQueue, axon);
            }
        }
    }
}

int main() {
    const char* input_spikes[5] = {
        "1010010000110001000010101010111011100111010100000110101000111000100100100011101010101111101101001010101111101000100110101000111010101010101001011001001001101010001010100110111010101111101010100111011111000001100011111001100101011100101010000001011000011001",
        "1111111010001010010010100010101111110000101000001011000110100100011010110100101100001000101100101010010010100100111110110110101100001010111001110110101010110110101010111001011011111000000010001001001101010100101000101000100010100101100001111011000001011110",
        "1100101010100000101100101001000010001001011010110110101010100000011101101001010010101001000000010110100101101010111110100101010100011111101000010010110010010101010110101101001010000001010100111001111110010100100001101010110011001100100100001101010000000011",
        "1100100101010101101001001001011100100000100010100001000001110010000100001010000101010111011100000000101010000101111100110000011010010001001110010111110100101001011100000001111001010010110101110111001010100101001010001101000010010010000111010001110111101100",
        "1010101010101010101001011001101010111110101010010110101111010010111000110100010001011000011010010101011101000111001111101011000101010101010011011100100011010101000101011010110110001101010100010111010010001110010110110101001000001101111110101100011100011001"
    };

    if (getNeuronData(cores)) {
        return 1; // Error in reading data
    }

    initializeCoreQueues();
    loadSpikesToQueue(input_spikes);

    // int done = 0;
    // while (!done) {
        // done = 1;
        for (int i = 0; i < NUM_CORES; i++) {
            printf("Core %d:\n",i);
            // while (!isEmpty(&cores[i].spikeQueue)) {
                int axonIndex = dequeue(&cores[i].spikeQueue);
                processSpikeEvent(&cores[i],axonIndex);
                 printf("Output spikes for Core %d:\n", i);
                for(int axon = 0; axon < 32; axon++) {
                    printf("%d", cores[i].output_axons[axon]);
                }
                printf("\n");
                // done = 0;
            }
        // }
    // }
    // for (int i = 0; i < NUM_CORES; i++) {
    // printf("Core %d:\n", i);
//     for (int j = 0; j < NEURONS_PER_CORE; j++) {
//         Neuron* neuron = &cores[i].neurons[j];
//         printf("Neuron %d:\n", j);
//         printf("Current Membrane Potential: %d\n", neuron->current_membrane_potential);
//         printf("Reset Potential: %d\n", neuron->reset_potential);
//         printf("Leakage Value: %d\n", neuron->leakage_value);
//         printf("Positive Threshold: %d\n", neuron->positive_threshold);
//         printf("Negative Threshold: %d\n", neuron->negative_threshold);
//         printf("Destination Axon: %d\n", neuron->destination_axon);
//         printf("Weights: ");
//         for (int k = 0; k < 4; k++) {
//             printf("%d ", neuron->weights[k]);
//         }
//         printf("\n");
//         printf("Synapse Connections: ");
//         for (int k = 0; k < AXONS; k++) {
//             printf("%d ", cores[i].synapse_connections[k][j]);
//         }
//         printf("\n");
//         printf("\n");
//     }
// }

    return 0;
}
