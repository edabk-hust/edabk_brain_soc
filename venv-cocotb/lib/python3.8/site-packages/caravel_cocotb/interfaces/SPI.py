import cocotb
from cocotb.triggers import FallingEdge, RisingEdge, Timer
from caravel_cocotb.interfaces.caravel import Caravel_env
from cocotb.clock import Clock
from cocotb.binary import BinaryValue
from enum import IntEnum


class SPI:
    """The interface is acting as an spi master. It communicates mainly with the slave inside the housekeeping

    :param caravelEnv: The Caravel environment object to use.
    :type caravelEnv: Caravel_env
    :param clk_period: The clock period in microseconds, or None to use the default (3 times the Caravel clock period).
    :type clk_period: float or None
    :param spi_pins: The SPI pins to use, as a dictionary with keys "CSB", "SCK", "SDO", and "SDI".
    :type spi_pins: dict[str, int]
    """

    def __init__(self, caravelEnv: Caravel_env, clk_period=None, spi_pins={"CSB": 3, "SCK": 4, "SDO": 2, "SDI": 1}) -> None:
        self.caravelEnv = caravelEnv
        self.spi_pins = spi_pins
        # if clock period is not given, use caravel clock * 3
        self.clk_period = clk_period if clk_period is not None else self.caravelEnv.get_clock_obj().period*3 / 1000
        self.caravelEnv.drive_gpio_in(self.spi_pins["SDO"], 0)
        # self._setup_spi_clk()

    def _setup_spi_clk(self):
        """Setup the spi clock"""
        self.caravelEnv.dut._id(f"gpio{self.spi_pins['SCK']}_en", False).value = 1
        # await cocotb.triggers.NextTimeStep()
        self.clk = self.caravelEnv.dut._id(f"gpio{self.spi_pins['SCK']}", False)
        clock = Clock(self.clk, self.clk_period, "ns")
        self.clock_op_thread = cocotb.start_soon(clock.start())
        self.start_time = cocotb.utils.get_sim_time("ns")

    def _kill_spi_clk(self):
        self.clock_op_thread.kill()
        self.caravelEnv.drive_gpio_in(self.spi_pins["SCK"], 0)

    async def _hk_read_byte(self):
        """Reads a byte from a housekeeping spi.
        Returns:
            int: The byte read from the spi.
        """
        read_data = ""
        # self._setup_spi_clk()
        cocotb.log.debug("[SPI][_hk_read_byte] reading from SDI")
        for i in range(8):
            await RisingEdge(self.clk)
            # await cocotb.triggers.NextTimeStep()
            read_data = f"{read_data}{self.caravelEnv.monitor_gpio(self.spi_pins['SDI'])}"
            cocotb.log.debug(f"[SPI][_hk_read_byte] read bit {i} = {hex(int(read_data, 2))}({read_data})")
        return int(read_data, 2)

    async def _hk_read_byte_with_failing(self):
        """Reads a byte from a housekeeping spi.
        Returns:
            int: The byte read from the spi.
        """
        read_data = ""
        # self._setup_spi_clk()
        cocotb.log.debug("[SPI][_hk_read_byte] reading from SDI")
        for i in range(8):
            await FallingEdge(self.clk)
            # await Timer(10, "ns")
            read_data = f"{read_data}{self.caravelEnv.monitor_gpio(self.spi_pins['SDI'])}"
        return int(read_data, 2)

    async def _hk_write_byte(self, data):
        """
        Writes a byte of data to housekeeping slave using SDI

        Args:
        - data (int): The 8-bit data to be written.

        Returns:
        - None
        """
        data_bit = BinaryValue(value=data, n_bits=8, bigEndian=False)
        cocotb.log.debug(f"[SPI][_hk_write_byte] writing {hex(data_bit)} to SDO")
        for i in range(7, -1, -1):
            await FallingEdge(self.clk)
            self.caravelEnv.drive_gpio_in(self.spi_pins["SDO"], int(data_bit[i]))

    async def _hk_write_read_byte(self, data):
        read_data = ""
        data_bit = BinaryValue(value=data, n_bits=8, bigEndian=False)
        cocotb.log.debug(f"[SPI][_hk_write_read_byte] writing {hex(data_bit)} to SDO and reading back")
        for i in range(7, -1, -1):
            await FallingEdge(self.clk)
            self.caravelEnv.drive_gpio_in(self.spi_pins["SDO"], int(data_bit[i]))
            await RisingEdge(self.clk)
            # await cocotb.triggers.NextTimeStep()
            read_data = f"{read_data}{self.caravelEnv.monitor_gpio(self.spi_pins['SDI'])}"
        return int(read_data, 2)

    def _get_cycle_number(self):

        current_time = cocotb.utils.get_sim_time("ns")
        cycle_num = (current_time - self.start_time)/self.clk_period
        return cycle_num

    async def disable_csb(self):
        """
        Disables the housekeeping SPI transmission by driving the CSB line high.

        **Note:** The function waits for 2 clock cycles before disabling to ensure that the previous writing command has been applied. 
        It then drives the CSB line high and kills the SPI clock.Finally, the function waits for some time to ensure that the disable has taken effect.
        """
        cocotb.log.info("[SPI][disable_csb] disable housekeeping SPI transmission")
        # should wait for 2 clock cycles before disabling 
        # to wait until writing command got applied
        await Timer(self.clk_period, "ns")
        await Timer(self.clk_period, "ns")
        self.caravelEnv.drive_gpio_in(self.spi_pins["CSB"], 1)
        self._kill_spi_clk()
        # must wait for some time for the disable get affected
        await Timer(self.clk_period, "ns")

    async def enable_csb(self):
        """
        Enable the chip select bar (CSB) pin for SPI transmission.

        **Note:** This method drives the CSB pin low and sets up the SPI clock. It also waits for some time after disabling the CSB pin for the changes to take effect.
        """
        cocotb.log.info("[SPI][enable_csb] enable housekeeping SPI transmission")
        if self.caravelEnv.monitor_gpio(self.spi_pins["CSB"]) == 0:
            cocotb.log.info("[SPI][enable_csb] housekeeping SPI transmission already enabled")
            return
        # must wait for some time for the disable get affected
        await Timer(self.clk_period, "ns")
        self._setup_spi_clk()
        await Timer(self.clk_period*0.5, "ns")
        self.caravelEnv.drive_gpio_in(self.spi_pins["CSB"], 0)

    async def write_reg_spi(self, address, data, disable_csb: bool = True):
        """
        Writes a register over SPI using the housekeeping SPI bus.

        :param address: The address of the register to write.
        :type address: int
        :param data: The value to write to the register.
        :type data: int
        :param disable_csb: Whether to disable the chip select line after the write is complete. Defaults to True.
        :type disable_csb: bool

        :return: None
        """
        cocotb.log.info(f"[SPI][write_reg_spi] writing address {hex(address)}({bin(address)}) with data {hex(data)}({bin(data)}) using housekeeping SPI")
        await self.enable_csb()
        await self._hk_write_byte(SPI.SPI_COMMAND.WRITE_STREAM.value)
        await self._hk_write_byte(address)
        await self._hk_write_byte(data)
        if disable_csb:
            await self.disable_csb()

    async def read_reg_spi(self, address, disable_csb: bool = True):
        """
        Reads a byte from a register at the given address over the housekeeping SPI interface.

        
        :param address: The address of the register to read from.
        :type address: int
        :param disable_csb: Whether to disable the chip select line after reading the byte. Defaults to True.
        :type disable_csb: bool

        :return: int: The byte read from the register.
        """
        cocotb.log.info(f"[SPI][read_reg_spi] reading {hex(address)} using housekeeping SPI")
        await self.enable_csb()
        await self._hk_write_byte(SPI.SPI_COMMAND.READ_STREAM.value)
        await self._hk_write_byte(address)
        await RisingEdge(self.clk)
        data = await self._hk_read_byte()
        if disable_csb:
            await self.disable_csb()
        return data

    async def read_write_reg_spi(self, address, data, disable_csb: bool = True):
        """
        Writes to and reads from a register at the given SPI address using the housekeeping SPI.

        :param address: The address of the register to write to/read from.
        :type address: int
        :param data: The data to write to the register.
        :type data: int
        :param disable_csb: Whether to disable the chip select line after the operation. Defaults to True.
        :type disable_csb: bool
        :return: int: The data read from the register.

        :examples:

            >>> result = await read_write_reg_spi(0x14, 0xAB)
            >>> print(hex(result))
            0x42
            >>> result = await read_write_reg_spi(0x14, 0xD7)
            >>> print(hex(result))
            0xAB
        """
        cocotb.log.info(f"[SPI][read_write_reg_spi] writing address {hex(address)}({bin(address)}) with data {hex(data)}({bin(data)}) using housekeeping SPI")
        await self.enable_csb()
        await self._hk_write_byte(SPI.SPI_COMMAND.WRITE_READ.value)
        await self._hk_write_byte(address)
        data = await self._hk_write_read_byte(data)
        if disable_csb:
            await self.disable_csb()
        return data
    
    async def write_reg_spi_nbytes(self, address, data, n_bytes, disable_csb: bool = True):
        """
        Writes to `n_bytes` bytes starting from the register at `address` over the housekeeping SPI.

        :param address: The address of the register to write.
        :type address: int
        :param data: The data to be written to the register.
        :type data: list(int)
        :param n_bytes: The number of bytes to be written.
        :type n_bytes: int
        :param disable_csb: Whether to disable chip select after write. Defaults to True.
        :type disable_csb: bool
        :return: None
        """
        cocotb.log.info(f"[SPI][write_reg_spi_nbytes] writing address {hex(address)}({bin(address)}) with data {data} using housekeeping SPI")
        write_command = SPI.SPI_COMMAND.WRITE_STREAM.value | n_bytes << 3
        await self.enable_csb()
        await self._hk_write_byte(write_command)  # Write n byte command
        await self._hk_write_byte(address)
        for byte in data:
            await self._hk_write_byte(byte)  # Data
        if disable_csb:
            await self.disable_csb()

    async def read_reg_spi_nbytes(self, address, n_bytes, disable_csb: bool = True):
        """
        Read `n_bytes` bytes starting from the register at `address` over the housekeeping SPI.

        :param address: The address of the register to read.
        :type address: int
        :param n_bytes: The number of bytes to read.
        :type n_bytes: int
        :param disable_csb: Whether to disable chip select after reading. Defaults to True.
        :type disable_csb: bool
        :return: A list with `n_bytes` integers, where each integer is a byte read from the register.
        """
        cocotb.log.info(f"[SPI][read_reg_spi_nbytes] reading {hex(address)}({bin(address)}) using housekeeping SPI")
        data = []
        await self.enable_csb()
        await self._hk_write_byte(SPI.SPI_COMMAND.READ_STREAM.value | n_bytes << 3)
        await self._hk_write_byte(address)
        await RisingEdge(self.clk)
        for i in range(n_bytes):
            data.append(await self._hk_read_byte())
        if disable_csb:
            await self.disable_csb()
        return data

    async def read_write_reg_nbytes(self, address, data_in, n_bytes, disable_csb: bool = True):
        """
        Read and write `n_bytes` bytes starting from the register at `address` over the housekeeping SPI.

        :param address: The address of the register to read.
        :type address: int
        :param data: The data to be written to the register.
        :type data: list(int)
        :param n_bytes: The number of bytes to read.
        :type n_bytes: int
        :param disable_csb: Whether to disable chip select after reading. Defaults to True.
        :type disable_csb: bool
        :return: A list with `n_bytes` integers, where each integer is a byte read from the register.
        """
        cocotb.log.info(f"[SPI][read_write_reg_nbytes] reading {hex(address)} and writing {data_in} using housekeeping SPI")
        data = []
        await self.enable_csb()
        await self._hk_write_byte(SPI.SPI_COMMAND.WRITE_READ.value | n_bytes << 3)
        await self._hk_write_byte(address)
        await RisingEdge(self.clk)
        for byte in data_in:
            data.append(await self._hk_write_read_byte(byte))
        if disable_csb:
            await self.disable_csb()
        return data

    async def reg_spi_user_pass_thru(self, send_data: list, read_byte_num: int = 0, disable_csb: bool = True):
        """
        Sends SPI data to a housekeeping SPI using user pass-thru command.
        
        :param send_data: A list of data to be sent includeing the commands and addresses.
        :type send_data: list
        :param read_byte_num: expected number of bytes to be read defaults to 0.
        :type read_byte_num: int
        :param disable_csb: Whether to disable CSB after the transaction defaults to True.
        :type disable_csb: bool
        :return: A list of read data if `read_byte_num` is not 0.
        """
        cocotb.log.info(f"[SPI][reg_spi_user_pass_thru] send data{send_data}")
        await self.enable_csb()
        # Apply user pass-thru command to housekeeping SPI
        await self._hk_write_byte(SPI.SPI_COMMAND.USER_PASS_THRU.value)
        await RisingEdge(self.clk)
        # SEND
        for data in send_data:
            await self._hk_write_byte(data)
        # READ
        data = []
        for i in range(read_byte_num):
            data.append(await self._hk_read_byte())
        if disable_csb:
            await self.disable_csb()
        return data

    async def reg_spi_mgmt_pass_thru_read(self, address: int, read_byte_num: int = 1, disable_csb: bool = True):
        """
        Sends SPI read data command to a housekeeping SPI using managment pass-thru command.
        
        :param address: The address of the register to read.
        :type address: int
        :param read_byte_num: expected number of bytes to be read defaults to 1.
        :type read_byte_num: int
        :param disable_csb: Whether to disable CSB after the transaction defaults to True.
        :type disable_csb: bool        
        """
        cocotb.log.debug(f"[SPI][reg_spi_mgmt_pass_thru_read] read addr {hex(address)}")
        await self.enable_csb()
        # Apply user pass-thru command to housekeeping SPI
        await self._hk_write_byte(SPI.SPI_COMMAND.MGMT_PATH_THRU.value)
        await RisingEdge(self.clk)
        # command and address
        await self._hk_write_byte(0x03)  # read
        addr_bits = bin(address)[2:].zfill(24)
        cocotb.log.debug(f"[SPI][reg_spi_mgmt_pass_thru_read] send addr {addr_bits}")
        for i in range(3):
            address_byte = int(addr_bits[i*8:(i+1)*8], 2)
            cocotb.log.debug(f"[SPI][reg_spi_mgmt_pass_thru_read] send addr {hex(address_byte)} i = {i}")
            await self._hk_write_byte(address_byte)
        # READ
        data = []
        await RisingEdge(self.clk)

        for i in range(read_byte_num):
            data.append(await self._hk_read_byte())
        if disable_csb:
            await self.disable_csb()
        return data

    # use for configure in mgmt pass thru or user pass thru or any other command that doesn't have a function 
    async def reg_spi_op(self, command, address, disable_csb: bool = True):
        """
        Perform a register SPI operation.

        :param command: The SPI command byte.
        :type command: int
        :param address: The SPI address byte.
        :type address: int
        :param disable_csb: Whether to disable the chip select line after the operation. Defaults to True.
        :type disable_csb: bool
        :return: None
        """
        await self.enable_csb()
        await self._hk_write_byte(command)
        await self._hk_write_byte(address)
        if disable_csb:
            await self.disable_csb()

    class SPI_COMMAND(IntEnum):
        """
        enum class representing SPI command bytes.

        members:
            :NO_OP: A command byte representing a no-op operation.
            :WRITE_STREAM: A command byte representing a write stream operation.
            :READ_STREAM: A command byte representing a read stream operation.
            :WRITE_READ: A command byte representing a write-and-read operation.
            :USER_PASS_THRU: A command byte representing a user pass-through operation.
            :MGMT_PATH_THRU: A command byte representing a management path pass-through operation.
        """

        NO_OP = 0x0
        WRITE_STREAM = 0x80
        READ_STREAM = 0x40
        WRITE_READ = 0xC0
        USER_PASS_THRU = 0x02
        MGMT_PATH_THRU = 0xC4

