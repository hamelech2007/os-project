import gdb

class StopOnPValue(gdb.Command):
    """Run 'next' until variable 'p' equals 0x102000. If 'p' is undefined, continue until it is defined."""

    def __init__(self):
        super(StopOnPValue, self).__init__("stop_on_p_value", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        target_value = 0x10d000
        while True:
            try:
                # Attempt to evaluate the variable 'p'
                p_value = gdb.parse_and_eval("p")
                # If 'p' is defined and matches the target value, stop
                if int(p_value) == target_value:
                    print(f"Variable 'p' reached target value: {hex(target_value)}")
                    break
                # Otherwise, proceed to the next instruction
                gdb.execute("next")
            except gdb.error:
                # If 'p' is undefined, continue stepping until it is defined
                gdb.execute("next")

# Register the command
StopOnPValue()
