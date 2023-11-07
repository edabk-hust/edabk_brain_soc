`timescale 1ns / 1ps

module synapse_matrix_tb;

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
wire [31:0] tb_neuron_conn;

// Memory to hold the written values for verification
reg [31:0] memory[0:255];

// Error counter
integer error_count;
integer total_tests;

// Instantiate the Device Under Test (DUT)
synapse_matrix DUT (
    .wb_clk_i(tb_clk),
    .wb_rst_i(tb_rst),
    .wbs_cyc_i(tb_cyc),
    .wbs_stb_i(tb_stb),
    .wbs_we_i(tb_we),
    .wbs_sel_i(tb_sel),
    .wbs_adr_i(tb_adr),
    .wbs_dat_i(tb_dat),
    .wbs_ack_o(tb_ack),
    .wbs_dat_o(), // Not used in this module
    .neurons_connections_o(tb_neuron_conn)
);

// Clock generation
always #5 tb_clk = !tb_clk; // Generate a clock with a period of 10ns (100MHz)

// Reset routine
initial begin
    // Initialize the testbench signals and error counter
    tb_clk = 0;
    tb_rst = 1;
    tb_cyc = 0;
    tb_stb = 0;
    tb_we = 0;
    tb_sel = 4'b1111;
    tb_adr = 32'h30000000;
    tb_dat = 0;
    error_count = 0;
    total_tests = 0;

    // Assert reset for a few cycles
    #40;
    tb_rst = 0;
    #20;

    // Start testing after reset deassertion
    perform_write_read_test();
    #100;

    // Report total errors and total test cases
    $display("Test complete with %0d errors out of %0d tests.", error_count, total_tests);
    $stop; // Stop simulation but keep the waveform open for debugging
end

// Test routine to write and read
task perform_write_read_test;
    integer i;
    begin
        for (i = 0; i < 256; i = i + 1) begin
            // Random data to write
            tb_dat = $random;
            tb_adr = 32'h30000000 + (i << 2); // Word aligned addresses
            memory[i] = tb_dat; // Store the data for later verification

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
        end

        // Read and verify cycle
        for (i = 0; i < 256; i = i + 1) begin
            tb_adr = 32'h30000000 + (i << 2); // Word aligned addresses

            // Read cycle
            @(negedge tb_clk) begin
                tb_cyc <= 1;
                tb_stb <= 1;
                tb_we <= 0;
            end

            // Wait for acknowledgment and verify the output
            @(posedge tb_clk);
            while (!tb_ack) @(posedge tb_clk);
            total_tests = total_tests + 1;
            if (tb_neuron_conn !== memory[i]) begin
                $display("Error at address %h: expected %h, got %h", tb_adr, memory[i], tb_neuron_conn);
                error_count = error_count + 1;
            end

            // Deassert signals
            @(negedge tb_clk) begin
                tb_cyc <= 0;
                tb_stb <= 0;
            end
        end
    end
endtask

endmodule

