#include <iostream>

int main()
{
    FILE* fptr;
    fopen_s(&fptr, "examplefile.vasyutkinlab", "r+");

    if (fptr == 0) {
        return -1;
    }

    char buf[1024] = { 0 };
    fread_s(buf, 1024, sizeof(char), 1024, fptr);
    printf_s("FILE CONTENTS:%s\n\n", buf);

    char buf_to_write[1024] = { "vasyutkin-lab-vasyutkin-lab-vasyutkin-lab-vasyutkin-lab" };
    fseek(fptr, 0, 0);
    fwrite(buf_to_write, sizeof(char), 1024, fptr);
    fflush(fptr);
    fclose(fptr);

    return 0;
}