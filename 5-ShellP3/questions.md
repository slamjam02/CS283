1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

My implementation calls waitpid() to ensure that the parent process lets all child processes complete before resuming its code. If you forget to call this, the child process won't be able to complete properly, will possible remain a zombie process, and the parent shell won't be able to see the exit status.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

It is necessary to close pipe ends after calling dup2() to prevent data corruption or leaks in the system. If you leave them open, information that is passed between pipes takes away limited resources from other pipe ends and could cause the reader to wait indefinitely.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

The cd command needs to be an internal command since the command changes the current working directory, which is something that directly affects the shell itself. Implementing this externally would make it significantly more difficult to actually change the current working directory of the shell.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

In order to be able to handle an arbitrary number of piped commands, we would need to make a size-dynamic array or list to handle the piped commands, rather than the fixed array in the current implementation. The only real tradeoffs I can think about are that would just be a bit more complicated and would maybe require new structures / additional code, but it's certainly doable.
