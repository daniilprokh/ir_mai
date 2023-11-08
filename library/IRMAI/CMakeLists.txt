cmake_minimum_required(VERSION 3.16)

project(IRMAI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS serialization)
find_package(Poco REQUIRED COMPONENTS Net Util JSON)

add_library(${PROJECT_NAME} STATIC)

set(PUBLIC_HEADERS
  include/IRMAI/corpus/corpus_handler.h
  include/IRMAI/corpus/document.h
  include/IRMAI/query/query_processor.h
  include/IRMAI/query/query.h
  include/IRMAI/search_engine/inverted_index.h
  include/IRMAI/search_engine/ranked_searcher.h
  include/IRMAI/search_engine/search_engine_initializer.h
  include/IRMAI/search_engine/search_engine.h
  include/IRMAI/web_server/handlers/document_handler.h
  include/IRMAI/web_server/handlers/index_handler.h
  include/IRMAI/web_server/handlers/query_handler.h
  include/IRMAI/web_server/http_request_handler.h
  include/IRMAI/web_server/http_web_server.h
  include/IRMAI/contains_substr.h
  include/IRMAI/file_size_converter.h
  include/IRMAI/get_text_from_file.h
  include/IRMAI/top_frequencies.h
  include/IRMAI/split_into_tokens.h
  include/IRMAI/split.h
  include/IRMAI/to_lower.h
)

set(SOURCES ${PUBLIC_HEADERS}
  src/query/query.cpp
  src/search_engine/inverted_index.cpp
  src/search_engine/ranked_searcher.cpp
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})

target_include_directories(${PROJECT_NAME} 
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    ${Boost_INCLUDE_DIRS}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

set_target_properties(${PROJECT_NAME} PROPERTIES
  PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

target_link_libraries(${PROJECT_NAME} 
  Boost::serialization
  Poco::Net
  Poco::Util
  Poco::JSON
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        # If the debug configuration pass the DEBUG define to the compiler 
        $<$<CONFIG:Debug>:DEBUG>
)
