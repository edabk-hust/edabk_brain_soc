from caravel_cocotb.caravel_interfaces import test_configure
from caravel_cocotb.caravel_interfaces import report_test
import cocotb

@cocotb.test()
@report_test
async def counter_la_reset(dut):
    caravelEnv = await test_configure(dut,timeout_cycles=1346140)

    cocotb.log.info(f"[TEST] Start counter_wb test")  
    # wait for start of sending
    await caravelEnv.release_csb()
    await caravelEnv.wait_mgmt_gpio(1)
    cocotb.log.info(f"[TEST] finish configuration") 
    overwrite_val = 0 # because of the reset 
    # expect value bigger than 7 
    received_val = int ((caravelEnv.monitor_gpio(37,30).binstr + caravelEnv.monitor_gpio(7,0).binstr ),2) 
    counter = received_val
    if received_val <= overwrite_val :
        cocotb.log.error(f"counter started late and value captured after configuration is smaller than overwrite value: {overwrite_val} receieved: {received_val}")
    await cocotb.triggers.ClockCycles(caravelEnv.clk,1)

    while True: # wait until reset asserted
        if await get_reset_val(caravelEnv) == 1: 
            cocotb.log.info(f"[TEST] Reset asserted by la")  
            break
    while True: # wait until reset deasserted
        if await get_reset_val(caravelEnv) == 0: 
            cocotb.log.info(f"[TEST] Reset deasserted by la")  
            break
    counter =0

    for i in range(100):
        if counter != int ((caravelEnv.monitor_gpio(37,30).binstr + caravelEnv.monitor_gpio(7,0).binstr ),2) :
            cocotb.log.error(f"counter have wrong value expected = {counter} recieved = {int ((caravelEnv.monitor_gpio(37,30).binstr + caravelEnv.monitor_gpio(7,0).binstr ),2) }")
        await cocotb.triggers.ClockCycles(caravelEnv.clk,1)
        counter +=1
    
async def get_reset_val(caravelEnv): 
    """ get the counter reset value"""
    await cocotb.triggers.ClockCycles(caravelEnv.clk,1)
    return caravelEnv.user_hdl.mprj.counter.reset.value
