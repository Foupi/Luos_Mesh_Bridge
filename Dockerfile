FROM    ubuntu
WORKDIR /luos_mesh_bridge

ENV TZ=Europe/Paris
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime \
    && echo $TZ > /etc/timezone

# Mandatory packages
RUN     apt update && apt install -y    \
            vim                 \
            wget                \
            unzip               \
            cmake               \
            gcc-arm-none-eabi   \
            git                 \
            python3

# Install JLink software
RUN     wget --post-data="accept_license_agreement=accepted&submit=Download&nbspsoftware" https://www.segger.com/downloads/jlink/JLink_Linux_x86_64.deb -O JLink.deb    \
            && dpkg -i JLink.deb    \
            && rm -f JLink.deb      \
            && apt update

# Install Nordic command line tools
RUN     wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/Desktop-software/nRF-command-line-tools/sw/Versions-10-x-x/10-12-1/nRFCommandLineTools10121Linuxamd64.tar.gz -O cl_tools.tar.gz    \
            && mkdir cl_tools                           \
            && tar -xzvf cl_tools.tar.gz -C cl_tools    \
            && dpkg -i cl_tools/nRF*.deb                \
            && rm -r cl_tools*                          \
            && apt update

# Install NRF5 SDK
RUN     wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries/nRF5SDK153059ac345.zip -O BLE_SDK.zip   \
            && unzip BLE_SDK.zip    \
            && mv nRF* BLE_SDK      \
            && rm BLE_SDK.zip

#Install NRF5 SDK for Mesh
RUN     wget https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5-SDK-for-Mesh/nrf5SDKforMeshv500src.zip -O Mesh_SDK.zip   \
            && mkdir Mesh_SDK                   \
            && unzip Mesh_SDK.zip -d Mesh_SDK   \
            && rm Mesh_SDK.zip

COPY resources          /luos_mesh_bridge/resources
COPY actuator_node/     /luos_mesh_bridge/actuator_node
COPY bridge_node/       /luos_mesh_bridge/bridge_node
COPY provisioner_node/  /luos_mesh_bridge/provisioner_node
COPY tests/             /luos_mesh_bridge/tests
COPY build.sh           /luos_mesh_bridge/build.sh
COPY flash.sh           /luos_mesh_bridge/flash.sh
COPY CMakeLists.txt     /luos_mesh_bridge/CMakeLists.txt

CMD ["/bin/bash"]
