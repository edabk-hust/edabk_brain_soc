import os
import shutil
import subprocess
from caravel_cocotb.scripts.verify_cocotb.read_defines import GetDefines
import re
import logging
import caravel_cocotb


class RunTest:
    def __init__(self, args, paths, test, logger) -> None:
        self.args = args
        self.paths = paths
        self.test = test
        self.logger = logger

    def run_test(self):
        if self.hex_generate() == "hex_generated":  # run test only if hex is generated
            self.runTest()
        self.test.end_of_test()

    def docker_command_str(self, docker_image="efabless/dv:cocotb", docker_dir="", env_vars="", addtional_switchs="", command=""):
        command = f"docker run {' --init -it --sig-proxy=true ' if not self.args.CI else ' ' } -u $(id -u $USER):$(id -g $USER) {addtional_switchs} {env_vars} {docker_dir} {docker_image} sh -ec '{command}'"
        return command

    def hex_riscv_command_gen(self):
        GCC_PATH = "/opt/riscv/bin/"
        GCC_PREFIX = "riscv32-unknown-linux-gnu"
        GCC_COMPILE = f"{GCC_PATH}/{GCC_PREFIX}"
        SOURCE_FILES = f"{self.paths.FIRMWARE_PATH}/crt0_vex.S {self.paths.FIRMWARE_PATH}/isr.c"

        LINKER_SCRIPT = f"-Wl,-Bstatic,-T,{self.test.linker_script_file},--strip-debug "
        CPUFLAGS = "-O2 -g -march=rv32i_zicsr -mabi=ilp32 -D__vexriscv__ -ffreestanding -nostdlib"
        # CPUFLAGS = "-O2 -g -march=rv32imc_zicsr -mabi=ilp32 -D__vexriscv__ -ffreestanding -nostdlib"
        includes = f" -I{self.paths.FIRMWARE_PATH} -I{self.paths.FIRMWARE_PATH}/APIs -I{self.paths.VERILOG_PATH}/dv/generated  -I{self.paths.VERILOG_PATH}/dv/ -I{self.paths.VERILOG_PATH}/common"
        includes += f" -I{self.paths.USER_PROJECT_ROOT}/verilog/dv/cocotb "
        elf_command = (
            f"{GCC_COMPILE}-gcc  {includes} {CPUFLAGS} {LINKER_SCRIPT}"
            f" -o {self.hex_dir}/{self.test.name}.elf {SOURCE_FILES} {self.c_file}"
        )
        lst_command = f"{GCC_COMPILE}-objdump -d -S {self.hex_dir}/{self.test.name}.elf > {self.hex_dir}/{self.test.name}.lst "
        hex_command = f"{GCC_COMPILE}-objcopy -O verilog {self.hex_dir}/{self.test.name}.elf {self.hex_dir}/{self.test.name}.hex "
        sed_command = f'sed -ie "s/@10/@00/g" {self.hex_dir}/{self.test.name}.hex'
        return f" {elf_command} && {lst_command} && {hex_command} && {sed_command}"

    def hex_arm_command_gen(self):
        GCC_COMPILE = "arm-none-eabi"
        SOURCE_FILES = f"{self.paths.FIRMWARE_PATH}/cm0_start.s"
        LINKER_SCRIPT = f"-T {self.test.linker_script_file}"
        CPUFLAGS = "-O2 -Wall -nostdlib -nostartfiles -ffreestanding -mcpu=cortex-m0 -Wno-unused-value"
        includes = f"-I{self.paths.FIRMWARE_PATH} -I{self.paths.USER_PROJECT_ROOT}/verilog/dv/cocotb"
        elf_command = (
            f"{GCC_COMPILE}-gcc  {includes} {CPUFLAGS} {LINKER_SCRIPT}"
            f" -o {self.hex_dir}/{self.test.name}.elf {SOURCE_FILES} {self.c_file}"
        )
        lst_command = f"{GCC_COMPILE}-objdump -d -S {self.hex_dir}/{self.test.name}.elf > {self.hex_dir}/{self.test.name}.lst "
        hex_command = f"{GCC_COMPILE}-objcopy -O verilog {self.hex_dir}/{self.test.name}.elf {self.hex_dir}/{self.test.name}.hex "
        sed_command = f'sed -ie "s/@10/@00/g" {self.hex_dir}/{self.test.name}.hex'
        return f" {elf_command} &&{lst_command}&& {hex_command}&& {sed_command}"

    def hex_generate(self) -> str:
        # get the test path from dv/cocotb
        test_path = self.test_path()
        # Create a new hex_files directory because it does not exist
        if not os.path.exists(f"{self.paths.SIM_PATH}/hex_files"):
            os.makedirs(f"{self.paths.SIM_PATH}/hex_files")  
        self.hex_dir = f"{self.paths.SIM_PATH}/hex_files/"
        self.c_file = f"{test_path}/{self.test.name}.c"
        if self.args.cpu_type == "ARM":
            command = self.hex_arm_command_gen()
        else:
            command = self.hex_riscv_command_gen()

        docker_dir = f"-v {self.hex_dir}:{self.hex_dir} -v {self.paths.RUN_PATH}:{self.paths.RUN_PATH} -v {self.paths.CARAVEL_ROOT}:{self.paths.CARAVEL_ROOT} -v {self.paths.MCW_ROOT}:{self.paths.MCW_ROOT} -v {self.test.test_dir}:{self.test.test_dir} "
        docker_dir = (docker_dir + f"-v {self.paths.USER_PROJECT_ROOT}:{self.paths.USER_PROJECT_ROOT}")
        docker_command = self.docker_command_str(docker_image="efabless/dv:cocotb", docker_dir=docker_dir, command=command)
        # don't run with docker with arm
        cmd = command if self.args.cpu_type == "ARM" else docker_command
        hex_gen_state = self.run_command_write_to_file(cmd, self.test.hex_log, self.logger, quiet=False if self.args.verbosity == "debug" else True)
        self.firmware_log = open(self.test.hex_log, "a")
        if hex_gen_state != 0:
            # open(self.test.firmware_log, "w").write(stdout)
            raise RuntimeError(
                f"{bcolors.FAIL}Error:{bcolors.ENDC} Fail to compile the C code for more info refer to {bcolors.OKCYAN }{self.test.hex_log}{bcolors.ENDC } "
            )
            self.firmware_log.write("Error: when generating hex")
            self.firmware_log.close()
            return "hex_error"
        self.firmware_log.write("Pass: hex generation")
        self.firmware_log.close()
        # move hex file to the test
        shutil.copyfile(f"{self.test.hex_dir}/{self.test.name}.hex", f"{self.test.test_dir}/firmware.hex")            
        return "hex_generated"

    def test_path(self, test_name=None):
        if test_name is None:
            test_name = self.test.name
        c_file_name = test_name + ".c"
        tests_path_user = os.path.abspath(f"{self.paths.USER_PROJECT_ROOT}/verilog/dv/cocotb")
        test_file = self.find(c_file_name, tests_path_user)
        test_path = os.path.dirname(test_file)
        return test_path

    def runTest(self):
        os.environ["COCOTB_RESULTS_FILE"] = f"{self.test.test_dir}/seed.xml"
        if self.args.iverilog:
            self.runTest_iverilog()
        elif self.args.vcs:
            self.runTest_vcs()

    # iverilog function
    def runTest_iverilog(self):
        if self.test.sim == "GL_SDF":
            raise RuntimeError(
                f"iverilog can't run SDF for test {self.test.name} Please use anothor simulator like cvc"
            )
            return
        self.write_iverilog_includes_file()
        if not os.path.isfile(f"{self.test.compilation_dir}/sim.vvp") or self.args.compile:
            self.iverilog_compile()
        self.iverilog_run()

    def write_iverilog_includes_file(self):
        self.iverilog_dirs = " "
        self.iverilog_dirs += f'-I {self.paths.USER_PROJECT_ROOT}/verilog/rtl'
        self.test.set_user_project()

    def iverilog_compile(self):
        macros = " -D" + " -D".join(self.test.macros)
        compile_command = (
            f"cd {self.test.compilation_dir} &&"
            f"iverilog -g2012 -Ttyp {macros} {self.iverilog_dirs} -o {self.test.compilation_dir}/sim.vvp"
            f" {self.paths.CARAVEL_VERILOG_PATH}/rtl/toplevel_cocotb.v -s caravel_top"
        )
        docker_compilation_command = self._iverilog_docker_command_str(compile_command)
        self.run_command_write_to_file(
            docker_compilation_command if not self.args.no_docker else compile_command,
            self.test.compilation_log,
            self.logger,
            quiet=False if self.args.verbosity == "debug" else True
        )

    def iverilog_run(self):
        defines = GetDefines(self.test.includes_file)
        seed = "" if self.args.seed is None else f"RANDOM_SEED={self.args.seed}"
        run_command = (f"cd {self.test.test_dir} && TESTCASE={self.test.name} MODULE=module_trail {seed} vvp -M $(cocotb-config --prefix)/cocotb/libs -m libcocotbvpi_icarus {self.test.compilation_dir}/sim.vvp +{ ' +'.join(self.test.macros) } {' '.join([f'+{k}={v}' if v != ''else f'+{k}' for k, v in defines.defines.items()])}")
        docker_run_command = self._iverilog_docker_command_str(run_command)
        self.run_command_write_to_file(
            docker_run_command if not self.args.no_docker else run_command,
            None if self.args.verbosity == "quiet" else self.test.test_log2,
            self.logger,
            quiet=True if self.args.verbosity == "quiet" else False
        )

    def _iverilog_docker_command_str(self, command=""):
        """the docker command without the command that would run"""
        env_vars = f"-e COCOTB_RESULTS_FILE={os.getenv('COCOTB_RESULTS_FILE')} -e CARAVEL_PATH={self.paths.CARAVEL_PATH} -e CARAVEL_VERILOG_PATH={self.paths.CARAVEL_VERILOG_PATH} -e VERILOG_PATH={self.paths.VERILOG_PATH} -e PDK_ROOT={self.paths.PDK_ROOT} -e PDK={self.paths.PDK} -e USER_PROJECT_VERILOG={self.paths.USER_PROJECT_ROOT}/verilog"
        local_caravel_cocotb_path = caravel_cocotb.__file__.replace("__init__.py", "")
        docker_caravel_cocotb_path = "/usr/local/lib/python3.8/dist-packages/caravel_cocotb/"
        docker_dir = f"-v {self.paths.RUN_PATH}:{self.paths.RUN_PATH} -v {self.paths.CARAVEL_ROOT}:{self.paths.CARAVEL_ROOT} -v {self.paths.MCW_ROOT}:{self.paths.MCW_ROOT} -v {self.paths.PDK_ROOT}:{self.paths.PDK_ROOT} -v {local_caravel_cocotb_path}:{docker_caravel_cocotb_path} "
        docker_dir += (f"-v {self.paths.USER_PROJECT_ROOT}:{self.paths.USER_PROJECT_ROOT}")
        if os.path.exists("/mnt/scratch/"):
            docker_dir += " -v /mnt/scratch/cocotb_runs/:/mnt/scratch/cocotb_runs/ "
        display = " -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v $HOME/.Xauthority:/.Xauthority --network host --security-opt seccomp=unconfined "
        command = self.docker_command_str(docker_image="efabless/dv:cocotb", docker_dir=docker_dir, env_vars=env_vars, addtional_switchs=display, command=command)
        return command

    # vcs function
    def runTest_vcs(self):
        self.write_vcs_includes_file()
        self.vcs_coverage_command = ''
        if self.test.sim == "RTL":
            self.vcs_coverage_command = "-cm line+tgl+cond+fsm+branch+assert "
        os.environ["TESTCASE"] = f"{self.test.name}"
        os.environ["MODULE"] = "module_trail"
        if not os.path.isfile(f"{self.test.compilation_dir}/simv") or self.args.compile:
            self.vcs_compile()
        self.vcs_run()

    def write_vcs_includes_file(self):
        # self.vcs_dirs = f'+incdir+\\"{self.paths.PDK_ROOT}/{self.paths.PDK}\\" '
        self.vcs_dirs = ' '
        if self.test.sim == "RTL":
            self.vcs_dirs += f'+incdir+\\"{self.paths.USER_PROJECT_ROOT}/verilog/rtl\\" '
        self.test.set_user_project()

    def vcs_compile(self):
        macros = " +define+" + " +define+".join(self.test.macros)        
        if self.args.seed is not None:
            os.environ["RANDOM_SEED"] = self.args.seed
        vlogan_cmd = f"cd {self.test.compilation_dir}; vlogan -full64 -sverilog +error+30 {self.paths.CARAVEL_VERILOG_PATH}/rtl/toplevel_cocotb.v {self.vcs_dirs}  {macros}   -l {self.test.compilation_dir}/analysis.log -o {self.test.compilation_dir} "
        self.run_command_write_to_file(vlogan_cmd, self.test.compilation_log, self.logger, quiet=False if self.args.verbosity == "debug" else True)
        lint = "+lint=all" if self.args.lint else ""
        ignored_errors = " -error=noZMMCM "
        vcs_cmd = f"cd {self.test.compilation_dir};  vcs {lint} {self.vcs_coverage_command} {ignored_errors}-debug_access+all +error+50 +vcs+loopreport+1000000 -diag=sdf:verbose +sdfverbose +neg_tchk -debug_access -full64  -l {self.test.compilation_dir}/test_compilation.log  caravel_top -Mdir={self.test.compilation_dir}/csrc -o {self.test.compilation_dir}/simv +vpi -P pli.tab -load $(cocotb-config --lib-name-path vpi vcs)"
        self.run_command_write_to_file(
            vcs_cmd,
            self.test.compilation_log,
            self.logger,
            quiet=False if self.args.verbosity == "debug" else True
        )

    def vcs_run(self):
        defines = GetDefines(self.test.includes_file)
        run_sim = f"cd {self.test.test_dir}; {self.test.compilation_dir}/simv +vcs+dumpvars+all {self.vcs_coverage_command} -cm_name {self.test.name} +{ ' +'.join(self.test.macros)} {' '.join([f'+{k}={v}' if v != ''else f'+{k}' for k, v in defines.defines.items()])}"
        self.run_command_write_to_file(
            run_sim,
            None if self.args.verbosity == "quiet" else self.test.test_log2,
            self.logger,
            quiet=True if self.args.verbosity == "quiet" else False
        )

    def find(self, name, path):
        for root, dirs, files in os.walk(path):
            if name in files:
                return os.path.join(root, name)
        raise RuntimeError(f"Test {name} doesn't exist or don't have a C file ")

    def run_command_write_to_file(self, cmd, file, logger, quiet=True):
        """run command and write output to file return 0 if no error"""
        if file is not None:
            logger_file = logging.getLogger(file)
            logger_file.setLevel(logging.INFO)
            # Configure file handler for the logger
            file_handler = logging.FileHandler(file)
            file_handler.setLevel(logging.INFO)
            file_formatter = logging.Formatter('%(message)s')
            file_handler.setFormatter(file_formatter)
            logger_file.addHandler(file_handler)
            f = open(file, "a")
            f.write("command:")
            f.write(os.path.expandvars(cmd) + "\n\n")
            f.close()
        try:
            process = subprocess.Popen(
                cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, bufsize=1024
            )
            while True:
                out = process.stdout.readline().decode("utf-8")
                ansi_escape = re.compile(r"\x1B[@-_][0-?]*[ -/]*[@-~]")
                stdout = ansi_escape.sub("", out)
                if process.poll() is not None:
                    break
                if out:
                    if not quiet:
                        logger.info(stdout.replace("\n", "", 1))
                        # time.sleep(0.01)
                    if file is not None:
                        logger_file.info(stdout.replace("\n", "", 1))
        except Exception as e:
            logger(f"Docker process stopped by user {e}")
            process.stdin.write(b'\x03') # Send the Ctrl+C signal to the Docker process
            process.terminate()

        return process.returncode


class bcolors:
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    UNDERLINE = "\033[4m"


def change_str(str, new_str, file_path):
    # Read in the file
    with open(file_path, "r") as file:
        filedata = file.read()

    filedata = filedata.replace(str, new_str)

    # Write the file out again
    with open(file_path, "w") as file:
        file.write(filedata)
