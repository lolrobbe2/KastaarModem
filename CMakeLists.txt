# CMakeLists.txt
idf_component_register(
    SRCS "src/KastaarModem.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_modem
)

