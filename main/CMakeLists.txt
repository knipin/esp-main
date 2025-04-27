idf_component_register(SRCS "main.cpp" 
                             "endpoints.cpp"
                             "pid_controller.cpp"
                             "temperature_sensor.cpp"
                             "pwm_controller.cpp"
                    INCLUDE_DIRS ".")

if (CONFIG_ENABLE_SET_CERT_DECLARATION_API)
    target_add_binary_data(${COMPONENT_TARGET} "certification_declaration/certification_declaration.der" BINARY)
endif()

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
