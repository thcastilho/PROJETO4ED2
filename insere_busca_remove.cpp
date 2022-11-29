#include<stdio.h>
#include<conio.h>
#include<string.h>

int main() {
    FILE *fd;
    
    //////////////////////////////   
    struct livro {
        char isbn[14];
        char titulo[50];
        char autor[50];
        char ano[5];
    } vet[10] = {{"3333333333333", "TiT-3", "Autor-3", "3333"},
                {"2222222222222", "TiT-2", "Autor-2", "2222"},                
                {"1111111111111", "TiT-1", "Autor-1", "1111"},                
                {"3333333333346", "TiT-0", "Autor-0", "3346"},
                {"6666666666666", "TiT-6", "Autor-6", "6666"},
                {"3100000000000", "TiT-3", "Autor-3", "3333"},
                {"3100000000001", "TiT-2", "Autor-2", "2222"},                
                {"3100000000031", "TiT-1", "Autor-1", "1111"},
                {"3100000000030", "TiT-1", "Autor-1", "1111"},
                {"3100000000062", "TiT-1", "Autor-1", "1111"}
               };
    
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca {
        char isbn[14];
    } vet_b[3] = {{"3333333333333"},
                  {"7777777777777"},
                  {"3100000000062"}
				 };
    
    fd = fopen("busca.bin", "w+b");
    fwrite(vet_b, sizeof(vet_b), 1, fd);
    fclose(fd);    
    
    //////////////////////////////
    struct remove {
        char isbn[14];
    } vet_r[3] = {{"3333333333333"},
                  {"7777777777777"},
                  {"3100000000000"}
				 };
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

