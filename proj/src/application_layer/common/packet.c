#include "../../../include/application_layer/packet.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../../include/errors/error_nos.h"

int process_control_packet(const unsigned char *bytes, size_t nb, control_packet_t *packet, bool is_start) {
    if (bytes == NULL || packet == NULL) { return NULL_POINTER_ERROR; }
    else if ((is_start && bytes[0] != C_START) || (!is_start && bytes[0] != C_END)) { return INVALID_RESPONSE; }
    memset(packet, 0, sizeof(*packet));

    unsigned char t, l;
    size_t i = 1;
    while (i < nb) {
        t = bytes[i++];
        l = bytes[i++];

        if (t == T_FILE_SIZE) {
            memcpy(&packet->file_size, bytes + i, l);
        } else if (t == T_FILE_NAME) {
            char *s = malloc(l);
            if (s == NULL) return BUFFER_OVERFLOW;
            else memcpy(s, bytes + i, l);
            packet->file_name = s;
        } else {}
        i += l;
    }
    return SUCCESS;
}

ssize_t process_data_packet(const unsigned char *bytes, size_t nb, unsigned char *dest, unsigned char no, size_t nbd) {
    if (bytes == NULL || dest == NULL) { return BUFFER_OVERFLOW; }

    unsigned char c = bytes[0];
    if (c != C_DATA) { return INVALID_RESPONSE; }

    unsigned char n = bytes[1];
    if (n != no) { return OUT_OF_ORDER; }

    unsigned char l1 = bytes[2], l2 = bytes[3];
    size_t l = 256 * l1 + l2;
    if (l > nb - 4 || l > nbd) { return BUFFER_OVERFLOW; }
    else { memcpy(dest, bytes + 4, l); }

    return (ssize_t) l;
}

size_t assemble_control_packet(control_packet_t packet, bool is_start, unsigned char *dest, size_t nbd) {
    if (dest == NULL) { return NULL_POINTER_ERROR; }

    if (nbd < 3 + sizeof(packet.file_size)) { return BUFFER_OVERFLOW; }
    size_t i = 0;
    dest[i++] = (is_start) ? C_START : C_END;
    dest[i++] = T_FILE_SIZE;
    dest[i++] = sizeof(packet.file_size);
    memcpy(dest + i, &packet.file_size, sizeof(packet.file_size));
    i += sizeof(packet.file_size);

    if (packet.file_name != NULL) {
        if (nbd < 4 + 2 + strlen(packet.file_name) + 1) { return BUFFER_OVERFLOW; }

        dest[i++] = T_FILE_NAME;
        dest[i++] = strlen(packet.file_name) + 1;
        strcpy((char *) (dest + i), packet.file_name);
        i += strlen(packet.file_name) + 1;
    }

    return i;
}

size_t assemble_data_packet(void *src, size_t nb, unsigned char *dest, size_t nbd, unsigned char no) {
    if (src == NULL || dest == NULL) { return NULL_POINTER_ERROR; }
    else if (nbd < 4 + nb) { return BUFFER_OVERFLOW; }

    size_t i = 0;
    dest[i++] = C_DATA;
    dest[i++] = no;
    dest[i++] = nb / 256;
    dest[i++] = nb - (nb / 256) * 256;
    memcpy(dest + i, src, nb);
    i += nb;

    return i;
}
