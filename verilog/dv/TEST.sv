module testbench();
  // Wishbone ports
  logic clk;
  logic reset_n;
  logic [31:0] wbs_dat_i;
  logic [31:0] wbs_adr_i;
  logic [31:0] wbs_dat_o;

  // Control signals
  logic wbs_cyc_i;
  logic wbs_stb_i;
  logic wbs_we_i;
  logic [3:0] wbs_sel_i;

  // DUT instantiation
  neuron_core DUT (
    
    .clk(clk),
    .rst(reset_n),
    .wbs_cyc_i(wbs_cyc_i),
    .wbs_stb_i(wbs_stb_i),
    .wbs_we_i(wbs_we_i),
    .wbs_sel_i(wbs_sel_i),
    .wbs_adr_i(wbs_adr_i),
    .wbs_dat_i(wbs_dat_i),
    .wbs_ack_o(wbs_ack_o),
    .wbs_dat_o(wbs_dat_o)
  );

  parameter SYNAPSE_BASE = 32'h30000000;  // Base address for Synapse Matrix
  parameter PARAM_BASE = 32'h30004000;    // Base address for Neuron Parameters
  parameter PADDING_PARAM = 32'h00000010; // Padding for Neuron Parameters
  parameter SPIKE_OUT_BASE = 32'h30008000; // Base address for Spike Out
  // -----------------------------------------------------------------------------
  // Filenames:
  string filename_synapse = "synaptic_connections.txt";
  string filename_nr_params = "neuron_parameter.txt";
  string filename_i_spikes = "input_spike.txt";
  string filename_o_axons = "output_axons.txt";

  int file; // File descriptor
  string line; // Store a line

  // -----------------------------------------------------------------------------
  // PACKET and INPUT LOADER

  logic [31:0] synapse_con_dat [255:0];
  logic [31:0] params [127:0];
  bit [31:0] output_axons;
  // int input_spikes [255:0];
  int input_spikes [];

  typedef struct {
    logic [31:0] syn_dat_fr;
    logic [31:0]    syn_adr;
  } Synapse_packet;

  typedef struct {
    logic [31:0] params_dat_fr;
    logic [31:0]    params_adr;
  } Params_packet;

  typedef struct {
    logic [31:0]  output_axons_fr;
    logic [31:0] output_axons_adr;
  } Output_axons_packet;

  // For checking
  logic [31:0] expected_spikes_o;
  logic [31:0] received_spike_o;

  //----------------------------------------------------------------------------------
  // Drive data and address
  int i;
  bit check;
  int axon_num;
  task DrivePacket();
    begin
      Synapse_packet syn;
      Params_packet syn1;
      // Drive Synapse_connection
      @(posedge clk);
      for (i = 0; i < 256; i++) begin
        syn = '{0, 0};
        Drive_Syn(syn, synapse_con_dat[i], i);
        wbs_dat_i = syn.syn_dat_fr;
        wbs_adr_i = syn.syn_adr;
        @(posedge clk);
      end

      // Drive Parameters
      for (i = 0; i < 128; i++) begin
        syn1 = '{0, 0};
        Drive_Param(syn1, params[i], i);
        wbs_dat_i = syn1.params_dat_fr;
        wbs_adr_i = syn1.params_adr;
        @(posedge clk);
      end

      // Drive input spikes 
      for (int i = 0; i < input_spikes.size(); i++) begin 
        axon_num = input_spikes[i];
        $display("input_spikes %d: %d",i, axon_num);
        wbs_adr_i = SYNAPSE_BASE + (axon_num*4);
        wbs_dat_i = 0;
        $display("wbs_adr_i = %h", wbs_adr_i);
        @(posedge clk);
      end
      expected_spikes_o = output_axons;

      check = 1;
    end
  endtask
  // Receive spike_out and out_params from DUT
  task Receive 

  endtask

  //-----------------------------------------------------------------------------------
  // List of child tasks
  task automatic Drive_Syn(ref Synapse_packet syn1, logic [31:0] synapse_con_dat_div, int i);
    begin
      // Drive data and address
      syn1.syn_dat_fr = synapse_con_dat_div;
      syn1.syn_adr = SYNAPSE_BASE + (4*i);
    end
  endtask : Drive_Syn

  task automatic Drive_Param(ref Params_packet syn , logic [31:0] params_div, int i);
    begin
      // Drive data and address
      syn.params_dat_fr = params_div ;
      syn.params_adr = PARAM_BASE + (4*i);
    end
  endtask : Drive_Param

  //-----------------------------------------------------------------------------------
  // Receive spike out and out_params from DUT
  // UPDATING ... 

  //-----------------------------------------------------------------------------------
  initial begin
    int idx;
    int length_input_spikes;
    // Read synapse connections
    $display("Open files");
    file = $fopen(filename_synapse, "r");
    if (file == 0) begin
      $display("Error: Unable to open file %s", filename_synapse);
      $finish;
    end

    for (int i = 0 ; i < 256 ; i++) begin
      line = " "; 
      $fscanf(file, "%s = %0b", line, idx);

      synapse_con_dat[i] = line.atobin();
  
    end
    $display("Synapse connections: %p", synapse_con_dat);
    $display("Synapse connections 1: %p", synapse_con_dat[1]);


    $fclose(file); 
    
    // Read file params
    file = $fopen(filename_nr_params, "r"); 
    if (file == 0) begin
      $display("Error: Unable to open file %s", filename_nr_params);
      $finish;
    end

    for (int i = 0 ; i < 128 ; i++) begin
      line = " ";
      $fscanf(file, "%s = %0b", line, idx);

      params[i] = line.atobin();
    end
    $display("Parameters: %p", params);

    $fclose(file); 
    // Read input spikes
    file = $fopen(filename_i_spikes, "r"); 
    if (file == 0) begin
      $display("Error: Unable to open file %s", filename_i_spikes);
      $finish;
    end

    $fscanf(file, "%s = %0b", line, idx);
    length_input_spikes = line.atoi();

    input_spikes = new[length_input_spikes];

    $display("length_input_spikes = %d", length_input_spikes);

    for (int i = 0 ; i < length_input_spikes ; i++) begin
        line = " ";
        $fscanf(file, "%s = %0b", line, idx);

        input_spikes[i] = line.atoi();

    end
    $display("Input spikes: %p", input_spikes);

    $fclose(file); 

    file = $fopen(filename_o_axons, "r"); 
    if (file == 0) begin
      $display("Error: Unable to open file %s", filename_o_axons);
      $finish;
    end

    for (int i = 0 ; i < 1 ; i++) begin
      line = " "; 
      $fscanf(file, "%s = %0b", line, idx);

      output_axons = line.atobin();
    end
    $display("Output axons: %p", output_axons);

    $fclose(file);

    // Reset inactive 
    reset_n = 1;
    clk = 1;
    wbs_sel_i = 4'b1111; 
    
    wbs_we_i = 0;
    wbs_cyc_i = 0;
    wbs_stb_i = 0;
    @(posedge clk);

    wbs_we_i = 1;
    wbs_stb_i = 1;
    wbs_cyc_i = 1;
    // Drive data for DUT
    // Dem xem gui dc bao nhieu data
    DrivePacket();
    wait(check);
    wbs_stb_i = 0;
    wbs_we_i = 0;
    wbs_stb_i = 1;
    $finish();
  end

  always #5 clk = ~clk ;

endmodule