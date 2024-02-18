/*
 * Memory Mapping
 * Address Range                | Address[15:14] | Description
 * -----------------------------------------------------
 * 0x30000000 - 0x30001FFF      | 00            | Synapse matrix
 * Memory region for synapse matrix configuration.
 *
 * 0x30004000 - 0x30004FFF      | 01            | Neuron Parameters
 * Region for setting neuron parameters.
 * Address[11:4] Decoding for determining the neuron index of parameter
 *
 * 0x30008000 - 0x30008003      | 10            | Neuron spike out
 * Output for neuron spike events.
 *
 * 0x3000C000 - 0x3000C0FF      | 11            | Spike event controller
 ** Address[7:0] for decoding number of packets of current image
 */
module AddressDecoder_256x256 (
    input [31:0] addr,
    output reg synap_matrix,
    output reg param,
    output reg [7:0] param_num,   // Will be valid only if address is in param range
    output reg neuron_spike_out,
    output reg image_spike_event,
    output reg [7:0] image_num_packets // Store number of packets of the current image
);

always @(addr) begin
    // Default outputs to 0
    synap_matrix = 0;
    param = 0;
    param_num = 8'b0;
    neuron_spike_out = 0;
    image_spike_event = 0;
    image_num_packets = 8'b0;

    // MODIFIED: Decode based on addr[15:14], not addr[14:13]!
    case(addr[15:14])
        2'b00: begin
            synap_matrix = 1;
        end
        2'b01: begin
            param = 1;
            param_num = addr[11:4]; // Param num takes 8 bit to represent 256 params
        end
        2'b10: neuron_spike_out = 1;
        2'b11: begin 
            image_spike_event = 1;
            image_num_packets = addr[7:0];
        end
        default: ; // Do nothing, outputs remain 0
    endcase
end

endmodule
