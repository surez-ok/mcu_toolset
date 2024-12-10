#include "mem_fops.h"
#include "stdbool.h"
#include "unity.h"

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

static inline int validate(int8_t *act, const int8_t *ref, int size)
{
    int test_passed = true;
    int count = 0;
    int total = 0;

    for (int i = 0; i < size; ++i)
    {
        total++;
        if (act[i] != ref[i])
        {
            count++;
            printf("ERROR at pos %d: Act: 0x%0x Ref: 0x%0x\r\n", i, (uint8_t)act[i], (uint8_t)ref[i]);
            test_passed = false;
        }
    }

    if (!test_passed)
    {
        printf("0x%0x of 0x%0x failed\r\n", (uint8_t)count, (uint8_t)total);
    }

    return test_passed;
}

#define BUFFER_SIZE 256
extern unsigned int example_bin_len;
extern unsigned char example_bin[];
void test_mem_fread(void) {
    size_t bytesRead;
    FILE *file;
    unsigned char buffer_ref[BUFFER_SIZE];

    file = fopen("example.bin", "rb");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    bytesRead = fread(buffer_ref, sizeof(unsigned char), BUFFER_SIZE, file);
    if (bytesRead != BUFFER_SIZE) {
        printf("Warning: Read less bytes than expected. bytesRead = %ld\n", bytesRead);
    }

    fclose(file);
    
    // test mem_fread
    MEMFILE *memfile;
    unsigned char membuffer[BUFFER_SIZE];
    memfile = mem_fopen(example_bin, example_bin_len);
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    bytesRead = mem_fread(membuffer, sizeof(unsigned char), BUFFER_SIZE, memfile);
    if (bytesRead != BUFFER_SIZE) {
        printf("Warning: Read less bytes than expected. bytesRead = %ld\n", bytesRead);
    }
    mem_fclose(memfile);

    TEST_ASSERT_TRUE(validate(membuffer, buffer_ref, BUFFER_SIZE));
}

int main(void) {

    UNITY_BEGIN();
    RUN_TEST(test_mem_fread);
    return UNITY_END();
}
