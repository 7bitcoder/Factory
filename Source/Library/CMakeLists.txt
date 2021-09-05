file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cpp)

add_library(FactoryLib ${SOURCES})

target_include_directories(FactoryLib PUBLIC
  h
)

