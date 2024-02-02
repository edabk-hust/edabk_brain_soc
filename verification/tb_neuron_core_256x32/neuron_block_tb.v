`timescale 1ns / 1ps

module neuron_block_tb;

    // Inputs
    reg [7:0] voltage_potential_i;
    reg [7:0] pos_threshold_i;     // Fixed to 8'b11111100 for all test cases
    reg [7:0] neg_threshold_i;     // Fixed to 0 for all test cases
    reg [7:0] leak_value_i;        // Fixed to 8'b00001111 for all test cases
    reg [7:0] weight_type1_i;
    reg [7:0] weight_type2_i;
    reg [7:0] weight_type3_i;
    reg [7:0] weight_type4_i;
    reg [7:0] weight_select_i;     // Can only be 0, 1, 2, or 3
    reg [7:0] pos_reset_i;         // Fixed to 8'b00000011 for all test cases
    reg [7:0] neg_reset_i;         // Fixed to 0 for all test cases
    reg enable_i;

    // Outputs
    wire [7:0] new_potential_o;
    wire spike_o;

    // Instantiate the Unit Under Test (UUT)
    neuron_block uut_neuron_block (
        .voltage_potential_i(voltage_potential_i),
        .pos_threshold_i(pos_threshold_i),
        .neg_threshold_i(neg_threshold_i),
        .leak_value_i(leak_value_i),
        .weight_type1_i(weight_type1_i),
        .weight_type2_i(weight_type2_i),
        .weight_type3_i(weight_type3_i),
        .weight_type4_i(weight_type4_i),
        .weight_select_i(weight_select_i),
        .pos_reset_i(pos_reset_i),
        .neg_reset_i(neg_reset_i),
        .enable_i(enable_i),
        .new_potential_o(new_potential_o),
        .spike_o(spike_o)
    );

    // Error tracking
    integer error_count = 0;
    integer test_count = 0;

    // Test cases
    integer i;
    reg [8:0] expected_new_potential;
    reg expected_spike;

    // Algorithm to calculate expected new_potential_o and spike_o
    function [8:0] calculate_new_potential;
        input [7:0] voltage_potential;
        input [2:0] weight_select;
        input [7:0] weight1;
        input [7:0] weight2;
        input [7:0] weight3;
        input [7:0] weight4;
        input [7:0] leak;
        reg [7:0] weight;
        begin
            case (weight_select)
                0: weight = weight1;
                1: weight = weight2;
                2: weight = weight3;
                3: weight = weight4;
                default: weight = 8'b0;
            endcase
            // Calculate new potential, watch for overflow/underflow
            calculate_new_potential = {1'b0, voltage_potential} + weight - leak;
            // Check underflow
            if (calculate_new_potential[8] == 1'b1) begin
                calculate_new_potential = 9'b0;
            // Check overflow
            end else if (calculate_new_potential > 8'b11111111) begin
                calculate_new_potential = 9'b011111111;
            end
        end
    endfunction

    function calculate_spike;
        input [8:0] new_potential;
        input [7:0] pos_threshold;
        input [7:0] neg_threshold;
        begin
            // Determine if a spike should occur
            if (new_potential >= pos_threshold) begin
                calculate_spike = 1'b1;
            end else if (new_potential <= neg_threshold) begin
                calculate_spike = 1'b0;
            end else begin
                calculate_spike = 1'b0;
            end
        end
    endfunction

    // Testbench initial block
    initial begin
        // Initialize Inputs
        voltage_potential_i = 0;
        pos_threshold_i = 8'b11111100;
        neg_threshold_i = 0;
        leak_value_i = 8'b00001111;
        weight_type1_i = 0;
        weight_type2_i = 0;
        weight_type3_i = 0;
        weight_type4_i = 0;
        weight_select_i = 0;
        pos_reset_i = 8'b00000011;
        neg_reset_i = 0;
        enable_i = 0;

        // Wait for global reset
        #100;

        // Run test cases
        for (i = 0; i < 100; i = i + 1) begin
            // Generate random inputs
            voltage_potential_i = $random;
            weight_type1_i = $random;
            weight_type2_i = $random;
            weight_type3_i = $random;
            weight_type4_i = $random;
            weight_select_i = $random % 4; // Ensure only 0 to 3
            enable_i = $random % 2; // Randomly enable or disable

            // Calculate expected values

            expected_new_potential = calculate_new_potential(voltage_potential_i, weight_select_i, weight_type1_i, weight_type2_i, weight_type3_i, weight_type4_i, leak_value_i);
            expected_spike = calculate_spike(expected_new_potential, pos_threshold_i, neg_threshold_i);

            // Apply potential reset based on thresholds
            if (expected_new_potential >= pos_threshold_i) begin
                expected_new_potential = pos_reset_i;
            end else if (expected_new_potential <= neg_threshold_i) begin
                expected_new_potential = neg_reset_i;
            end

            // Apply the enable_i effect
            if (!enable_i) begin
                expected_new_potential = 0;
                expected_spike = 0;
            end

            // Update test count
            test_count = test_count + 1;

            // Stimulate the UUT
            #10; // Wait for changes to propagate

            // Check results
            if (new_potential_o !== expected_new_potential[7:0] || spike_o !== expected_spike) begin
                $display("Error at test #%d: Expected new_potential_o=%h, spike_o=%b; Got new_potential_o=%h, spike_o=%b",
                         test_count, expected_new_potential[7:0], expected_spike, new_potential_o, spike_o);
                error_count = error_count + 1;
            end
        end

        // Display test results
        $display("Error: %0d/%0d (test)", error_count, test_count);

        // Finish simulation
        $finish;
    end

endmodule

