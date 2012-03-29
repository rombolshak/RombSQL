project(rombsqlserver)

add_executable(rombsqlserver RCellValue.cpp RTable.cpp main.cpp jsoncpp/src/json_reader.cpp jsoncpp/src/json_writer.cpp jsoncpp/src/json_value.cpp)

