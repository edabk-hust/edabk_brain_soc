module neuron_block (
    input [7:0] voltage_potential_i, // Current voltage potential
    input [7:0] pos_threshold_i,     // Positive threshold
    input [7:0] neg_threshold_i,     // Negative threshold
    input [7:0] leak_value_i,        // Leak value
    input [7:0] weight_type1_i,      // 1st weight type
    input [7:0] weight_type2_i,      // 2nd weight type
    input [7:0] weight_type3_i,      // 3rd weight type
    input [7:0] weight_type4_i,      // 4th weight type
    input [7:0] weight_select_i,     // Weight selection
    input [7:0] pos_reset_i,         // Positive reset
    input [7:0] neg_reset_i,         // Negative reset
    input enable_i,                  // Enable signal
    output reg [7:0] new_potential_o, // New voltage potential - changed to reg
    output reg spike_o               // Spike output (1-bit) - changed to reg
);

    reg [7:0] selected_weight;
    reg [8:0] potential_calc; // changed to reg

    // Adjusted Weight selection
    always @(*) begin
        case(weight_select_i)
            8'd0: selected_weight = weight_type1_i;
            8'd1: selected_weight = weight_type2_i;
            8'd2: selected_weight = weight_type3_i;
            8'd3: selected_weight = weight_type4_i;
            default: selected_weight = 8'b0;
        endcase
    end

    // Neuron logic
    always @(*) begin
        if (enable_i == 1) begin
            // Calculate potential
            potential_calc = {1'b0, voltage_potential_i} + selected_weight - leak_value_i;

            // Handle potential overflow/underflow
            if (potential_calc[8]) new_potential_o = 8'b00000000; // Underflow
            else if (potential_calc > 8'b11111111) new_potential_o = 8'b11111111; // Overflow
            else new_potential_o = potential_calc[7:0];

            // Check for spike
            if (new_potential_o >= pos_threshold_i) begin
                new_potential_o = pos_reset_i;
                spike_o = 1;
            end else if (new_potential_o <= neg_threshold_i) begin
                new_potential_o = neg_reset_i;
                spike_o = 0;
            end else begin
                spike_o = 0;
            end
        end else begin
            new_potential_o = 8'b0;
            spike_o = 0;
        end
    end

endmodule
