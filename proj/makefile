all: reader writer

reader: src/data_link_layer/alarm.c src/data_link_layer/port.c src/data_link_layer/connection.c src/data_link_layer/link_layer.c src/application_layer/packet.c src/application_layer/utils.c src/application_layer/receiver.c
	gcc src/application_layer/receiver.c src/application_layer/utils.c src/application_layer/packet.c src/data_link_layer/alarm.c src/data_link_layer/port.c src/data_link_layer/connection.c src/data_link_layer/link_layer.c -o receiver

writer: src/data_link_layer/alarm.c src/data_link_layer/port.c src/data_link_layer/connection.c src/data_link_layer/link_layer.c src/application_layer/packet.c src/application_layer/utils.c src/application_layer/emitter.c
	gcc src/application_layer/emitter.c src/application_layer/utils.c src/application_layer/packet.c src/data_link_layer/alarm.c src/data_link_layer/port.c src/data_link_layer/connection.c src/data_link_layer/link_layer.c -o emitter

clean:
	rm -rf receiver emitter