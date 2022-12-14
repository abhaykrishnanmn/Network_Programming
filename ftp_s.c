#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

void send_file_content(const char *filepath, int sock)
{
	char buffer[1024] = {0};
	printf("Reading from file %s\n", filepath);
	FILE *f = fopen(filepath, "r");
	if (!f) {
		printf("Sending Unknown file\n");
		sprintf(buffer, "Unknown file `%s`", filepath);
		send(sock, buffer, sizeof(buffer), 0);
		return;
	} else {
	
		fread(buffer, sizeof(buffer), 1, f);
		printf("Sending content %s\n", buffer);
		send(sock, buffer, sizeof(buffer), 0);
		fclose(f);
	}

}

void request_filename(int sock, const char *filename)
{
	printf("Reading from socket\n");
	read(sock, filename, 1024);
	printf("Requesting file %s\n", filename);
}

int main(int argc, char const* argv[])
{
	int server_fd, new_socket, valread;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0))
		== 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket
		= accept(server_fd, (struct sockaddr*)&address,
				(socklen_t*)&addrlen))
		< 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
	
	while(1) {
		char filename[1024] = { 0 };
		request_filename(new_socket, filename);
		send_file_content(filename, new_socket);
	}
	
	close(new_socket);
	shutdown(server_fd, SHUT_RDWR);
	return 0;
}


