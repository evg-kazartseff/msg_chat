set(SOURCE_FILES main.c server.c server.h message_queue.c message_queue.h server_type.h)
add_executable(chat_server ${SOURCE_FILES})
target_link_libraries(chat_server pthread)