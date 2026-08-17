all:

	gcc -o main src/funcs/get_utils.c src/funcs/checksum_funcs.c src/funcs/packet_build_funcs.c src/main.c

t:	
	gcc -o main src/funcs/get_utils.c src/funcs/checksum_funcs.c src/funcs/packet_build_funcs.c src/main.c
	rustc scripts/listening_serv.rs