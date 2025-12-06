# CMakeList.txt: проект CMake для BCD_SORM; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (BCD_SORM "BCD_SORM.cpp" "BCD_SORM.h" "MAIN_TEST.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET BCD_SORM PROPERTY CXX_STANDARD 23)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
