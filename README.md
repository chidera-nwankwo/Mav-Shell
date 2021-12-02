# Mav-Shell
The shell, like csh or bash, will run and accept commands until the user exits the shell.

Will quit when "quit" is entered. Accepts commands listed in the current directory, /usr/local/bin, /usr/bin, and /bin. Implements fork(), wait(), and exec() in order to run the commands. handles "cd" and will display "command not found" if the command is invalid.
