default: feeder
feeder: object/feeder.o object/semaphores.o object/shared_memory.o 
	gcc -pthread object/feeder.o object/semaphores.o object/shared_memory.o -o feeder
object/feeder.o:
	gcc -c source/feeder.c -o object/feeder.o 
object/semaphores.o:
	gcc -c source/semaphores.c -o object/semaphores.o 
object/shared_memory.o:
	gcc -c source/shared_memory.c -o object/shared_memory.o
clean:
	rm feeder object/feeder.o object/semaphores.o object/shared_memory.o files/*