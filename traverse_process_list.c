#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/pid.h>

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

// task checker check "task_struct" fullfill certain conditions or not =============================================================
int (*get_task_checker(const enum question_type identifier))(struct task_struct*); // get certain task checker with identifier

// pid_getter return pid_t of certain namespace of a task_struct ===================================================================
pid_t (*get_pid_getter(const enum question_type identifier))(struct task_struct*); // get certain pid getter with identifier

// struct to help implements function interface ====================================================================================
struct interface {
	int (*const task_checker)(struct task_struct*);
	pid_t (*const pid_getter)(struct task_struct*);
};
// initialize interface with identifier
#define INIT_INTERFACE(identifier) {				\
	.task_checker = get_task_checker(identifier),		\
	.pid_getter = get_pid_getter(identifier)		\
}


// main function ===================================================================================================================
asmlinkage int sys_traverse_process_list(unsigned int *const buffer, const int size, const int identifier) {
	pid_t tmp_pid;
	struct task_struct *task;
	unsigned int *ptr = buffer;
	struct interface helper = INIT_INTERFACE(identifier);

	for_each_process(task) {
		if ((ptr - buffer) >= size) break;
		else if (helper.task_checker(task)) {
			tmp_pid = helper.pid_getter(task);
			copy_to_user(ptr++, &tmp_pid, sizeof(pid_t));
		}
	}
	return (ptr - buffer);
}

// implementations =================================================================================================================

// the followings are the workhorses of task_checker ==================================================================
int is_same_session_with_init_task(struct task_struct *task) { 
	return task_session(&init_task) == task_session(task); 
}

int is_same_session_with_current_task(struct task_struct *task) { 
	return task_session(current) == task_session(task); 
}

int is_thread_group_leader(struct task_struct *task) {
	return task->group_leader == task;
}

int is_not_thread_group_leader(struct task_struct *task) {
	return !is_thread_group_leader(task);
}

int is_process_group_leader(struct task_struct *task) {
	return task->pids[PIDTYPE_PGID].pid == task_pid(task);
}

int is_not_process_group_leader(struct task_struct *task) {
	return !is_process_group_leader(task);
}

int is_session_leader(struct task_struct *task) {
	return task->pids[PIDTYPE_SID].pid == task_pid(task);
}

int is_not_session_leader(struct task_struct *task) {
	return !is_session_leader(task);
}
// implementations of selector of task checker ========================================================================
int (*get_task_checker(const enum question_type identifier))(struct task_struct*) {
	switch(identifier) {
		case GLOBAL_NAMESPACE_WITH_INIT_TASK: return is_same_session_with_init_task;
		case LOCAL_NAMESPACE_WITH_CURRENT_TASK: return is_same_session_with_current_task;
		case CHECK_TGL: return is_thread_group_leader;
		case CHECK_NOT_TGL: return is_not_thread_group_leader;
		case CHECK_PGL: return is_process_group_leader;
		case CHECK_NOT_PGL: return is_not_process_group_leader;
		case CHECK_SL: return is_session_leader;
		case CHECK_NOT_SL: return is_not_session_leader;
		default:
			printk("[!] Fatal: invalid indetifier in get_session_checker()!");
			return NULL;
	}
}

// the follwoings are the workhorses of pid_getter ====================================================================
pid_t get_pid_in_global_namespace(struct task_struct *task) {
	return pid_nr(task_tgid(task)); 
}

pid_t get_pid_in_local_namespace(struct task_struct *task) {
	return pid_vnr(task_tgid(task));
}
// implementations of selector of pid getter ==========================================================================
pid_t (*get_pid_getter(const enum question_type identifier))(struct task_struct*) {
	switch(identifier) {
		case GLOBAL_NAMESPACE_WITH_INIT_TASK: return get_pid_in_global_namespace;
		case LOCAL_NAMESPACE_WITH_CURRENT_TASK: return get_pid_in_local_namespace;
		case CHECK_TGL: return get_pid_in_global_namespace;
		case CHECK_NOT_TGL: return get_pid_in_global_namespace;
		case CHECK_PGL: return get_pid_in_global_namespace;
		case CHECK_NOT_PGL: return get_pid_in_global_namespace;
		case CHECK_SL: return get_pid_in_global_namespace;
		case CHECK_NOT_SL: return get_pid_in_global_namespace;
		default: 
			printk("[!] Fatal: invalid indetifier in get_pid_getter()!");
			return NULL;
	}
}
// end of syscall ==================================================================================================================