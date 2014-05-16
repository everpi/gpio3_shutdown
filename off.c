/*  
 *  GPIO3 Button system shutdown for Raspberry Pi
 *
 *  Copyright (C) 2014 EverPi - everpi[at]tsar[dot]in
 *  blog.everpi.net 
 * 
 *  This file is part of off.
 *
 *  Off is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Off is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Off.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>

#include <string.h>
#include <stdio.h>

#define _ERR(c) if(c == -1){ fprintf(stderr,"%s",strerror(c)); return 1; }

int main(){

	int fd = 0;
	int er = 0;
	char a[2];
	struct pollfd in[1];
	
	// exporta a interface do gpio3
	fd = open("/sys/class/gpio/export", O_WRONLY);
	_ERR(fd);	
	
	er = write(fd,"3",1);
	close(fd);
	_ERR(er);	
	
	/* visto que a exportação leva um tempo X, espera-se 100ms para que o 
	 * diretório seria criado. Então ativa-se as interrupções de modo a 
	 * ser possível a utilização de poll().
	 */

	usleep(100000);	
	fd = open("/sys/class/gpio/gpio3/edge", O_WRONLY);
	_ERR(fd);
	
	er = write(fd,"both",4);
	close(fd);
	_ERR(er);
	
	fd = open("/sys/class/gpio/gpio3/value", O_RDONLY);
	_ERR(fd);	

	er = read(fd,&a,2);	
	_ERR(er);	

	lseek(fd,0,SEEK_SET);	
	_ERR(er);	
	
	er = daemon(1,0);
	_ERR(er);	

	if(fd == -1) return 0;
	memset(&in[0],0,sizeof(struct pollfd));	
	
	in[0].events = POLLPRI;
	in[0].fd = fd;	
	
	while(1){

		er = poll(in, 1, -1);
		_ERR(er);

	  	er = read(fd,&a,2);
		_ERR(er);
		
				
		if(a[0] == '0'){
			er = execl("/usr/bin/sudo","sudo","shutdown","-h","now",NULL);
			_ERR(er);
		}			

		er = lseek(fd,0,SEEK_SET);
		_ERR(er);
		
	}

	return 0;
}
