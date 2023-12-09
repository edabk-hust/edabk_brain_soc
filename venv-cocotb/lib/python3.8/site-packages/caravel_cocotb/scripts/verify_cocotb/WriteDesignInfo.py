from collections import namedtuple
import ruamel.yaml

yaml_str = """\
#eg CARAVEL_ROOT: "/usr/Desktop/caravel_project/caravel/"
#like repo https://github.com/efabless/caravel
CARAVEL_ROOT: "/home/rady/caravel/caravel_orginal/caravel/"

#eg MCW_ROOT: "/usr/Desktop/caravel_project/caravel_mgmt_soc_litex/"
#like repo https://github.com/efabless/caravel_mgmt_soc_litex
MCW_ROOT: '/home/rady/caravel/caravel_orginal/caravel_mgmt_soc_litex/'

#eg USER_PROJECT_ROOT: "/usr/Desktop/caravel_project/caravel_user_project/"
#like repo https://github.com/efabless/caravel_user_project

USER_PROJECT_ROOT: "/home/rady/caravel/swift/caravel_user_project/"



#eg PDK_ROOT: "/usr/Desktop/caravel_project/pdk/"
#exported by volare
PDK_ROOT: "/home/rady/caravel/files4vcs/pdk"

#eg PDK: "sky130A"
PDK: sky130A
#PDK: gf180mcuC

#clock in ns
clk: 25

# true when caravan are simulated instead of caravel
caravan: false

# optional email address to send the results to
emailto: [None]
"""
EnvironmentPaths = namedtuple(
    "EnvironmentPaths", "CARAVEL_ROOT MCW_ROOT PDK_ROOT PDK USER_PROJECT_ROOT"
)


class WriteDesignInfo:
    def __init__(
        self,
        cocotb_path,
        env_paths: EnvironmentPaths,
        clk=25,
        is_caravan=False,
        Emailto=None,
    ) -> None:
        self.update_yaml_args(env_paths, clk, is_caravan, Emailto)
        self.write_yaml_f(cocotb_path)

    def update_yaml_args(
        self, env_paths: EnvironmentPaths, clk, is_caravan=False, Emailto=None
    ):
        self.yaml = ruamel.yaml.YAML()  # defaults to round-trip if no parameters given
        self.code = self.yaml.load(yaml_str)
        self.code["CARAVEL_ROOT"] = env_paths.CARAVEL_ROOT
        self.code["MCW_ROOT"] = env_paths.MCW_ROOT
        self.code["USER_PROJECT_ROOT"] = env_paths.USER_PROJECT_ROOT
        self.code["PDK"] = env_paths.PDK
        self.code["PDK_ROOT"] = env_paths.PDK_ROOT
        self.code["clk"] = clk
        self.code["caravan"] = is_caravan
        self.code["emailto"] = [None] if Emailto is None else Emailto

    def write_yaml_f(self, cocotb_path):
        file_path = f"{cocotb_path}/design_info.yaml"
        with open(file_path, "w") as outfile:
            self.yaml.dump(self.code, outfile)


# paths = EnvironmentPaths("/home/rady/caravel/caravel_orginal/caravel/",
# "/home/rady/caravel/caravel_orginal/caravel_mgmt_soc_litex/",
# "/home/rady/caravel/files4vcs/pdk","sky130A",
# "/home/rady/caravel/swift/caravel_user_project/")

# WriteDesignInfo("/home/Marwan/caravel/swift/caravel-dynamic-sims/cocotb/",paths,Emailto=["mostafa.rady@efabless.com"])
