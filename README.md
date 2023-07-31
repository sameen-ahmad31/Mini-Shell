# Mini Shell
I developed Mini Shell, an alternative Linux Shell, using the C programming language. This project aimed to create a fully functional command-line interface with multiple features and functionalities, offering users a robust and efficient shell experience. In the process of designing and implementing Mini Shell, I focused on providing users with a reliable and powerful tool for interacting with their operating system. The shell offers a range of capabilities, making it easy for users to execute commands, navigate directories, and manage processes effectively. I ensured the shell's performance was optimized, resulting in a smooth and responsive user experience. Overall, Mini Shell serves as a practical Linux Shell alternative, giving users a versatile and feature-rich command-line interface.

I created a shell using C programming and implemented various features to ensure its functionality. The design of the shell was motivated by the need to efficiently parse user inputs, enable program composition into pipelines, and ensure proper waiting on child processes through "wait" or "waitpid" operations.

During the development process, I paid close attention to handling interactions between the shell and its children using signals, allowing for smooth termination of processes when required. The shell also supported background computation, enabling tasks to run in the background while still remaining responsive to other commands.

To enhance user experience, I implemented proper handling of interactions between the terminal and the shell, which included managing keyboard input and signals like "cntl-c" or "cntl-z" effectively. Additionally, I made sure to incorporate the functionality for redirecting child process output to files, allowing users to capture and store program output for later use or analysis.

By successfully addressing these requirements and implementing the essential features, the shell I created offered a comprehensive, user-friendly command-line interface. It empowered users to execute various commands, compose complex operations, and manage background tasks seamlessly. The careful consideration of these features during the development process significantly impacted the shell's overall functionality and user experience, resulting in a well-designed and efficient C-based shell.

Skills: C (Programming Language)
