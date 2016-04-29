main: main.o probes.o tcp_session.o trans_session.o
	gcc -o main main.o probes.o tcp_session.o trans_session.o -lpthread
main.o: main.c
	gcc -c main.c
probes.o: probes.c
	gcc -c probes.c
tcp_session.o: tcp_session.c
	gcc -c tcp_session.c
trans_session.o: trans_session.c
	gcc -c trans_session.c
clean:
	rm -rf *.o main
