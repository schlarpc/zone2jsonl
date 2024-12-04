#include <inttypes.h>
#include <jansson.h>
#include <ldns/ldns.h>
#include <resolv.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <zone.h>


char *all_rdf2str(ldns_rr *rr) {
  size_t buffer_size = 1024;
  char *buffer = malloc(buffer_size);
  if (!buffer) {
    perror("Failed to allocate memory");
    exit(EXIT_FAILURE);
  }

  buffer[0] = '\0';
  size_t current_length = 0;

  for (int i = 0; i < ldns_rr_rd_count(rr); i++) {
    const char *rdfstr = ldns_rdf2str(ldns_rr_rdf(rr, i));
    size_t str_length = strlen(rdfstr);
    // include space separator for i > 0
    size_t needed_length = current_length + str_length + 1 + (i > 0 ? 1 : 0);
    if (needed_length > buffer_size) {
      buffer_size = needed_length * 2;
      char *new_buffer = realloc(buffer, buffer_size);
      if (!new_buffer) {
        free(buffer);
        perror("Failed to reallocate memory");
        exit(EXIT_FAILURE);
      }
      buffer = new_buffer;
    }

    if (i > 0) {
      strcat(buffer, " ");
    }
    strcat(buffer, rdfstr);
    free(rdfstr);
    current_length = strlen(buffer);
  }
  return buffer;
}

ldns_rr *rr_from_rdata(uint16_t type, uint16_t rdlength, const uint8_t *rdata) {
  ldns_rr_descriptor *rr_descriptor = ldns_rr_descript(type);
  uint8_t *buffer = (uint8_t *)calloc(rdlength + 2, 1);
  buffer[0] = (rdlength >> 8) & 0xFF;
  buffer[1] = rdlength & 0xFF;
  memcpy(buffer + 2, rdata, rdlength);
  ldns_rr *rr = ldns_rr_new();
  ldns_rr_set_type(rr, rr_descriptor->_type);
  size_t pos = 0;
  ldns_wire2rdf(rr, buffer, rdlength + 2, &pos);
  free(buffer);
  return rr;
}

static const int32_t accept_rr(zone_parser_t *parser, const zone_name_t *owner,
                               uint16_t type, uint16_t class, uint32_t ttl,
                               uint16_t rdlength, const uint8_t *rdata,
                               void *user_data) {

  char name[256];
  dn_expand(owner->octets, owner->octets + owner->length, owner->octets, name,
            sizeof(name));
  char *type_str = ldns_rr_type2str(type);
  char *class_str = ldns_rr_class2str(class);
  ldns_rr *rr = rr_from_rdata(type, rdlength, rdata);
  char *rdata_str = all_rdf2str(rr);

  json_t *json_rr = json_object();
  json_object_set_new(json_rr, "name", json_string(name));
  json_object_set_new(json_rr, "ttl", json_integer(ttl));
  json_object_set_new(json_rr, "type", json_string(type_str));
  json_object_set_new(json_rr, "class", json_string(class_str));
  json_object_set_new(json_rr, "rdata", json_string(rdata_str));
  char *json_str = json_dumps(json_rr, JSON_COMPACT);
  printf("%s\n", json_str);

  free(json_str);
  free(class_str);
  free(type_str);
  free(rdata_str);
  ldns_rr_free(rr);
  json_decref(json_rr);

  return ZONE_SUCCESS;
}

int main(int argc, char *argv[]) {
  zone_parser_t parser;
  zone_name_buffer_t name;
  zone_rdata_buffer_t rdata;
  zone_buffers_t buffers = {1, &name, &rdata};
  zone_options_t options = {0}; // must be properly initialized

  if (argc != 2) {
    fprintf(stderr, "Usage: %s zone-file\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  options.accept.callback = accept_rr;
  options.origin = (zone_name_t){.octets = (uint8_t[]){0}, .length = 1};
  options.default_ttl = 3600;
  options.default_class = LDNS_RR_CLASS_IN;

  int32_t result = zone_parse(&parser, &options, &buffers, argv[1], NULL);
  if (result < 0) {
    fprintf(stderr, "Could not parse %s\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  return 0;
}
