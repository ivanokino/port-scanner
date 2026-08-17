# port-scanner

TCP SYN port scanner written in C using raw sockets.

For Linux, requires root privileges cuz of raw socket. 
To build:
```bash
make
sudo ./main <ip> <start_port> <end_port>
```