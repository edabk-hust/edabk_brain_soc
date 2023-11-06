module synapse_matrix (
    // Wishbone slave interface
    input wb_clk_i,             // Clock
    input wb_rst_i,             // Reset
    input wbs_cyc_i,            // Indicates an active Wishbone cycle
    input wbs_stb_i,            // Active during a valid address phase
    input wbs_we_i,             // Determines read or write operation
    input [3:0] wbs_sel_i,      // Byte lanes selector (expected as 4'b1111)
    input [31:0] wbs_adr_i,     // Address input
    input [31:0] wbs_dat_i,     // Data input for writes
    output reg wbs_ack_o,       // Acknowledgment for data transfer
    output reg [31:0] wbs_dat_o, // Data output (not used in this module)

    // Synapse matrix specific output
    output [31:0] neurons_connections_o  // Represents connections of an axon with 32 neurons
);

parameter BASE_ADDR = 32'h30000000;
reg [31:0] sram [255:0];         // SRAM storage for 256 words
wire [31:0] address;              // Calculated from the input address

// Continuous assignment for address computation
assign address = (wbs_adr_i - BASE_ADDR) >> 2;

// Handling read/write operations and the acknowledgment signal
always @(negedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i) begin
        wbs_ack_o <= 1'b0;
        wbs_dat_o <= 32'b0;
    end
    else if (wbs_cyc_i && wbs_stb_i) begin
        if (address >= 0 && address < 256) begin  // Updated address limit
            if (wbs_we_i) begin
                // Byte-specific writes based on wbs_sel_i
                if (wbs_sel_i[0]) sram[address][7:0] <= wbs_dat_i[7:0];
                if (wbs_sel_i[1]) sram[address][15:8] <= wbs_dat_i[15:8];
                if (wbs_sel_i[2]) sram[address][23:16] <= wbs_dat_i[23:16];
                if (wbs_sel_i[3]) sram[address][31:24] <= wbs_dat_i[31:24];
            end
            wbs_ack_o <= 1'b1;
        end
    end
    else begin
        wbs_ack_o <= 1'b0;
    end
end

// Generating the connections based on the input address during read operations
// Outputs all zeros when there's a write operation or an invalid Wishbone transaction
assign neurons_connections_o = (!wbs_we_i && wbs_cyc_i && wbs_stb_i) ? 
            sram[address] : 32'b0;  // Simplified connection generation for 32 neurons

endmodule

