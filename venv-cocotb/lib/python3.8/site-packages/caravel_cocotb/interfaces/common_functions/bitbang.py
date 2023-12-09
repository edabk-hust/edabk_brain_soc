from interfaces.common import SPI_COMMAND


"""
reg_mprj_xfer contain
bit 0 : busy
bit 1 : bitbang enable
bit 2 : bitbang reset active low
bit 3 : bitbang load registers
bit 4 : bitbang clock
bit 5 : serial data 1
bit 6 : serial data 2
"""

reg_mprj_xfer = 0x13


async def bb_clock11_spi(caravelEnv):
    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x66)  # Data = 0x01 (enable bit-bang mode)
    await caravelEnv.disable_csb()

    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x76)  # 11
    await caravelEnv.disable_csb()


async def bb_clock00_spi(caravelEnv):
    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x06)  # Data = 0x01 (enable bit-bang mode)
    await caravelEnv.disable_csb()

    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x16)  # 00
    await caravelEnv.disable_csb()


async def bb_clock01_spi(caravelEnv):
    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x26)  # Data = 0x01 (enable bit-bang mode)
    await caravelEnv.disable_csb()

    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x36)  # 01
    await caravelEnv.disable_csb()


async def bb_clock10_spi(caravelEnv):
    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x46)  # Data = 0x01 (enable bit-bang mode)
    await caravelEnv.disable_csb()

    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x56)  # 10
    await caravelEnv.disable_csb()


async def bb_load_spi(caravelEnv):
    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x0E)  # load enable
    await caravelEnv.disable_csb()

    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x06)  # 00
    await caravelEnv.disable_csb()


async def bb_reset_spi(caravelEnv):
    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x04)  # load enable
    await caravelEnv.disable_csb()

    await caravelEnv.enable_csb()
    await caravelEnv.hk_write_byte(
        SPI_COMMAND.WRITE_STREAM.value
    )  # Write stream command
    await caravelEnv.hk_write_byte(
        reg_mprj_xfer
    )  # Address (register 19 = GPIO bit-bang control)
    await caravelEnv.hk_write_byte(0x06)  # 00
    await caravelEnv.disable_csb()


# configure the GPIO  in the left chain with configL and the GPIO  in
# the right chain with configR
# left | right
# 18	& 19
# 17	& 20
# 16	& 21
# 15	& 22
# 14	& 23
# 13	& 24
# 12	& 25
# 11	& 26
# 10	& 27
# 9	& 28
# 8	& 29
# 7	& 30
# 6	& 31
# 5	& 32
# 4	& 33
# 3	& 34
# 2	& 35
# 1	& 36
# 0	& 37
async def bb_configure_2_gpios_spi(configL, configR, caravelEnv):
    from interfaces.common import get_gpio_num_bit

    num_bits = get_gpio_num_bit()
    mask = 0x1 << num_bits - 1
    for i in reversed(range(num_bits)):
        left = (configL & mask) >> i
        right = (configR & mask) >> i
        mask = mask >> 1
        if left:
            if right:
                await bb_clock11_spi(caravelEnv)
            else:
                await bb_clock10_spi(caravelEnv)

        else:
            if right:
                await bb_clock01_spi(caravelEnv)
            else:
                await bb_clock00_spi(caravelEnv)


async def bb_configure_all_gpios(config, caravelEnv):
    await bb_reset_spi(caravelEnv)
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 18	& 19
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 17	& 20
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 16	& 21
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 15	& 22
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 14	& 23
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 13	& 24
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 12	& 25
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 11	& 26
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 10	& 27
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 9	& 28
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 8	& 29
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 7	& 30
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 6	& 31
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 5	& 32
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 4	& 33
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 3	& 34
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 2	& 35
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 1	& 36
    await bb_configure_2_gpios_spi(config, config, caravelEnv)  # 0	& 37
    await bb_load_spi(caravelEnv)
