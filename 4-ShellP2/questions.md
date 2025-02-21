1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Using the fork function before execvp lets the shell continue after the user-called program is run. If we don't fork the original process gets overwritten in its entirety, and the shell won't resume.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork system call fails in my implementation, the error "fork failed" will get printed to the stderr and the application will exit with error code 1.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  The function execvp() uses the $PATH enviroment variable to find the command to execute. $PATH contains all of the directories where executables are generally located.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  We call wait() in the parent process so that it doesn't immediately interrupt the child process after forking. The wait() function ensures that the child process finishes before returning to the shell.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() contains the exit status of the child process. Exit status 0 is successful, and the other numbers represent different types of errors, which can be important for the user to understand what might have went wrong in a child process.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation essentially just uses a switch that turns on when a quote is met, and that switch determines whether spaces are ignored. Quoted arguments are important for any arguments that contain spaces like "hello world". We want this treated as one argument, rather than two.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  I had to make several changes. Essentially, I had to rewrite it to make it compatible with quoted spaces and the new command buffer struct. That being said, I could have accounted for this possible change in the prior assignment by making temp variables to store the arguments in. Though, we also had to consider pipes in the last assignment, and we didn't for this one (yet).

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in Linux are a method for the kernel to communicate with ongoing processes. They differ from other forms of IPC because they are simple and only notify processes about a set of predefined events.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL immediately kills a process without cleanup, SIGTERM is similar but it lets the process terminate properly with cleanup, and SIGINT interrupts a process where it can either ignore that signal or terminate (ctrl + c).

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP cannot be ignored by a process, it will immediately terminate because it is a signal directly from the kernel. Kernels implement this signal for safety and security reasons, such that they can suspend processes.
