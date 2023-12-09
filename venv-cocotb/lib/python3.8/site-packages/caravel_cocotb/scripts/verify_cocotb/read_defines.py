import re


class GetDefines:
    def __init__(self, file):
        self.__call__(file)

    def __call__(self, file):
        """file is the netlist file"""
        define_paths = self.get_include_paths_verilog(file)
        self.defines = {}
        for path in define_paths:
            self.defines.update(self.read_define_file(path))
        return self.defines

    def verilog_num_parser(self, number):
        number = str(number)
        number = number.replace("\t", " ").replace(" ", "")
        match = re.match(r"^([0-9]+'[bdho])([0-9A-Fa-f]+)$", number)
        match2 = re.match(r"^('[bdho])([0-9A-Fa-f]+)$", number)
        if number.isdigit():
            return int(number)
        elif match:
            base = match.group(1)[-1]
            value = match.group(2)
            if base == "d":
                return int(value)
            return int(value, base=2 if base == "b" else 8 if base == "o" else 16)
        elif match2:
            base = match2.group(1)[-1]
            value = match2.group(2)
            if base == "d":
                return int(value)
            return int(value, base=2 if base == "b" else 8 if base == "o" else 16)
        else:
            print(f"Error: '{number}' is not a valid Verilog number.")
            return 0

    def extract_string(self, input_string):
        quote_count = 0
        start_word = False
        words = list()
        for char in input_string:
            if not start_word:
                if char == "`":
                    quote_count += 1
                    start_word = True
                    word = ""
            else:
                if char.isalnum() or char in ["_", "-"]:
                    word += char
                else:
                    start_word = False
                    words.append(word)
                    word = ""
        if word != "":
            words.append(word)
        return words

    def read_define_file(self, file_path):
        with open(file_path, "r") as f:
            define_lines = f.readlines()
        defines = {}
        for line in define_lines:
            line = re.sub(r"//.*|/\*.*?\*/", "", line)
            match = re.findall(r"^`define\s+(\w+)\s+(.*)", line)
            if match:
                if "`" in match[0][1]:
                    new_str = match[0][1]
                    extracts = self.extract_string(match[0][1])
                    for extract in extracts:
                        new_str = new_str.replace(f"`{extract}", defines[extract])
                    val = eval(new_str)
                else:
                    val = (
                        self.verilog_num_parser(match[0][1])
                        if match[0][1] != ""
                        else match[0][1]
                    )
                defines[match[0][0]] = str(val)
        return defines

    def get_include_paths_verilog(self, verilog_file):
        # Open the verilog file
        with open(verilog_file, "r") as f:
            contents = f.read()

        # Regular expression pattern to match include statements
        include_pattern = r"^\s*`include\s+[\'\"](.*)[\'\"]"

        # Find all include statements in the verilog file
        include_matches = re.findall(include_pattern, contents, re.MULTILINE)
        # Get the full path of each included file
        define_paths = []
        for include_match in include_matches:
            include_file = include_match.strip()
            if include_file.startswith("/"):
                # The include path is an absolute path
                include_path = include_file
            else:
                # The include path is a relative path
                include_dir = "/".join(verilog_file.split("/")[:-1])
                include_path = include_dir + "/" + include_file
            if include_path.endswith("defines.v") or include_path.endswith(
                "user_defines.v"
            ):
                define_paths.append(include_path)
        return define_paths
