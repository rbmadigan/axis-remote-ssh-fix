/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * axis_remote_server.cc
 * Copyright (C) 2019 Bruce Madigan <madigan@weldworks.com>
	 * 
 * axis_remote_server is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
	 * 
 * axis_remote_server is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * axis_remote_server listens for --mdi command strings to be sent to linuxcnc
 * running the axis gui interface.  axis_remote_server must be running in a 
 * terminal on the same computer as linuxcnc is running
 */

#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>

#define SHMSZ 10000  			// shared memory size to max
int shmid;						// id of the shared memory
key_t key = 2947;				// instance of the shared memory key , must be the same in the axis_remote_client app
char *shm, *s;					// pointer to the shared memory location
unsigned int toggle;			// a flag used to update the spinner in the terminal window where we are running
#define update_interval 0.01    // seconds between shared memory content checks

int main()
{
	printf("starting axis_remote_server\n");
	if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) // create the segment
	{
		perror("shmget");
		exit(1);
	}
	if ((shm = (char *)shmat(shmid, NULL, 0)) == (char *) -1) // attach the segment to our data space
	{
		perror("shmat");
		exit(1);
	}
	printf("waiting for command...\n");

	toggle=0; // set the flag for the spinner in the terminal

	for(;;)  // sit in a loop forever (stop server with ctrl-c)
	{
		char command[SHMSZ];  // the command sent by the axis_remote_client
		int command_length=0; // the length command received from the axis_remote_client
		s=shm; // a copy of the shared memory pointer that we can manipulate
		// useconds the server will sleep between shared memory content checks
		unsigned long update_period=(unsigned long)(update_interval*1000000.0);

		// update the crude spinner in the terminal so we know the server is running
		toggle++;
		if(toggle==0.25/update_interval) // update the spinner every 0.25 seconds
		{
			printf("/");
			printf("\b");
		    fflush(stdout);
		}
		if(toggle==0.5/update_interval)
		{
			printf("\\");
			printf("\b");
			toggle=0;
		    fflush(stdout);
		}

		// check shared memory to see if client sent a command string
		if(*s == '*') // we got a command from the client, begins with '*'
		{
			*s++;
			command_length=0;
			while(*s != '\0')
			{
				command[command_length]=*s;
				command_length++;
				*s++;
			}
			command[command_length] = 0; // truncate command string

			// acknowledge receipt of command 
			s=shm;
			*s='%';  // % character is used as acknowledge

			char command_to_axis_remote[SHMSZ]; // string in which to build the system command
			sprintf(command_to_axis_remote,"%s %s%s%s","axis-remote --mdi ", "\"",command,"\"");
			//sprintf(command_to_axis_remote,"%s %s%s%s","axis-remote ", "\"",command,"\"");
			// notice there is no error checking on the command string, proof the command elsewhere
			printf("%s\n",command_to_axis_remote);  // display the axis-remote --mdi command
			int ret=system(command_to_axis_remote); // call axis-remote with the --mdi command
			//if(ret !=0)
			//	printf("error in system call to axis-remote\n");
		}
		else
		{
			usleep(update_period); // sleep until it is time to check the shared memory again
		}
	}
	return 0;
}

