import yaml
import os
import random
from caravel_cocotb.scripts.verify_cocotb.Test import Test


class TestDefaults:
    def __init__(self, args, paths, run_function, tests, macros=None) -> None:
        self.cocotb_path = os.getcwd()
        self.gpio_num = 38
        self.gpio_config_covered = [set() for _ in range(self.gpio_num)]
        self.args = args
        self.paths = paths
        self.macros = macros
        self.run_function = run_function
        self.tests = tests
        self.update_random_user_defines()
        pass

    def update_random_user_defines(self):
        """
        Update user-defined modes in a Verilog file with random values.

        Reads valid modes from a YAML file, writes them to a Verilog file along
        with random values for user-defined modes.

        Args:
            self: An instance of SomeClass.

        Returns:
            None
        """
        self.gpio_configs = [""] * self.gpio_num

        valid_modes, unvalid_modes = self.set_valid_modes()
        for i in range(len(valid_modes)):

            self.write_user_defines_to_file(valid_modes, unvalid_modes)

            self.write_random_values_to_file(valid_modes)
            if "GL" in self.args.sim or "GL_SDF" in self.args.sim:
                self.run_gen_gpio_defaults()
                        
            if "GL_SDF" in self.args.sim:
                for corner in self.args.corner:
                    self.run_SDF_test(i, corner)
            elif "GL" in self.args.sim:
                self.run_GL_test(i)
            elif "RTL" in self.args.sim:
                self.run_RTL_test(i)

    def set_valid_modes(self):
        valid_modes = {"GPIO_MODE_MGMT_STD_INPUT_NOPULL": 0x0403, "GPIO_MODE_MGMT_STD_INPUT_PULLDOWN": 0x0c01, "GPIO_MODE_MGMT_STD_INPUT_PULLUP": 0x0801, "GPIO_MODE_MGMT_STD_OUTPUT": 0x1809, "GPIO_MODE_USER_STD_INPUT_NOPULL": 0x0402, "GPIO_MODE_USER_STD_INPUT_PULLDOWN": 0x0c00, "GPIO_MODE_USER_STD_INPUT_PULLUP": 0x0800, "GPIO_MODE_USER_STD_OUTPUT": 0x1808, "GPIO_MODE_USER_STD_BIDIRECTIONAL": 0x1800}
        # unvalid modes that can't be testsed
        unvalid_modes = {"GPIO_MODE_MGMT_STD_BIDIRECTIONAL": 0x1801, "GPIO_MODE_MGMT_STD_ANALOG": 0x000b, "GPIO_MODE_USER_STD_ANALOG": 0x000a}
        return valid_modes, unvalid_modes

    def write_user_defines_to_file(self, valid_modes, unvalid_modes):
        with open(f'{self.paths.USER_PROJECT_ROOT}/verilog/rtl/user_defines.v', 'w') as user_define_file:
            user_define_file.write("""`ifndef __USER_DEFINES_H \n`define __USER_DEFINES_H\n\n\n""")

            for mode, value in valid_modes.items():
                line = f"`define {mode} 13'h{value:04x}\n"  # format: `define <mode> <value>
                user_define_file.write(line)

            for mode, value in unvalid_modes.items():
                line = f"`define {mode} 13'h{value:04x}\n"  # format: `define <mode> <value>
                user_define_file.write(line)


    def write_random_values_to_file(self, valid_modes):
        with open(f'{self.paths.USER_PROJECT_ROOT}/verilog/rtl/user_defines.v', 'a') as user_define_file:
            with open(f'{self.paths.USER_PROJECT_ROOT}/verilog/rtl/user_define_temp.txt', 'w') as user_define_txt_file:
                user_define_file.write("\n\n\n")
                for i in range(self.gpio_num):
                    modes_difference = self.get_modes_difference(valid_modes, i)
                    random_mode = random.choice(list(modes_difference))
                    # check for the fixed configured GPIO
                    if i in [0, 1, 2, 3, 4]:
                        if i in [0,1]:
                            random_mode = "GPIO_MODE_MGMT_STD_INPUT_NOPULL"
                        elif i in [2,4]:
                            random_mode = "GPIO_MODE_MGMT_STD_INPUT_NOPULL"
                        elif i == 3:
                            random_mode = "GPIO_MODE_MGMT_STD_INPUT_PULLUP"

                    self.gpio_configs[i] = random_mode
                    self.gpio_config_covered[i].add(random_mode)

                    line = f"`define USER_CONFIG_GPIO_{i}_INIT  `{random_mode}\n"  # format: `define USER_CONFIG_GPIO_<i>_INIT  <random mode>
                    user_define_file.write(line)
                    user_define_txt_file.write(f"{random_mode}\n")
                user_define_file.write("\n\n\n`endif // __USER_DEFINES_H")

    def get_modes_difference(self, valid_modes, i):
        valid_modes_set = set(valid_modes.keys())
        modes_difference = valid_modes_set - self.gpio_config_covered[i]
        if len(modes_difference) == 0:
            modes_difference = valid_modes_set
        return modes_difference
    

    def run_gen_gpio_defaults(self):
        script_path = f'{self.paths.CARAVEL_ROOT}/scripts/gen_gpio_defaults.py'
        os.system(f'cd {self.paths.CARAVEL_ROOT}; python3 {script_path} {self.paths.USER_PROJECT_ROOT}')

    def run_RTL_test(self, i):
        RTL_test = Test("check_defaults", "RTL", "nom-t", self.args, self.paths, self.macros)
        self.tests.append(RTL_test)
        self.run_function(RTL_test)

        old_folder_path = RTL_test.test_dir
        new_folder_name = f"{RTL_test.full_name}{i}"
        new_folder_path = os.path.join(os.path.dirname(old_folder_path), new_folder_name)
        try:
            os.rename(old_folder_path, new_folder_path)
            print(f"Folder path changed from '{old_folder_path}' to '{new_folder_path}'.")
        except FileNotFoundError:
            print(f"Folder '{old_folder_path}' not found.")
        except FileExistsError:
            print(f"Folder '{new_folder_path}' already exists.")
        except Exception as e:
            print(f"An error occurred: {e}")
        # os.system(f'python3 verify_cocotb.py -t check_defaults -tag default_run{i} -v -verbosity quiet -no_wave')

    def run_GL_test(self, i):
        GL_test = Test("check_defaults", "GL", "nom-t", self.args, self.paths, self.macros)
        self.tests.append(GL_test)
        self.run_function(GL_test)

        old_folder_path = GL_test.test_dir
        new_folder_name = f"{GL_test.full_name}{i}"
        new_folder_path = os.path.join(os.path.dirname(old_folder_path), new_folder_name)
        try:
            os.rename(old_folder_path, new_folder_path)
            print(f"Folder path changed from '{old_folder_path}' to '{new_folder_path}'.")
        except FileNotFoundError:
            print(f"Folder '{old_folder_path}' not found.")
        except FileExistsError:
            print(f"Folder '{new_folder_path}' already exists.")
        except Exception as e:
            print(f"An error occurred: {e}")

    def run_SDF_test(self, i, corner):
        GL_test = Test("check_defaults", "GL_SDF", corner, self.args, self.paths, self.macros)
        self.tests.append(GL_test)
        self.run_function(GL_test)

        old_folder_path = GL_test.test_dir
        new_folder_name = f"{GL_test.full_name}{i}"
        new_folder_path = os.path.join(os.path.dirname(old_folder_path), new_folder_name)
        try:
            os.rename(old_folder_path, new_folder_path)
            print(f"Folder path changed from '{old_folder_path}' to '{new_folder_path}'.")
        except FileNotFoundError:
            print(f"Folder '{old_folder_path}' not found.")
        except FileExistsError:
            print(f"Folder '{new_folder_path}' already exists.")
        except Exception as e:
            print(f"An error occurred: {e}")



