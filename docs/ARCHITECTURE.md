# Architecture and limitations

```text
Analog speed input (A5) ---> Sensing_Data (Mbed Thread)
Analog obstacle input (A4) ------> |
                                   v
                             Mail<Data_sense_t,6>
                                   |
                                   v
                            Control thread
                                   |
                        Threshold decision
                                   |
                    PWM indication / status LEDs

Button interrupts ---> ignition / parking state handlers
```

Front position ADC (A2) is declared, but the original implementation calculates a proxy as `round(0.4166 * Speed)` rather than measuring actual front distance. No calibrated physical units or measured real-world stopping distance are available.

Threshold decisions of 10 and 5 project units overlap. Shared flags have no synchronization, PWM speed may exceed valid bounds, and ignition state/flag logic requires review. There is no sensor fault validation, watchdog, ISO 26262 safety case, road test, or actuator interface.

Desktop simulation is intentionally isolated from the Mbed firmware; passing Python tests does not validate the embedded logic. Do not use this prototype for a vehicle.
