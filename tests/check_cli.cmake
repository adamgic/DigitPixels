if(NOT DEFINED APP OR NOT DEFINED MODE)
    message(FATAL_ERROR "APP and MODE are required")
endif()

if(MODE STREQUAL "usage")
    execute_process(
        COMMAND "${APP}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error
        TIMEOUT 10
    )
    set(expected 1)
    set(pattern "Usage: DigitRecognizer")
elseif(MODE STREQUAL "missing-training")
    if(NOT DEFINED DATA_DIR)
        message(FATAL_ERROR "DATA_DIR is required")
    endif()
    execute_process(
        COMMAND "${APP}"
            "${DATA_DIR}/does-not-exist"
            "${DATA_DIR}/inputImage.png"
            --headless
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE error
        TIMEOUT 10
    )
    set(expected 2)
    set(pattern "Cannot load training image")
else()
    message(FATAL_ERROR "Unknown mode: ${MODE}")
endif()

if(NOT "${result}" STREQUAL "${expected}")
    message(FATAL_ERROR "Expected exit ${expected}, got ${result}\n${output}\n${error}")
endif()
if(NOT "${output}\n${error}" MATCHES "${pattern}")
    message(FATAL_ERROR "Expected diagnostic '${pattern}'\n${output}\n${error}")
endif()
