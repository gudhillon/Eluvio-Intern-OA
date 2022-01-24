#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#define BUFFER_SIZE 4096

// Initialize struct for an array of files
struct file_info {
    char* file_name;
    int strand_length;
    int strand_offset;
};
struct file_info *files = NULL;
size_t files_length = 0;

// Function to compute longest strands of each file
void longest_strand() {

    int infile = open(files[files_length - 1].file_name, O_RDONLY);

    if (infile < 0) {
        warn("%s", files[files_length - 1].file_name);
        return;
    }

    uint8_t buffer[BUFFER_SIZE];
    for (int b = 0; b < BUFFER_SIZE; b++) buffer[b] = 0;

    // Loop until there are no more bytes to read
    // Iterating until a newline appears represents a strand
    int bytes_read = 1, strand_offset = 0, strand_length = 0;
    while (bytes_read != 0) {
        bytes_read = read(infile, buffer, BUFFER_SIZE);

        if (bytes_read < 0) {
            warn("%d", bytes_read);
            return;
        }

        for (int i = 0; i < bytes_read; i++) {
            strand_offset++;
            strand_length++;
            if (buffer[i] == '\n') {
                if (files[files_length - 1].strand_length < strand_length) {
                    files[files_length - 1].strand_length = strand_length;
                    files[files_length - 1].strand_offset = strand_offset;
                    strand_length = 0;
                }
            }
        }

    }
    close(infile);
}

int main(int argc , char **argv) {

    // Error check to see if files are provided
    if (argc < 2) {
        fprintf(stderr, "strand: option requires an argument");
        return 0;
    }

    // Compute longest strands from given files
    for (int i = 1; i < argc; i++) {
        struct file_info *temp = realloc(files, ++files_length * sizeof *temp);
        files = temp;
        files[files_length - 1].file_name = argv[i];
        files[files_length - 1].strand_length = 0;
        files[files_length - 1].strand_offset = 0;
        longest_strand();
    }

    // Find the longest strand of bytes that is identical between two or more files
    int longest_strand = 0;
    for (size_t i = 0; i < files_length; i++) {
        for (size_t j = i + 1; j < files_length; j++) {
            if (files[i].strand_length == files[j].strand_length) {
                if (files[i].strand_length > longest_strand) {
                    longest_strand = files[i].strand_length;
                }
            }
        }
    }

    // Display the results
    if (longest_strand > 0) {
        printf("Length of the longest strand:  %d\n", longest_strand);
        for (size_t i = 0; i < files_length; i++) {
            if (files[i].strand_length == longest_strand) {
                printf("File name:  %s\tOffset:  %d\n", files[i].file_name, files[i].strand_offset);
            }
        }
    } else {
        printf("longest strand of bytes that is identical between two or more files not found\n");
    }

    // Deallocate the memory previously allocated by realloc()
    if (files_length > 0) free(files);

    return 0;
}
