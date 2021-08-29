#include <stdio.h>
#include <elf.h>

#ifdef __x86_64__
    typedef Elf64_auxv_t* Elf_auxv_p;
    #define PRINT_AUX(aux)  printf("a_type = %2ld, a_val = 0x%016lx\n", aux->a_type, aux->a_un.a_val)
#else
    typedef Elf32_auxv_t* Elf_auxv_p;
    #define PRINT_AUX(aux)  printf("a_type = %2d, a_val = 0x%08x\n", aux->a_type, aux->a_un.a_val)
#endif

int main(int argc, char *argv[])
{
    size_t *p = (size_t *)argv;
    int i;
    Elf_auxv_p aux;

    printf("Argument count: %d\n", (int)*(p-1));

    // print argument
    i = 0;
    while (*p)
    {
        printf("arg%d = %p, %s\n", i, (void *)*p, (char*)*p);
        p++;
        i++;
    }
    p++; // skip 0

    // print enviroment
    i = 0;
    while (*p)
    {
        // 会输出较多内容，暂时屏蔽
        //printf("env%d = %p, %s\n", i, (void *)*p, (char *)*p);
        p++;
        i++;
    }
    p++; // skip 0

    aux = (Elf_auxv_p)p;
    while (aux->a_type != AT_NULL)
    {
        PRINT_AUX(aux);
        aux++;
    }

    return 0;
}

/*

64 位下示例输出：
Argument count: 1
arg0 = 0x7fffedd1060f, ./a.out
a_type = 33, a_val = 0x00007fffee170000
a_type = 16, a_val = 0x000000001f8bfbff
a_type =  6, a_val = 0x0000000000001000
a_type = 17, a_val = 0x0000000000000064
a_type =  3, a_val = 0x00007f73cb800040
a_type =  4, a_val = 0x0000000000000038
a_type =  5, a_val = 0x0000000000000009
a_type =  7, a_val = 0x00007f73cb400000
a_type =  8, a_val = 0x0000000000000000
a_type =  9, a_val = 0x00007f73cb800540
a_type = 11, a_val = 0x00000000000003e8
a_type = 12, a_val = 0x00000000000003e8
a_type = 13, a_val = 0x00000000000003e8
a_type = 14, a_val = 0x00000000000003e8
a_type = 23, a_val = 0x0000000000000000
a_type = 25, a_val = 0x00007fffedd105f8
a_type = 31, a_val = 0x00007fffedd115aa
a_type = 15, a_val = 0x00007fffedd10608

*/