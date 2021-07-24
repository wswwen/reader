#需要cmake的最低版本
cmake_minimum_required (VERSION 2.8)

#项目信息
project (reader)

#把存放目标的位置设置为工程根目录下的bin目录，EXECUTABLE_OUTPUT_PATH ：目标二进制可执行文件的存放位置，PROJECT_SOURCE_DIR：工程的根目录
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/apps)

#查找src目录下所有源文件,并将文件名保存在SRC_LIST
aux_source_directory (src SRC_LIST)

#查找src目录下所有源文件,并将文件名保存在SRC_LIST1
aux_source_directory (apps SRC_LIST1)

#指定生成目标
add_executable (${PROJECT_NAME} ${SRC_LIST} ${SRC_LIST1})

#链接库文件
target_link_libraries(${PROJECT_NAME} m config uhd pthread)