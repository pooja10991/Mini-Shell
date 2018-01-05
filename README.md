# Mini-Shell
Implementation in C which runs basic shell functionality.
This project is about how the shell works. Some of the properties of shell are replicated here like basic navigation commands, running different commands with any number of arguments, running multiple commands, bringing the background process to foreground and many more. fork and exec command used for the implementation.

## Capabilities : 
a.	Run any basic navigation commands.
Eg  cd .. – which will change the current working directory to the previous directory in the hierarchy.
cd somedirectory – will navigate to the next directory in the hierarchy.
cd fullPath – whill navigate to the given directory.
pwd – will show the current working directory.

b.	 Run any command with any number of arguments.
Capable of running “ls” (without arguments) as well as “ls -ltr” (with arguments). 

c.	Run multiple commands : 
Multiple commands can be run at a time by providing the “&” sign in between the commands.
Eg : 	ls &
	pwd
this will run “ls” command as the background process.
Capable of running more than two commands.
Eg : 	sleep 3 &
	ls -ltr &
	ps -ef
here the first two commands will be running in background and as soon as they are ready they will print the output.
Note that any of the command can be given any numbers of arguments.

d.	To bring the background process to foreground.
Eg : fg backgrounfPID
This will bring the specified process to run to the foreground, that means the shell will wait for the command to finish and then only other command will be accepted as input.

e.	Get the information of the processes running in background.
Eg : listjobs
This command will list all the background process with its PID and the status.

f.	Terminate the background running processes.
Ctrl + c will kill the background process running.

g.	Terminating the shell.
“exit” command will terminate from the shell “minish”.
