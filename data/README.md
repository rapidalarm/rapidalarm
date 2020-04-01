# Sample Data 2020-04-01

- `exit_port_leak` - opened then closed hole near exit port, bypassing rate valve
- `lung_disconnect` - disconnected then reconnected false lung
- `lung_partial_disconnect` - loosely disconnected then reconnected false lung, (lung stopped cycling and remained inflated)
- `source_disconnect` - complete disconnect then reconnect of pressured air

This is sample data from tests with the ventilator setup at CTS.  Data was sampled in a free running loop at approximately 133Hz.  Columns 1 and 2 correspond to pitot tube pressure and wall pressure, respectively.

To convert the measurements to cm H20, use the expression

    10.197 * (x / 1024 - 0.03) / 0.09
    
