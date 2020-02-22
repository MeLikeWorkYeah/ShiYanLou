main.c->main():move init()'s disk and file setup into main() then open /var/process.log

printk.c->fprintk:
 //fprintk(1,xxx,xxx) -> print to stdout
 //fprintk(3,xxx,xxx) -> print to log
 #include "linux/sched.h"
 #include "sys/stat.h"

 static char logbuf[1024]
 int fprintk(int fd,const char *fmt,...)
 {
 	va_list args;
	int count;
	struct file* file;
	struct minode* inode;

	va_start(args,fmt);
	count=vsprintf(logbuf,fmt,args);
	va_end(args);
	
	//print to stdout or stderr --- call sys_write
	if(fd<3)
	{
		__asm__ volatile ("push %%fs\n\t"
			"push %%ds\n\t"
			"pop %%fs\n\t"
			"pushl %0\n\t"
			"pushl $logbuf\n\t"
			"pushl %1\n\t"
			"call sys_write\n\t"
			"addl $8,%%esp\n\t"
			"popl %0\n\t"
			"pop %%fs"
			:
			:"r"(count),"r"(fd)
			:"ax","cx","dx");
	}
	else
	{//get file handle from task[0](task init's parent)'s descriptor
		if(!(file=task[0]->filp[fd])) return 0;
		inode=file->f_inode;
		__asm__ volatile ("push %%fs\n\t"
			"push %%ds\n\t"
			"pop %%fs\n\t"
			"pushl %0\n\t"
			"pushl $logbuf\n\t"
			"pushl %1\n\t"
			"pushl %2\n\t"
			"call file_write\n\t"
			"addl $12,%%esp\n\t"
			"popl %0\n\t"
			"pop %%fs"
			:
			:"r"(count),"r"(file),"r"(inode)
			:"ax","cx","dx");
	}
	return count;
 }

fork.c->copy_process://before p->state = TASK_RUNNING
 fprintk(3,"%ld\t%c\t%ld\n",last_pid,'N',jiffies);//new task
 fprintk(3,"%ld\t%c\t%ld\n",last_pid,'J',jiffies);//new task into ready state

sched.c->schedule():
 //before (*p)->state = TASK_RUNNING
 if((*p)->state != TASK_RUNNING)
 	fprintk(3,"%ld\t%c\t%ld\n",(*p)->pid,'J',jiffies);//task into block state
 //set task into running
 if(task[next]->pid != current->pid)
 {
 	if(current->state == TASK_RUNNING)
	{
		fprintk(3,"%ld\t%c\t%ld\n",current->pid,'J',jiffies);
	}
	fprintk(3,"%ld\t%c\t%ld\n",task[next]->pid,'J',jiffies);
 }
 switch_to(next);

sched.c->sys_pause():judge then fprintk before state changed
 if(current->state != TASK_INTERRUPTIBLE)
 	fprintk(3,"%ld\t%c\t%ld\n",current->pid,'W',jiffies);

sched.c->sleep_on():judge then fprintk before state change to uniterruptable AND 
 if(tmp != TASK_RUNNING)
 	fprintk(3,"%ld\t%c\t%ld\n",tmp->pid,'J',jiffies);

sched.c->interruptable_sleep_on():add wait and wakeup record after judge

sched.c->wake_up():add wakeup record after state detect

exit.c->do_exit():add exit record

exit.c->sys_waitpid():add wait record
