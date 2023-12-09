import os
from datetime import datetime
from pathlib import Path
from collections import namedtuple
import yaml
from caravel_cocotb.scripts.verify_cocotb.RunRegression import RunRegression
from caravel_cocotb.scripts.verify_cocotb.check_git import GitRepoChecker
import re
import logging
import random


def check_valid_mail_addr(address):
    pat = r"\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,7}\b"
    if re.match(pat, address):
        print(f"valid mail {address}")
        return True
    print(f"invalid mail {address}")
    return False


class RunFLow:
    def __init__(self, args) -> None:
        self.args = args
        self.run_path = self.args.run_path
        self.configure_logger()
        self.check_valid_args()
        design_info = self.get_design_info()
        self.set_paths(design_info)
        self.set_cpu_type()
        self.set_tag()
        self.set_args(design_info)
        self.set_config_script(design_info)
        RunRegression(self.args, self.paths, self.logger)
        

    def configure_logger(self):
        self.logger = logging.getLogger(__name__)
        self.logger.setLevel(logging.INFO)
        formatter = logging.Formatter('%(message)s')
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        console_handler.setFormatter(formatter)
        self.logger.addHandler(console_handler)

    def check_valid_args(self):
        if all(
            v is None
            for v in [self.args.test, self.args.testlist]
        ):
            raise EnvironmentError(
                "Should provide at least one of the following options test or testlist for more info use --help"
            )
        if self.args.sim is not None:
            self.args.sim = (
                [self.args.sim]
                if not isinstance(self.args.sim, list)
                else self.args.sim
            )
            for sim in self.args.sim:
                if sim not in ["RTL", "GL", "GL_SDF"]:
                    raise ValueError(
                        f"{self.args.sim} isnt a correct value for -sim it should be one or combination of the following RTL, GL or GL_SDF"
                    )

    def set_tag(self):
        if self.args.tag is None:
            self.args.tag = (f'run_{datetime.now().strftime("%d_%b_%H_%M_%S_%f")[:-4]}')
        Path(f"{self.paths.SIM_PATH}").mkdir(
            parents=True, exist_ok=True
        )
        # check if scratch disk exists
        if os.path.exists("/mnt/scratch/"):
            if os.path.lexists(f"{self.paths.SIM_PATH}/{self.args.tag}"):
                if os.path.islink(f"{self.paths.SIM_PATH}/{self.args.tag}"):
                    os.unlink(f"{self.paths.SIM_PATH}/{self.args.tag}")
                else:
                    os.rmdir(f"{self.paths.SIM_PATH}/{self.args.tag}")
            scratch_folder_name = f"cocotb_run_{datetime.now().strftime('%Y%m%d%H%M%S')}.{random.randint(0, 1000)}_{self.args.tag}"
            scratch_folder = f"/mnt/scratch/cocotb_runs/{scratch_folder_name}"
            if not os.path.exists(scratch_folder):
                os.makedirs(scratch_folder)
            os.symlink(f"{scratch_folder}", f"{self.paths.SIM_PATH}/{self.args.tag}")
        else:
            Path(f"{self.paths.SIM_PATH}/{self.args.tag}").mkdir(parents=True, exist_ok=True)
        self.logger.info(f"Run tag: {self.args.tag} ")

    def set_paths(self, design_info):
        if not os.path.exists(design_info["CARAVEL_ROOT"]) or not os.path.exists(
            design_info["MCW_ROOT"]
        ):
            raise NotADirectoryError(
                f"CARAVEL_ROOT or MCW_ROOT not a correct directory CARAVEL_ROOT:{design_info['CARAVEL_ROOT']} MCW_ROOT:{design_info['MCW_ROOT']}"
            )
        if self.args.check_commits:
            GitRepoChecker(design_info["CARAVEL_ROOT"]) # check repo synced with last commit
            GitRepoChecker(design_info["MCW_ROOT"]) # check repo synced with last commit
        if not os.path.exists(f'{design_info["PDK_ROOT"]}/{design_info["PDK"]}'):
            raise NotADirectoryError(
                f"PDK_ROOT/PDK is not a directory PDK_ROOT:{design_info['PDK_ROOT']}/{design_info['PDK']}"
            )
        if not os.path.exists(design_info["USER_PROJECT_ROOT"]):
            raise NotADirectoryError(
                f"USER_PROJECT_ROOT is not a directory USER_PROJECT_ROOT:{design_info['USER_PROJECT_ROOT']}"
            )
        else:
            if self.args.check_commits:
                GitRepoChecker(design_info["USER_PROJECT_ROOT"]) # check repo synced with last commit
        Paths = namedtuple(
            "Paths",
            "CARAVEL_ROOT MCW_ROOT PDK_ROOT PDK CARAVEL_VERILOG_PATH VERILOG_PATH CARAVEL_PATH FIRMWARE_PATH RUN_PATH USER_PROJECT_ROOT SIM_PATH",
        )
        CARAVEL_VERILOG_PATH = f"{design_info['CARAVEL_ROOT']}/verilog"
        VERILOG_PATH = f"{design_info['MCW_ROOT']}/verilog"
        CARAVEL_PATH = f"{CARAVEL_VERILOG_PATH}"
        if os.path.exists(f"{design_info['MCW_ROOT']}/verilog/dv/fw"):
            FIRMWARE_PATH = f"{design_info['MCW_ROOT']}/verilog/dv/fw"
        else:
            FIRMWARE_PATH = f"{design_info['MCW_ROOT']}/verilog/dv/firmware"
        RUN_PATH = self.args.run_path
        SIM_PATH = (
            f"{RUN_PATH}/sim"
            if self.args.sim_path is None
            else f"{self.args.sim_path}/sim"
        )
        self.paths = Paths(
            design_info["CARAVEL_ROOT"],
            design_info["MCW_ROOT"],
            design_info["PDK_ROOT"],
            design_info["PDK"],
            CARAVEL_VERILOG_PATH,
            VERILOG_PATH,
            CARAVEL_PATH,
            FIRMWARE_PATH,
            RUN_PATH,
            design_info["USER_PROJECT_ROOT"],
            SIM_PATH,
        )

    def set_cpu_type(self):
        def_h_file = f'{self.paths.FIRMWARE_PATH}/defs.h'
        pattern = r'^#define CPU_TYPE\s+(\w+)$'
        with open(def_h_file, "r") as f:
            for line in f:
                match = re.match(pattern, line)
                if match:
                    self.args.cpu_type = match.group(1)
                    return
        raise EnvironmentError("Can't find cpu type please add #define CPU_TYPE to defs.h in managment repo")
    
    def set_args(self, design_info):
        if self.args.clk is None:
            self.args.clk = design_info["clk"]
        else:
            self.args.clk = int(self.args.clk)

        if self.args.maxerr is None:
            self.args.maxerr = 3

        self.args.caravan = design_info["caravan"]

        if self.args.sim is None:
            self.args.sim = ["RTL"]

        if self.args.corner is None:
            self.args.corner = ["nom-t"]

        if "sky130" in design_info["PDK"]:
            self.args.pdk = "sky130"
        elif "gf180" in design_info["PDK"]:
            self.args.pdk = "gf180"

        self.args.iverilog = False
        if not self.args.vcs:
            self.args.iverilog = True

        if self.args.emailto is None:
            self.args.emailto = [
                mail_addr
                for mail_addr in design_info["emailto"]
                if mail_addr is not None and check_valid_mail_addr(mail_addr)
            ]
        else:
            if not check_valid_mail_addr(self.args.emailto):
                self.args.emailto = [
                    [
                        mail_addr
                        for mail_addr in self.args.emailto
                        if check_valid_mail_addr(mail_addr)
                    ]
                ]  # if mail input aren't a valid mail will ignore it

        if self.args.verbosity is None:
            self.args.verbosity = "normal"

    def set_config_script(self, design_info):
        new_config_path = f"{self.paths.SIM_PATH}/{self.args.tag}/configs.yaml"
        design_configs = dict(
            clock=self.args.clk, max_err=self.args.maxerr, PDK=self.args.pdk
        )
        design_configs.update(
            dict(
                CARAVEL_ROOT=self.paths.CARAVEL_ROOT,
                MCW_ROOT=self.paths.MCW_ROOT,
                PDK_ROOT=f'{self.paths.PDK_ROOT}/{design_info["PDK"]}',
            )
        )
        with open(new_config_path, "w") as file:
            yaml.dump(design_configs, file)

    def get_design_info(self):
        yaml_file = open(f"{f'{self.run_path}/design_info.yaml' if self.args.design_info is None else self.args.design_info}", "r")
        design_info = yaml.safe_load(yaml_file)
        return design_info


class CocotbArgs:
    def __init__(
        self,
        test=None,
        sim=None,
        testlist=None,
        tag=None,
        maxerr=3,
        corner=None,
        seed=None,
        no_wave=False,
        clk=25,
        vcs=False,
        emailto=None,
        sdf_setup=None,
        macros=None,
        sim_path=None,
        run_path=".",
        verbosity="normal",
        check_commits=False,
        design_info=None,
        no_docker=False,
        compile=False,
        run_defaults=False,
        CI=False,
        no_gen_defaults=False
    ) -> None:
        self.test = test
        self.sim = sim
        self.testlist = testlist
        self.tag = tag
        self.maxerr = maxerr
        self.vcs = vcs
        self.corner = corner
        self.emailto = emailto
        self.seed = seed
        self.no_wave = no_wave
        self.sdf_setup = sdf_setup
        self.clk = clk
        self.macros = macros
        self.sim_path = sim_path
        self.run_path = run_path
        self.verbosity = verbosity
        # dev only
        self.lint = None
        # related to repos
        self.cpu_type = None  # would be filled by other class
        self.check_commits = check_commits
        self.design_info = design_info
        self.no_docker = no_docker
        self.compile = compile
        self.run_defaults = run_defaults
        self.CI = CI
        self.no_gen_defaults = no_gen_defaults

    def argparse_to_CocotbArgs(self, args):
        self.test = args.test
        self.sim = args.sim
        self.testlist = args.testlist
        self.tag = args.tag
        self.maxerr = args.maxerr
        self.vcs = args.vcs
        self.corner = args.corner
        self.emailto = args.emailto
        self.seed = args.seed
        self.no_wave = args.no_wave
        self.sdf_setup = args.sdf_setup
        self.clk = args.clk
        self.macros = args.macros
        self.sim_path = args.sim_path
        self.lint = args.lint
        self.run_path = os.getcwd()
        self.verbosity = args.verbosity
        self.check_commits = args.check_commits
        self.design_info = args.design_info
        self.no_docker = args.no_docker
        self.compile = args.compile
        self.run_defaults = args.run_defaults
        self.CI = args.CI
        self.no_gen_defaults = args.no_gen_defaults