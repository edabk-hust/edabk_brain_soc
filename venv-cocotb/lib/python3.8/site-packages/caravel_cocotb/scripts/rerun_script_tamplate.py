rerun_script_template = '''
import os
import shutil
import argparse

parser = argparse.ArgumentParser(description="Run cocotb tests")
parser.add_argument("-extend", help="extend the command")
args = parser.parse_args()

os.environ["CARAVEL_ROOT"] = "replace by caravel Root"
os.environ["MCW_ROOT"] = "replace by mgmt Root"

os.chdir("replace by cocotb path")

command = "replace by test command"
if args.extend is not None:
    command += f" {args.extend}"
os.system(command)

shutil.copyfile("replace by orignal rerun script", "replace by new rerun script")
'''
