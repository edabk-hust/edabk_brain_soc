
`timescale 1ns / 1ps

module neuron_core_tb;

    reg clk;
    reg rst;

    // Wishbone interface signals
    reg wbs_cyc_i;
    reg wbs_stb_i;
    reg wbs_we_i;
    reg [3:0] wbs_sel_i;
    reg [31:0] wbs_adr_i;
    reg [31:0] wbs_dat_i;
    wire wbs_ack_o;
    wire [31:0] wbs_dat_o;

    // Variables
    integer i;

    // Instantiate the Unit Under Test (UUT)
    neuron_core uut_neuron_core (
        .clk(clk),
        .rst(rst),
        .wbs_cyc_i(wbs_cyc_i),
        .wbs_stb_i(wbs_stb_i),
        .wbs_we_i(wbs_we_i),
        .wbs_sel_i(wbs_sel_i),
        .wbs_adr_i(wbs_adr_i),
        .wbs_dat_i(wbs_dat_i),
        .wbs_ack_o(wbs_ack_o),
        .wbs_dat_o(wbs_dat_o)
    );

    // Clock generation
    initial clk = 0;
    always #5 clk = ~clk; // Generate a clock with 100 MHz frequency

    // Task for writing to wishbone without waiting for acknowledgment
    task wishbone_write;
        input [31:0] address;
        input [31:0] data;
        begin
            @(negedge clk) begin
                wbs_cyc_i = 1'b1;
                wbs_stb_i = 1'b1;
                wbs_we_i  = 1'b1;
                wbs_sel_i = 4'b1111;
                wbs_adr_i = address;
                wbs_dat_i = data;
            end
            @(negedge clk) begin
                wbs_cyc_i = 1'b0;
                wbs_stb_i = 1'b0;
                wbs_we_i  = 1'b0;
                wbs_sel_i = 4'b0000;
            end
        end
    endtask

    // Task for reading from wishbone without waiting for acknowledgment
    task wishbone_read;
        input [31:0] address;
        begin
            @(negedge clk) begin
                wbs_cyc_i = 1'b1;
                wbs_stb_i = 1'b1;
                wbs_we_i  = 1'b0;
                wbs_sel_i = 4'b1111;
                wbs_adr_i = address;
            end
            @(negedge clk) begin
                wbs_cyc_i = 1'b0;
                wbs_stb_i = 1'b0;
            end
        end
    endtask

    // Initial block for reset and tests
    initial begin
        // Initialize Inputs
        rst = 1'b1; // Start with reset asserted
        wbs_cyc_i = 1'b0;
        wbs_stb_i = 1'b0;
        wbs_we_i = 1'b0;
        wbs_sel_i = 4'b0000;
        wbs_adr_i = 32'b0;
        wbs_dat_i = 32'b0;

        // Release the reset signal
        #20 rst = 1'b0;
        #30;

        // Write random values to synapse_matrix
        for (i = 32'h30000000; i <= 32'h30003FFF; i = i + 4) begin
            wishbone_write(i, $random);
        end

        // Write values to neuron_parameters
        for (i = 32'h30004000; i <= 32'h3000401EB; i = i + 16) begin
            wishbone_write(i, 32'h0CFC0000); // First word
            wishbone_write(i + 4, $random); // Second word
            wishbone_write(i + 8, {2'b00, $random} & 32'h30F0300); // Third word
            wishbone_write(i + 12, $random); // Fourth word
        end

        // Write zeros to neuron_spike_out
        for (i = 32'h30008000; i <= 32'h30008003; i = i + 4) begin
            wishbone_write(i, 32'b0);
        end

        // Read from random memory words in the synapse_matrix range 20 times
        for (i = 0; i < 20; i = i + 1) begin
            wishbone_read(32'h30000000 + ($random % 4096));
        end

        // Complete the test
        $display("Test Completed.");
        $finish;
    end

endmodule
