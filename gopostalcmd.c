#include <stdio.h>
#include <stdlib.h>
#include <libpostal/libpostal.h>
#include <msgpack.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 

#define PORT 8080 
int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 10) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
        // Setup (only called once at the beginning of your program)
    if (!libpostal_setup() || !libpostal_setup_parser()) {
        exit(EXIT_FAILURE);
    }

    libpostal_address_parser_options_t options;
    options = libpostal_get_address_parser_default_options();

	libpostal_address_parser_response_t *parsed;
    msgpack_sbuffer* mpbuffer = msgpack_sbuffer_new();
    msgpack_packer* pk = msgpack_packer_new(mpbuffer, msgpack_sbuffer_write);

    NextConnection:
    while (true) {
        printf("Waiting on socket\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                        (socklen_t*)&addrlen))<0) 
        {
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        while(true) {
            valread = read( new_socket , buffer, 1024); 
            if (valread == -1 || valread == 0) {
                goto NextConnection;
            }
            parsed = libpostal_parse_address(buffer,options);
            
            msgpack_sbuffer_clear(mpbuffer);
            msgpack_pack_array(pk, parsed->num_components);
            for (size_t i = 0; i < parsed->num_components; i++) {
                msgpack_pack_array(pk, 2);
                msgpack_pack_str(pk, strlen(parsed->labels[i]));
                msgpack_pack_str_body(pk, parsed->labels[i], strlen(parsed->labels[i]));
                msgpack_pack_str(pk, strlen(parsed->components[i]));
                msgpack_pack_str_body(pk, parsed->components[i], strlen(parsed->components[i]));
            }

            // Free responses
            libpostal_address_parser_response_destroy(parsed);
            
            valread = send(new_socket , mpbuffer->data , mpbuffer->size , 0 ); 
            if (valread == -1) {
                goto NextConnection;
            }
        }
    }
    // msgpack_sbuffer_destroy(mpbuffer);
    // return 0;
}