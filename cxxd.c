#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int min(int a, int b) {
  if (a < b) {
    return a;
  }

  return b;
}

int main(int argc, char *argv[]) {
  char *argv_filename = argv[1];
  char *filepath = realpath(argv_filename, NULL);

  if (filepath == NULL) {
    printf("Could not resolve file path\n");
    printf("%s\n", filepath);
    return 1;
  }

  printf("%s\n", filepath);

  FILE *file = fopen(filepath, "r");

  if (!file) {
    printf("could not get file");
    return 1;
  }

  int64_t length;

  fseek(file, 0, SEEK_END);
  length = ftell(file);

  fseek(file, 0, SEEK_SET);

  // char text[length];
  int64_t hex_buffer[length];
  int64_t s;
  int64_t hex_index = 0;
  while ((s = fgetc(file)) != EOF) {
    hex_buffer[hex_index] = s;
    hex_index++;
  }

  // printf("length: %d\nhex length: %d\n", length, (int)sizeof(hex_buffer));

  fclose(file);

  int64_t offset = 0;
  int64_t offset_size = 16;
  for (int i = 0; i < length; i += offset_size) {
    // printf(">> %#08x %d\n", offset, hex_buffer[i]);
    int64_t offset_end = min(offset + offset_size, length);

    // int chunk_size = offset_end - offset;
    // printf("chunk size: %d\n", offset_end - offset);
    // int chuck[chunk_size];

    // 8 + 43 + 16
    char text[100];
    char output[100];
    // int space_count = 0;
    for (int64_t j = offset; j < offset_end; j++) {
      // printf(">count %d %d %d\n", j, offset_end, length);

      int64_t val = hex_buffer[j];
      int64_t text_pos = j - offset;

      if (val > 31 && val < 128) {
        text[text_pos] = val;
      } else {
        text[text_pos] = 46;
      }

      // printf("%d %c %d\n", val, text[text_pos], j);

      // printf("%d %c\n", hex_buffer[j], (char)hex_buffer[j]);
      sprintf(output + strlen(output), "%02x", (char)val);
      // sprintf(&output[strlen(output)], " ");
      //
      if (j % 2 != 0) {
        strcat(output, " ");
      }
      // printf("size: %d \n", sizeof(output));
      // space_count++;
    }

    int64_t chunk_size = offset_end - offset;
    // printf("chunc size %d\n", chunk_size);

    if (chunk_size % offset_size != 0) {
      int64_t diff = offset_size - chunk_size;

      for (int64_t i = 0; i < diff; i++) {
        sprintf(text + strlen(text), "%c", '.');
        sprintf(output + strlen(output), "%s", "  ");

        if ((diff - i) % 2 != 0) {
          strcat(output, " ");
        }
      }
    }

    printf("%08x: %s %s\n", offset, output, text);
    // printf(">text: %s\n", text);

    memset(output, 0, sizeof(output));
    memset(text, 0, sizeof(text));
    offset += 16;
  }

  return 0;
}
