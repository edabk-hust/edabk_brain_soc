module spike_event_controller(
    input wire wb_clk_i,          // Wishbone clock
    input wire wb_rst_i,          // Wishbone reset
    input wire wbs_cyc_i,         // Wishbone cycle valid input
    input wire wbs_stb_i,         // Wishbone strobe input
    input wire wbs_we_i,          // Wishbone write enable input
    input wire [7:0] image_num_packets_i, // Number of packets in the current image
    input wire image_spike_event_i, // High when spike event controller is the target
    output reg last_image_packet_o  // High when the last packet is processed
);

// Internal counter for the packets
reg [7:0] packet_counter;
// State to track if counting is active
reg counting;

// Reset logic
always @(negedge wb_clk_i or posedge wb_rst_i) begin
    if (wb_rst_i) begin
        packet_counter <= 8'd0;
        last_image_packet_o <= 1'b0;
        counting <= 1'b0;
    end
    else if (wbs_cyc_i && wbs_stb_i) begin
        if (image_spike_event_i) begin
            // Load the counter with the number of packets when image_spike_event_i is high
            packet_counter <= image_num_packets_i;
            counting <= 1'b1;
            last_image_packet_o <= 1'b0;
        end
        else if (counting) begin
            if (packet_counter > 0) begin
                // Decrement the counter
                packet_counter <= packet_counter - 1;
            end
            else begin
                // When counter reaches 0, set last_image_packet_o high and stop counting
                last_image_packet_o <= 1'b1;
                counting <= 1'b0;
            end
        end
    end
end

endmodule
