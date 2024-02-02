def crop_bits(input_file_path, output_file_path, start_bit, end_bit):
    with open(input_file_path, 'r') as input_file, open(output_file_path, 'w') as output_file:
        for line in input_file:
            line = line.rstrip('\n')  # Remove the newline character for processing
            # Adjust the end index for inclusive slicing and check line length
            end_index = end_bit if end_bit < len(line) else len(line) - 1
            # Crop the line from start_bit to end_bit, inclusive
            cropped_line = line[start_bit:end_index + 1]
            # Write the cropped line to the output file
            output_file.write(cropped_line + '\n')

# Example usage
input_file_path = 'csram_000_336bit.txt'

# output_file_path = 'tb_synap_matrix.txt'
# crop_bits(input_file_path, output_file_path, 0, 255)

output_file_path = 'tb_neuron_params.txt'
crop_bits(input_file_path, output_file_path, 256, 335)