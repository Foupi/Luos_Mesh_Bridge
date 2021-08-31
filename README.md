# Luos Mesh Bridge

This project is a proof of concept for the implementation of a Bridge
between two or more Luos networks over a Bluetooth Mesh network. It is
designed to run exclusively on Nordic PCA10040 boards.

The system is composed of at least three nodes:
- The Provisioner node, which role is to add nodes in the Bluetooth Mesh
network.
- The Bridge node, hosting a Gate container and a Mesh Bridge container,
interfacing the user with the other Luos networks.
- One or more Actuator node(s), hosting a Mesh Bridge container and a
LED Toggler container.

The Bridge node shall be connected via UART to a pilot computer, so that
the user can communicate with the Luos networks through a serial
emulator.

## Mandatory packages

In order for all the steps in this README to be executed, the packages
listed below must be present on the host environment:

* `sudo`: There shall be root operations (mostly regarding packages).

* `docker` _(and friends)_: Build and flash operations shall be
performed using a container.

## Available programs

This repository contains the source code of the three nodes decribed
above:

* The Provisioner node, in the `provisioner_node` directory.
* The Bridge node, in the `bridge_node` directory.
* The Actuator node, in the `actuator_node` directory.

Along with these nodes, two test programs are provided in the `tests`
directory:

* `build_rtb`, test for the RTB Builder container: runs a detection at
each press of Button 1.
* `container_generator`, test for the `Luos_DestroyContainer` function:
Creates a dummy container and runs a detection at each press of Button
1, destroys a dummy container and runs a detection at each press of
Button 2.

In the following parts of this document, the name of a program refers
to the name of the directory containing its sources.

## Build

A Docker image of the environment of this project shall first be created
using the `Dockerfile` present at the root of this repository:

```bash
docker build -t <IMAGE_NAME> .
```

Once the image is built, a program can be built using the following
command:

```bash
docker run -t                                                               \ # Display output on terminal.
    -v $(pwd)/<HOST_OUTPUT_DIR>:/luos_mesh_bridge/<CONTAINER_OUTPUT_DIR>    \ # Link host and container output directories.
    <IMAGE_NAME>                                                            \ # Name provided to the `docker build` command.
    [BUILD_DIR_NAME=<CONTAINER_BUILD_DIR>]                                  \ # Default build directory is named `build`.
    [OUTPUT_DIR_NAME=<CONTAINER_OUTPUT_DIR>]                                \ # Default output directory is named `output`.
    ./build.sh <PROGRAM_NAME> [<PROGRAM_NAME> ...]                          \ # This script can build several programs.
```

A built program can be found in the corresponding
`<HOST_OUTPUT_DIR>/<PROGRAM_NAME>` folder, with the name
`<PROGRAM_NAME>_merged.hex`.

## Flash

In order to flash a built program on a board, the Docker image of the
project environment shall already have been created.

A built program can be flashed on a board using the following command:

```bash
docker run -t                                                               \ # Display output on terminal.
    --privileged=true                                                       \ # Allows JLink to access the boards.
    -v $(pwd)/<HOST_OUTPUT_DIR>:/luos_mesh_bridge/<CONTAINER_OUTPUT_DIR>    \ # Link host and container output directories.
    <IMAGE_NAME>                                                            \ # Name provided to the `docker build` command.
    [OUTPUT_DIR_NAME=<CONTAINER_OUTPUT_DIR>]                                \ # Default output directory is named `output`.
    ./flash.sh <PROGRAM_NAME> <BOARD_NUMBER>                                \ # <BOARD_NUMBER> being the serial number of the board.
        [(<PROGRAM_NAME> <BOARD_NUMBER>) ...]                               \ # This script can flash several programs.
```

Each program name must be followed with the serial number of a board.

## Physical setup

If programs are run in `DEBUG` mode, they must be connected to the pilot
PC to allow reading logs through the `JLinkRTTViewerExe` command.

The Bridge node shall be connected to the pilot PC through UART with the
following connections:

| **PC FUNCTIONALITY** | **BOARD FUNCTIONALITY** | **BOARD PIN** |
| -------------------- | ----------------------- | ------------- |
| GND | GND | GND |
| RX | TX | P0.06 |
| TX | RX | P0.08 |

The terminal emulator settings for communication with the Gate container
must be the following:

| **VARIABLE** | **VALUE** |
| ------------ | --------- |
| Baudrate | 115 200 |
| Word length | 8 |
| Stop bits | 1 |
| Parity | None |
| Hardware Flow Control | None |

## System usage

When at least the Provisioner node, the Bridge node and at least one
Actuator node are flashed:

* Press the Button 1 of the Provisioner node to start scanning for
unprovisioned devices.

* Once all nodes have been provisioned in the Bluetooth Mesh network,
press once again the Button 1 of the Provisioner node to stop scanning.

* In the terminal emulator connected to the Bridge node, enter the
following command to run a detection of its local Luos network:

```
{"detection":{}}
```

* Once the detection has been run and the Mesh Bridge local container
table has been filled, you can run the Luos routing table extension
procedure by entering the following command:

```
{"containers":{"mesh_bridge":{"ext_rtb":{}}}}
```

* Once the extension procedure is complete, you can start manipulating
the containers of the Actuator nodes through the terminal emulator. You
can see the extent of possible interactions here:
<https://docs.luos.io/pages/software/json-api.html>

## Debug mode

A debug mode is available for all programs, allowing the user to view
RTT logs emitted by the program.

In order to enable it, you must uncomment the `DEBUG` compile option in
the `CMakeLists.txt` of the desired program. Once this is done, you can
build and flash the program on the boards using the commands described
above.

To access the logs, run the following command:

```bash
JLinkRTTViewerExe --autoconnect     \ # Automatically connect.
    --device nrf52832_xxaa          \ # Board family.
    --serialnumber <BOARD_NUMBER>   \ # Serial number of the desired board.
```

## Notes

* The system can manage up to 4 Actuator nodes.

* The `cmake` folder in the `resources` directory comes from a
third-party repository: <https://github.com/polidea/cmake-nrf5x>. As a
matter of convenience and maintainability, and since we only use a part
of it, it has been elected to copy this part in our repository rather
than creating a submodule.

* The `Luos` submodule in the `resources/luos_lib` directory is a fork
from the following repository: <https://github.com/Luos-io/Luos.git>
_(from commit 302d14c)_.
