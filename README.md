# axis-remote-ssh-fix
allows linuxcnc running the axis gui to accept axis-remote commands via ssh sessions

axis−remote is part of linuxcnc and is a small script that triggers commands in a running axis gui.  axis-remote is a command line application run from a terminal.  axis-remote communicates with linuxcnc via the x11 protocol.  As a result, axis-remote will not function when typed at the command line of a ssh terminal from a remote client.  Trying to run axis-remote from a remote terminal produces the error message "no application named "axis"" because the x11 display number on the linuxcnc computer is different from the remote ssh x11 display number. 

The solution here is a pair of shared memory programs (axis_remote_client and axis_remote_server) running on the same computer as linuxcnc.  The axis_remote_server application runs in a terminal on the linuxcnc computer and "listens" for commands from the axis_remote_client application.  The axis_remote_server application completes a system call to axis-remote with the command sent by the axis_remote_client application.  The axis_remote_client application can be run at the command line of a terminal on the linuxcnc computer or from the ssh terminal of a remote computer.  Also provided is a version of the client application that can be built to run under Octave.

Limitation:  presently the axis_remote_server sends commands received from axis_remote_client as mdi commands

Building the sources:

Anjuta IDE was originally used to create the codes but is not necessary for compiling and linking.  Download or clone the sources with the directory structure intact.  

To build the server, cd to the axis_remote_server directory on your local computer. Execute the typical automake command sequence:

chmod +x ./configure

./configure

make

sudo make install

The install copies axis_remote_server to /usr/local/bin

To build the client, cd to the axis_remote_client directory on your local computer. Execute the typical automake command sequence:

chmod +x ./configure

./configure

make

sudo make install

The install copies axis_remote_client to /usr/local/bin

To build the octave version of the client, cd to the axis_remote_client_octave directory on your local computer. Execute the the Octave build command:

mkoctfile axis_remote_client.cc

which produces the file axis_remote_client.oct.  See octave documentation to incorprate the new function into octave.

Example Usage:

Open a command line terminal on the computer running linuxcnc and start axis_remote_server.  axis_remote_server displays a crude spinner indicating it is waiting for the shared memory to be filled with a axis-remote --mid command by an axis_remote_client.

Open another command line terminal on the computer running linuxcnc and execute axis_remote_client "F10 G91 X0.1 Y0.1 Z0.1"
caution - the argument to axis_remote_client in the example is fictional.

The command sent by axis_remote_client should be displayed in the axis_remote_server terminal window and axis_remote_server will execute a system call as axis-remote --mdi "F10 G91 X0.1 Y0.1 Z0.1"

Now ssh into the linuxcnc computer from a remote computer.  It is not necessary to enable x11 forwarding in the ssh connection for axis_remote_client to function correctly but maybe useful for other things you do with the ssh session.  At the command line of the ssh terminal execute axis_remote_client "F10 G91 X0.1 Y0.1 Z0.1"

The command sent by the axis_remote_client instance running in the ssh terminal from the remote computer should be displayed in the axis_remote_server terminal window on the linuxcnc computer.

To do:

remove the hard-coded functionality in axis_remote_server where axis_remote_server prepends --mdi to commands sent by a axis_remote_client.  Doing so will allow the axis_remote_server/axis_remote_client pair to be identical in functionality to axis-remote.
