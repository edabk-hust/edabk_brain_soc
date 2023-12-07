# Hardware/Software Co-Simulation of SNN Chip

We run the `stimulus.c` file to process `neuron_data.txt` file as input, which contains information of neuron cores. The neuron cores' data is trained in advance in Tensorflow. The stimulus file would output sequentially:

- information of a virtual core (synapse_matrix, neuron_parameters) into the physical core in SoC.
- the 256-bit input spike of that corresponding core
    
After the core returns output_spike of 32 neurons (via Wishbone bus), the C Code would save the output_spike into a Queue and repeat the above steps (send data of a virtual core to SoC => send input spike of that core to SoC => save returned output_spike )

### `neuron_data.txt`
- The neuron_data.txt file contains 160 lines, grouped into 5 cores (each core has 32 neurons => 32 lines)
- Each line has total 336-bits, containing information of 1 neuron
    + 256-bit: synap connection (will be sent to synapse_matrix of DUT)
    + **Next 80-bit will be processed to send 11 fields to neuron_parameter of DUT**
        + (1)8-bit: current_membrane_potential
        
        We let reset_posi_potential =  reset_nega_potential, hence we need only 8-bit in neuron_data.txt to represent (2)&(3)

        + (2)8-bit: reset_posi_potential 
        + (3)8-bit: reset_nega_potential
        + (4)8-bit: weights[0]
        + (5)8-bit: weights[1]
        + (6)8-bit: weights[2]
        + (7)8-bit: weights[3]
        + (8)8-bit: leakage_value
        + (9)8-bit: positive_threshold
        + (10)8-bit: negative_threshold
        + (11)8-bit: destination_axon
    
    


