`timescale 1ns / 1ps

module neuron_spike_out_tb;

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
    
    reg [31:0] tb_external_spike_data;
    reg tb_external_write_en;
    
    // Single 32-bit register to hold the written value for verification
    reg [31:0] memory; // Single word memory
    
    // Error counters
    integer total_errors = 0;
    integer total_tests = 0;

    // Instantiate the Unit Under Test (UUT)
    neuron_spike_out uut_spike (
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
        .external_spike_data_i(tb_external_spike_data),
        .external_write_en_i(tb_external_write_en)
    );

    // Clock generation with negedge active
    always #5 tb_clk = !tb_clk; // Generate a clock with a period of 10ns (100MHz)

    // Testbench main task
    initial begin
        // Initialize the testbench signals
        tb_clk = 1;
        tb_rst = 1;
        tb_cyc = 0;
        tb_stb = 0;
        tb_we = 0;
        tb_sel = 4'b1111; // All byte lanes valid
        tb_adr = 32'h30008000;
        tb_dat = 0;
        tb_external_spike_data = 0;
        tb_external_write_en = 0;

        // Assert reset for a few cycles
        #40;
        tb_rst = 0;
        #20;

        // Start the tests after reset deassertion
        write_wishbone_test();
        read_wishbone_test();
        external_write_test();

        // Report total errors
        $display("Error: %0d/%0d (test)", total_errors, total_tests);

        $stop; // Stop simulation
    end

    // Task to perform write test
    task write_wishbone_test;
        begin
            tb_dat = $random; // Generate a random data
            memory = tb_dat; // Store data for later verification
            
            // Write cycle using negedge of the clock
            @(negedge tb_clk) begin
                tb_cyc <= 1;
                tb_stb <= 1;
                tb_we <= 1;
                tb_adr <= 32'h30008000; // Base address of the module
                tb_dat <= tb_dat;
            end
            
            // Wait for acknowledgment
            @(posedge tb_clk); // Check for ack at the posedge as the UUT works on negedge
            while (!tb_ack) @(posedge tb_clk);

            // Deassert signals
            @(negedge tb_clk) begin
                tb_cyc <= 0;
                tb_stb <= 0;
                tb_we <= 0;
            end
            
            // Increment test count
            total_tests = total_tests + 1;
        end
    endtask

    // Task to perform read test
    task read_wishbone_test;
        begin
            // Read cycle using negedge of the clock
            @(negedge tb_clk) begin
                tb_cyc <= 1;
                tb_stb <= 1;
                tb_we <= 0;
                tb_adr <= 32'h30008000; // Base address of the module
            end
            
            // Wait for acknowledgment and read data
            @(posedge tb_clk); // Check for ack and read data at the posedge as the UUT works on negedge
            while (!tb_ack) @(posedge tb_clk);

            // Check if the read data matches the written data
            if (tb_dat_o !== memory) begin
                $display("Read mismatch on address %h: %h expected, got %h", tb_adr, memory, tb_dat_o);
                total_errors = total_errors + 1;
            end

            // Deassert signals
            @(negedge tb_clk) begin
                tb_cyc <= 0;
                tb_stb <= 0;
            end

            // Increment test count
            total_tests = total_tests + 1;
        end
    endtask

    // Task to perform external write test
    task external_write_test;
        begin
            tb_external_spike_data = $random; // Generate a random data
            tb_external_write_en = 1'b1; // Enable external write
            
            // Apply the external write at negedge of the clock
            @(negedge tb_clk) begin
                // External write happens
            end

            // Disable the write enable signal
            @(negedge tb_clk) begin
                tb_external_write_en <= 0;
            end

            // Perform a read cycle using negedge of the clock
            @(negedge tb_clk) begin
                tb_cyc <= 1;
                tb_stb <= 1;
                tb_we <= 0;
                tb_adr <= 32'h30008000; // Base address of the module
            end
            
            // Wait for acknowledgment and read data
            @(posedge tb_clk); // Check for ack and read data at the posedge as the UUT works on negedge
            while (!tb_ack) @(posedge tb_clk);

            // Check if the read data matches the external write data
            if (tb_dat_o !== tb_external_spike_data) begin
                $display("External write mismatch: %h expected, got %h", tb_external_spike_data, tb_dat_o);
                total_errors = total_errors + 1;
            end

            // Deassert signals
            @(negedge tb_clk) begin
                tb_cyc <= 0;
                tb_stb <= 0;
            end

            // Increment test count
            total_tests = total_tests + 1;
        end
    endtask

endmodule

