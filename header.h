#ifndef __PROJECT_2
#define __PROJECT_2

#include <unistd.h>
#define SYSCALL_NUMBER 352 // new syscall number

// represent which type of question we are dealing with
enum question_type {GLOBAL_NAMESPACE_WITH_INIT_TASK, LOCAL_NAMESPACE_WITH_CURRENT_TASK, CHECK_TGL, CHECK_NOT_TGL, CHECK_PGL, CHECK_NOT_PGL, CHECK_SL, CHECK_NOT_SL};
/* GLOBAL_NAMESPACE_WITH_INIT_TASK: i.e. project02-q1.
 * LOCAL_NAMESPACE_WITH_CURRENT_TASK: i.e. project02-q2.
 * CHECK_TGL: return pids of thread group leaders in process list
 * CHECK_NOT_TGL: return pids of not thread group leaders in process list
 * CHECK_PGL: return pids of process group leaders in process list
 * CHECK_NOT_PGL: return pids of not process leaders in process list
 * CHECK_SL: return pids of session leaders in process lsit
 * CHECL_NOT_SL: return pids of not session leaders in process lsit
*/

#define get_process_zero_session_group(buffer, size) syscall(SYSCALL_NUMBER, (buffer), (size), (GLOBAL_NAMESPACE_WITH_INIT_TASK))

#define get_process_session_group(buffer, size) syscall(SYSCALL_NUMBER, (buffer), (size), (LOCAL_NAMESPACE_WITH_CURRENT_TASK))

#define get_tgl_in_process_list(buffer, size) syscall(SYSCALL_NUMBER, (buffer), (size), (CHECK_TGL))

#define get_not_tgl_in_process_list(buffer, size) syscall(SYSCALL_NUMBER, (buffer), (size), (CHECK_NOT_TGL))

#define get_pgl_in_process_list(buffer, size) syscall(SYSCALL_NUMBER, (buffer), (size), (CHECK_PGL))

#define get_not_pgl_in_process_list(buffer, size) syscall(SYSCALL_NUMBER, (buffer), (size), (CHECK_NOT_PGL))

#define get_sl_in_process_list(buffer, size) syscall(SYSCALL_NUMBER, (buffer), (size), (CHECK_SL))

#define get_not_sl_in_process_list(buffer, size) syscall(SYSCALL_NUMBER, (buffer), (size), (CHECK_NOT_SL))

#endif