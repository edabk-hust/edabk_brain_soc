import cocotb
from cocotb.triggers import ClockCycles
import cocotb.log
import cocotb.simulator
from cocotb.handle import SimHandleBase
import caravel_cocotb.interfaces.common as common


class LA:
    def __init__(self, dut: SimHandleBase):
        self.dut = dut
        self.clk = dut.clock_tb
        self.core_hdl = dut.uut.chip_core.soc.core

    """ Configure the value of LA probes [0:127]
        writing 1 to any bit means bit acts as outputs from the cpu
        writing 0 to any bit means bit acts as inputs to the cpu """

    async def configure_la_en(self, bits, data):
        self.__drive_la_iena(bits, data)
        self.__drive_la_oenb(bits, data)
        await ClockCycles(self.clk, 1)

    def __drive_la_iena(self, bits, data):
        iena, n_bits = common.signal_value_size(self.core_hdl.la_ien_storage)
        cocotb.log.debug(
            f" [LA] before change iena with {iena} and data = {data} bit [{n_bits-1-bits[0]}]:[{n_bits-1-bits[1]}]"
        )
        iena[n_bits - 1 - bits[0] : n_bits - 1 - bits[1]] = data
        self.core_hdl.la_ien_storage.value = iena
        cocotb.log.info(f" [LA] drive reg_la_iena  with {hex(iena)}")

    def __drive_la_oenb(self, bits, data):
        oenb, n_bits = common.signal_value_size(self.core_hdl.la_oe_storage)
        cocotb.log.debug(
            f" [LA] before change oenb with {oenb} and data = {data} bit [{n_bits-1-bits[0]}]:[{n_bits-1-bits[1]}]"
        )
        oenb[n_bits - 1 - bits[0] : n_bits - 1 - bits[1]] = data
        self.core_hdl.la_oe_storage.value = oenb
        cocotb.log.info(f" [LA] drive reg_la_oenb  with {hex(oenb)}")

    """ update the value of LA data input from cpu to user project """

    def drive_la_data_to_user(self, bits, data):
        la, n_bits = common.signal_value_size(self.core_hdl.la_out_storage)
        cocotb.log.debug(
            f" [LA] before la data update with LA ={la} and data = {data} bit [{n_bits-1-bits[0]}]:[{n_bits-1-bits[1]}]"
        )
        la[n_bits - 1 - bits[0] : n_bits - 1 - bits[1]] = data
        self.core_hdl.la_out_storage.value = la
        cocotb.log.info(
            f" [LA] drive_la_data_to_user: drive data {hex(la)} to user project"
        )

    """return the value of LA data output from user project tp cpu"""

    def check_la_user_out(self):
        LA_out = self.core_hdl.la_input.value
        if LA_out.is_resolvable:
            cocotb.log.info(
                f" [LA] Monitor : reg_la_data_out from user = {hex(LA_out)}"
            )
        else:
            cocotb.log.info(f" [LA] Monitor : reg_la_data_out from user = {LA_out}")
        return LA_out

    """return the value of LA data output from user project tp cpu"""

    def check_la_ctrl_reg(self):
        LA_out = self.dut.uut.chip_core.la_oenb_mprj.value
        if LA_out.is_resolvable:
            cocotb.log.info(
                f" [LA] Monitor : reg_la_data_out from user = {hex(LA_out)}"
            )
        else:
            cocotb.log.info(f" [LA] Monitor : reg_la_data_out from user = {LA_out}")
        return LA_out
