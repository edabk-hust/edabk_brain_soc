#ifndef SNN_DATA_H
#define SNN_DATA_H

#define NUM_CORES 5
#define NEURONS_PER_CORE 32
#define AXONS_PER_CORE 256

typedef struct {
    int8_t membrane_potential;
    int8_t reset_posi_potential;
    int8_t reset_nega_potential;
    int8_t weights[4];
    int8_t leakage_value;
    int8_t positive_threshold;
    int8_t negative_threshold;
    uint8_t axon_dest;
} Neuron;

typedef struct {
    Neuron neurons[NEURONS_PER_CORE];
    uint32_t synapse_connection[AXONS_PER_CORE];
} Core;

typedef struct {
    int8_t dx;
    int8_t dy;
    uint8_t axon_dest;
} Packet;

extern Packet packets[1024];
extern Core cores[NUM_CORES];

#endif