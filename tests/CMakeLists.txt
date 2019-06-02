
set(krosstest_SRCS
    testobject.cpp
    main.cpp
)
add_executable(krosstest ${krosstest_SRCS})
target_link_libraries(krosstest
    KF5::KrossCore
    Qt5::Widgets
)

add_test(NAME kross-unittest-es COMMAND krosstest ${CMAKE_CURRENT_SOURCE_DIR}/unittest.es)

add_test(NAME kross-guiform-py COMMAND krosstest ${CMAKE_CURRENT_SOURCE_DIR}/testguiform.py)
add_test(NAME kross-guiqt-py COMMAND krosstest ${CMAKE_CURRENT_SOURCE_DIR}/testguiqt.py)
add_test(NAME kross-guitk-py COMMAND krosstest ${CMAKE_CURRENT_SOURCE_DIR}/testguitk.py)
add_test(NAME kross-test-py COMMAND krosstest ${CMAKE_CURRENT_SOURCE_DIR}/testkross.py)
add_test(NAME kross-unittest-py COMMAND krosstest ${CMAKE_CURRENT_SOURCE_DIR}/unittest.py)

add_test(NAME kross-unittest-rb COMMAND krosstest ${CMAKE_CURRENT_SOURCE_DIR}/unittest.rb)
