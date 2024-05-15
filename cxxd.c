#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int min(int a, int b);
void hex_dump(FILE *file, FILE *output);

struct HexFileData {
  char *data;
  int length;
};
struct HexFileData read_file(FILE *file);

int main(int argc, char *argv[]) {
  char *argv_filename = argv[1];

  int opt;
  FILE *ouput_file = stdout;
  char *output_filepath = (char *)malloc(PATH_MAX);

  if (output_filepath == NULL) {
    printf("Could not allocate memory\n");
    exit(0);
    return 1;
  }

  while ((opt = getopt(argc, argv, "o:")) != -1) {

    switch (opt) {
    case 'o':

      // TODO: handle realpath outside loop
      realpath(optarg, output_filepath);

      if (output_filepath == NULL) {
        printf("Could not resolve file path\n");
        free(output_filepath);
        output_filepath = NULL;
        exit(1);
        return 1;
      }

      break;
    default:
      break;
    }
  }

  char *filepath = realpath(argv_filename, NULL);
  if (filepath == NULL) {
    printf("Could not resolve file path\n");
    exit(1);
    return 1;
  }

  FILE *file = fopen(filepath, "r");
  free(filepath);
  filepath = NULL;

  if (!file) {
    printf("could not get file");
    exit(1);
    return 1;
  }

  if (output_filepath != NULL && strlen(output_filepath) > 0) {
    ouput_file = fopen(output_filepath, "w");
    free(output_filepath);
    output_filepath = NULL;
  }
  hex_dump(file, ouput_file);

  // fwrite(result, 1, strlen(result), ouput_file);
  fclose(ouput_file);
  ouput_file = NULL;

  // free(result);
  // result = NULL;

  return 0;
}

void hex_dump(FILE *file, FILE *output_file) {
  struct HexFileData hex_file = read_file(file);

  int offset = 0;
  int offset_size = 16;
  int output_size = (hex_file.length * hex_file.length) * sizeof(char *);
  // char *output_text = (char *)malloc(output_size);
  char output_text[100] = "";

  // if (output_text == NULL) {
  //   printf("Could not allocate memory for");
  //   exit(0);
  // }

  // printf(">length: %ld, start: %d, end %d\n", length, start, end);
  // printf("> length: %d\n> text: %s\n", length, output_text);

  /* sprintf(output_text + strlen(output_text),
          "filepath: %s\nlength: %d bytes\n\n", filepath, length); */

  for (int i = 0; i < hex_file.length; i += offset_size) {
    int offset_end = min(offset + offset_size, hex_file.length);

    // 8 + 43 + 16
    char text[100];
    char output[100];

    for (int j = offset; j < offset_end; j++) {
      int val = hex_file.data[j];
      int text_pos = j - offset;

      if (val >= 32 && val <= 126) {
        text[text_pos] = val;
      } else {
        text[text_pos] = '.';
      }

      sprintf(output + strlen(output), "%02x", (char)val);
      // printf("j: %d text: %s\n", j, output);

      if (j % 2 != 0) {
        strcat(output, " ");
      }
      output[43] = '\0';
      text[16] = '\0';
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

    // printf("out_text %s\n", output_text);

    sprintf(output_text + strlen(output_text), "%08x: %s %s\n", offset, output,
            text);

    fwrite(output_text, 1, strlen(output_text), output_file);

    memset(output_text, 0, strlen(output_text));
    memset(output, 0, strlen(output));
    memset(text, 0, strlen(text));
    offset += 16;
  }

  // sprintf(output_text + strlen(output_text), "%c", '\0');

  free(hex_file.data);
  hex_file.data = NULL;
}

struct HexFileData read_file(FILE *file) {
  fseek(file, 0, SEEK_END);
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *data = (char *)malloc(length);

  fread(data, 1, length, file);
  data[length] = '\0';
  fclose(file);
  // file = NULL;

  struct HexFileData result = {data, length};
  return result;
}

int min(int a, int b) {
  if (a < b) {
    return a;
  }

  return b;
}
