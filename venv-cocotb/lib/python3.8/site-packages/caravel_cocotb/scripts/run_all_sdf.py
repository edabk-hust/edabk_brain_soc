import subprocess
from datetime import datetime
import time

test_list = "/home/rady/caravel/caravel_release/caravel_cocotb_tests/verilog/dv/cocotb/regression/all_sdf_sky130.yaml"
low = "high"

sim_location = " -sim_path /home/rady/caravel/caravel_release/caravel-dynamic-sims/cocotb/sim_high_sdf "

# corners = ["nom-t", "min-t", "max-t", "nom-f", "min-f", "max-f", "nom-s", "min-s", "max-s"]
corners = ["nom-t", "min-t", "max-t", "nom-f", "min-f", "max-f"]
commands = []
for corner in corners:
    command = f"python3 verify_cocotb.py -tl {test_list} -sim GL_SDF -vcs  -no_wave -tag run_sdf_{corner}_25ns_{low}_{datetime.now().strftime('%d_%b_%H_%M_%S_%f')[:-4]}  -corner {corner} -verbosity quiet "
    if low == "high":
        command += " -sdf_setup "
    command += sim_location
    commands.append(command)



# create a list to store the subprocess objects
procs = []

# run each command asynchronously
for cmd in commands:
    proc = subprocess.Popen(cmd, shell=True)
    procs.append(proc)
    time.sleep(3)


# wait for all processes to finish
for proc in procs:
    proc.wait()
