#include <stdio.h>
#include <epoll.h>

static int CreateAndBind(char *port);
static int SetNonBlocking(int fd);

#define MAXEVENTS 64

int main(void){
    int sfd, s, efd;
    struct epoll_event event;
    struct epoll_event *events;

    if(argc != 2){
        fprintf(stderr, "Usage: %s [port]\n" argv[0]);
        exit(EXIT_FAILURE);
    }

    sfd = CreateAndBind(agrv[1]);
    if(sfd == -1){
        abort();
    }
    s = SetNonBlocking(sfd);
    if(s == -1){
        abort();
    }
    s = listen(sfd, 2);
    if(s == -1){
        perror("listening error");
        abort();
    }

    efd = epoll_create1(0);
    if(efd == -1){
        perror("epollcreate1 error");
        abort();
    }

    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl(efd, EPOLL_CTL_ADD

    events = calloc(MAXEVENTS, sizeof(event));
    if(s == -1){
        perror("epoll_ctl error");
        abort();
    }

    events = calloc(2, sizeof(event));

    while(1){
        int n, i;

        n = epoll_wait(efd, events, MAXEVENTS, -1);
        for(i = 0; i<n; i++){
            if((events[i].events & EPOLLERR) ||(events[i].events &EPOLLUP)) {
                //socket closed or socket is not ready for reading
                fprintf(stderr, "epoll error\n");
                close(events[i].data.fd);
                continue;
            } else if(sfd == events[i].data.fd){
                //notifcation that listening socket, one or more connections
                while(1){
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                    in_len = sizeof(in_addr);
                    infd = accept(sfd, &in_addr, &in_len);
                    if(infd == -1){
                        if ((errno == EGAIN) || (errno == EWOULDBLOCK)){
                            //processed all incoming connections
                            break;
                        } else {
                            perror("accept");
                            break;
                        }
                    }

                    s = getnameinfo(&in_addr, in_len, hbuf, sizeof(hbuf),sbuf,sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);

                    if(s==0){
                        printf"Accepted connection on descriptor %d " "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                    }
                    //makine incoming socket non-blocking and add to list

                    s = SetNonBlocking(infd);
                    if(s == -1){
                        abort();
                    }

                    event.data.fd = infd;
                    event.events = EPOLLIN | EPOLLET;
                    s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
                    if(s == -1){
                        perror("epoll_ctl");
                        abort();
                    }
                }
                continue;
            } else {
                echo((epoll_data*)event.data.ptr);
            }
        }

    }
    free(events);
    close(sfd);
    return 0;
}

static int CreateAndBind(char *port){
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int ga, sfd;
    

    //defining getaddrinfo function
    memset(&hints, - sizeof(struct addrinfo)); //zeroing the structure
    hints.ai_family = AF_UNSPEC; //use IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; //use TCP socket
    hints.ai_flags = AI_PASSIVE; //pass NULL in nodename if using AI_PASSIVE

    ga = getaddrinfo (NULL, port, &hints, &result);
    if (ga != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ga));
        return -1;
    }
    //loop thorugh all results and connect to first one
    for (rp = result; rp != NULL; rp->ai_next){
        //create socket
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(sfd == -1){
            continue;
        }
        ga = bind (sfd, rp->ai_addr, rp->ai_addrlen);
        if(ga == 0){
            //binded sucessfully
            break;
        }
        close(sfd);
    }
    if (rp == NULL){
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(result); //done with struct

    return sfd;
}

static int SetNonBlocking(int fd){
    int flags,s;
    //fetch status flag of file descriptor
    flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1){
        perror("fcntl");
        return -1;
    }
    //if file descriptor is not nonblocking
    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if(s == -1){
        perror("fcntl");
        return -1;
    }

    return 0;
}
