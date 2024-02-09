/* TODO: Signal first packet and last packet of an image

Approach 1: Have a pseudo block in memory mapping (32’b3000Cxxx ⇒ addr[15:14] = 2’b11),
            storing 1 event new_image_spike + 1 num_spikes value) 
            & Create a new module Controller to store controller states 
            new_image_spike, process_image_spike, last_image_spike, process_last_spike
Approach 2: Try logic analyzer to control new_image_packet and last_image_packet

*/
module neuron_core_256x256
(
    `ifdef USE_POWER_PINS
    inout vccd1,    // User area 1 1.8V supply
    inout vssd1,    // User area 1 digital ground
    `endif

    input  wire clk,
    input  wire rst,
    
    // Wishbone interface
    input  wire wbs_cyc_i,
    input  wire wbs_stb_i,
    input  wire wbs_we_i,
    input  wire [3:0] wbs_sel_i,
    input  wire [31:0] wbs_adr_i,
    input  wire [31:0] wbs_dat_i,
    output wire wbs_ack_o,
    output wire [31:0] wbs_dat_o
);

parameter SYNAPSE_BASE = 32'h30000000;  // Base address for Synapse Matrix
parameter PARAM_BASE = 32'h30004000;    // Base address for Neuron Parameters
parameter PADDING_PARAM = 32'h00000010; // Padding for Neuron Parameters
parameter SPIKE_OUT_BASE = 32'h30008000; // Base address for Spike Out

wire synap_matrix_select;               // Active when synapse matrix is the target
wire param_select;                      // Active when any neuron parameter is the target
wire [7:0] param_num;                   // Specifies the specific neuron parameter targeted
wire neuron_spike_out_select;           // Active when neuron spike out is the target
wire new_image_packet, last_image_packet; // Signals to indicate new and last image packets

wire [255:0] neurons_connections;
wire [255:0] spike_out;
wire external_write_en;

/*
* Each 32-bit entry in the slave_dat_o array corresponds to wbs_dat_o from a sub-module
* Since slave_dat_o is only `wire` => it main use is to combine with circuit to wire matching wbs_dat_o signals 
of submodule to wbs_dat_o of the top module 
*/
wire [31:0] slave_dat_o [257:0];        // Data outputs from each of the 258 memory blocks
wire [257:0] slave_ack_o;               // Acknowledgment signals from each memory block

/* 
* AddressDecoder_256x256: Decodes the incoming address from the Wishbone
* interface (wbs_adr_i) and determines the target memory block among the 258 blocks
* (1 synapse matrix, 256 neuron parameters, and 1 neuron spike out). It routes the 
* interface signals to the correct destination within the SNN core and ensures that
* the correct data output from these blocks is sent back to the Wishbone interface.
*/
AddressDecoder_256x256 addr_decoder (
    .addr(wbs_adr_i),
    .synap_matrix(synap_matrix_select),
    .param(param_select),
    .param_num(param_num),
    .neuron_spike_out(neuron_spike_out_select),
    .new_image_packet(new_image_packet),
    .last_image_packet(last_image_packet)
);


synapse_matrix_256x256 #(.BASE_ADDR(SYNAPSE_BASE)) sm (
    .wb_clk_i(clk),
    .wb_rst_i(rst),
    .wbs_cyc_i(wbs_cyc_i & synap_matrix_select),
    .wbs_stb_i(wbs_stb_i & synap_matrix_select),
    .wbs_we_i(wbs_we_i & synap_matrix_select),
    .wbs_sel_i(wbs_sel_i),
    .wbs_adr_i(wbs_adr_i),
    .wbs_dat_i(wbs_dat_i),
    .wbs_ack_o(slave_ack_o[0]),
    .wbs_dat_o(slave_dat_o[0]),
    .neurons_connections_o(neurons_connections)
);

generate
    genvar i;
    for (i = 0; i < 256; i = i + 1) begin : neuron_instances
        // wires for interfacing neuron_parameters and neuron_block
        wire signed [7:0] voltage_potential, pos_threshold, neg_threshold, leak_value;
        wire signed [7:0] weight_type1, weight_type2, weight_type3, weight_type4;
        wire signed [7:0] pos_reset, neg_reset;
        wire [7:0] weight_select;
        wire signed [7:0] new_potential;

        neuron_parameters_256x256 #(.BASE_ADDR(PARAM_BASE + i*PADDING_PARAM)) np_inst (
            .wb_clk_i(clk),
            .wb_rst_i(rst),
            .wbs_cyc_i(wbs_cyc_i & param_select & (param_num == i)),
            .wbs_stb_i(wbs_stb_i & param_select & (param_num == i)),
            .wbs_we_i(wbs_we_i & param_select & (param_num == i)),
            .wbs_sel_i(wbs_sel_i),
            .wbs_adr_i(wbs_adr_i),
            .wbs_dat_i(wbs_dat_i),
            .wbs_ack_o(slave_ack_o[i+1]),
            .wbs_dat_o(slave_dat_o[i+1]),
            .ext_voltage_potential_i(new_potential),
            .ext_write_enable_i(neurons_connections[i]),
            .voltage_potential_o(voltage_potential),
            .pos_threshold_o(pos_threshold),
            .neg_threshold_o(neg_threshold),
            .leak_value_o(leak_value),
            .weight_type1_o(weight_type1),
            .weight_type2_o(weight_type2),
            .weight_type3_o(weight_type3),
            .weight_type4_o(weight_type4),
            .weight_select_o(weight_select),
            .pos_reset_o(pos_reset),
            .neg_reset_o(neg_reset)
        );
        
        neuron_block_256x256 nb_inst (
            .voltage_potential_i(voltage_potential),
            .pos_threshold_i(pos_threshold),
            .neg_threshold_i(neg_threshold),
            .leak_value_i(leak_value),
            .weight_type1_i(weight_type1),
            .weight_type2_i(weight_type2),
            .weight_type3_i(weight_type3),
            .weight_type4_i(weight_type4),
            .weight_select_i(weight_select),
            .pos_reset_i(pos_reset),
            .neg_reset_i(neg_reset),
            .new_potential_o(new_potential),
            .enable_i(neurons_connections[i]),
            .spike_o(spike_out[i]),
            .new_image_packet_i(new_image_packet),
            .last_image_packet_i(last_image_packet)
        );
    end
endgenerate

assign external_write_en = |spike_out;

neuron_spike_out_256x256 #(.BASE_ADDR(SPIKE_OUT_BASE)) spike_out_inst (
    .wb_clk_i(clk),
    .wb_rst_i(rst),
    .wbs_cyc_i(wbs_cyc_i & neuron_spike_out_select),
    .wbs_stb_i(wbs_stb_i & neuron_spike_out_select),
    .wbs_we_i(wbs_we_i & neuron_spike_out_select),
    .wbs_sel_i(wbs_sel_i),
    .wbs_adr_i(wbs_adr_i),
    .wbs_dat_i(wbs_dat_i),
    .wbs_ack_o(slave_ack_o[257]),
    .wbs_dat_o(slave_dat_o[257]),
    .external_spike_data_i(spike_out),
    .external_write_en_i(external_write_en)
);

// The AddressDecoder's output signals determine which slave_dat_o signal's data is routed to the wbs_dat_o
assign wbs_dat_o = synap_matrix_select ? slave_dat_o[0] : 
                    neuron_spike_out_select ? slave_dat_o[257] : 
                    param_select ? slave_dat_o[param_num] : 
                    32'b0;

assign wbs_ack_o = |slave_ack_o;

endmodule
