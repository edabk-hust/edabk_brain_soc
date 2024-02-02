
`timescale 1ns / 1ps

module tb_neuron_core_256x256;

parameter NUM_OUTPUT = 250; // Số spike bắn ra
parameter NUM_PICTURE = 50;//10000; // Số ảnh test
parameter NUM_PACKET = 1444;//341397; // số lượng input packet trong file

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

// đọc số lượng packet trong mỗi tick => 50x7
reg [6:0] num_pic [0:NUM_PICTURE - 1]; 
initial $readmemh("./train_output_files/tb_num_inputs.txt", num_pic);

// đọc tất cả các packet (packet chỉ chứa 8-bit axon destination) => NUM_PACKETx8
reg [7:0] packet [0:NUM_PACKET - 1];
initial $readmemb("./train_output_files/tb_input.txt", packet);

// đọc 256x256 synapse_matrix từ file
reg [255:0] synapse_matrix [0:255];
initial $readmemb("./train_output_files/tb_synapse_matrix.txt", synapse_matrix);

// đọc 80-bit neuron_parameters cho 256 neurons => 256x80
reg [79:0] neuron_parameters [0:255];
initial $readmemb("./train_output_files/tb_neuron_parameters.txt", neuron_parameters);

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

    // Write entire synapse_matrix to Wishbone
    integer row, segment;
    reg [31:0] synap_base = 32'h30000000; // Base address for writing to the synapse_matrix
    reg [31:0] synap_offset;

    for (row = 0; row < 256; row = row + 1) begin
        for (segment = 0; segment < 8; segment = segment + 1) begin
            synap_offset = (row * 32) + (segment * 4);
            wishbone_write(synap_base + synap_offset, synapse_matrix[row][segment*32 +: 32]);
        end
    end

    // Write entire neuron_parameters to Wishbone
    integer neuron_idx, segment;
    reg [31:0] param_base = 32'h30004000; // Base address for Neuron Parameters
    reg [31:0] param_offset;
    reg [79:0] current_neuron_param;

    for (neuron_idx = 0; neuron_idx < 256; neuron_idx = neuron_idx + 1) begin
        current_neuron_param = neuron_parameters[neuron_idx];
        param_offset = neuron_idx * 16;

        // Write the first 32-bit segment
        wishbone_write(param_base + param_offset, current_neuron_param[79:48]);

        // Write the second 32-bit segment
        wishbone_write(param_base + param_offset + 4, current_neuron_param[47:16]);

        // Write the remaining 16-bit segment, padded to 32 bits
        wishbone_write(param_base + param_offset + 8, {current_neuron_param[15:0], 16'b0});
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
