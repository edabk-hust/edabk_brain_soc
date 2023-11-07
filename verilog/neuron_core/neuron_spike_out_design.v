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
    input [31:0] external_spike_data_i,
    input external_write_en_i
);

parameter BASE_ADDR = 32'h30008000;  // Base address for this SRAM
reg [31:0] sram;                      // SRAM storage for spikes (single word now)

wire [31:0] address;                  // 32-bit address signal
assign address = (wbs_adr_i - BASE_ADDR) >> 2;

// Handling read/write operations, the acknowledgment signal, and the external spike update
always @(negedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i) begin
        wbs_ack_o <= 1'b0;
        wbs_dat_o <= 32'b0;
    end
    else if (wbs_cyc_i && wbs_stb_i) begin
        if (address == 32'd0) begin  // Only one address location now
            if (wbs_we_i) begin
                // Byte-specific writes based on wbs_sel_i
                if (wbs_sel_i[0]) sram[7:0] <= wbs_dat_i[7:0];
                if (wbs_sel_i[1]) sram[15:8] <= wbs_dat_i[15:8];
                if (wbs_sel_i[2]) sram[23:16] <= wbs_dat_i[23:16];
                if (wbs_sel_i[3]) sram[31:24] <= wbs_dat_i[31:24];
            end
            wbs_dat_o <= sram;
            wbs_ack_o <= 1'b1;
        end
    end 
    else begin
        wbs_ack_o <= 1'b0;
        if (external_write_en_i) begin
            // External spike data update
            sram <= external_spike_data_i;
        end 
    end
end

endmodule
