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
    } vet[15] = {{"3333333333333", "TiT-3", "Autor-3", "3333"}, //13
                {"2222222222222", "TiT-2", "Autor-2", "2222"},  //19              
                {"1111111111111", "TiT-1", "Autor-1", "1111"}, //25                
                {"3333333333346", "TiT-0", "Autor-0", "3346"}, //26
                {"6666666666666", "TiT-6", "Autor-6", "6666"}, //26
                {"3100000000030", "TiT-1", "Autor-1", "1111"}, //30 1�
                {"3100000000031", "TiT-1", "Autor-1", "1111"}, //00 2�
                {"3100000000000", "TiT-3", "Autor-3", "3333"}, //00 3�
                {"3100000000062", "TiT-1", "Autor-1", "1111"}, //00 4�
                {"3100000000001", "TiT-2", "Autor-2", "2222"}, //01 5�
				{"3100000000002", "TiT-2", "Autor-2", "2222"}, //02 antes de inserir, remover o 62
				{"3100000000030", "TiT-1", "Autor-1", "1111"}, //30 1�
                {"3100000000031", "TiT-1", "Autor-1", "1111"}, //00 2�
                {"3100000000000", "TiT-3", "Autor-3", "3333"}, //00 3�
                {"3100000000062", "TiT-1", "Autor-1", "1111"}  //00 4�
               };
    
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca {
        char isbn[14];
    } vet_b[4] = {{"3333333333333"},
                  {"7777777777777"},
                  {"3100000000062"},
                  {"3100000000002"}
				 };
    
    fd = fopen("busca.bin", "w+b");
    fwrite(vet_b, sizeof(vet_b), 1, fd);
    fclose(fd);    
    
    //////////////////////////////
    struct remove {
        char isbn[14];
    } vet_r[5] = {{"3333333333333"},
                  {"7777777777777"},
                  {"3100000000031"},
                  {"3100000000000"},
                  {"3100000000062"}
				 };
       
    fd = fopen("remove.bin", "w+b");
    fwrite(vet_r, sizeof(vet_r), 1, fd);
    fclose(fd);
}

