add_library(
    os
    unix_cgroup.cc
    unix_command.cc
    unix_countdown_latch.cc
    unix_current_thread.cc
    unix_logger.cc
    unix_pid_file.cc
    unix_sysinfo.cc
    unix_thread.cc
    unix_thread_proc.cc
    unix_timer.cc
    unix_timestamp.cc)
add_library(libcore::os ALIAS os)
target_link_libraries(os PRIVATE fmt::fmt ${LIBCGROUP_LINK_LIBRARIES})
