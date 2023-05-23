set (Boost_USE_STATIC_LIBS OFF)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(Boost_Tests ../model/common/Includes.h ../model/object/Object.cpp ../model/object/Object.h ../model/table/BasicSerializable.cpp ../model/table/BasicSerializable.h ../model/table/Serializable.h ../model/table/View.h ../model/table/View.cpp ElementValueTests.cpp SerializableTest.cpp ViewTest.cpp)
target_link_libraries(Boost_Tests ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})