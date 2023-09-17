cmake_minimum_required(VERSION 3.16)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(ir_04 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Poco REQUIRED COMPONENTS Net Util JSON)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC
  IRMAI
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    # If the debug configuration pass the DEBUG define to the compiler 
    $<$<CONFIG:Debug>:DEBUG>
)

set(WEB_SERVICE ${PROJECT_NAME}_web_service)

set(WEB_HEADERS
  web_server/handlers/document_handler.h
  web_server/handlers/index_handler.h
  web_server/handlers/query_handler.h
  web_server/http_request_handler.h
  web_server/http_web_server.h
)

add_executable(${WEB_SERVICE} ${WEB_HEADERS} web_server/web_server.cpp)

target_link_libraries(${WEB_SERVICE} PUBLIC
  IRMAI
  Poco::Net
  Poco::Util
  Poco::JSON
)

target_compile_definitions(${WEB_SERVICE}
  PRIVATE
    # If the debug configuration pass the DEBUG define to the compiler 
    $<$<CONFIG:Debug>:DEBUG>
)
