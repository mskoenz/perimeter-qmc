FOREACH (name   measure_periodic_access
                grid_playground
                grid_performance_test
                multi_grid_state_test
                rng_example
                loop_analysis
    )
    add_executable(${name} ${name}.cpp)
    add_dependencies(${name} build_nr) #increases the build-number
    target_link_libraries(${name} ${LIB})
    #~ target_link_libraries(${name} ${LIB} "/usr/lib/libboost_timer.so" "/usr/lib/libboost_system.so.1.50.0")
    add_default_test(${name})
ENDFOREACH(name)
