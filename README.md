# axis-remote-ssh-fix
allows linuxcnc running the axis gui to accept axis-remote commands via ssh sessions

axis-remote is a command line application run from a terminal that sends commands to linuxcnc running the axis gui.  axis-remote communicates with linuxcnc via the x11 protocol.  As a result, axis-remote will not function when typed at the command line of a ssh terminal from a remote client.  Trying to run axis-remote from a remote terminal produces the error message "no application named "axis"" because the x11 display number on the linuxcnc computer is different from the remote ssh display number. 

The solution here is a pair of shared memory programs (axis_remote_client and axis_remote_server) running on the same computer as linuxcnc.  The axis_remote_server application runs in a terminal on the linuxcnc computer and "listens" for commands from the axis_remote_client application.  The axis_remote_server application completes a system call to axis-remote with the command sent by the axis_remote_client application.  The axis_remote_client application can be run at the command line of a terminal on the linuxcnc computer or from the ssh terminal of a remote computer.  
