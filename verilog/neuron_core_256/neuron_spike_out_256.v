module neuron_spike_out (
    // Wishbone slave interface
    input wb_clk_i,             // Clock
    input wb_rst_i,             // Reset
    input wbs_cyc_i,            // Indicates an active Wishbone cycle
    input wbs_stb_i,            // Active during a valid address phase
    input wbs_we_i,             // Determines read or write operation
    input [3:0] wbs_sel_i,      // Byte lanes selector
    input [31:0] wbs_adr_i,     // Address input
    input [31:0] wbs_dat_i,     // Data input for writes
    output reg wbs_ack_o,       // Acknowledgment for data transfer
    output reg [31:0] wbs_dat_o, // Data output 

    // External spike data inputs
    input [255:0] external_spike_data_i,
    input external_write_en_i
);

parameter BASE_ADDR = 32'h50000000;  // Base address for this SRAM
reg [31:0] sram [7:0];               // SRAM storage for spikes

wire [2:0] address;                  // Calculated from the input address
assign address = (wbs_adr_i - BASE_ADDR) >> 2;

// Handling read/write operations, the acknowledgment signal, and the external spike update
always @(negedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i) begin
        wbs_ack_o <= 1'b0;
        wbs_dat_o <= 32'b0;
    end
    else if (wbs_cyc_i && wbs_stb_i) begin
        if (address >= 0 && address < 8) begin
            if (wbs_we_i) begin
                // Byte-specific writes based on wbs_sel_i
                if (wbs_sel_i[0]) sram[address][7:0] <= wbs_dat_i[7:0];
                if (wbs_sel_i[1]) sram[address][15:8] <= wbs_dat_i[15:8];
                if (wbs_sel_i[2]) sram[address][23:16] <= wbs_dat_i[23:16];
                if (wbs_sel_i[3]) sram[address][31:24] <= wbs_dat_i[31:24];
            end
            wbs_dat_o <= sram[address];
            wbs_ack_o <= 1'b1;
        end
    end 
    else begin
        wbs_ack_o <= 1'b0;
        if (external_write_en_i) begin
            // External spike data update
            sram[7] <= external_spike_data_i[255:224];
            sram[6] <= external_spike_data_i[223:192];
            sram[5] <= external_spike_data_i[191:160];
            sram[4] <= external_spike_data_i[159:128];
            sram[3] <= external_spike_data_i[127:96];
            sram[2] <= external_spike_data_i[95:64];
            sram[1] <= external_spike_data_i[63:32];
            sram[0] <= external_spike_data_i[31:0];
        end 
    end
end

endmodule
