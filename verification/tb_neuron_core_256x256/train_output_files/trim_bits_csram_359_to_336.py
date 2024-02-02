
def trim_bits(input_file_path, output_file_path, ranges):
    """
    trim from start bit to end bit, including the end bit
    """
    with open(input_file_path, 'r') as input_file, open(output_file_path, 'w') as output_file:
        for idx, line in enumerate(input_file):
            # Remove the newline character at the end of the line for processing
            line = line.rstrip('\n')
            modified_line = line
            offset = 0
            print("Before trim: ", len(modified_line))

            for start, end in sorted(ranges, key=lambda x: x[0]):
                # Correctly set to zero-based index
                start = start -1 
                end = end -1
                modified_line = modified_line[:start + offset] + modified_line[end + 1 + offset:]
                offset -= (end - start + 1)
                
            print("After trim: ", len(modified_line))
            
            output_file.write(modified_line+'\n')
        print("Total lines: ", idx+1)

# Example usage
input_file_path = 'mem_files/csram_000.mem'
output_file_path = 'mem_files/csram_000_trimmed_336bit.txt'
ranges_to_trim = [ 
                    # 1-bit reset_mode
                    (329, 329), 
                    # dx, dy 9-bit pairs
                    (330, 347), 
                    # 4-bit tick instance
                    (356, 359)
                   ]
                
trim_bits(input_file_path, output_file_path, ranges_to_trim)
