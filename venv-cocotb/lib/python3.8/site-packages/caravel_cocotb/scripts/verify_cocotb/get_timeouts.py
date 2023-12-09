#!/usr/bin/python

import click
import os


@click.command()
@click.argument("sim_tag_path", default="./")
def get_timeouts(sim_tag_path):
    """SIM_TAG_PATH:  path for simulation results"""
    tests_logs = list()
    for root, dirs, files in os.walk(sim_tag_path):
        for dir in dirs:
            if "RTL-" in dir:
                tests_logs.append(f"{dir[4:]}.log")

    files_paths = dict()
    for root, dirs, files in os.walk(sim_tag_path):
        for file in files:
            if file in tests_logs:
                files_paths[file] = os.path.join(root, file)
        
    for name, files_path in files_paths.items():
        with open(files_path) as file:
            for line in file:
                if "recommened timeout = " in line:
                    s_comp = ""
                    for s in line.split():
                        s_comp += s
                        if s.isdigit() and "recommened" in s_comp:
                            cycles = int(s)
                            print(f"{name[:-4]:<30}  {cycles}")


if __name__ == "__main__":
    get_timeouts()
