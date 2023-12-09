from cocotb.handle import SimHandleBase
from cocotb.binary import BinaryValue
from enum import Enum
import cocotb
import os

"""return the value and the size of the signal"""


def signal_value_size(path: SimHandleBase):
    value = path.value
    size = value.n_bits
    return value, size


"""
Create a binaryValue object with all Z that helps when drive to drive only the bits needed
return value with all z and the size
"""


def signal_valueZ_size(path: SimHandleBase):
    value = path.value
    size = value.n_bits
    value = BinaryValue(value=int(size) * "z", n_bits=size)
    return value, size


def int_to_bin_list(number: bin, number_of_bits) -> list:
    data = bin(number)
    data = data[2:].zfill(number_of_bits)[::-1]
    bits = [int(bit) for bit in data]
    return bits


def drive_hdl(path, bits, data):
    hdl, n_bits = signal_value_size(path)
    is_list_of_lists = all(isinstance(x, list) for x in bits)
    is_list_of_tuples = all(isinstance(x, tuple) for x in bits)
    if is_list_of_lists | is_list_of_tuples:
        for i, bits2 in enumerate(bits):
            hdl[n_bits - 1 - bits2[0] : n_bits - 1 - bits2[1]] = data[i]
    else:
        hdl[n_bits - 1 - bits[0] : n_bits - 1 - bits[1]] = data
    path.value = hdl
    cocotb.log.debug(f" [common] drive { path._path }  with {hdl}")


"""Enum for GPIO modes valus used to configured the pins"""
tag = os.getenv("RUNTAG")
# config_file = f"sim.{tag.replace('/','.')}.configs" // TODO: fix this


# class GPIO_MODEMeta(EnumMeta):
#     def __call__(cls, *args, **kwargs):
#         value = kwargs.pop('value', None)
#         obj = super().__call__(*args, **kwargs)
#         obj._value_ = value
#         return obj


class GPIO_MODE(Enum):
    try: 
        GPIO_MODE_MGMT_STD_INPUT_NOPULL = int(
            cocotb.plusargs["GPIO_MODE_MGMT_STD_INPUT_NOPULL"]
        )
        GPIO_MODE_MGMT_STD_INPUT_PULLDOWN = int(
            cocotb.plusargs["GPIO_MODE_MGMT_STD_INPUT_PULLDOWN"]
        )
        GPIO_MODE_MGMT_STD_INPUT_PULLUP = int(
            cocotb.plusargs["GPIO_MODE_MGMT_STD_INPUT_PULLUP"]
        )
        GPIO_MODE_MGMT_STD_OUTPUT = int(cocotb.plusargs["GPIO_MODE_MGMT_STD_OUTPUT"])
        GPIO_MODE_MGMT_STD_BIDIRECTIONAL = int(
            cocotb.plusargs["GPIO_MODE_MGMT_STD_BIDIRECTIONAL"]
        )
        GPIO_MODE_MGMT_STD_ANALOG = int(cocotb.plusargs["GPIO_MODE_MGMT_STD_ANALOG"])
        GPIO_MODE_USER_STD_INPUT_NOPULL = int(
            cocotb.plusargs["GPIO_MODE_USER_STD_INPUT_NOPULL"]
        )
        GPIO_MODE_USER_STD_INPUT_PULLDOWN = int(
            cocotb.plusargs["GPIO_MODE_USER_STD_INPUT_PULLDOWN"]
        )
        GPIO_MODE_USER_STD_INPUT_PULLUP = int(
            cocotb.plusargs["GPIO_MODE_USER_STD_INPUT_PULLUP"]
        )
        GPIO_MODE_USER_STD_OUTPUT = int(cocotb.plusargs["GPIO_MODE_USER_STD_OUTPUT"])
        GPIO_MODE_USER_STD_BIDIRECTIONAL = int(
            cocotb.plusargs["GPIO_MODE_USER_STD_BIDIRECTIONAL"]
        )
        GPIO_MODE_USER_STD_OUT_MONITORED = int(
            cocotb.plusargs["GPIO_MODE_USER_STD_OUT_MONITORED"]
        )
        GPIO_MODE_USER_STD_ANALOG = int(cocotb.plusargs["GPIO_MODE_USER_STD_ANALOG"])
    except:
        cocotb.log.warning(f"Some configuration isn't valid please review it's define at user_defines file")
        pass
    


class MASK_GPIO_CTRL(Enum):
    MASK_GPIO_CTRL_MGMT_EN = 0
    MASK_GPIO_CTRL_OUT_DIS = 1
    MASK_GPIO_CTRL_OVERRIDE = 2
    MASK_GPIO_CTRL_INP_DIS = 3
    MASK_GPIO_CTRL_MOD_SEL = 4
    MASK_GPIO_CTRL_ANLG_EN = 5
    MASK_GPIO_CTRL_ANLG_SEL = 6
    MASK_GPIO_CTRL_ANLG_POL = 7
    MASK_GPIO_CTRL_SLOW = 8
    MASK_GPIO_CTRL_TRIP = 9
    MASK_GPIO_CTRL_DGTL_MODE = 10


def get_gpio_num_bit():
    if 1:
        return 13
    else:
        return 10
