`timescale 1ns / 1ps

module tb_neuron_core_256x256;

parameter NUM_OUTPUT = 250; // Number of spikes
parameter NUM_PICTURE = 50; // Number of test images
parameter NUM_PACKET = 1444; // Number of input packets in file

logic clk;
logic rst;

// Wishbone interface signals
logic wbs_cyc_i;
logic wbs_stb_i;
logic wbs_we_i;
logic [3:0] wbs_sel_i;
logic [31:0] wbs_adr_i;
logic [31:0] wbs_dat_i;
wire wbs_ack_o;
wire [31:0] wbs_dat_o;

// Instantiate the Unit Under Test (UUT)
neuron_core_256x256 uut_neuron_core (
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

////////////////////////////////////////////////////////////////
// Define tasks for writing and reading from Wishbone        
////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////
// Read data from files using readmemh and readmemb functions 
// NOTE: Root directory is directory containing the work folder!
////////////////////////////////////////////////////////////////
// Read packet counts for each tick => 50x7
logic [6:0] num_pic [0:NUM_PICTURE - 1]; 
initial $readmemh("../train_output_files/tb_num_inputs.txt", num_pic);

// Read all packets (each packet contains 8-bit axon destination) => NUM_PACKETx8
logic [7:0] packet [0:NUM_PACKET - 1];
initial $readmemb("../train_output_files/tb_input_8bit_axon_dest_only.txt", packet);

// Read 256x256 synapse_matrix from file
logic [255:0] synapse_matrix [0:255]; // each 256-bit synap matrix row is read from right to left (descending array)
// logic [0:255] synapse_matrix [0:255]; // each 256-bit synap matrix row is read from left to right (ascending array)
initial $readmemb("../train_output_files/tb_synap_matrix.txt", synapse_matrix);

// Read 80-bit neuron_parameters for 256 neurons => 256x80
logic [79:0] neuron_parameters [0:255];
initial $readmemb("../train_output_files/tb_neuron_params.txt", neuron_parameters);

////////////////////////////////////////////////////////////////
// Initialize variables for writing to Wishbone             //
////////////////////////////////////////////////////////////////
logic [31:0] synap_base = 32'h30000000; // Base address for writing to the synapse_matrix
logic [31:0] synap_offset;
logic [31:0] param_base = 32'h30004000; // Base address for Neuron Parameters
logic [31:0] param_offset;
logic [79:0] current_neuron_param;

////////////////////////////////////////////////////////////////
// Define events for tracking the progress of the test       
////////////////////////////////////////////////////////////////
event synap_matrix_start, synap_matrix_done; 
event neuron_params_start, neuron_params_done;
event new_image_packet, last_image_packet;

// Initial block for reset and tests
initial begin //initial begin MUST NOT include initialization statements
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

    // Write entire synapse_matrix to Wishbone
    #20 -> synap_matrix_start;
    for (int row = 0; row < 256; row++) begin
        for (int synap_segment = 0; synap_segment < 8; synap_segment++) begin
            synap_offset = (row * 32) + (synap_segment * 4);
            wishbone_write(synap_base + synap_offset, synapse_matrix[row][synap_segment*32 +: 32]);
        end
    end
    -> synap_matrix_done;

    // Write entire neuron_parameters to Wishbone
    #20 -> neuron_params_start;
    for (int neuron_idx = 0; neuron_idx < 256; neuron_idx++) begin
        current_neuron_param = neuron_parameters[neuron_idx];
        param_offset = neuron_idx * 16;

        // Write the first 32-bit segment
        wishbone_write(param_base + param_offset, current_neuron_param[0+:32]);

        // Write the second 32-bit segment
        wishbone_write(param_base + param_offset + 4, current_neuron_param[32+:32]);

        // Write the remaining 16-bit segment, padded to 32 bits
        wishbone_write(param_base + param_offset + 8, {16'b0, current_neuron_param[64+:16]});
    end
    -> neuron_params_done;


    // // Write zeros to neuron_spike_out
    // for (int i = 32'h30008000; i <= 32'h30008003; i = i + 4) begin
    //     wishbone_write(i, 32'b0);
    // end

    // Send packets in (8-bit axon dest) by read from wishbone the corresponding axon dest of the synapse matrix
    // for (int i = 0; i < NUM_PICTURE; i++) begin
        
    // end
    // Test by sending packets of 1st image
    #20 -> new_image_packet;
    wishbone_read(32'h3000C000 + num_pic[0]); // new_image_packet = 1
    for (int j = 0; j < num_pic[0]; j++) begin
        wishbone_read(32'h30000000 + packet[j]*32); // Each synap connection of a neuron takes up 32 memory locations
    end
    -> last_image_packet;
    

    // Complete the test
    $display("Test Completed.");
    $finish;        
end


endmodule
