cmake_minimum_required(VERSION 3.16.3)

project (CrawlerCPP)

set(CRAWLER_CONFIG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/config")
set(CRAWLER_WEB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/web")

add_definitions(
	-DCRAWLER_CONFIG_PATH="${CRAWLER_CONFIG_PATH}"
	-DLEDLIB_WEB_PATH="${CRAWLER_WEB_PATH}"
)

add_subdirectory(cpp)

