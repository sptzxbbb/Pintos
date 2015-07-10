## Project Log ##

This is a log recording my project process.

[Pintos is a Stanford CS-140 OS project](http://web.stanford.edu/class/cs140/projects/pintos/pintos.html#SEC_Top) 

To see more details about my project, please click [My Blog](http://www.sptzxb.com/)

### Project 1 ###

Sun May 17 10:17:19 2015  
Analyze the subroutine timer_sleep() call.  
Study the process scheduling algorithm, that is, simple FCFS.  
Study page management using bitmap.

---

Tue May 19 10:25:40 2015  
Modified timer_sleep()  
Add a new member wakeup_ticks to thread.  
Add a new function thread_cmp_wakeup_ticks  

---

Tue May 19 10:29:23 2015  
Modified timer_interrupt()  
Add a new function thread_wakeup()  

---

Wed May 20 10:30:45 2015  
**Alarm Clock** pass tests.  
Implement preempt priority schedule in thread_create() and thread_set_priority().  
Study Priority Donation.

---

Fri May 22 10:38:05 2015  
Modified thread.  
Modified lock_acquire() to implement nested donation.  
Add thread_yield() to sema_up() to reschedule.  

---

Sat May 23 10:41:44 2015  
Modified functions related to sema.  
Add priority compare functions.  
**Priority Schedule**  pass tests.
Study Advanced Scheduler.

---

Sun May 24 10:44:48 2015  
Implement Fixed-Point Real Arithmetic.  
Modified timer_interrupt() to updata data.  
Implement caculate_load_avg().  
Implement calculate_recent_cpu_foreach().  

---

Mon May 25 10:49:40 2015  
Implement calculate_priority_foreach().  
Implement thread_set_nice(), thread_get_nice(), thread_get_load_avg(), thread_get_recent_cpu().  

---

Tue May 26 10:51:54 2015  
Fixed a few bugs about initialization.  
**Advanced Scheduler** pass tests.  

Project 1 Pass!

~~~
$ make check
~~~

~~~
pass tests/threads/alarm-single
pass tests/threads/alarm-multiple
pass tests/threads/alarm-simultaneous
pass tests/threads/alarm-priority
pass tests/threads/alarm-zero
pass tests/threads/alarm-negative
pass tests/threads/priority-change
pass tests/threads/priority-donate-one
pass tests/threads/priority-donate-multiple
pass tests/threads/priority-donate-multiple2
pass tests/threads/priority-donate-nest
pass tests/threads/priority-donate-sema
pass tests/threads/priority-donate-lower
pass tests/threads/priority-fifo
pass tests/threads/priority-preempt
pass tests/threads/priority-sema
pass tests/threads/priority-condvar
pass tests/threads/priority-donate-chain
pass tests/threads/mlfqs-load-1
pass tests/threads/mlfqs-load-60
pass tests/threads/mlfqs-load-avg
pass tests/threads/mlfqs-recent-1
pass tests/threads/mlfqs-fair-2
pass tests/threads/mlfqs-fair-20
pass tests/threads/mlfqs-nice-2
pass tests/threads/mlfqs-nice-10
pass tests/threads/mlfqs-block
All 27 tests passed.
~~~


### Project2 ###

Wed Jul  8 22:00:17 2015  
Read the document about Pintos' file system.  
Fixed an mysterious bug project1 resulted in.  

Fri Jul 10 08:08:18 2015  
Working on argument passing.  