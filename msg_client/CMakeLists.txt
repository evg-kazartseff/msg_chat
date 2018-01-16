set(SOURCE_FILES main.c client.c client.h msg_t.h)
add_executable(chat_client ${SOURCE_FILES})
target_link_libraries(chat_client pthread)