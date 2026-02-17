#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Define clear untuk windows dan linux
#ifdef _WIN32
#define CLEAR "cls" 
#else
#define CLEAR "clear"
#endif

#define MAX 26 // Maksimal Sesuai Jumlah Alphabet 
// Dianggap tidak ada ketika char bernilai '0' dan integer bernilai -1

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    char area;
    Point point;
} Result;

char** bacaFile (int *rows, int *cols, bool *valid, int *area){
    FILE *file;

    char areas[MAX];
    *area = 0;
    int buffer;
    *rows=0,*cols=0;

    char nama[144];
    printf("Masukkan nama file (di folder test): ");
    scanf("%s", nama);

    char path[200];
    sprintf(path, "../test/%s.txt", nama);
    file = fopen(path, "r");
    if (file == NULL){
        printf("Error boys!");
        exit(1);
    }

    // Dimensi File
    buffer = fgetc(file);
    while (buffer!=EOF){
        bool ada = false;
        if (buffer=='\n'){
            *rows+=1;
        }
        if (*rows==0 && buffer != '\n' && buffer !=' '){
            *cols+=1;
        }
        for (int i=0;i<*area;i++){
            if (areas[i]==(char)buffer){
                ada = true;
                break;
            }else if (buffer=='\n' || buffer==' '){
                ada = true;
                break;
            }
        }
        if (!ada) {
            areas[*area]=(char)buffer;
            *area+=1;
        }
        buffer = fgetc(file);
    }
    *rows +=1;
    if (*rows!=*cols || *rows!=*area ){
        *valid = false;
        return 0;
    }
    rewind(file);


    // Assign ke Matrix
    char **A =  malloc(*rows * sizeof(char *));
    for (int i=0;i< *rows;i++){
        A[i] = malloc((*cols) * sizeof(char));
    }
    int i=0,j=0;
    buffer = fgetc(file);
    while (buffer!=EOF){
        if (buffer == '\n'){
            i+=1;
            j=0;
        }else if (buffer == ' '){
        }else{
            A[i][j] = buffer;
            j+=1;
        }
        buffer = fgetc(file);
    }
    fclose(file);

    *valid = true;
    return A;
}

bool accept(Result *RL, char area, int row, int col, int n){
    for (int i=0;i<n;i++){
        if (RL[i].area==area || RL[i].point.x==row || RL[i].point.y==col){
            return false;
        }
    }
    return true;
}

void putQueen(Result *RL, char area, int row, int col, int n){
    for (int i=0;i<n;i++){
        if (RL[i].area=='0'){
            RL[i].area=area;
            RL[i].point.x=row;
            RL[i].point.y=col;
            return;
        }
    }
}

void removeQueen(Result *RL, char area, int n){
    for (int i=0;i<n;i++){
        if (RL[i].area==area){
            RL[i].area='0';
            RL[i].point.x=-1;
            RL[i].point.y=-1;
            return;
        }
    }
}

bool cekResult(Result *RL, int n){
    for (int i=0;i<n;i++){
        if (RL[i].area=='0'){
            return false;
        }
    }
    return true;
}

void printHasil(char** matrix, Result *RL, int rows, int area){
    for (int i=0;i<rows;i++){
        for (int j=0;j<rows;j++){
            bool found = false;
            for (int k=0;k<area;k++){
                if (RL[k].point.x==i && RL[k].point.y==j){
                    found = true;
                    break;
                }
            }
            if (found) printf("#");
            else printf("%c",matrix[i][j]);
        }
        printf("\n");
    }
    return;
}

bool rekursif(char** matrix, Result *RL, int rows, int cols, int *try, int freq){
    if (cekResult(RL,rows)) {
        return true;
    }

    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            if (accept(RL,matrix[i][j],i,j,rows)){
                putQueen(RL,matrix[i][j],i,j,cols);

                if (rekursif(matrix,RL,rows,cols,try,freq)){
                    return true;
                }
                *try += 1;

                if (freq>0 && *try % freq == 0){
                    printHasil(matrix,RL,rows,cols);
                    system(CLEAR);
                    printf("\n");
                }
                
                removeQueen(RL,matrix[i][j],rows);
            }
        }
    }
    return false;
}

void tulisFile(char **matrix, Result *RL, int n, char *nama, int try, int time){
    FILE *file = fopen(nama, "a");
    
    fprintf(file, "Hasil untuk %d x %d\n", n, n);
    for (int i=0;i<n;i++){
        for (int j=0;j<n;j++){
            bool found = false;
            for (int k=0;k<n;k++){
                if (RL[k].point.x==i && RL[k].point.y==j){
                    found = true;
                    break;
                }
            }
            if (found) fprintf(file, "#");
            else fprintf(file, "%c",matrix[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "Jumlah percobaan: %d\n", try);
    fprintf(file, "Waktu total: %d ms\n", time);
    fclose(file);
}

int main(){
    int rows,cols,area;
    bool valid;
    char** matrix;
    
    int freq;
    printf("[PERINGATAN] Live Update akan memperlambat algoritma, masukkan 0 untuk menonaktifkan Live Update\n");
    printf("[SARAN] Gunakan frekuensi 100.000 untuk n=8 supaya tidak terlalu lambat\n");
    printf("Masukkan frekuensi iterasi untuk Live Update : ");
    scanf("%d",&freq);

    matrix = bacaFile(&rows,&cols,&valid,&area);

    if (!(valid)){
        printf("Validasi Input salah!\n");
        printf("Input kolom, baris, dan warna harus berjumlah sama.\n");
        return 0;
    }

    Result *RL = malloc(area * sizeof(Result));
    for (int a=0;a<area;a++) {
        RL[a].area = '0';
        RL[a].point.x = -1;
        RL[a].point.y = -1;
    }
    
    int try=0;
    time_t start;
    start=clock();
    if(rekursif(matrix,RL,rows,cols,&try,freq)){
        printf("Ada solusi dengan %d percobaan\n", try);
        printHasil(matrix,RL,rows,area);
        printf("\n");
        int time = clock()-start;
        printf("Waktu total: %d ms\n",time);
        char save;
        printf("Apakah ingin menyimpan hasil? (y/n) :");
        scanf(" %c", &save);
        if (save=='y' || save=='Y'){
            char nama[144];
            printf("Masukkan nama file untuk menyimpan hasil (di folder test): ");
            scanf("%s", nama);
            char path[200];
            sprintf(path, "../test/%s.txt", nama);
            tulisFile(matrix,RL,rows,path,try,time);
        }
    }else{
        printf("Gak nemu lagi!");
    }
}
