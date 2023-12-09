import cocotb
from cocotb.clock import Clock
from cocotb.triggers import FallingEdge, RisingEdge, ClockCycles
import cocotb.log
import cocotb.simulator
from cocotb.handle import SimHandleBase
from cocotb.binary import BinaryValue
from itertools import groupby
import caravel_cocotb.interfaces.common as common
from caravel_cocotb.interfaces.common import GPIO_MODE
from caravel_cocotb.interfaces.common import MASK_GPIO_CTRL
from collections.abc import Iterable
from collections import namedtuple


def gpio_mode(gpios_values: list):
    gpios = []
    for array in gpios_values:
        gpio_value = GPIO_MODE(array[1]).name
        for gpio in array[0]:
            gpios.append((gpio, gpio_value))
    cocotb.log.info(f"[caravel][gpio_mode] gpios {gpios}")
    return gpios


class Caravel_env:
    """
    Verification environment for caraval

    - provide APIs for monitoring and driving caravel GPIOs, power pins, clock and reset pins

    :param SimHandle dut: dut handle
    """

    def __init__(self, dut: SimHandleBase):
        self.dut = dut
        self.clk = dut.clock_tb
        try:
            self.caravel_hdl = dut.uut.chip_core
            self.hk_hdl = dut.uut.chip_core.housekeeping
            self.user_hdl = dut.uut.chip_core.mprj
        except AttributeError:
            pass
        self.get_macros()
        self.active_gpios_num = int(self.design_macros.MPRJ_IO_PADS)  # number of active gpios
        if "OPENFRAME" in self.design_macros._asdict():
            self.active_gpios_num = int(self.design_macros.OPENFRAME_IO_PADS)

    def get_macros(self):
        valid_macros = {
            k: v
            for k, v in cocotb.plusargs.items()
            if not k.startswith("_") and "+" not in k
        }
        Macros = namedtuple("Macros", valid_macros.keys())
        self.design_macros = Macros(**valid_macros)

    """start carvel by insert power then reset"""

    async def start_up(self):
        """Start carvel by insert power then reset"""
        await self.power_up()
        await self.disable_csb()  # no need for this anymore as default for gpio3 is now pullup
        await self.reset()
        await self.disable_bins()

    async def disable_bins(self, ignore_bins=[3, 4]):
        for i in range(self.active_gpios_num):
            if i in ignore_bins:  # CSB and SCK
                continue
            common.drive_hdl(self.dut._id(f"gpio{i}_en", False), (0, 0), 0)
        await ClockCycles(self.clk, 1)

    async def power_up(self):
        """Setup the vdd and vcc power pins"""
        cocotb.log.info(" [caravel] start powering up")
        self.set_vdd(0)
        await ClockCycles(self.clk, 10)
        cocotb.log.info(" [caravel] power up -> connect vdd")
        self.set_vdd(1)
        # await ClockCycles(self.clk, 10)
        cocotb.log.info(" [caravel] power up -> connect vcc")
        await ClockCycles(self.clk, 10)

    async def reset(self):
        """Reset caravel"""
        # initial value of reset is x
        cocotb.log.info(" [caravel] start resetting")
        self.dut.resetb_tb.value = 0
        await ClockCycles(self.clk, 20)
        self.dut.resetb_tb.value = 1
        await ClockCycles(self.clk, 1)
        cocotb.log.info(" [caravel] finish resetting")

    def set_vdd(self, value: bool):
        self.dut.vddio_tb.value = value
        self.dut.vssio_tb.value = 0
        if int(self.design_macros.POWER_DOMAINS) == 3:
            self.dut.vddio_2_tb.value = value
            self.dut.vssio_2_tb.value = 0
            self.dut.vdda_tb.value = value
            self.dut.vssa_tb.value = 0
            self.dut.vdda1_tb.value = value
            self.dut.vssa1_tb.value = 0
            self.dut.vdda1_2_tb.value = value
            self.dut.vssa1_2_tb.value = 0
            self.dut.vdda2_tb.value = value
            self.dut.vssa2_tb.value = 0
            self.dut.vccd_tb.value = value
            self.dut.vssd_tb.value = 0
            self.dut.vccd1_tb.value = value
            self.dut.vssd1_tb.value = 0
            self.dut.vccd2_tb.value = value
            self.dut.vssd2_tb.value = 0

    async def drive_csb(self, bit):
        """Drive csb signal pin E8 mprj[3]"""
        self.drive_gpio_in((3, 3), bit)
        self.drive_gpio_in((4, 4), 0)
        await ClockCycles(self.clk, 1)

    async def disable_csb(self):
        """Set the SPI CSB  signal high to disable housekeeping SPI transmission pin E8 mprj[3]"""
        cocotb.log.info(" [caravel] disable housekeeping SPI transmission")
        await self.drive_csb(1)
        # await self.release_csb()
        await ClockCycles(self.clk, 1)

    async def release_csb(self):
        """Set the SPI CSB  signal high impedance"""
        cocotb.log.info(" [caravel] release housekeeping SPI transmission")
        self.release_gpio(2)
        self.release_gpio(3)
        self.release_gpio(4)
        await ClockCycles(self.clk, 1)

    async def enable_csb(self):
        """Set the SPI CSB  signal low to enable housekeeping SPI transmission pin E8 mprj[3]"""
        cocotb.log.info(" [caravel] enable housekeeping SPI transmission")
        await self.drive_csb(0)

    def monitor_gpio(self, h_bit, l_bit=None) -> cocotb.binary.BinaryValue:
        """monitor GPIOs output value

        :param h_bit: highest GPIO number of the tuple of (high GPIO, low GPIO)
        :param l_bit: lowest GPIO to monitor number
        :type h_bit: int or tuple(int, int)
        :raise exception: If h_bit is lower than l_bit
        :return: cocotb.binary.BinaryValue

        Example:

        .. code-block:: python

            monitor_gpio(7) #get output value of GPIO 7 (GPIOs[7])
            monitor_gpio(7,0) # get output value from GPIO 7 to 0 (GPIOs[7:0])
            monitor_gpio((7,0)) #get output value from GPIO 7 to 0 (GPIOs[7:0])

        """
        mprj = self.dut.mprj_io_tb.value
        size = mprj.n_bits - 1  # size of pins array
        if isinstance(h_bit, Iterable):
            l_bit = h_bit[1]
            h_bit = h_bit[0]
        if l_bit is None:
            l_bit = h_bit
        mprj_out = self.dut.mprj_io_tb.value[size - h_bit: size - l_bit]
        if mprj_out.is_resolvable:
            cocotb.log.debug(
                f" [caravel] Monitor : mprj[{h_bit}:{l_bit}] = {hex(mprj_out)}"
            )
        else:
            cocotb.log.debug(f" [caravel] Monitor : mprj[{h_bit}:{l_bit}] = {mprj_out}")
        return mprj_out

    """return the value of management GPIO"""

    def monitor_mgmt_gpio(self) -> str:
        """monitor management GPIO output

        :return: return the value of management GPIO in string format possible values ``"0"`` ``"1"`` ``"X"`` ``"Z"``
        """
        data = self.dut.gpio_tb.value.binstr
        cocotb.log.debug(f" [caravel] Monitor mgmt GPIO = {data}")
        return data

    def monitor_discontinuous_gpios(self, arr: list) -> str:
        """monitor discontinuous GPIOs output value

        :param arr: highest GPIO number of the tuple of (high GPIO, low GPIO)
        :type arr: list(ints) or tuple(ints)
        :return: str
        Example:

        .. code-block:: python

            monitor_discontinuous_gpios([3,2,5]]) #return the value at 3,1,5 in str, str[0] = gpio[5], str[-1]=gpio[3]
        """
        val = ""
        for i in arr:
            cocotb.log.debug(f" [caravel][monitor_discontinuous_gpios] Monitor gpio[{i}] = {self.monitor_gpio(i).binstr}")
            val += self.monitor_gpio(i).binstr
        return val

    async def wait_mgmt_gpio(self, data: int) -> None:
        """wait for specific management GPIO value

        :param data: data to wait for possible inputs ``"0"`` ``"1"`` ``0`` ``1``
        :type data: int or str
        """
        data_s = str(data)
        while True:
            if data_s == self.monitor_mgmt_gpio():
                break
            await ClockCycles(self.clk, 1)

    """change the configration of the GPIOs by overwrite their defaults value then reset
        need to take at least 1 cycle for reset """

    # dont use back door accessing
    async def configure_gpio_defaults(self, gpios_values: list):
        gpio_defaults = self.caravel_hdl.gpio_defaults.value
        cocotb.log.info("[caravel] start cofigure gpio gpios ")
        size = gpio_defaults.n_bits - 1  # number of bins in gpio_defaults
        # list example [[(gpios),value],[(gpios),value],[(gpios),value]]
        for array in gpios_values:
            gpio_value = array[1]
            for gpio in array[0]:
                gpio_defaults[
                    size - (gpio * 13 + 12): size - gpio * 13
                ] = gpio_value.value
                # cocotb.log.info(f' [caravel] gpio_defaults[{size - (gpio*13 + 12)}:{size -gpio*13}] = {gpio_value.value} ')
        self.caravel_hdl.gpio_defaults.value = gpio_defaults
        # reset
        self.caravel_hdl.gpio_resetn_1_shifted.value = 0
        self.caravel_hdl.gpio_resetn_2_shifted.value = 0
        await ClockCycles(self.clk, 1)
        self.caravel_hdl.gpio_resetn_1_shifted.value = 1
        self.caravel_hdl.gpio_resetn_2_shifted.value = 1
        cocotb.log.info(" [caravel] finish configuring gpios, the curret gpios value: ")
        self.print_gpios_ctrl_val()

    """change the configration of the GPIOs by overwrite the register value
        in control registers and housekeeping regs, don't consume simulation cycles"""

    # dont use back door accessing
    def configure_gpios_regs(self, gpios_values: list):
        cocotb.log.info(" [caravel] start cofigure gpio gpios ")
        control_modules = self.control_blocks_paths()
        # list example [[(gpios),value],[(gpios),value],[(gpios),value]]
        for array in gpios_values:
            gpio_value = array[1]
            for gpio in array[0]:
                self.gpio_control_reg_write(
                    control_modules[gpio], gpio_value.value
                )  # for control blocks regs
                self.caravel_hdl.housekeeping.gpio_configure[
                    gpio
                ].value = gpio_value.value  # for house keeping regs
        cocotb.log.info(" [caravel] finish configuring gpios, the curret gpios value: ")
        self.print_gpios_ctrl_val()
        self.print_gpios_HW_val()

    def print_gpios_default_val(self, print=1):
        gpio_defaults = self.caravel_hdl.gpio_defaults.value
        size = gpio_defaults.n_bits - 1  # number of bins in gpio_defaults
        gpios = []
        for gpio in range(self.design_macros["MPRJ_IO_PADS"]):
            gpio_value = gpio_defaults[size - (gpio * 13 + 12): size - gpio * 13]
            gpio_enum = GPIO_MODE(gpio_value.integer)
            gpios.append((gpio, gpio_enum))
        group_pins = groupby(gpios, key=lambda x: x[1])
        for key, value in group_pins:
            gpios = []
            for gpio in list(value):
                gpios.append(gpio[0])
            if print:
                cocotb.log.info(f" [caravel] gpios[{gpios}] are  {key} ")
        return gpios

    """print the values return in the gpio of control block mode in GPIO Mode format"""

    def print_gpios_ctrl_val(self, print=1):
        control_modules = self.control_blocks_paths()
        gpios = []
        for i, gpio in enumerate(control_modules):
            gpios.append((i, self.gpio_control_reg_read(gpio)))
        group_pins = groupby(gpios, key=lambda x: x[1])
        for key, value in group_pins:
            gpios = []
            for gpio in list(value):
                gpios.append(gpio[0])
            if print:
                cocotb.log.info(f" [caravel] gpios[{gpios}] are  {key} ")
        return gpios

    def _check_gpio_ctrl_eq_HW(self):
        assert self.print_gpios_ctrl_val(1) == self.print_gpios_HW_val(
            1
        ), "there is an issue while configuration the control block register value isn't the same as the house keeping gpio register"

    """print the values return in the GPIO of housekeeping block mode in GPIO Mode format"""

    def print_gpios_HW_val(self, print=1):
        gpios = []
        for pin in range(self.design_macros["MPRJ_IO_PADS"]):
            gpios.append(
                (
                    pin,
                    GPIO_MODE(self.caravel_hdl.housekeeping.gpio_configure[pin].value),
                )
            )
        group_pins = groupby(gpios, key=lambda x: x[1])
        for key, value in group_pins:
            gpios = []
            for gpio in list(value):
                gpios.append(gpio[0])
            if print:
                cocotb.log.info(f" [caravel] gpios[{gpios}] are  {key} ")
        return gpios

    """return the paths of the control blocks"""

    def control_blocks_paths(self) -> list:
        car = self.caravel_hdl
        control_modules = [
            car._id("gpio_control_bidir_1[0]", False),
            car._id("gpio_control_bidir_1[1]", False),
        ]
        # add gpio_control_in_1a (GPIO 2 to 7)
        for i in range(6):
            control_modules.append(car._id(f"gpio_control_in_1a[{i}]", False))
        # add gpio_control_in_1 (GPIO 8 to 18)
        for i in range(self.design_macros["MPRJ_IO_PADS_1"] - 9 + 1):
            control_modules.append(car._id(f"gpio_control_in_1[{i}]", False))
        # add gpio_control_in_2 (GPIO 19 to 34)
        for i in range(self.design_macros["MPRJ_IO_PADS_2"] - 4 + 1):
            control_modules.append(car._id(f"gpio_control_in_2[{i}]", False))
        # Last three GPIOs (spi_sdo, flash_io2, and flash_io3) gpio_control_bidir_2
        for i in range(3):
            control_modules.append(car._id(f"gpio_control_bidir_2[{i}]", False))
        return control_modules

    """read the control register and return a GPIO Mode it takes the path to the control reg"""

    def gpio_control_reg_read(self, path: SimHandleBase) -> GPIO_MODE:
        gpio_mgmt_en = (
            path.mgmt_ena.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_MGMT_EN.value
        )
        gpio_out_dis = (
            path.gpio_outenb.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_OUT_DIS.value
        )
        gpio_holdover = (
            path.gpio_holdover.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_OVERRIDE.value
        )
        gpio_in_dis = (
            path.gpio_inenb.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_INP_DIS.value
        )
        gpio_mode_sel = (
            path.gpio_ib_mode_sel.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_MOD_SEL.value
        )
        gpio_anlg_en = (
            path.gpio_ana_en.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_ANLG_EN.value
        )
        gpio_anlg_sel = (
            path.gpio_ana_sel.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_ANLG_SEL.value
        )
        gpio_anlg_pol = (
            path.gpio_ana_pol.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_ANLG_POL.value
        )
        gpio_slow_sel = (
            path.gpio_slow_sel.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_SLOW.value
        )
        gpio_vtrip_sel = (
            path.gpio_vtrip_sel.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_TRIP.value
        )
        gpio_dgtl_mode = (
            path.gpio_dm.value << MASK_GPIO_CTRL.MASK_GPIO_CTRL_DGTL_MODE.value
        )
        control_reg = (
            gpio_mgmt_en
            | gpio_out_dis
            | gpio_holdover
            | gpio_in_dis
            | gpio_mode_sel
            | gpio_anlg_en
            | gpio_anlg_sel
            | gpio_anlg_pol
            | gpio_slow_sel
            | gpio_vtrip_sel
            | gpio_dgtl_mode
        )
        return GPIO_MODE(control_reg)

    """read the control register and return a GPIO Mode it takes the path to the control reg"""

    def gpio_control_reg_write(self, path: SimHandleBase, data):
        bits = common.int_to_bin_list(data, 14)
        path.mgmt_ena.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_MGMT_EN.value]
        path.gpio_outenb.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_OUT_DIS.value]
        path.gpio_holdover.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_OVERRIDE.value]
        path.gpio_inenb.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_INP_DIS.value]
        path.gpio_ib_mode_sel.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_MOD_SEL.value]
        path.gpio_ana_en.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_ANLG_EN.value]
        path.gpio_ana_sel.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_ANLG_SEL.value]
        path.gpio_ana_pol.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_ANLG_POL.value]
        path.gpio_slow_sel.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_SLOW.value]
        path.gpio_vtrip_sel.value = bits[MASK_GPIO_CTRL.MASK_GPIO_CTRL_TRIP.value]
        gpio_dm = bits[
            MASK_GPIO_CTRL.MASK_GPIO_CTRL_DGTL_MODE.value: MASK_GPIO_CTRL.MASK_GPIO_CTRL_DGTL_MODE.value
            + 3
        ]
        gpio_dm = sum(
            d * 2**i for i, d in enumerate(gpio_dm)
        )  # convert list to binary int
        path.gpio_dm.value = gpio_dm

    # """drive the value of mprj bits with specific data  at the top"""
    # def release_gpio(self):
    #     io = self.caravel_hdl.padframe.mprj_pads.io
    #     mprj , n_bits = common.signal_valueZ_size(io)
    #     io.value  =  mprj
    #     cocotb.log.info(f' [caravel] drive_gpio_in pad mprj with {mprj}')

    def drive_gpio_in(self, bits, data) -> None:
        """drive input GPIOs with specific data

        :param bits: GPIOs to drive
        :param int data: data to drive the GPIOs with
        :type bits: int or tuple(int, int)

        Example:

        .. code-block:: python

            drive_gpio_in(7,0x1) # drive GPIO 7 with 1 (GPIOs[7]=1)
            drive_gpio_in((31,0),0xFFFFFFFF) # drive GPIO 31 to 0 with ones (GPIOs[31:0]=32'hFFFFFFFF)
        """
        # io = self.caravel_hdl.padframe.mprj_pads.io
        # mprj , n_bits = common.signal_value_size(io)
        # cocotb.log.debug(f' [caravel] before mprj with {mprj} and data = {data} bit [{n_bits-1-bits[0]}]:[{n_bits-1-bits[1]}]')
        # mprj[n_bits-1-bits[0]:n_bits-1-bits[1]] = data
        # io.value  =  mprj
        # cocotb.log.info(f' [caravel] drive_gpio_in pad mprj with {mprj}')
        data_bits = []
        is_list = isinstance(bits, (list, tuple))
        if is_list:
            cocotb.log.debug(
                f"[caravel] [drive_gpio_in] start bits[1] = {bits[1]} bits[0]= {bits[0]}"
            )
            data_bits = BinaryValue(
                value=data, n_bits=bits[0] - bits[1] + 1, bigEndian=(bits[0] < bits[1])
            )
            for i, bits2 in enumerate(range(bits[1], bits[0] + 1)):
                self.dut._id(f"gpio{bits2}", False).value = data_bits[i]
                self.dut._id(f"gpio{bits2}_en", False).value = 1
                cocotb.log.debug(
                    f"[caravel] [drive_gpio_in] drive gpio{bits2} with {data_bits[i]} and gpio{bits2}_en with 1"
                )
        else:
            self.dut._id(f"gpio{bits}", False).value = data
            self.dut._id(f"gpio{bits}_en", False).value = 1
            cocotb.log.debug(
                f"[caravel] [drive_gpio_in] drive gpio{bits} with {data} and gpio{bits}_en with 1"
            )

    def release_gpio(self, bits):
        """release driving the value of mprj bits
        :param bits: GPIOs to drive
        :type bits: int or tuple(int, int)
        """
        is_list = isinstance(bits, (list, tuple))
        if is_list:
            cocotb.log.debug(
                f"[caravel] [drive_gpio_disable] start bits[1] = {bits[1]} bits[0]= {bits[0]}"
            )
            for i, bits2 in enumerate(range(bits[1], bits[0] + 1)):
                self.dut._id(f"gpio{bits2}_en", False).value = 0
                cocotb.log.debug(
                    f"[caravel] [drive_gpio_disable] release driving gpio{bits2}"
                )
        else:
            self.dut._id(f"gpio{bits}_en", False).value = 0
            cocotb.log.debug(
                f"[caravel] [drive_gpio_disable] release driving gpio{bits}"
            )

    def get_mgmt_gpi_hdl(self):
        return self.dut.gpio_tb

    """drive the value of  GPIO management"""

    def drive_mgmt_gpio(self, data):
        """drive GPIO management with specific data

        :param int data: data to drive the GPIOs with
        """
        self.get_mgmt_gpi_hdl().value = BinaryValue(value=data, n_bits=1)
        cocotb.log.info(
            f" [caravel] drive_mgmt_gpio through management area mprj with {data}"
        )

    """update the value of mprj bits with specific data then after certain number of cycle drive z to free the signal"""

    async def drive_gpio_in_with_cycles(self, bits, data, num_cycles):
        self.drive_gpio_in(bits, data)
        cocotb.log.info(f" [caravel] wait {num_cycles} cycles")
        await cocotb.start(self.wait_then_undrive(bits, num_cycles))
        cocotb.log.info(" [caravel] finish drive_gpio_with_in_cycles ")

    """drive the value of mprj bits with specific data from management area then after certain number of cycle drive z to free the signal"""

    async def drive_mgmt_gpio_with_cycles(self, bits, data, num_cycles):
        self.drive_mgmt_gpio(bits, data)
        cocotb.log.info(f" [caravel] wait {num_cycles} cycles")
        await cocotb.start(self.wait_then_undrive(bits, num_cycles))
        cocotb.log.info(" [caravel] finish drive_gpio_with_in_cycles ")

    async def wait_then_undrive(self, bits, num_cycles):
        await ClockCycles(self.clk, num_cycles)
        n_bits = bits[0] - bits[1] + 1
        self.drive_gpio_in(bits, (n_bits) * "z")
        cocotb.log.info(" [caravel] finish wait_then_drive ")

    """write to the house keeping registers by back door no need for commands and waiting for the data to show on mprj"""

    async def hk_write_backdoor(self, addr, data):
        await RisingEdge(self.dut.wb_clk_i)
        self.hk_hdl.wb_stb_i.value = 1
        self.hk_hdl.wb_cyc_i.value = 1
        self.hk_hdl.wb_sel_i.value = 0xF
        self.hk_hdl.wb_we_i.value = 1
        self.hk_hdl.wb_adr_i.value = addr
        self.hk_hdl.wb_dat_i.value = data
        cocotb.log.info(f"Monitor: Start Writing to {hex(addr)} -> {data}")
        await FallingEdge(self.dut.wb_ack_o)  # wait for acknowledge
        self.hk_hdl.wb_stb_i.value = 0
        self.hk_hdl.wb_cyc_i.value = 0
        cocotb.log.info(f"Monitor: End writing {hex(addr)} -> {data}")

    """read from the house keeping registers by back door no need for commands and waiting for the data to show on mprj"""

    async def hk_read_backdoor(self, addr):
        await RisingEdge(self.clk)
        self.hk_hdl.wb_stb_i.value = 1
        self.hk_hdl.wb_cyc_i.value = 1
        self.hk_hdl.wb_sel_i.value = 0
        self.hk_hdl.wb_we_i.value = 0
        self.hk_hdl.wb_adr_i.value = addr
        cocotb.log.info(f" [housekeeping] Monitor: Start reading from {hex(addr)}")
        await FallingEdge(self.hk_hdl.wb_ack_o)
        self.hk_hdl.wb_stb_i.value = 0
        self.hk_hdl.wb_cyc_i.value = 0
        cocotb.log.info(
            f" [housekeeping] Monitor: read from {hex(addr)} value {(self.hk_hdl.wb_dat_o.value)}"
        )
        return self.hk_hdl.wb_dat_o.value

    def setup_clock(self, period, unit="ns") -> cocotb.clock.Clock:
        """
        Configure caravel clock and start it

        :param int period: clock period
        :param str unit: One of ``'step'``, ``'fs'``, ``'ps'``, ``'ns'``, ``'us'``, ``'ms'``, ``'sec'``.When *unit* is ``'step'``,the timestep is determined by the simulator.
        :return: Object of type Caravel_env (caravel environment)
        """
        self.clock_obj = Clock(
            self.clk, period, unit
        )  # Create a 25ns period clock on port clk
        cocotb.start_soon(self.clock_obj.start())  # Start the clock
        return self.clock_obj

    def get_clock_obj(self) -> cocotb.clock.Clock:
        """return the used clock object of cocotb.clock.Clock used mostly to get info about simulation time or clock period info"""
        return self.clock_obj
    
    def get_clock_period(self) -> int:
        """return the used clock period in ns"""
        return self.clock_obj.period / 1000
