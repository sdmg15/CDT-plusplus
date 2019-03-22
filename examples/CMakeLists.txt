cmake_minimum_required(VERSION 3.10)

project(CDT-examples VERSION 0.1.0 LANGUAGES CXX)

# Qt only properly installed on Linux
if(UNIX)

  # CMake modules
  include(CMakeDependentOption)

  # Compile commands
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Threads
  set(CMAKE_THREAD_PREFER_PTHREAD TRUE)

  # Turn off CGAL Triangulation Assertions and Postconditions
  add_definitions(-DCGAL_TRIANGULATION_NO_ASSERTIONS
                  -DCGAL_TRIANGULATION_NO_POSTCONDITIONS
                  -DCGAL_USE_BASIC_VIEWER
                  -DWITH_CGAL_Qt5)

  # Qt
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  # Qt Homebrew install location
  set(CMAKE_PREFIX_PATH "/usr/local/Cellar/qt/5.12.2/lib/cmake/Qt5/")
  find_package(Qt5
               COMPONENTS Core
                          Widgets
                          Xml
                          OpenGL
                          REQUIRED)

  # Build executable with C++17
  include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include)
  add_executable(draw_triangulation_3 draw_triangulation_3.cpp)
  target_link_libraries(draw_triangulation_3
                        PRIVATE ${CGAL_LIBRARIES}
                                Qt5::Core
                                Qt5::Widgets
                                Qt5::Xml
                                Qt5::OpenGL)
  target_compile_features(draw_triangulation_3 PRIVATE cxx_std_17)

  # Tests
  enable_testing()

  add_test(NAME DrawTriangulation COMMAND $<TARGET_FILE:draw_triangulation_3>)

endif(UNIX)
