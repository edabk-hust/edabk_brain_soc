import cocotb
import cocotb.log
import caravel_cocotb.interfaces.caravel as caravel
import logging
from cocotb.log import SimTimeContextFilter
from cocotb.log import SimLogFormatter
from caravel_cocotb.interfaces.common_functions.Timeout import Timeout
from cocotb.triggers import ClockCycles
import oyaml as yaml
try:
    from cocotb_coverage.coverage import coverage_db
except ImportError:
    pass


"""configure the test log file location and log verbosity
   configure the test clock
   configure the test timeout
   configure whitbox models
   start up the test connecting power vdd to the design then reset and disable the CSB bit
   return the caravel environmnet with clock and start up
"""


def read_config_file():
    config_file = f"{cocotb.plusargs['SIM_DIR']}/configs.yaml".replace('"', "")

    with open(config_file) as file:
        # The FullLoader parameter handles the conversion from YAML
        # scalar values to Python the dictionary format
        configs = yaml.load(file, Loader=yaml.FullLoader)

        # print(configs)
        return configs


CLOCK_GLOBAL = 25


# async def test_configure(dut:cocotb.handle.SimHandle,timeout_cycles=1000000,clk=25,timeout_precision=0.2,num_error=3)-> caravel.Caravel_env:
async def test_configure(
    dut: cocotb.handle.SimHandle,
    timeout_cycles=1000000,
    clk=read_config_file()['clock'],
    timeout_precision=0.2,
    num_error=int(read_config_file()['max_err']),
    start_up=True
) -> caravel.Caravel_env:
    """
    Configure caravel power, clock, and reset and setup the timeout watchdog then return object of caravel environment.

    :param SimHandle dut: dut handle
    :param int timeout_cycles: Number of cycles before reporting timeout and exit the test default = 1000000 cycles
    :param int clk: The clock period to be used in the design in ``'ns'`` default 12.5 ``'ns'``
    :param int timeout_precision: Precision of logging how many cycle left until the timeout default is 0.2 meaning if time is 100 cycle every 20 cycles there would be a warning message for timeout
    :param int num_error: Maximum number of errors reported before terminate the test
    :param bool start_up: start up the test connecting power and reset
    :return: Object of type Caravel_env (caravel environment)
    """
    caravelEnv = caravel.Caravel_env(dut)
    Timeout(caravelEnv.clk, timeout_cycles, timeout_precision)
    cocotb.scheduler.add(max_num_error(num_error, caravelEnv.clk))
    caravelEnv.setup_clock(clk)
    if start_up:
        await caravelEnv.start_up()
        await ClockCycles(caravelEnv.clk, 10)
    # coverage = 'COVERAGE' in caravelEnv.design_macros._asdict()
    # checker = 'CHECKERS' in caravelEnv.design_macros._asdict()
    # if checker:
    #     HK_whiteBox(dut, checkers=True)
    #     GPIOs_ctrlWB(dut, checkers=True)
    # elif coverage:
    #     HK_whiteBox(dut)
    #     GPIOs_ctrlWB(dut)
    if "CPU_TYPE_ARM" in caravelEnv.design_macros._asdict():
        caravelEnv.active_gpios_num = (
            34  # with ARM the last 3 gpios are not configurable
        )

    # For calculating recommended timeout
    global CLOCK_GLOBAL
    CLOCK_GLOBAL = clk

    return caravelEnv


class CallCounted:
    """Decorator to determine number of calls for a method"""

    def __init__(self, method):
        self.method = method
        self.counter = 0

    def __call__(self, *args, **kwargs):
        self.counter += 1
        return self.method(*args, **kwargs)


def report_test(func):
    async def wrapper_func(*args, **kwargs):
        # configure logging
        TESTFULLNAME = cocotb.plusargs["FTESTNAME"]
        sim_dir = f"{cocotb.plusargs['SIM_PATH']}/{cocotb.plusargs['TAG']}"
        TestName = func.__name__
        logger_file = f"{sim_dir}/{TESTFULLNAME}/{TestName}.log".replace('"', "")
        cocotb.log.setLevel(logging.INFO)
        cocotb.log.error = CallCounted(cocotb.log.error)
        cocotb.log.critical = CallCounted(cocotb.log.critical)
        cocotb.log.warning = CallCounted(cocotb.log.warning)
        handler = logging.FileHandler(logger_file, mode="w")
        handler.addFilter(SimTimeContextFilter())
        handler.setFormatter(SimLogFormatter())
        cocotb.log.addHandler(handler)
        # call test
        await func(*args, **kwargs)
        if "COVERAGE_COLLECT" in cocotb.plusargs:
            coverage_db.export_to_yaml(
                filename=f"{sim_dir}/{TESTFULLNAME}/coverage.yalm".replace('"', ""),
            )
        # report after finish simulation
        msg = f"with ({cocotb.log.critical.counter})criticals ({cocotb.log.error.counter})errors ({cocotb.log.warning.counter})warnings "
        if cocotb.log.error.counter > 0 or cocotb.log.critical.counter > 0:
            raise cocotb.result.TestComplete(f"Test failed {msg}")
        else:
            cocotb.log.info(f"Test passed {msg}")
            cocotb.log.info(
                f'Cycles consumed = {int(cocotb.utils.get_sim_time("ns")/CLOCK_GLOBAL)} recommened timeout = {int(cocotb.utils.get_sim_time("ns")*1.1/CLOCK_GLOBAL)+1} cycles'
            )

    return wrapper_func


async def max_num_error(num_error, clk):
    while True:
        await ClockCycles(clk, 1)
        if cocotb.log.error.counter + cocotb.log.critical.counter > num_error:
            msg = f"Test failed with max number of errors {num_error} ({cocotb.log.critical.counter})criticals ({cocotb.log.error.counter})errors ({cocotb.log.warning.counter})warnings "
            raise cocotb.result.TestFailure(msg)
