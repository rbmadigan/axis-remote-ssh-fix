/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * axis_remote_client.cc
 * Copyright (C) 2019 Bruce Madigan <madigan@weldworks.com>
	 * 
 * axis_remote_client is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
	 * 
 * axis_remote_client is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>

#define SHMSZ 10000				// shared memory size (increase size of not enough for your use
int shmid;						// id of the shared memory
key_t key = 2947;				// instance of the shared memory key , must be the same in the axis_remote_server app
char *shm, *s;					// pointer to the shared memory location
unsigned int i;					// index into shared memory


int main(int argc, char *argv[])
{
	if ( argc != 2 ) // argc should be 2 for correct execution
	{
		// We print argv[0] assuming it is the program name
		printf("usage: %s %s\n",argv[0], "mdi_command");
		exit(-1);
	}
	else 
	{
		if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) // create the segment
		{
			perror("shmget");
			exit(-1);
		}
		if ((shm = (char *)shmat(shmid, NULL, 0)) == (char *) -1) // attach the segment to our data space
		{
			perror("shmat");
			exit(-1);
		}
		s=shm; // a copy of the shared memory pointer that we can manipulate
		for (i = 0; i < SHMSZ; i++) // clear the shared memory
		{
			*s++ = '\0';
		}

		s=shm; // reset shared memory pointer to the beginning of shared memory
		s++; // save the first memory space for * which is begining of a command to server
		std::string command=argv[1]; // get the argument, note: no error checking on argument
		std::string command_to_send=command; // could send command as typed on command line
		
		printf("argument is %s\n",command.c_str());
		printf("sending --mdi command to axis_remote_server \"%s\"\n",command_to_send.c_str());
		for (i=0; i<command_to_send.length(); i++)
		{
			*s++=command_to_send[i];
		}
		*s++ = '\0';
		s=shm; // reset pointer to beginning of the shared memory
		*s='*';  // set the first location to * indicating to server a command is ready

		//wait for the axis_remote_server to acknowledge command
		// should have a timeout here to prevent a hang
		s=shm;
		while(*s != '%')
		{
			usleep(1); // wait for the server to acknowledge receipt of command
		}

		s=shm; // clear the shared memory and leave
		for (i = 0; i < SHMSZ; i++)
		{
			*s++ = '\0';
		}
	}
	return 0;
}

