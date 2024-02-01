#include <stdint.h>
#define NUM_CORES 5
#define NEURONS_PER_CORE 32
#define AXONS_PER_CORE 256
#define NEURONS_PER_CORE 32

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
    // uint8_t instruction[AXONS_PER_CORE];                // Instruction (2-bit) for axon to choose 1 in 4 weights
    uint32_t synapse_connection[AXONS_PER_CORE];        // A (256 x 32) binary matrix to represent synaptic connection
} Core;

////////////////////////////////////////////// Packet data structure
typedef struct {
    uint8_t axon_dest; 
    int8_t dx;
    int8_t dy;
} Packet;

// SNN data array initialization
Packet packet_data[1024];
Core core_data[NUM_CORES];