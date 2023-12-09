import cocotb 
from cocotb.triggers import RisingEdge


class WishboneInterface:
    """ Wishbone interface
        :param inputs: have the hdl for the inputs should be a dictionary and have key [clk,rst,stb,cyc,we,sel,addr,data]
        :param outputs: have the hdl for the outputs should be a dictionary and have key [ack,data]
        :param registers: have the hdl for the registers of class RegisterFile
        :type inputs: dict
        :type outputs: dict
        :type registers: RegisterFile
    """
    def __init__(self, inputs, outputs, registers):
        self.wishbone_in = {}
        self.wishbone_out = {}
        for key, hdl in inputs.items():
            self.wishbone_in[key] = _WishboneSignalInput(hdl)
        for key, hdl in outputs.items():
            self.wishbone_out[key] = _WishboneSignalOutput(hdl)
        self.registers = registers

    async def start(self):
        """start wishbone interface"""
        cocotb.log.info("[WishboneInterface][start] start wishbone interface")
        await cocotb.start(self.read_write_op())

    async def valid_cycle(self):
        cocotb.log.debug("[WishboneInterface][valid_cycle] start waiting over use access space valid cycle")
        while True:
            await RisingEdge(self.wishbone_in["clk"].get_hdl())

            # Check for reset
            if self.wishbone_in["rst"].get_val() == 1:
                self.registers.reset()
                continue

            # wait check for valid cycle 
            if self.wishbone_in["cyc"].get_val() == 0 or self.wishbone_in["stb"].get_val() == 0:
                self.wishbone_out["ack"].set_val(0)
                continue

            # check for valid address 
            if not self.registers.is_valid_address(self.wishbone_in["addr"].get_val()):
                self.wishbone_out["ack"].set_val(0)
                cocotb.log.error(f"[WishboneInterface][valid_cycle] trying to access invalid address {hex(self.wishbone_in['addr'].get_val())} the firmware will stuck waiting for ack that will never be sent")
                continue

            cocotb.log.debug("[WishboneInterface][valid_cycle] valid cycle")

            return

    async def read_write_op(self):
        while True:
            await self.valid_cycle()
            self.wishbone_out["ack"].set_val(1)

            # read operation 
            if self.wishbone_in["we"].get_val() == 0:
                cocotb.log.debug(f"[WishboneInterface][read_write_op] start reading address {hex(self.wishbone_in['addr'].get_val())}")
                data = self.registers.read(self.wishbone_in["addr"].get_val(), self.wishbone_in["sel"].get_val())
                self.wishbone_out["data"].set_val(data)
                continue

            # write operation
            cocotb.log.debug(f"[WishboneInterface][read_write_op] start writing address {hex(self.wishbone_in['addr'].get_val())} with data {hex(self.wishbone_in['data'].get_val())}")
            data = self.wishbone_in["data"].get_val()
            self.registers.write(self.wishbone_in["addr"].get_val(), data, self.wishbone_in["sel"].get_val())


class _WishboneSignalInput:
    def __init__(self, hdl):
        self.hdl = hdl

    def get_val(self):
        return self.hdl.value.integer

    def get_hdl(self):
        return self.hdl


class _WishboneSignalOutput:
    def __init__(self, hdl):
        self.hdl = hdl

    def set_val(self, val):
        self.hdl.value = val
    
    def get_hdl(self):
        return self.hdl