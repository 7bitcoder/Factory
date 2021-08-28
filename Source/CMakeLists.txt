
add_subdirectory(Library)

add_executable(Factory
  main.cpp
)

target_link_libraries(Factory 
  FactoryLib
)