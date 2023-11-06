module AddressDecoder (
    input [31:0] addr,
    output reg synap_matrix,
    output reg [4:0] param_num,   // Will be valid only if address is in param range
    output reg neuron_spike_out,
    output reg param
);

    always @(addr) begin
        // Default outputs to 0
        synap_matrix = 0;
        param = 0;
        param_num = 5'b0;
        neuron_spike_out = 0;

        // Decode based on addr[14:13]
        case(addr[14:13])
            2'b00: synap_matrix = 1;
            2'b01: begin
                param = 1;
                param_num = addr[8:4];
            end
            2'b10: neuron_spike_out = 1;
            default: ; // Do nothing, outputs remain 0
        endcase
    end

endmodule

