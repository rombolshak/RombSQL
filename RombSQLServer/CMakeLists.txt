cmake_minimum_required(VERSION 2.8)
	
project(rombsqlserver)

add_executable(rombsqlserver RTableCondition.cpp RLexer.cpp RParser.cpp RDbCommand.cpp RServer.cpp RTable.cpp RException.cpp RCell.cpp RTableFile.cpp main.cpp)

FILE(GLOB UnitTests_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "tests/*Test.cpp" )
ADD_EXECUTABLE(UnitTester tests/main.cpp ${UnitTests_SRCS} )
TARGET_LINK_LIBRARIES(UnitTester cppunit)
FOREACH(test ${UnitTests_SRCS})
MESSAGE("  ######  adding test ${test}")
GET_FILENAME_COMPONENT(TestName ${test} NAME_WE)
ADD_TEST(${TestName} UnitTester ${TestName})
ENDFOREACH(test)