# VelocityAdvisor Firmware
## Purpose
- NK's popular speed coach utility costs upwards of $400 USD
- I do not want to pay $400 USD despite all the good engineering
- Therefor I have set out to assemble a primitive rowing utility
## Goals
- Track live splits in 500m split form
- Record splits piece by piece in short term memory
- Display time pased since recording began
- Have Active, paused, and reset recording states
- User interaction with at least one button
- Accelerometer data for stroke rate in all recording states
- Add data recording to microSD in various supported GPS file formats
    - Preferably used by most fitness GPS platforms
## Design
- Incremental Approach
    - Began with display
    - Moved to GPS
    - Development cycle for split information
    - Add accelerometer data to display
## Wiring
![](/images/Speedcoach_circuit_diagram.png)
## Prototypes
![](/images/sample1.HEIC)
![](/images/sample2.HEIC)