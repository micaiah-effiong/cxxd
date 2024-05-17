#include <getopt.h>
#include <linux/limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// read and convert values to hex dump
void rdump_hex(FILE *fp, FILE *output_file);
void print_usage();

int main(int argc, char *argv[]) {
  char *argv_filename = argv[1];
  // printf("file: %s\n", argv_filename);

  int opt;
  FILE *ouput_file = stdout;
  char *output_filepath = (char *)malloc(PATH_MAX);

  if (output_filepath == NULL) {
    printf("Could not allocate memory\n");
    exit(0);
    return 1;
  }

  static struct option long_options[] = {{"help", no_argument, 0, 0},
                                         {0, 0, 0, 0}};

  int opt_index = 0;
  while ((opt = getopt_long(argc, argv, "o:hv", long_options, &opt_index)) !=
         -1) {

    switch (opt) {
    case 0:
      // printf("arg: %s, %s\n", optarg, long_options[opt_index].name);
      // printf("val: %s\n", long_options[opt_index].val);
      if (strcmp(long_options[opt_index].name, "help") == 0) {
        print_usage();
        exit(EXIT_SUCCESS);
      }
      break;
    case 'h':
      print_usage();
      exit(EXIT_SUCCESS);
      break;
    case 'v':
      printf("cxxd 2024-5-17 by Micah Effiong.\n");
      exit(EXIT_SUCCESS);
      break;
    case 'o':
      // TODO: handle realpath outside loop
      realpath(optarg, output_filepath);

      if (output_filepath == NULL) {
        fprintf(stderr, "Could not resolve file path\n");
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
  rdump_hex(file, ouput_file);

  fclose(ouput_file);
  ouput_file = NULL;

  return 0;
}

void hex_dump(int offset, char buff[17], bool is_final, FILE *output_file) {
  int buff_size = is_final ? strlen(buff) : 16; // strlen(buff);

  // 8 + 43 + 16
  // offset = 8
  // hex column = 43 (including ":" and whitespaces)
  // text = 16
  // extra room for null byte

  char text[16 + 1] = "";
  char output[43 + 1] = "";
  char output_text[67 + 1 + 1] = ""; // text + newline + null byte

  for (int j = 0; j < buff_size; j++) {
    int val = buff[j];
    int text_pos = j;

    if (val >= 32 && val <= 126) {
      text[text_pos] = val;
    } else {
      text[text_pos] = '.';
    }

    if ((int)val < 0) {
      val = 256 + val;
    }

    sprintf(output + strlen(output), "%02x", (int)val);
    // sprintf(output + strlen(output), "%d", (char)val);

    if (j % 2 != 0) {
      strcat(output, " ");
    }
    output[43] = '\0';
    text[16] = '\0';
  }

  int chunk_size = is_final ? strlen(buff) : 16; // strlen(buff);

  if (chunk_size % 16 != 0) {
    int diff = 16 - chunk_size;
    for (int i = 0; i < diff; i++) {
      sprintf(output + strlen(output), "%s", "  ");

      if ((diff - i) % 2 != 0) {
        strcat(output, " ");
      }
    }
  }

  sprintf(output_text + strlen(output_text), "%08x: %s %s\n", offset, output,
          text);
  output_text[67 + 1] = '\0';

  fwrite(output_text, 1, strlen(output_text), output_file);

  // memset(output_text, 0, strlen(output_text));
  // memset(output, 0, strlen(output));
  // memset(text, 0, strlen(text));
}

void rdump_hex(FILE *fp, FILE *output_file) {
  char buff[17] = "";
  int count = 0;
  int offset = 0;
  int s;
  bool is_lastline = false;

  fseek(fp, 0, SEEK_END);
  int length = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  int hex_lines = (int)ceil((float)length / 16);

  while ((s = fgetc(fp)) != EOF) {
    buff[count % 16] = s;
    is_lastline = (offset + 1) == hex_lines;

    if (count > 0 && count % 16 == 15) {
      buff[strlen(buff)] = '\0';

      hex_dump(offset * 16, buff, is_lastline, output_file);

      memset(buff, 0, 16);
      offset += 1;
    }

    // if (count > 16 * 5) {
    //   break;
    // }
    count++;
  }

  if (strlen(buff) != 0) {
    buff[strlen(buff)] = '\0';
    hex_dump(offset * 16, buff, is_lastline, output_file);
  }

  fclose(fp);
}

const char *usage = "\
Usage:\n\
    cxxd [options] in-file\n\
Options:\n\
    -o          path to output file. Default print to stdout.\n\
    -h --help   display this help text and exit.\n\
    -v          display version and exit.\n\
";
void print_usage() { printf("%s", usage); }
