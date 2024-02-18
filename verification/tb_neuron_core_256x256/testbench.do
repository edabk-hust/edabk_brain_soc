cd C:/Users/ADMIN/GithubClonedRepo/edabk_brain_soc/verification/tb_neuron_core_256x256/sim

vlog ../../../verilog/neuron_core_256x256/*.v

vlog ../tb_neuron_core_256x256.sv

vsim -voptargs=+acc work.tb_neuron_core_256x256

add wave -position insertpoint sim:/tb_neuron_core_256x256/uut_neuron_core/*

add wave -position insertpoint  \
sim:/tb_neuron_core_256x256/synap_matrix_start \
sim:/tb_neuron_core_256x256/synap_matrix_done \
sim:/tb_neuron_core_256x256/neuron_params_start \
sim:/tb_neuron_core_256x256/neuron_params_done \
sim:/tb_neuron_core_256x256/new_image_packet \
sim:/tb_neuron_core_256x256/last_image_packet

add wave -position insertpoint {sim:/tb_neuron_core_256x256/uut_neuron_core/neuron_instances[7]/*}

add wave -position insertpoint  \
sim:/tb_neuron_core_256x256/uut_neuron_core/spike_event_inst/packet_counter \
sim:/tb_neuron_core_256x256/uut_neuron_core/spike_event_inst/counting
add wave -position insertpoint  \
{sim:/tb_neuron_core_256x256/uut_neuron_core/neuron_instances[7]/nb_inst/selected_weight} \
{sim:/tb_neuron_core_256x256/uut_neuron_core/neuron_instances[7]/nb_inst/potential_calc}

run -all