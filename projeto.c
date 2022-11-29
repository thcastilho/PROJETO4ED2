#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TRUE_EMPTY "$$$$$$$$$$$$$"
#define FAKE_EMPTY "#############"
#define MAX_ITENS_INSERCAO 10
#define MAX_ITENS_BUSCA 2
#define MAX_ITENS_REMOCAO 3
#define N 31

typedef struct s_livro {
    char isbn[14];
    char titulo[50];
    char autor[50];
    char ano[5];
}LIVRO;

//struct para escrever no arquivo hash
typedef struct s_chave {
    char isbn[14];
    int bOS; //byte offset no endereco principal
}CHAVE; 

#define TAM_CHAVE 17 // isbn + bOS = 13 + sizeof(int) = 13 + 4 = 17

void printaMenu();
LIVRO* carregaInsercao();
char** carregaRemocao();
char** carregaBusca();
int divisaoInteira(long long int key); // metodo da divisao inteira
void insereRegistro(LIVRO* dados, FILE** fp_biblioteca, FILE** fp_hash);
void removeRegistro(char** dados, FILE** fp_biblioteca, FILE** fp_hash);
void buscaRegistro(char** dados, FILE** fp_biblioteca, FILE** fp_hash);

int main() {
    int opcao;

    LIVRO* dados_insercao = carregaInsercao();
    char** dados_remocao = carregaRemocao();
    char** dados_busca = carregaBusca();

    FILE* fp_biblioteca;
    FILE* fp_hash;

    do {
        printaMenu();
        scanf("%d", &opcao);
        printf("==================================\n");

        switch (opcao)
        {
        case 1:
            insereRegistro(dados_insercao, &fp_biblioteca, &fp_hash);
            break;

        case 2:
            removeRegistro(dados_remocao, &fp_biblioteca, &fp_hash);
            break;

        case 3:
            buscaRegistro(dados_busca, &fp_biblioteca, &fp_hash);
            break;

        case 0:
            printf("Encerrando programa...");
            break;
        
        default:
            printf("Opcao invalida!\n");
            break;
        }
    }while(opcao != 0);

    //dar free() nos ponteiros de insercao, remocao e busca dps
}

void printaMenu() {
    printf("==================================\n");
	printf("> 1. Insere registro  <\n");
	printf("> 2. Remove registro <\n");
	printf("> 3. Busca registro <\n");
	printf("> Opcao: ");
	
    return;
}

LIVRO* carregaInsercao() {
    FILE* in = fopen("insere.bin", "rb");

    if (in == NULL) {
        printf("Falha ao abrir o arquivo!\n");
        return NULL;
    }

    LIVRO* vet = malloc(sizeof(LIVRO) * MAX_ITENS_INSERCAO);
    fread(vet, sizeof(LIVRO), MAX_ITENS_INSERCAO, in);

    fclose(in);

    return vet;
}

char** carregaRemocao() {
	FILE* in = fopen("remove.bin", "rb");

	if (in == NULL) {
		printf("Falha ao abrir o arquivo!");
		return NULL;
	}

	char **vet;
    vet = malloc(MAX_ITENS_REMOCAO * sizeof(char*));
    int i;
	for(i = 0; i < MAX_ITENS_REMOCAO; i++) {
        vet[i] = malloc(14 * sizeof(char)); //aloca cada uma das linhas
    }

	for(i = 0; i < MAX_ITENS_REMOCAO; i++) {
		fread(vet[i], sizeof(char), 14, in);
		// printf("%s\n", vet[i]);
	}

	fclose(in);

	return vet;
}

char** carregaBusca() {
    FILE* in = fopen("busca.bin", "rb");

    if (in == NULL) {
        printf("Falha ao abrir o arquivo!\n");
        return NULL;
    }

    char **vet;
    vet = malloc(MAX_ITENS_BUSCA * sizeof(char*));
    
    int i;
    for (i = 0; i < MAX_ITENS_BUSCA; i++) {
        vet[i] = malloc(14 * sizeof(char));
    }

    for (i = 0; i < MAX_ITENS_BUSCA; i++) {
        fread(vet[i], sizeof(char), 14, in);
    }

    fclose(in);

    return vet;
}

int divisaoInteira(long long int key) {
    return key % N;
}

bool bibliotecaOpen(FILE** fp_biblioteca) {
    *fp_biblioteca = fopen("biblioteca.bin", "r+b");

    if (*fp_biblioteca == NULL) {
        return false;
    }

    return true;
}

void bibliotecaClose(FILE** fp_biblioteca) {
    fclose(*fp_biblioteca);
}

void insereRegistro(LIVRO* dados, FILE** fp_biblioteca, FILE** fp_hash) {
    int a = 0; //numero de registros inseridos
    int b = 0; //numero de registros removidos
    int c = 0; //numero de registros buscados

    int i;

    if (!bibliotecaOpen(fp_biblioteca)) { //se nao conseguir abrir
        *fp_biblioteca = fopen("biblioteca.bin", "w+b");
        if(*fp_biblioteca == NULL) {
            printf("Falha ao criar o arquivo da biblioteca!\n");
            return;
        }
        fwrite(&a, sizeof(int), 1, *fp_biblioteca);
        fwrite(&b, sizeof(int), 1, *fp_biblioteca);
        fwrite(&c, sizeof(int), 1, *fp_biblioteca);

        fclose(*fp_biblioteca); //perguntar sobre esse fclose() aq

        bibliotecaOpen(fp_biblioteca);
        *fp_hash = fopen("hash.bin", "w+b");
        if (*fp_hash == NULL) {
            printf("Falha ao criar o arquivo de hash!\n");
            return;
        }
        
        CHAVE aux;
        aux.bOS = -1;
        strcpy(aux.isbn, TRUE_EMPTY);
        
        for (i = 0; i < N; i++) {
            fwrite(aux.isbn, sizeof(char), 13, *fp_hash);   //preenche o arquivo com N TRUE_EMPTYs
            fwrite(&aux.bOS, sizeof(int), 1, *fp_hash);
        }
    } else {
        rewind(*fp_biblioteca);
        fread(&a, sizeof(int), 1, *fp_biblioteca);

        if (a == MAX_ITENS_INSERCAO) {
            printf("Todos os registros foram inseridos!\n");
            bibliotecaClose(fp_biblioteca);
            return;
        }

        *fp_hash = fopen("hash.bin", "r+b");
        if (*fp_hash == NULL) {
            printf("Falha ao abrir o arquivo de hash!\n");
            return;
        }
    }

    fseek(*fp_biblioteca, 0, SEEK_END);
    int byteOFFSET = ftell(*fp_biblioteca);

    LIVRO registro = dados[a];

    CHAVE key;
    key.bOS = byteOFFSET;
    strcpy(key.isbn, registro.isbn);

    int hash = divisaoInteira(atoll(key.isbn));
    int posicaoVerdadeira = hash;
    printf("Endereco %d\n", posicaoVerdadeira);
    bool escreveu = false;
    int contador = 1;

    char* temp = malloc(14 * sizeof(char));
    do {
    	printf("hash no comeco: %d\n", hash);
        if (hash >= N) { //quando atinge o final do arquivo
            if (posicaoVerdadeira == 0) {
                break;
            }
            hash = 0;
        }
        if (hash == posicaoVerdadeira - 1) { //rodou todas as posicao possiveis
            break;
        }
        fseek(*fp_hash, hash * TAM_CHAVE, SEEK_SET);
        // printf("posicao: %d\n", ftell(*fp_hash) / TAM_CHAVE);
        fread(temp, sizeof(char), 13, *fp_hash);
        // printf("posicao: %d\n", ftell(*fp_hash) / TAM_CHAVE);
        temp[13] = '\0';
        // printf("temp: %s\n", temp);
        if ((strcmp(temp, TRUE_EMPTY) != 0) && (strcmp(temp, FAKE_EMPTY) != 0)) {
            printf("Colisao\n");
            printf("Tentativa %d\n", contador);
            contador++;
            hash++;
        } else {
            // printf("key.isbn: %s\n", key.isbn);
            // printf("key.bOS: %d\n", key.bOS);
            fseek(*fp_hash, hash * TAM_CHAVE, SEEK_SET);
            // printf("posicao: %d\n", ftell(*fp_hash) / TAM_CHAVE);
            fwrite(key.isbn, sizeof(char), 13, *fp_hash);
            fwrite(&key.bOS, sizeof(int), 1, *fp_hash);
            escreveu = true;
        }
        printf("hash no final: %d\n", hash);
    }while(!escreveu);

    if (escreveu) {
        printf("Chave %s inserida com sucesso\n", key.isbn);
        fwrite(&registro, sizeof(registro), 1, *fp_biblioteca);
        a++;
        rewind(*fp_biblioteca);
        fwrite(&a, sizeof(int), 1, *fp_biblioteca);
    } else {
        printf("Chave %s nao inserida (NAO HA ESPACO)\n", key.isbn);
    }

    bibliotecaClose(fp_biblioteca);
    fclose(*fp_hash);
    
    	free(temp);
    return;
}

void removeRegistro(char** dados, FILE** fp_biblioteca, FILE** fp_hash) {
    int b; //numero de registros removidos

    if(!bibliotecaOpen(fp_biblioteca)) {
        printf("Nenhum registro inserido!\n");
        return;
    }

    fseek(*fp_biblioteca, sizeof(int), SEEK_SET);
    fread(&b, sizeof(int), 1, *fp_biblioteca);
    printf("b: %d\n", b);

    if (b == MAX_ITENS_REMOCAO) {
        printf("Todos os registros foram removidos!\n");
        bibliotecaClose(fp_biblioteca);
        return;
    }

    *fp_hash = fopen("hash.bin", "r+b");
    if (*fp_hash == NULL) {
        printf("Falha ao abrir o arquivo de hash!\n");
        return;
    }

    char* key;
    strcpy(key, dados[b]);

    int hash = divisaoInteira(atoll(key));
    int posicaoVerdadeira = hash;
    bool encontrou = false;

    char* temp = malloc(14 * sizeof(char));
    char* fake = malloc(14 * sizeof(char));
    int end = -1;
    do {
        printf("Hash: %d\n", hash);
        if (hash >= N) {
            if (posicaoVerdadeira == 0) {
                break;
            }
            hash = 0;
        }
        if (hash == posicaoVerdadeira - 1) {
            break;
        }
        fseek(*fp_hash, hash * TAM_CHAVE, SEEK_SET);
        fread(temp, sizeof(char), 13, *fp_hash);
        temp[13] = '\0';
        if(strcmp(temp, TRUE_EMPTY) == 0) {
            break;
        } else if (strcmp(temp, key) == 0) {
            fseek(*fp_hash, hash * TAM_CHAVE, SEEK_SET);
            strcpy(fake, FAKE_EMPTY);
            fwrite(fake, sizeof(char), 13, *fp_hash);
            fwrite(&end, sizeof(int), 1, *fp_hash);
            encontrou = true;
        }
        hash++;
    }while(!encontrou);
    
    if (encontrou) {
        printf("Chave %s removida com sucesso\n", key);
    } else {
        printf("Chave %s nao encontrada\n", key);
    }

	b++;
    fseek(*fp_biblioteca, sizeof(int), SEEK_SET);
    fwrite(&b, sizeof(int), 1, *fp_biblioteca);
        
    bibliotecaClose(fp_biblioteca);
    fclose(*fp_hash);
    free(temp);
    free(fake);
    
    return;
}

void buscaRegistro(char** dados, FILE** fp_biblioteca, FILE** fp_hash) {
	int c; //numero de buscas realizadas

    if(!bibliotecaOpen(fp_biblioteca)) {
        printf("Nenhum registro inserido!\n");
        return;
    }

    fseek(*fp_biblioteca, 2*sizeof(int), SEEK_SET);
    fread(&c, sizeof(int), 1, *fp_biblioteca);
    
    if (c == MAX_ITENS_BUSCA) {
        printf("Todos os registros foram buscados!\n");
        bibliotecaClose(fp_biblioteca);
        return;
    }

    *fp_hash = fopen("hash.bin", "r+b");
    if (*fp_hash == NULL) {
        printf("Falha ao abrir o arquivo de hash!\n");
        return;
    }
	
	char* key = malloc(14 * sizeof(char));
	strcpy(key, dados[c]);
	
	int hash = divisaoInteira(atoll(key));
	int posicaoVerdadeira = hash;
	int acessos = 0;
	bool encontrou = false;
//	char* temp = malloc(14 * sizeof(char));
	CHAVE chave;
	
	do {
		if (hash >= N){
			if (posicaoVerdadeira == 0){
				break;
			}
			hash = 0;
		}
		if (hash == posicaoVerdadeira - 1) {
            break;
        }
		
		fseek(*fp_hash, hash * TAM_CHAVE, SEEK_SET);
		fread(chave.isbn, sizeof(char), 13, *fp_hash);
		chave.isbn[13] = '\0';
		
		acessos++;
		
		if (strcmp(chave.isbn, TRUE_EMPTY) == 0) {
			break;
		} else if (strcmp(key, chave.isbn) == 0) {
			fread(&chave.bOS, sizeof(int), 1, *fp_hash);
			encontrou = true;
		}
		hash++;
	}while(!encontrou);
	
	LIVRO livro_temp;
	
	if(encontrou){
		printf("Chave %s encontrada, endereço %d, %d acessos\n", key, hash-1, acessos);
		//printa o registro encontrado
		fseek(*fp_biblioteca, chave.bOS, SEEK_SET);
		fread(&livro_temp, sizeof(LIVRO), 1, *fp_biblioteca);
		printf("ISBN: %s\n", livro_temp.isbn);
		printf("Titulo: %s\n", livro_temp.titulo);
		printf("Autor: %s\n", livro_temp.autor);
		printf("Ano: %s\n", livro_temp.ano);
	} else {
		printf("Chave %s nao encontrada\n", key);
	}
	
	c++;
	fseek(*fp_biblioteca, 2*sizeof(int), SEEK_SET);
    fwrite(&c, sizeof(int), 1, *fp_biblioteca);
    
	bibliotecaClose(fp_biblioteca);
	fclose(*fp_hash);
	free(key);
	
    return;
}
