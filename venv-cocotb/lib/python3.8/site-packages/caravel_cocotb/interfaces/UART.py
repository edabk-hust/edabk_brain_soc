import cocotb
from cocotb.triggers import Timer, FallingEdge, NextTimeStep
from caravel_cocotb.interfaces.caravel import Caravel_env


class UART:
    """UART Verification environment to provide APIs to communicate with caravel UART through caravel gpios

    :param Caravel_env caravelEnv: caravel environment"""

    def __init__(self, caravelEnv: Caravel_env, uart_pins={"tx": 6, "rx": 5}) -> None:
        self.caravelEnv = caravelEnv
        clock = caravelEnv.get_clock_obj()
        self.period = clock.period / 1000
        self.bit_time_ns = round(
            1.01 * 10**5 * self.period / (96)
        )  # 10% factor of safety
        cocotb.log.info(f"[UART] configure UART bit_time_ns = {self.bit_time_ns}ns")
        self.uart_pins = uart_pins

    async def get_line(self):
        """Read line sent through UART (msg is sent by the software)

        - Line is a bunch of ASCII sybmols ended by linefeed '\\\\n'"""
        line = ""
        while True:
            new_char = await self.get_char()
            if new_char == "\n":
                break
            cocotb.log.info(f"[UART] new char = {new_char}")
            line += new_char
            cocotb.log.debug(f"[UART] part of the line recieved = {line}")
        cocotb.log.info(f"[UART] line recieved = {line}")
        return line[0:-1]

    async def get_int(self) -> int:
        """read int sent by firmware API uart_put_int
        """
        line = ""
        while True:
            new_char = await self.get_char()
            if new_char == "\n":
                break
            cocotb.log.info(f"[UART] new hex = 0x{new_char}")
            line += new_char
            cocotb.log.debug(f"[UART] part of the line recieved = {line}")
        line = line[::-1]
        cocotb.log.info(f"[UART] line recieved = 0x{line}")
        return int(line, 16)

    async def get_char(self):
        """Read character sent through UART (character is sent by the software)

        - Character is a 8 bit ASCII symbol"""
        await self.start_of_tx()
        char = ""
        for i in range(8):
            char = self.caravelEnv.monitor_gpio(self.uart_pins["tx"]).binstr + char
            await Timer(self.bit_time_ns, units="ns")
        return chr(int(char, 2))

    async def start_of_tx(self):
        while True:
            await FallingEdge(self.caravelEnv.dut._id(f"gpio{self.uart_pins['tx']}_monitor", False))
            await Timer(2, units="ns")
            if self.caravelEnv.dut._id(f"gpio{self.uart_pins['tx']}_monitor", False).value == 1:
                continue  # to skip latches
            await Timer(self.bit_time_ns - 2, units="ns")
            await Timer(int(self.bit_time_ns/2), units="ns")  # read the bit from the middle
            break

    async def uart_send_char(self, char):
        """Send character to UART (character is sent to the software)

        - Character is a 8 bit ASCII symbol"""
        char_bits = [int(x) for x in "{:08b}".format(ord(char))]
        cocotb.log.info(f"[TEST] start sending on UART {char}")
        # send start bit
        self.caravelEnv.drive_gpio_in(self.uart_pins["rx"], 0)
        extra_time = 0
        if "CPU_TYPE_ARM" in self.caravelEnv.design_macros._asdict():
            extra_time = (
                -479 * self.period
            )  # there is state 1 which takes 11975 ns and this time isn't in ARM only
        cocotb.log.info(f"[TEST] extra_time = {extra_time}ns")

        await Timer(self.bit_time_ns + extra_time, units="ns")
        # send bits
        for i in reversed(range(8)):
            self.caravelEnv.drive_gpio_in(self.uart_pins["rx"], char_bits[i])
            await Timer(self.bit_time_ns, units="ns")
            await NextTimeStep()

        # stop of frame
        self.caravelEnv.drive_gpio_in(self.uart_pins["rx"], 1)
        await Timer(self.bit_time_ns, units="ns")
        await Timer(self.bit_time_ns, units="ns")
        # insert 4 bit delay just for debugging
        await Timer(self.bit_time_ns, units="ns")
        await Timer(self.bit_time_ns, units="ns")
        await Timer(self.bit_time_ns, units="ns")
        await Timer(self.bit_time_ns, units="ns")

    async def uart_send_line(self, line):
        """Send line to UART (msg is sent to the software)

        - Line is a bunch of ASCII symbols  ended by linefeed '\\\\n'
        """
        for char in line:
            await self.uart_send_char(char)

        # end of line \n
        await self.uart_send_char("\n")
