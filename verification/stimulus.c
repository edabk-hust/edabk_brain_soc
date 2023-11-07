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


void printSynapticConnections(SNNCore* core) {
    uint8_t transposed_matrix[AXONS][NEURONS_PER_CORE];
    // printf("Transposed synaptic connections for Core:\n");
    
    // Transpose synaptic connections into a 256x32 matrix
    for (int neuronIndex = 0; neuronIndex < NEURONS_PER_CORE; neuronIndex++) {
        for (int axonIndex = 0; axonIndex < AXONS; axonIndex++) {
            transposed_matrix[axonIndex][neuronIndex] = core->synapse_connections[axonIndex][neuronIndex];
        }
    }

    // Print the transposed synaptic connections
    for (int axonIndex = 0; axonIndex < AXONS; axonIndex++) {
        for (int neuronIndex = 0; neuronIndex < NEURONS_PER_CORE; neuronIndex++) {
            printf("%d", transposed_matrix[axonIndex][neuronIndex]);
        }
        printf("\n");
    }
}
void saveSynapticConnections(SNNCore* cores, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Unable to open file %s for writing.\n", filename);
        return;
    }

    for (int coreIndex = 0; coreIndex < NUM_CORES; coreIndex++) {
        for (int axonIndex = 0; axonIndex < AXONS; axonIndex++) {
            for (int neuronIndex = 0; neuronIndex < NEURONS_PER_CORE; neuronIndex++) {
                fprintf(file, "%d", cores[coreIndex].synapse_connections[axonIndex][neuronIndex]);
            }
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

void packParameters(uint32_t *row, uint32_t p1, uint32_t p2, uint32_t p3, uint32_t p4) {
    *row = (p1 << 24) | (p2 << 16) | (p3 << 8) | p4;
}
void saveNeuronParameters(SNNCore* cores, uint32_t binaryParameters[NUM_CORES][NEURONS_PER_CORE][4]) {
    for (int c = 0; c < NUM_CORES; c++) {
        for (int n = 0; n < NEURONS_PER_CORE; n++) {
            packParameters(&binaryParameters[c][n][0],
                cores[c].neurons[n].current_membrane_potential & 0xFF,
                cores[c].neurons[n].reset_posi_potential & 0xFF,
                cores[c].neurons[n].reset_nega_potential & 0xFF,
                cores[c].neurons[n].weights[0] & 0xFF);

            packParameters(&binaryParameters[c][n][1],
                cores[c].neurons[n].weights[1] & 0xFF,
                cores[c].neurons[n].weights[2] & 0xFF,
                cores[c].neurons[n].weights[3] & 0xFF,
                cores[c].neurons[n].leakage_value & 0xFF);

            packParameters(&binaryParameters[c][n][2],
                cores[c].neurons[n].positive_threshold & 0xFF,
                cores[c].neurons[n].negative_threshold & 0xFF,
                cores[c].neurons[n].destination_axon & 0xFF,
                0); // Last 8 bits are zeros as specified

            binaryParameters[c][n][3] = 0; // Fourth row is all zeros
        }
    }
}

void saveBinaryParametersToFile(const char* filename, uint32_t binaryParameters[NUM_CORES][NEURONS_PER_CORE][4]) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Không thể mở tệp để ghi dữ liệu.\n");
        return;
    }

    for (int c = 0; c < NUM_CORES; c++) {
        for (int n = 0; n < NEURONS_PER_CORE; n++) {
            for (int i = 0; i < 4; i++) {
                uint32_t value = binaryParameters[c][n][i];
                
                for (int bit = 31; bit >= 0; bit--) {
                    int bitValue = (value >> bit) & 1;
                    fprintf(file, "%d", bitValue);
                }
                fprintf(file, "\n"); 
            }
        }
    }

    fclose(file);
}

void printNeuronParameters(uint32_t binaryParameters[NUM_CORES][NEURONS_PER_CORE][4]) {
    for (int c = 0; c < NUM_CORES; c++) {
        for (int n = 0; n < NEURONS_PER_CORE; n++) {
            printf("Core %d, Neuron %d:\n", c, n);
            for (int i = 0; i < 4; i++) {
                for (int bit = 31; bit >= 0; bit--) {
                    printf("%u", (binaryParameters[c][n][i] >> bit) & 1);
                    // if (bit % 8 == 0) printf(" "); // Separator for visual aid
                }
                printf("\n");
            }
            printf("\n");
        }
    }
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

                neuron->current_membrane_potential += neuron->weights[i % 4];
                neuron->current_membrane_potential -= neuron->leakage_value;

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

void printQueue(Queue* queue) {
    printf("Queue contents: ");
    for (int i = queue->front; i <= queue->rear; i++) {
        printf("%d ", queue->array[i]);
    }
    printf("\n");
}

void initializeCoreQueues() {
    for (int i = 0; i < NUM_CORES; i++) {
        cores[i].spikeQueue = *createQueue(FIFO_SIZE);
    }
}

Queue* loadSpikesToQueue(const char* input_spike[NUM_CORES]) {
    Queue* lastSpikeQueue = NULL;
    int count_input = 0;

   FILE* output_file = fopen("input_spike.txt", "w");
    if (output_file == NULL) {
        printf("Không thể mở tệp input_spike.txt để ghi.\n");
        return NULL;
    }

    for (int core = 0; core < NUM_CORES; core++) {
        int total_spikes = 0;
        for (int j = 0; j < AXONS; ++j) {
            if (input_spike[core][j] == '1') {
                total_spikes ++;
            }
        }

        // Ghi tổng vào đầu file cho từng core
        fprintf(output_file, "%d\n", total_spikes);

        for (int axon = 0; axon < AXONS; axon++) {
            if (input_spike[core][axon] == '1') {
                count_input++;
                enqueue(&cores[core].spikeQueue, axon);
                fprintf(output_file, "%d ", axon); 
            }
        }
        lastSpikeQueue = &cores[core].spikeQueue; 
        fprintf(output_file, "\n"); 
    }

    fclose(output_file); 

    return lastSpikeQueue;
}

void saveOutputAxons(SNNCore* cores, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Unable to open file %s for writing.\n", filename);
        return;
    }

    for (int axonIndex = 0; axonIndex < NEURONS_PER_CORE; axonIndex++) {
        fprintf(file, "%d", cores[4].output_axons[axonIndex]);
    }
    fprintf(file, "\n");

    fclose(file);
}

int main() {
    const char* input_spikes[5] = {
        "1010010000110001000010101010111011100111010100000110101000111000100100100011101010101111101101001010101111101000100110101000111010101010101001011001001001101010001010100110111010101111101010100111011111000001100011111001100101011100101010000001011000011001",
        "1111111010001010010010100010101111110000101000001011000110100100011010110100101100001000101100101010010010100100111110110110101100001010111001110110101010110110101010111001011011111000000010001001001101010100101000101000100010100101100001111011000001011110",
        "1100101010100000101100101001000010001001011010110110101010100000011101101001010010101001000000010110100101101010111110100101010100011111101000010010110010010101010110101101001010000001010100111001111110010100100001101010110011001100100100001101010000000011",
        "1100100101010101101001001001011100100000100010100001000001110010000100001010000101010111011100000000101010000101111100110000011010010001001110010111110100101001011100000001111001010010110101110111001010100101001010001101000010010010000111010001110111101100",
        "1000101000000000100010000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    };

    if (getNeuronData(cores)) {
        return 1; // Error in reading data
    }

    initializeCoreQueues();
    loadSpikesToQueue(input_spikes);

    for (int i = 0; i < NUM_CORES; i++) {
            int axonIndex = dequeue(&cores[i].spikeQueue);
            processSpikeEvent(&cores[i],axonIndex);
                printf("Output spikes for Core %d:\n", i);
            printf("\n");
    }
    for(int axon = 0; axon < 32; axon++) {
            printf("%d", cores[4].output_axons[axon]);
    }
    saveSynapticConnections(cores, "synaptic_connections.txt");
    
    FILE* file = fopen("input_spike.txt", "w");
    if (!file) {
        perror("Error opening file");
        return -1;
    }

    for (int i = 0; i < NUM_CORES; ++i) {
        
    }

    Queue* lastSpikeQueue = loadSpikesToQueue(input_spikes);
    
    for (int i = 0; i < NUM_CORES; i++) {
        printf("Core %d Synaptic Connections:\n", i);
        printSynapticConnections(&cores[i]);
        printf("\n");
    }
    uint32_t binaryParameters[NUM_CORES][NEURONS_PER_CORE][4];
     for (int coreIndex = 0; coreIndex < NUM_CORES; coreIndex++) {
        printf("Parameters for Core %d Neurons:\n", coreIndex);

        for (int neuronIndex = 0; neuronIndex < NEURONS_PER_CORE; neuronIndex++) {
            // Save the neuron parameters into the matrix
            saveNeuronParameters(cores, binaryParameters);
            saveBinaryParametersToFile("neuron_parameter.txt", binaryParameters);

            // Print the neuron parameters matrix
            printNeuronParameters(binaryParameters);
        }
    }
    
    saveOutputAxons(cores, "output_axons.txt");
    

    return 0;
}