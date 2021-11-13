all: reader writer

reader: write.c read.c alarm.c setup.c noncanonical.c
	gcc noncanonical.c write.c read.c alarm.c setup.c -o receiver

writer: write.c read.c alarm.c setup.c writenoncanonical.c
	gcc writenoncanonical.c write.c read.c alarm.c setup.c -o emitter

clean:
	rm -rf receiver emitter