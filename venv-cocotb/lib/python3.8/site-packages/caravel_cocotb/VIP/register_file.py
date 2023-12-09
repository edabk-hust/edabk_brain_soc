import cocotb


class RegisterFile:
    """Register file
        create a register file that can contain multiple registers
    """
    def __init__(self):
        self.registers = {}

    def add_register(self, name, address, reset_val=0, size=32):
        """
        Add a new register to the RegisterFile instance.

        Parameters:
            name (str): The name of the register. This will be used as an attribute name to access the register.
            address (int): The address of the register. Must be a unique value within the RegisterFile.
            reset_val (int, optional): The reset value of the register. Defaults to 0.
            size (int, optional): The size (in bits) of the register. Defaults to 32.
        Raises:
            ValueError: If the given address is already assigned to another register within the RegisterFile.
        Returns:
            None
        Example:
            reg_file = RegisterFile()
            reg_file.add_register("counter", 0x1000, reset_val=0, size=16)
        After calling this function, a new register with the specified properties will be added to the RegisterFile instance,
        and it can be accessed as an attribute using the given name. The Register instance can be accessed using `reg_file.counter`.
        """

        cocotb.log.debug(f"[RegisterFile][add_register] name={name}, address={hex(address)} reset_val={hex(reset_val)} size={size}")
        if address in self.registers:
            raise ValueError("Address already assigned")
        self.registers[address] = Register(name, reset_val, size)
        setattr(self, name, self.registers[address])

    def reset(self):
        """
        Reset all registers in the RegisterFile instance to their initial values.

        Example:
            reg_file = RegisterFile()
            reg_file.add_register("counter", 0x1000, reset_val=0, size=16)
            reg_file.add_register("status", 0x1004, reset_val=0, size=8)

            # Perform some operations on the registers

            reg_file.reset()

        After calling this function, all registers within the RegisterFile instance will be reset to their initial values.
        The initial values are specified during the register addition using the `reset_val` argument of the `add_register` method.
        """
        cocotb.log.debug("[RegisterFile][reset] reset all registers")
        for reg in self.registers.values():
            reg.reset()

    def read(self, address, select=0xF):
        """
        Read specific bytes from the specified register at the given address in the RegisterFile instance.

        Parameters:
            address (int): The address of the register to read.
            select (int): The select value used to determine which bytes to read from the register.

        Returns:
            int: The value read from the specified register, considering the selected bytes.

        Raises:
            ValueError: If the given address does not correspond to any registered register in the RegisterFile.

        Example:
            reg_file = RegisterFile()
            reg_file.add_register("counter", 0x1000, reset_val=0, size=16)
            reg_file.add_register("status", 0x1004, reset_val=0, size=8)

            # Perform some operations on the registers

            # Read the first two bytes of the "counter" register
            value = reg_file.read(0x1000, 0b0011)

            # Read the last two bytes of the "status" register
            value = reg_file.read(0x1004, 0b1100)

        This function reads the value from the specified register at the given address, considering the selected bytes based on the `select` parameter.
        The `select` parameter is used to determine which bytes to read from the register. Each bit of `select` corresponds to a byte in the register, and if a bit is set to 1, the corresponding byte will be read.
        If the address does not correspond to any registered register in the RegisterFile, a ValueError is raised.
        """
        cocotb.log.debug(f"[RegisterFile][read] read  from reg {self.registers[address].name}, address={hex(address)}, select={bin(select)[2:][::-1]}")
        if address in self.registers:
            return self.registers[address].read(select)
        else:
            raise ValueError("Invalid register address")

    def write(self, address, data, select=0xF):
        """
        Write specific bytes to the specified register at the given address in the RegisterFile instance.

        Parameters:
            address (int): The address of the register to write to.
            data (int): The data to be written to the register.
            select (int): The select value used to determine which bytes to write in the register.

        Returns:
            None

        Raises:
            ValueError: If the given address does not correspond to any registered register in the RegisterFile.

        Example:
            reg_file = RegisterFile()
            reg_file.add_register("counter", 0x1000, reset_val=0, size=16)
            reg_file.add_register("status", 0x1004, reset_val=0, size=8)

            # Perform some operations on the registers

            # Write the value 0xABCD to the "counter" register, updating the first two bytes
            reg_file.write(0x1000, 0xABCD, 0b0011)

            # Write the value 0x12 to the "status" register, updating the last two bytes
            reg_file.write(0x1004, 0x12, 0b1100)

        This function writes specific bytes to the specified register at the given address in the RegisterFile instance.
        The `select` parameter is used to determine which bytes to update in the register. Each bit of `select` corresponds to a byte in the register, and if a bit is set to 1, the corresponding byte will be updated with the corresponding part of the data.
        If the address does not correspond to any registered register in the RegisterFile, a ValueError is raised.
        """
        cocotb.log.debug(f"[RegisterFile][write] write to reg {self.registers[address].name}, address={hex(address)}, data={hex(data)}, select={bin(select)[2:][::-1]}")
        if address in self.registers:
            self.registers[address].write(data, select)
        else:
            raise ValueError("Invalid register address")

    def is_valid_address(self, address):
        if address not in self.registers:
            return False
        return True


class Register:
    def __init__(self, name, reset_val, size):
        self.name = name
        self.size = size
        self.value = [0] * (size // 8)  # Initialize the register value to zeros
        self.reset_val = reset_val
        self.reset()

    def reset(self):
        self.write(self.reset_val, 0xf)

    def read(self, select):
        data = 0
        binary_select = bin(select)[2:][::-1]
        for index, sel in enumerate(binary_select):
            if sel == "1":
                masking = self.value[index] << (index * 8)
                data |= masking
        cocotb.log.debug(f"[Register][read] value={[hex(hex_val) for hex_val in self.value]}, select={binary_select} data={hex(data)}")
        return data

    def write(self, data, select):
        binary_select = bin(select)[2:][::-1]
        for index, sel in enumerate(binary_select):
            if sel == "1":
                byte = data >> (index * 8) & 0xFF
                self.value[index] = byte
        cocotb.log.debug(f"[Register][write] value={[hex(hex_val) for hex_val in self.value]}, select={bin(select)[2:][::-1]} data={hex(data)} value[0]={self.value[0]}")