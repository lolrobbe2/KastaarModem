# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

idf_component_register(
    SRCS "src/KastaarModem.cpp" "src/proto/socket/Socket.cpp" "src/proto/socket/SocketManager.cpp"
    INCLUDE_DIRS "src"
    REQUIRES esp_modem driver nvs_flash
)
