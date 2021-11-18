all: reader writer

reader:  src/data_link_layer/alarm.c src/data_link_layer/application.c src/data_link_layer/port.c src/data_link_layer/connection.c src/data_link_layer/link_layer.c src/data_link_layer/receiver.c
	gcc src/data_link_layer/receiver.c src/data_link_layer/application.c src/data_link_layer/alarm.c src/data_link_layer/port.c src/data_link_layer/connection.c src/data_link_layer/link_layer.c -o receiver

writer: src/data_link_layer/alarm.c src/data_link_layer/application.c src/data_link_layer/port.c src/data_link_layer/connection.c src/data_link_layer/link_layer.c src/data_link_layer/emitter.c
	gcc src/data_link_layer/emitter.c src/data_link_layer/application.c src/data_link_layer/alarm.c src/data_link_layer/port.c src/data_link_layer/connection.c src/data_link_layer/link_layer.c -o emitter

clean:
	rm -rf receiver emitter