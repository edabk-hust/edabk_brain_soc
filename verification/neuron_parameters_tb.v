`timescale 1ns / 1ps

module neuron_parameters_tb;

    // Testbench Signals
    reg tb_clk;
    reg tb_rst;
    reg tb_cyc;
    reg tb_stb;
    reg tb_we;
    reg [3:0] tb_sel;
    reg [31:0] tb_adr;
    reg [31:0] tb_dat;
    wire tb_ack;
    wire [31:0] tb_dat_o;
    
    reg [7:0] tb_ext_voltage_potential;
    reg tb_ext_write_enable;
    
    wire [7:0] tb_voltage_potential;
    wire [7:0] tb_pos_threshold;
    wire [7:0] tb_neg_threshold;
    wire [7:0] tb_leak_value;
    wire [7:0] tb_weight_type1;
    wire [7:0] tb_weight_type2;
    wire [7:0] tb_weight_type3;
    wire [7:0] tb_weight_type4;
    wire [7:0] tb_weight_select;
    wire [7:0] tb_pos_reset;
    wire [7:0] tb_neg_reset;
    
    // Memory to hold the written values for verification
    reg [31:0] parameter_memory[0:2];
    
    // Instantiate the Device Under Test (DUT)
    neuron_parameters uut_param (
        .wb_clk_i(tb_clk),
        .wb_rst_i(tb_rst),
        .wbs_cyc_i(tb_cyc),
        .wbs_stb_i(tb_stb),
        .wbs_we_i(tb_we),
        .wbs_sel_i(tb_sel),
        .wbs_adr_i(tb_adr),
        .wbs_dat_i(tb_dat),
        .wbs_ack_o(tb_ack),
        .wbs_dat_o(tb_dat_o),
        .ext_voltage_potential_i(tb_ext_voltage_potential),
        .ext_write_enable_i(tb_ext_write_enable),
        .voltage_potential_o(tb_voltage_potential),
        .pos_threshold_o(tb_pos_threshold),
        .neg_threshold_o(tb_neg_threshold),
        .leak_value_o(tb_leak_value),
        .weight_type1_o(tb_weight_type1),
        .weight_type2_o(tb_weight_type2),
        .weight_type3_o(tb_weight_type3),
        .weight_type4_o(tb_weight_type4),
        .weight_select_o(tb_weight_select),
        .pos_reset_o(tb_pos_reset),
        .neg_reset_o(tb_neg_reset)
    );

    // Clock generation
    always #5 tb_clk = !tb_clk; // Generate a clock with a period of 10ns (100MHz)

    // Reset routine
    initial begin
        // Initialize the testbench signals
        tb_clk = 0;
        tb_rst = 1;
        tb_cyc = 0;
        tb_stb = 0;
        tb_we = 0;
        tb_sel = 4'b1111;
        tb_adr = 0;
        tb_dat = 0;
        tb_ext_voltage_potential = 0;
        tb_ext_write_enable = 0;

        // Assert reset for a few cycles
        #40;
        tb_rst = 0;
        #20;

        // Start the test after reset deassertion
        perform_write_test();
        perform_read_test();
        perform_external_write_test();

        $stop; // Stop simulation but keep the waveform open for debugging
    end

    // Task to perform write test
    task perform_write_test;
        integer i;
        reg [31:0] expected_data;
        begin
            for (i = 0; i < 3; i = i + 1) begin
                tb_dat = $random; // Generate a random data
                tb_adr = 32'h30004000 + (i * 4); // Calculate address
                parameter_memory[i] = tb_dat; // Store data to verify later
                
                // Write cycle
                @(negedge tb_clk) begin
                    tb_cyc <= 1;
                    tb_stb <= 1;
                    tb_we <= 1;
                end
                
                // Wait for acknowledgment
                @(posedge tb_clk);
                while (!tb_ack) @(posedge tb_clk);

                // Deassert signals
                @(negedge tb_clk) begin
                    tb_cyc <= 0;
                    tb_stb <= 0;
                    tb_we <= 0;
                end
                
                // Verification is done by checking the output ports after the write operation
                // This assumes immediate response to the write operation as per the specification
                @(posedge tb_clk); // Wait one clock cycle to capture outputs

                // Verify the outputs match the expected data
                expected_data = parameter_memory[i];
                if (i == 0) begin
                    if (tb_voltage_potential !== expected_data[7:0]) $display("Mismatch in voltage_potential: %h expected, got %h", expected_data[7:0], tb_voltage_potential);
                    if (tb_pos_threshold !== expected_data[15:8]) $display("Mismatch in pos_threshold: %h expected, got %h", expected_data[15:8], tb_pos_threshold);
                    if (tb_neg_threshold !== expected_data[23:16]) $display("Mismatch in neg_threshold: %h expected, got %h", expected_data[23:16], tb_neg_threshold);
                    if (tb_leak_value !== expected_data[31:24]) $display("Mismatch in leak_value: %h expected, got %h", expected_data[31:24], tb_leak_value);
                end else if (i == 1) begin
                    if (tb_weight_type1 !== expected_data[7:0]) $display("Mismatch in weight_type1: %h expected, got %h", expected_data[7:0], tb_weight_type1);
                    if (tb_weight_type2 !== expected_data[15:8]) $display("Mismatch in weight_type2: %h expected, got %h", expected_data[15:8], tb_weight_type2);
                    if (tb_weight_type3 !== expected_data[23:16]) $display("Mismatch in weight_type3: %h expected, got %h", expected_data[23:16], tb_weight_type3);
                    if (tb_weight_type4 !== expected_data[31:24]) $display("Mismatch in weight_type4: %h expected, got %h", expected_data[31:24], tb_weight_type4);
                end else if (i == 2) begin
                    if (tb_weight_select !== expected_data[7:0]) $display("Mismatch in weight_select: %h expected, got %h", expected_data[7:0], tb_weight_select);
                    if (tb_pos_reset !== expected_data[15:8]) $display("Mismatch in pos_reset: %h expected, got %h", expected_data[15:8], tb_pos_reset);
                    if (tb_neg_reset !== expected_data[23:16]) $display("Mismatch in neg_reset: %h expected, got %h", expected_data[23:16], tb_neg_reset);
                end
            end
        end
    endtask

    // Task to perform read test
    task perform_read_test;
        integer i;
        begin
            for (i = 0; i < 3; i = i + 1) begin
                tb_adr = 32'h30004000 + (i * 4); // Calculate address
                
                // Read cycle
                @(negedge tb_clk) begin
                    tb_cyc <= 1;
                    tb_stb <= 1;
                    tb_we <= 0;
                end
                
                // Wait for acknowledgment and for read data to become available
                @(posedge tb_clk);
                while (!tb_ack) @(posedge tb_clk);

                // Deassert signals
                @(negedge tb_clk) begin
                    tb_cyc <= 0;
                    tb_stb <= 0;
                end
                
                // Verify read data after one clock cycle delay
                @(posedge tb_clk);
                if (tb_dat_o !== parameter_memory[i]) $display("Read mismatch on address %h: %h expected, got %h", tb_adr, parameter_memory[i], tb_dat_o);
            end
        end
    endtask

    // Task to perform external write test
    task perform_external_write_test;
        begin
            // External write to voltage potential
            tb_ext_voltage_potential = $random;
            tb_ext_write_enable = 1'b1;

            // Apply the external write
            @(negedge tb_clk) begin
                // External write happens at the falling edge
            end
            
            // Verify the external write on the next rising edge
            @(posedge tb_clk);
            if (tb_voltage_potential !== tb_ext_voltage_potential) $display("Mismatch in voltage_potential after external write: %h expected, got %h", tb_ext_voltage_potential, tb_voltage_potential);

            // Reset external write enable
            tb_ext_write_enable = 1'b0;
        end
    endtask

endmodule
