add_library(
    event
    event_buffer_channel.cc
    event_channel.cc
    event_loop.cc
    event_no_buffer_channel.cc
    event_queue.cc
    event_queue_handler.cc
    event_timer.cc)

add_library(libcore::event ALIAS event)
target_link_libraries(event PRIVATE libevent::core libevent::extra)
