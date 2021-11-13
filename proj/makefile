all: reader writer

reader:  src/data_link_layer/alarm.c src/data_link_layer/setup.c src/data_link_layer/connection.c src/data_link_layer/noncanonical.c
	gcc src/data_link_layer/noncanonical.c src/data_link_layer/alarm.c src/data_link_layer/setup.c src/data_link_layer/connection.c -o receiver

writer: src/data_link_layer/alarm.c src/data_link_layer/setup.c src/data_link_layer/connection.c src/data_link_layer/writenoncanonical.c
	gcc src/data_link_layer/writenoncanonical.c src/data_link_layer/alarm.c src/data_link_layer/setup.c src/data_link_layer/connection.c -o emitter

clean:
	rm -rf receiver emitter