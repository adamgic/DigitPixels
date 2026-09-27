add_executable(test_preprocessing test_preprocessing.cpp)
target_link_libraries(test_preprocessing PRIVATE digitpixels ${OpenCV_LIBS})
target_include_directories(test_preprocessing PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_include_directories(test_preprocessing SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})

add_test(NAME preprocessing COMMAND test_preprocessing)
set_tests_properties(preprocessing PROPERTIES LABELS "unit" TIMEOUT 10)

add_executable(test_recognition test_recognition.cpp)
target_link_libraries(test_recognition PRIVATE digitpixels ${OpenCV_LIBS})
target_include_directories(test_recognition SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})

add_test(NAME recognition_smoke
    COMMAND test_recognition
        "${PROJECT_SOURCE_DIR}/data/digits"
        "${PROJECT_SOURCE_DIR}/data/inputImage.png"
)
set_tests_properties(recognition_smoke PROPERTIES LABELS "integration" TIMEOUT 180)

add_test(NAME cli_usage
    COMMAND "${CMAKE_COMMAND}"
        "-DAPP=$<TARGET_FILE:DigitRecognizer>"
        -DMODE=usage
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_cli.cmake"
)

add_test(NAME cli_missing_training
    COMMAND "${CMAKE_COMMAND}"
        "-DAPP=$<TARGET_FILE:DigitRecognizer>"
        -DMODE=missing-training
        "-DDATA_DIR=${PROJECT_SOURCE_DIR}/data"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_cli.cmake"
)
set_tests_properties(cli_usage cli_missing_training PROPERTIES
    LABELS "cli"
    TIMEOUT 15
)
