FOREACH (name   periodic_boundary_test
    )
    add_executable(${name} ${name}.cpp)
    add_dependencies(${name} build_nr) #increases the build-number
    target_link_libraries(${name} ${LIB})
    add_default_test(${name})
ENDFOREACH(name)
