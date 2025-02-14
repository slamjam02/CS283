1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: I believe fgets() is generally a good function to use when you have a maximum buffer size, as it lets you specify how many characters you want to read.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Yes, malloc() allows us to dynamically allocate memory to the buffer size to store the command in. If we hard-coded the buffer size, that wouldn't be a smart design, so instead we just use SH_CMD_MAX as the value, which is in the header file and can be changed later.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Well first, we might run into issues actually splitting the command on the tokens, as it would be much more difficult to count the commands. Removing the trailing and leading spaces early cleans up that code a lot. Then, in the actual shell program, if the spaces are kept there, it could lead to executables not being called by the correct name. Like the shell would call " hello" instead of "hello".

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  In Linux shells, you can redirect the data of stdin, stdout, and stderr. In bash, the character for redirecting stdin is <, the token for redirecting stdout is > and stderr is 2>. We should implement these, though it might be challenging to include these in the command line processor. They are yet another string to take care of within the tokenizer.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection is moreso for users to change how the input or output of an executable are managed at runtime, yet pipes are helpful for creating chains. I believe you can get the same functionality with redirection, but pipes are much more efficient, as you simply need to type a | symbol. This redirects the output of one executable to the input of another. 

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping stderr separate from stdout is important as to not mix proper output from error messages. It is just more organized and lets programs function better, as they can differentiate between the two.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  I think errors from commands that fail should be sent to stderr, of course. Though bash allows the user to merge the two, I don't think it's necessary for the scope of our shell to specifically chose to allow this, but rather I think that this functionality should be enabled as a byproduct of how the shell commands are parsed. You should be able to redirect stderr and stdout, so you can combine them in one file.