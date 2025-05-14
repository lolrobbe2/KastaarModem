# CMakeLists.txt
idf_component_register(
    SRCS "src/KastaarModem.cpp"
    INCLUDE_DIRS "src"
    REQUIRES esp_modem nvs_flash driver
)

