#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int min(int a, int b);

int main(int argc, char *argv[]) {
  char *argv_filename = argv[1];
  char *filepath = realpath(argv_filename, NULL);

  if (filepath == NULL) {
    printf("Could not resolve file path\n");
    exit(1);
    return 1;
  }

  FILE *file = fopen(filepath, "r");

  if (!file) {
    printf("could not get file");
    exit(1);
    return 1;
  }

  int length;

  fseek(file, 0, SEEK_END);
  length = ftell(file);

  fseek(file, 0, SEEK_SET);

  // char text[length];
  int hex_buffer[length];
  int chunk;
  int hex_index = 0;
  while ((chunk = fgetc(file)) != EOF) {
    hex_buffer[hex_index] = chunk;
    hex_index++;
  }

  fclose(file);

  int offset = 0;
  int offset_size = 16;
  char output_text[length + 100]; // buffer_length + 100(meta data)
  sprintf(output_text + strlen(output_text),
          "filepath: %s\nlength: %d bytes\n\n", filepath, length);

  for (int i = 0; i < length; i += offset_size) {
    int offset_end = min(offset + offset_size, length);

    // 8 + 43 + 16
    char text[100];
    char output[100];

    for (int j = offset; j < offset_end; j++) {
      int val = hex_buffer[j];
      int text_pos = j - offset;

      if (val > 31 && val < 128) {
        text[text_pos] = val;
      } else {
        text[text_pos] = 46;
      }

      sprintf(output + strlen(output), "%02x", (char)val);
      // printf("j: %d text: %s\n", j, output);

      if (j % 2 != 0) {
        strcat(output, " ");
      }
    }

    int chunk_size = offset_end - offset;
    // printf("chunc size %d\n", chunk_size);

    if (chunk_size % offset_size != 0) {
      int diff = offset_size - chunk_size;

      for (int i = 0; i < diff; i++) {
        // sprintf(text + strlen(text), "%c", '.');
        sprintf(output + strlen(output), "%s", "  ");

        if ((diff - i) % 2 != 0) {
          strcat(output, " ");
        }
      }
    }

    sprintf(output_text + strlen(output_text), "%08x: %s %s\n", (int)offset,
            output, text);

    memset(output, 0, sizeof(output));
    memset(text, 0, sizeof(text));
    offset += 16;
  }

  printf("%s", output_text);

  return 0;
}

int min(int a, int b) {
  if (a < b) {
    return a;
  }

  return b;
}
