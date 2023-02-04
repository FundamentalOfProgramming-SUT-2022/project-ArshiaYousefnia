#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <locale.h>

#define SIZE 1080000000
#define SIZE2 4000000

//Arshia Yousefnia

/////////////////////////utility////////////////////////////////

// /s in string except /root
int char_no( char *buffer){
    int no=0;
    for ( int i=0; i<strlen(buffer);i++){
        if ( *(buffer+i)=='/'){no++;}
    }
    return no-1;
}

int s_no( char *buffer){
    int no=0;
    for ( int i=0; i<strlen(buffer);i++){
        if ( *(buffer+i)==' '){no++;}
    }
    return no;
}

int d_no( char *buffer){
    int no=0;
    for ( int i=0; i<strlen(buffer);i++){
        if ( *(buffer+i)=='\"'){no++;}
    }
    return no%2;
}


/****************************utility****************************/


/////////////////////////////logic///////////////////////////////

//createfile
//createfile --file <inp>
int createfile ( char *main_buffer){

    int junk=0;

    char *buffer=(char*)malloc(SIZE);
    strcpy(buffer,main_buffer+junk);
    int x=strlen(buffer);
    char * location = (char*)malloc(x);
    char * l_location = (char*)malloc(x);

    int ch_no=char_no(buffer);

    //extracting address

    if (*(buffer)=='/'){
        //clean address
        for ( int i=1;i<x-1;i++){
            *(location+i-1)=*(buffer+i);
        }
        *(location+x-2)='\0';
        x -= 2;
    }
    else if ( *(buffer)=='"'){
        if ( *(buffer+x-2) != '"'){return 2;}
        //dirty
        for ( int i=2;i<x-2;i++){
            *(location+i-2)=*(buffer+i);
        }
        *(location+x-4)='\0';
        x -= 4;
    }
    else{
        strcpy(location,buffer);
    }

    //printf("%s\n%d\n",location,x);

    FILE *file = fopen(location,"r");
    if (file != NULL){
        fclose(file);
        free(location);
        free(l_location);
        free(buffer);
        return 0; //already exists
    }

    int num=0;
    for ( int index=0;index<x;index++){

        *(l_location+index)=*(location+index);

        if ( *(l_location+index)=='/'){
            num++;
            *(l_location+index+1)='\0';
            mkdir(l_location ,0777);
        }

        if ( num == ch_no){
            break;
        }

    }

    file = fopen(location,"w");
    FILE *b = file;
    fclose(file);
    
    free(location);
    free(l_location);
    free(buffer);

    if(b == NULL){ return 2;}   //bad address
    return 1;                   //success

}


//cat
//cat --file <add>
int cat ( char *main_buffer,char *result){

    int junk=11;

    char *buffer=(char*)malloc(SIZE);
    strcpy(buffer,main_buffer+junk);
    int x=strlen(buffer);
    char * location = (char*)malloc(x);
    char * l_location = (char*)malloc(x);

    int ch_no=char_no(buffer);

    //extracting address

    if (*(buffer)=='/'){
        //clean address
        for ( int i=1;i<x-1;i++){
            *(location+i-1)=*(buffer+i);
        }
        *(location+x-2)='\0';
        x -= 2;
    }
    else if ( *(buffer)=='"'){
        if ( *(buffer+x-2) != '"'){return 0;}
        //dirty
        for ( int i=2;i<x-2;i++){
            *(location+i-2)=*(buffer+i);
        }
        *(location+x-4)='\0';
        x -= 4;
    }

    //printf("%s\n%d\n",location,x);

    FILE *file = fopen(location,"r");
    if (file == NULL){
        free(location);
        free(l_location);
        free(buffer);
        return 0; //doesn't exist or bad address
    }


    int sizet = 6000000;
    char *temp = (char*)malloc(sizet);

    //output
    while( fgets(temp,sizet,file) != NULL){
        //console output
        //printf("%s",temp);
        strcat(result,temp);
        //
    }
    //printf("\n");
    //

    fclose(file);

    free(temp);    
    free(location);
    free(l_location);
    free(buffer);

    return 1;                   //success

}

//tree
//tree <depth>
void tree(int indent , int limit , char *address , char *result){

    if ( limit < -1){
        strcat(result,"-Invalid depth\n");
        return;
    }///////////////////
    if ( (indent == 0) && ( limit > -1)){
        limit++;
    }
    ///////////////////////////
    if ( indent == limit*2 ){
        return;
    }

    struct dirent *de;
    DIR *dr = opendir(address);

    char * b_address = (char *)malloc(SIZE2/10);

    if ( dr == NULL){
        free(b_address);
        return;
    }

    while (( de = readdir(dr)) != NULL){

        if ( ( strcmp(de->d_name,".") != 0) && ( strcmp(de->d_name,"..")) ){

            for ( int i=0 ; i<indent;i++){

                if ( i==indent-2){
                    strcat(result,"└");//├└
                    continue;
                }

                if ( i== indent-1){
                    strcat(result,"─");
                    continue;
                }

                if ( i%2 == 0){
                    strcat(result," ");// '│
                    continue;
                }

                strcat(result," ");

            }

            strcat(result,de->d_name);
            strcat(result,"\n");

            strcpy(b_address,address);
            strcat(b_address,"/");
            strcat(b_address,de->d_name);

            tree( indent+2 , limit , b_address , result);
        }
    }

    free(b_address);
    return;

}

//compare
//compare <f1> <f2>
void compare( char *address1 , char *address2 , char *result){
    //
    //printf("%s\n%s\n",address1,address2);
    //
    int i=0,j=0;

    if ( *address1 == '/' ){
        i++;
    }

    if ( (*address1 == '"') && (*(address1+1) == '/') ){
        i=2;
        *(address1+strlen(address1)-1)='\0';
    }

    FILE *file1 = fopen(address1+i,"r");

    i=0;
    if ( *address2 == '/' ){
        i++;
    }

    if ( (*address2 == '"') && (*(address2+1) == '/') ){
        i=2;
        *(address2+strlen(address2)-1)='\0';
    }

    FILE *file2 = fopen(address2+i,"r");

    i=0;

    //error handling
    if ( (file1 == NULL) && (file2 == NULL) ){
        strcat(result,"-error: both addresses are invalid or no such files.\n");
        return;
    }

    else if ( file1 == NULL){
        strcat(result,"-error: first address is invalid or no such file.\n");
        fclose(file2);
        return;
    }

    else if ( file2 == NULL){
        strcat(result,"-error: second address is invalid or no such file.\n");
        fclose(file1);
        return;
    }

    char *line_from1 =(char *)malloc(SIZE2/100);
    char *line_from2 =(char *)malloc(SIZE2/100);
    char *status1;
    char *status2;

    status1 =fgets(line_from1,SIZE2/100,file1);
    status2 =fgets(line_from2,SIZE2/100,file2);

    while (  (status1!= NULL) && (status2 != NULL) ){
        i++;
        if ( *(line_from1+strlen(line_from1)-1) != '\n'){
            strcat(line_from1,"\n");
        }
        if ( *(line_from2+strlen(line_from2)-1) != '\n'){
            strcat(line_from2,"\n");
        }

        if ( strcmp(line_from1,line_from2) != 0){

            strcat(result,"============ #");
            char *temporary =(char *)malloc(SIZE);
            sprintf(temporary,"%d",i);
            strcat(result,temporary);
            strcat(result," ============\n");
            free(temporary);

            // if ( *(line_from1+strlen(line_from1)-1) != '\n'){
            //     strcat(line_from1,"\n");
            // }
            // if ( *(line_from2+strlen(line_from2)-1) != '\n'){
            //     strcat(line_from2,"\n");
            // }

            strcat(result,line_from1);
            strcat(result,line_from2);

            

        }

        status1 =fgets(line_from1,SIZE2/100,file1);
        status2 =fgets(line_from2,SIZE2/100,file2);
    }
    
    ///

    if ( status1 != NULL ){
        free(line_from2);
        j=i;
        char *remainder =(char *)malloc(SIZE2);
        *remainder='\0';

        do{
            strcat( remainder , line_from1 );
            j++;
        }while( (status1 =fgets(line_from1,SIZE2/100,file1))!= NULL );

        strcat(result,"<<<<<<<<<<<< #");
        char *temporary =(char *)malloc(SIZE);
        sprintf(temporary,"%d - #%d",i+1,j);
        strcat(result,temporary);
        strcat(result," <<<<<<<<<<<<\n");
        free(temporary);
        strcat(result,remainder);
        strcat(result,"\n");
        free(remainder);
        return;
    }

    if ( status2 != NULL ){
        free(line_from1);
        j=i;
        char *remainder =(char *)malloc(SIZE2);
        *remainder='\0';

        do{
            strcat( remainder , line_from2 );
            j++;
        }while( (status2 =fgets(line_from2,SIZE2/100,file2))!= NULL );

        strcat(result,">>>>>>>>>>>> #");
        char *temporary =(char *)malloc(SIZE);
        sprintf(temporary,"%d - #%d",i+1,j);
        strcat(result,temporary);
        strcat(result," >>>>>>>>>>>>\n");
        free(temporary);
        strcat(result,remainder);
        strcat(result,"\n");
        free(remainder);
        fclose(file1);
        fclose(file2);
        return;

    }

    fclose(file1);
    fclose(file2);
    return;

}

//insert
//insertstr --file <ad> --str <text> --pos line:pos
void Insert(char *address,char *text,int Line,int pos, char *result , int back_up_control ){

    if ( pos < 0){
        strcat(result,"-error: invalid position in line.\n");
        return;
    }
    int ii=0;
    if ( *address == '/'){
        ii++;
    }

    FILE *file = fopen(address+ii,"r");

    if ( file == NULL){
        strcat(result,"-error:bad address or no such file.\n");
        return;
    }

    char *before=(char *)malloc(SIZE2/10);
    char *after=(char *)malloc(SIZE2/10);
    char *middle=(char *)malloc(SIZE2/10);
    *before='\0';
    *after='\0';
    *middle='\0';
    char *status;
    char *line=(char *)malloc(SIZE2/100);
    *line='\0';


    for ( int c=1 ; c<Line;c++){
        status = fgets(line,SIZE2/100,file);
        if ( status == NULL){
            strcat(result,"-error: incorrect line for this file.\n");
            free(before);
            free(after);
            free(middle);
            free(line);
            return;
        }
        strcat(before,line);
    }
    status = fgets(line,SIZE2/100,file);

    //error handling
    if ( (Line != 1) && (status == NULL) && ( (*(before+strlen(before)-1)) != '\n' ) ){
        strcat(result,"-error: incorrect line for this file.\n");
        free(before);
        free(after);
        free(middle);
        free(line);
        return;
    }

    if ((status == NULL) && ( pos != 0)){
        strcat(result,"-error: incorrect position in line for this file.\n");
        free(before);
        free(after);
        free(middle);
        free(line);
        return;
    }

    if ( (status != NULL) && ((*(line+strlen(line)-1)) == '\n') && (pos > (strlen(line)-1))){
        strcat(result,"-error: incorrect position in line for this file.\n");
        free(before);
        free(after);
        free(middle);
        free(line);
        return;
    }

    if ( (status != NULL) && ((*(line+strlen(line)-1)) != '\n') && (pos > (strlen(line)))){
        strcat(result,"-error: incorrect position in line for this file.\n");
        free(before);
        free(after);
        free(middle);
        free(line);
        return;
    }/////check if pos is correct and add before pos to before and rest to after and other lines to after,then create middle from text

    //text//
    int check=0,index = 0;
    if ( s_no(text) != 0){
        index=1;
        check=1;
    }
    
    while ( index < strlen(text)-check){
        if ( ((*(text+index))=='\\') && ((*(text+index+1))=='\\') && ((*(text+index+2))=='n')){
            strcat(middle,"\\n");
            index+=3;
            continue;
        }

        if ( ((*(text+index))=='\\') && ((*(text+index+1))=='\\') && ((*(text+index+2))=='t')){
            strcat(middle,"\\t");
            index+=3;
            continue;
        }

        if ( ((*(text+index))=='\\') && ((*(text+index+1))=='n')){
            strcat(middle,"\n");
            index+=2;
            continue;
        }

        if ( ((*(text+index))=='\\') && ((*(text+index+1))=='t')){
            strcat(middle,"\t");
            index+=2;
            continue;
        }
        char sag[] = {*(text+index),'\0'};
        strcat(middle,sag);
        index++;

    }
    //

    if ( (Line!=1) && (status == NULL) ){
        /////undo

        fclose(file);
        
        if ( back_up_control != 0){

        char *backup_address = ( char *)malloc ( SIZE2/100);
        *backup_address='\0';
    
        strcat(backup_address,"/backup/");
        strcat(backup_address,address+ii);
        strcat(backup_address,"\n");
        createfile(backup_address);
        *(backup_address+strlen(backup_address)-1)='\0';
        FILE *file2 = fopen(backup_address+1,"w");
        fprintf(file2,"%s%s",before,after);
        fclose(file2);
        free(backup_address);
        }

        file = fopen(address+ii,"w");
        fprintf(file,"%s%s",before,middle);
        fclose(file);
        free(before);
        free(after);
        free(middle);
        free(line);
        return;
    }

    char *tempo = (char *)malloc(SIZE2/10);
    *tempo='\0';
    for ( int m=0;m<pos;m++){
        (*(tempo+m))=(*(line+m));
        (*(tempo+m+1))='\0';
    }
    strcat(before,tempo);
    free(tempo);

    strcat(after,line+pos);
    while ( (status = fgets(line,SIZE2/100,file)) != NULL){
        strcat(after,line);
    }

    /////undo

    fclose(file);

    if ( back_up_control != 0){
    char *backup_address = ( char *)malloc ( SIZE2/100);
    *backup_address='\0';
    
    strcat(backup_address,"/backup/");
    strcat(backup_address,address+ii);
    strcat(backup_address,"\n");
    createfile(backup_address);
    *(backup_address+strlen(backup_address)-1)='\0';
    FILE *file2 = fopen(backup_address+1,"w");
    fprintf(file2,"%s%s",before,after);
    fclose(file2);
    free(backup_address);
    }

    file = fopen(address+ii,"w");
    fprintf(file,"%s%s%s",before,middle,after);
    fclose(file);
    free(before);
    free(after);
    free(middle);
    free(line);
    return;

}

//remove
//removestr --file <> --pos 1:2 --size 1 -f/b
// Remove(temp,int1,int2,int3,input,Result);
void Remove(char *address,int Line,int pos,int size,char direction, char * result , int back_up_control ){

    if ( (direction != 'f') && (direction != 'b')){
        strcat(result,"-error: bad removing direction.\n");
        return;
    }
    if ( pos < 0){
        strcat(result,"-error: invalid position in line.\n");
        return;
    }
    int ii=0;
    if ( *address == '/'){
        ii++;
    }

    FILE *file = fopen(address+ii,"r");

    if ( file == NULL){
        strcat(result,"-error:bad address or no such file.\n");
        return;
    }

    char *before=(char *)malloc(SIZE2/10);
    char *after=(char *)malloc(SIZE2/10);
    *before='\0';
    *after='\0';
    char *status;
    char *line=(char *)malloc(SIZE2/100);
    *line='\0';


    for ( int c=1 ; c<Line;c++){
        status = fgets(line,SIZE2/100,file);
        if ( status == NULL){
            strcat(result,"-error: incorrect line for this file.\n");
            free(before);
            free(after);
            free(line);
            return;
        }
        strcat(before,line);
    }
    status = fgets(line,SIZE2/100,file);

    //error handling
    if ( (Line != 1) && (status == NULL) && ( (*(before+strlen(before)-1)) != '\n' ) ){
        strcat(result,"-error: incorrect line for this file.\n");
        free(before);
        free(after);
        free(line);
        return;
    }

    if ((status == NULL) && ( pos != 0)){
        strcat(result,"-error: incorrect position in line for this file.\n");
        free(before);
        free(after);
        free(line);
        return;
    }

    if ( (status != NULL) && ((*(line+strlen(line)-1)) == '\n') && (pos > (strlen(line)-1))){
        strcat(result,"-error: incorrect position in line for this file.\n");
        free(before);
        free(after);
        free(line);
        return;
    }

    if ( (status != NULL) && ((*(line+strlen(line)-1)) != '\n') && (pos > (strlen(line)))){
        strcat(result,"-error: incorrect position in line for this file.\n");
        free(before);
        free(after);
        free(line);
        return;
    }

    char *tempo = (char *)malloc(SIZE2/10);
    *tempo='\0';
    for ( int m=0;m<pos;m++){
        (*(tempo+m))=(*(line+m));
        (*(tempo+m+1))='\0';
    }
    strcat(before,tempo);
    free(tempo);
    
    strcat(after,line+pos);
    while ( (status = fgets(line,SIZE2/100,file)) != NULL){
        strcat(after,line);
    }
    fclose(file);
    file = fopen(address+ii,"w");

    if ( direction == 'f'){

        if ( back_up_control != 0){
        char *backup_address = ( char *)malloc ( SIZE2/100);
        *backup_address='\0';
    
        strcat(backup_address,"/backup/");
        strcat(backup_address,address+ii);
        strcat(backup_address,"\n");
        createfile(backup_address);
        *(backup_address+strlen(backup_address)-1)='\0';
        FILE *file2 = fopen(backup_address+1,"w");
        fprintf(file2,"%s%s",before,after);
        fclose(file2);
        free(backup_address);
        }
        if ( strlen(after) < size){
            fprintf(file,"%s",before);
            fclose(file);
        }
        
        else{
            fprintf(file,"%s%s",before,after+size);
            fclose(file);
        }
        free(before);
        free(after);
        free(line);
        return;
    }

    if ( direction == 'b'){

        if ( back_up_control != 0){
        char *backup_address = ( char *)malloc ( SIZE2/100);
        *backup_address='\0';
    
        strcat(backup_address,"/backup/");
        strcat(backup_address,address+ii);
        strcat(backup_address,"\n");
        createfile(backup_address);
        *(backup_address+strlen(backup_address)-1)='\0';
        FILE *file2 = fopen(backup_address+1,"w");
        fprintf(file2,"%s%s",before,after);
        fclose(file2);
        free(backup_address);
        }

        if ( strlen(before) < size){
            fprintf(file,"%s",after);
            fclose(file);
        }
        
        else{
            *(before+strlen(before)-size)='\0';
            fprintf(file,"%s%s",before,after);
            fclose(file);
        }
        free(before);
        free(after);
        free(line);
        return;
    }  

}

//copy
//copystr --file <> --pos 1:2 --size 1 -f/b
void copy(char *address,int Line,int pos,int size,char direction, char * result, char *clipboard){

    if ( (direction != 'f') && (direction != 'b')){
        strcat(result,"-error: bad removing direction.\n");
        return;
    }
    if ( pos < 0){
        strcat(result,"-error: invalid position in line.\n");
        return;
    }
    int ii=0;
    if ( *address == '/'){
        ii++;
    }

    FILE *file = fopen(address+ii,"r");

    if ( file == NULL){
        strcat(result,"-error:bad address or no such file.\n");
        return;
    }

    char *before=(char *)malloc(SIZE2/10);
    char *after=(char *)malloc(SIZE2/10);
    *before='\0';
    *after='\0';
    char *status;
    char *line=(char *)malloc(SIZE2/10);
    *line='\0';


    for ( int c=1 ; c<Line;c++){
        status = fgets(line,SIZE2/100,file);
        if ( status == NULL){
            strcat(result,"-error: incorrect line for this file.\n");
            free(before);
            free(after);
            free(line);
            return;
        }
        strcat(before,line);
    }
    status = fgets(line,SIZE2/100,file);

    //error handling
    if ( (Line != 1) && (status == NULL) && ( (*(before+strlen(before)-1)) != '\n' ) ){
        strcat(result,"-error: incorrect line for this file.\n");
        free(before);
        free(after);
        free(line);
        return;
    }

    if ((status == NULL) && ( pos != 0)){
        strcat(result,"-error: incorrect position in line for this file.\n");
        free(before);
        free(after);
        free(line);
        return;
    }

    if ( (status != NULL) && ((*(line+strlen(line)-1)) == '\n') && (pos > (strlen(line)-1))){
        strcat(result,"-error: incorrect position in line for this file.\n");
        free(before);
        free(after);
        free(line);
        return;
    }

    if ( (status != NULL) && ((*(line+strlen(line)-1)) != '\n') && (pos > (strlen(line)))){
        strcat(result,"-error: incorrect position in line for this file.\n");
        free(before);
        free(after);
        free(line);
        return;
    }

    char *tempo = (char *)malloc(SIZE2);
    *tempo='\0';
    for ( int m=0;m<pos;m++){
        (*(tempo+m))=(*(line+m));
        (*(tempo+m+1))='\0';
    }
    strcat(before,tempo);
    free(tempo);
    
    strcat(after,line+pos);
    while ( (status = fgets(line,SIZE2/100,file)) != NULL){
        strcat(after,line);
    }

    fclose(file);

    if ( direction == 'f'){

        if ( strlen(after) < size){
            strcpy(clipboard,after);
        }
        
        else{
            *(after+size)='\0';
            strcpy(clipboard,after);
        }
        free(before);
        free(after);
        free(line);
        return;
    }

    if ( direction == 'b'){

        if ( strlen(before) < size){
            strcpy(clipboard,before);
        }
        
        else{
            strcpy(clipboard,before+strlen(before)-size);
        }

        free(before);
        free(after);
        free(line);
        return;
    }  

}

//////////////////////////grep
//grep -switch --str e --files jggkg  jgrokjeg
void Grep( char *string , char * files , char * Result){
    int pointer = 0,index = 0;
    char check;
    FILE *file;
    char *current = ( char *)malloc(SIZE2/100);
    while (1){
        if ( ((sscanf(files+pointer , "\"%[^\"]%*c",current)==1) || (sscanf(files+pointer , "%s" , current) == 1)) && ( strlen(current) >= 5)){
            char *line =( char *)malloc ( SIZE2);
            index++;
            if ( *(files+pointer) == '\"'){
                pointer+=2;
            }
            pointer += strlen(current)+1;

            file = fopen ( current+1 , "r");
            if ( file == NULL){
                sprintf(Result+strlen(Result),"-error: address number %d is invalid or empty: %s\n",index,current+1);
                continue;
            }

            int l_no=0;
            while ( fgets(line,SIZE2,file) != NULL ){
                //
                l_no++;
                if ( strlen(line) < strlen(string)){
                    continue;
                }
                for ( int i=0 ; i<= strlen(line)-strlen(string) ; i++){
                    check = *(line + i + strlen(string));
                    *(line + i + strlen(string)) = '\0';
                    if (strcmp(string,line+i)==0){
                        *(line + i + strlen(string)) = check;
                        sprintf(Result+strlen(Result),"%s line %d: %s",current,l_no,line);
                        if (*(Result+strlen(Result)-1)!='\n'){
                            strcat(Result,"\n");
                        }
                        break;
                    }
                    *(line + i + strlen(string)) = check;
                }
            }
            continue;
        }
        break;
    }

    free(current);
    return;
}

void Grep2( char *string , char * files , char * Result){
    int pointer = 0,index = 0;
    int result=0;
    char check;
    FILE *file;
    char *current = ( char *)malloc(SIZE2/100);
    while (1){
        if ( ((sscanf(files+pointer , "\"%[^\"]%*c",current)==1) || (sscanf(files+pointer , "%s" , current) == 1)) && ( strlen(current) >= 5)){
            char *line =( char *)malloc ( SIZE2);
            index++;
            if ( *(files+pointer) == '\"'){
                pointer+=2;
            }
            pointer += strlen(current)+1;

            file = fopen ( current+1 , "r");
            if ( file == NULL){
                sprintf(Result+strlen(Result),"-error: address number %d is invalid or empty: %s\n",index,current+1);
                continue;
            }

            int l_no=0;
            while ( fgets(line,SIZE2,file) != NULL ){
                //
                l_no++;
                if ( strlen(line) < strlen(string)){
                    continue;
                }
                for ( int i=0 ; i<= strlen(line)-strlen(string) ; i++){
                    check = *(line + i + strlen(string));
                    *(line + i + strlen(string)) = '\0';
                    if (strcmp(string,line+i)==0){
                        *(line + i + strlen(string)) = check;
                        result++;
                        // sprintf(Result+strlen(Result),"%s line %d: %s",current,l_no,line);
                        // if (*(Result+strlen(Result)-1)!='\n'){
                        //     strcat(Result,"\n");
                        // }
                        break;
                    }
                    *(line + i + strlen(string)) = check;
                }
            }
            continue;
        }
        sprintf(Result+strlen(Result),"%d\n",result);
        break;
    }
    free(current);
    return;
}

void Grep3( char *string , char * files , char * Result){
    int pointer = 0,index = 0 , file_check=0;
    char check;
    FILE *file;
    char *current = ( char *)malloc(SIZE2/100);
    while (1){
        if ( ((sscanf(files+pointer , "\"%[^\"]%*c",current)==1) || (sscanf(files+pointer , "%s" , current) == 1)) && ( strlen(current) >= 5)){
            char *line =( char *)malloc ( SIZE2);
            index++;
            if ( *(files+pointer) == '\"'){
                pointer+=2;
            }
            pointer += strlen(current)+1;

            file = fopen ( current+1 , "r");
            if ( file == NULL){
                sprintf(Result+strlen(Result),"-error: address number %d is invalid or empty: %s\n",index,current+1);
                continue;
            }

            int l_no=0;
            file_check=0;
            while ( (fgets(line,SIZE2,file) != NULL) && ( file_check == 0 ) ){
                //
                l_no++;
                if ( strlen(line) < strlen(string)){
                    continue;
                }
                for ( int i=0 ; i<= strlen(line)-strlen(string) ; i++){
                    check = *(line + i + strlen(string));
                    *(line + i + strlen(string)) = '\0';
                    if (strcmp(string,line+i)==0){
                        *(line + i + strlen(string)) = check;
                        sprintf(Result+strlen(Result),"%s",current);
                        if (*(Result+strlen(Result)-1)!='\n'){
                            strcat(Result,"\n");
                        }
                        file_check=1;
                        break;
                    }
                    *(line + i + strlen(string)) = check;
                }
            }
            continue;
        }
        break;
    }
    
    free(current);
    return;
}

void autoindent( char * address , char *Result ){
    FILE *file = fopen ( address , "r" );
    
    if ( file == NULL){
        sprintf(Result,"-error: bad address or no such file: %s\n",address);
        return;
    }

    char *control;
    char *message = ( char *) malloc ( SIZE);
    char *line = ( char *) malloc ( SIZE2);
    int counter=0;
    *line = '\0';
    *(message) = '\0';

    while ( ( control = fgets(line , SIZE2 , file) ) != NULL ){
        strcat(message,line);
    }

    char *backup_address = ( char *)malloc ( SIZE2/100);
    *backup_address='\0';
    
    strcat(backup_address,"/backup/");
    strcat(backup_address,address);
    strcat(backup_address,"\n");
    createfile(backup_address);
    *(backup_address+strlen(backup_address)-1)='\0';
    FILE *file2 = fopen(backup_address+1,"w");
    fprintf(file2,"%s",message);
    fclose(file2);
    free(backup_address);


    ///dump
    for ( int i=0 ; i<strlen(message) ; i++ ){
        if ( (*(message+i) == '}') || (*(message+i) == '{') || (*(message+i) == '\n') ){
            
            counter = 1;
            while ( ((i - counter) >= 0) && (( *(message+i-counter) == ' ') || ( *(message+i-counter) == '\t') || (*(message+i-counter) == '\n'))){
                *(message+i-counter) = -1;
                counter++;
            }

            counter = 1;
            while ( ((i + counter) < strlen(message) ) && ( (*(message+i+counter) == '\t') || ( *(message+i+counter) == ' ') || ( *(message+i+counter) == '\n'))){
                *(message+i+counter) = -1;
                counter++;
            }

        }
    }
    ///finalize
    counter = 0;
    file = fopen ( address , "w");
    char last_char = -1;
    for ( int i = 0 ; i< strlen(message) ; i++){

        if ( *(message + i) == -1){
            continue;
        }

        if ( *(message+i) == '{'){
            if ( (last_char != -1) && ( last_char != ' ') && ( last_char != '\t') && (last_char != '\n')){
                fputc(' ',file);
            }
            else{
                for ( int j=0 ; j < counter ; j++){
                fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
            }
            }
            fputc( '{' , file );
            counter++;
            fputc('\n',file);
            last_char = '\n';
            // for ( int j=0 ; j < counter ; j++){
            //     fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
            // }
            //last_char = ' ';
            continue;

        }

        if ( *(message+i) == '}'){
            if ( (last_char != '\n') && (last_char != ' ') && (last_char != '\t')){
                fputc('\n',file);
            }
            counter--;
            for ( int j=0 ; j < counter ; j++){
                    fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
            }

            fputc('}',file);
            fputc('\n',file);
            last_char='\n';
            // for ( int j=0 ; j < counter ; j++){
            //     fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
            //     last_char=' ';
            // }
            continue;
        }

        if (*(message+i) == '\n' ){
            fputc('\n',file);
            last_char='\n';
            // for ( int j=0 ; j < counter ; j++){
            //     fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
            // }
            // last_char=' ';
            continue;
        }

        if ( last_char == '\n'){
            for ( int j=0 ; j < counter ; j++){
                fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
            }
        }
        fputc(*(message+i),file);
        last_char= *(message+i);
    }

    fclose(file);
    free(line);
    free(message);
    return;
    
}

void find ( char *String , char * add_info , char * Result){
    //address may contain modes as well
    char *address = ( char *) malloc ( SIZE2);
    if (!( ( (*add_info != '\"')  && (sscanf( add_info , "%s" , address) == 1) ) || ( (*add_info == '\"') && ( sscanf(add_info , "\"%[^\"]%*c",address) == 1) ))){
        strcat(Result,"Bad address.\n");
        free(address);
        return;
    }
    FILE *file = fopen ( address+1 , "r");
    if ( file == NULL){
        strcat(Result,"-error: no such file.\n");
        free(address);
        fclose(file);
        return;
    }
    int push=0;
    if ( *add_info == '\"'){
        push = 2;
    }
    char *data = ( char *) malloc ( SIZE);
    fgets(data,SIZE , file);
    char * string = ( char *)malloc(SIZE2);
    int index=0;
    for ( int i = 0 ; i < strlen(String) ; i++){
        if ( (*(String+i) == '\\') && (*(String+i+1)=='*') ){
            *(string+index)='*';
            index++;
            i++;
            continue;
        }
        *(string+index)=*(String+i);
        index++;
    }
    *(string+index)='\0';

    //no option
    if ( strlen(add_info) - strlen(address) <= 3 ){
        char temp;
        if ( strlen(data) <= strlen(string)){
            sprintf(Result,"-1\n");
            free(address);
            free(data);
            free(string);
            fclose(file);
            return;
        }

        for ( int i=0 ; i <= strlen(data) - strlen(string) ; i++){
            temp = *(data+i+strlen(string));
            *(data+i+strlen(string)) ='\0';
            if ( strcmp ( data+i , string ) == 0){
                *(data+i+strlen(string)) = temp;
                sprintf(Result,"%d\n",i);
                free(address);
                free(data);
                free(string);
                fclose(file);
                return;
            }
            *(data+i+strlen(string)) = temp;
        }
        sprintf(Result,"-1\n");
        free(address);
        free(data);
        free(string);
        fclose(file);
        return;

    }

    //count
    if ( strcmp ( " -count\n",add_info+strlen(address)+push)==0){
        int count=0;
        char temp;
        if ( strlen(data) <= strlen(string)){
            sprintf(Result,"0\n");
            free(address);
            free(data);
            free(string);
            fclose(file);
            return;
        }

        for ( int i=0 ; i <= strlen(data) - strlen(string) ; i++){
            temp = *(data+i+strlen(string));
            *(data+i+strlen(string)) ='\0';
            if ( strcmp ( data+i , string ) == 0){
                *(data+i+strlen(string))=temp;
                count++;
                i += strlen(string)-1;
                continue;
            }
            *(data+i+strlen(string)) = temp;
        }
        sprintf(Result,"%d\n",count);
        free(address);
        free(data);
        free(string);
        fclose(file);
        return;
    }

    //at
    int at;
    char con;
    if ( (sscanf(add_info+strlen(address)+push," -at %d%c",&at,&con)==2) && (con == '\n')){ 
        char temp;
        int count=0;
        if ( strlen(data) <= strlen(string)){
            sprintf(Result,"-1\n");
            free(address);
            free(data);
            free(string);
            fclose(file);
            return;
        }

        for ( int i=0 ; i <= strlen(data) - strlen(string) ; i++){
            temp = *(data+i+strlen(string));
            *(data+i+strlen(string)) ='\0';
            if ( strcmp ( data+i , string ) == 0){
                *(data+i+strlen(string))=temp;
                count++;
                if ( count == at){
                    sprintf(Result,"%d\n",i);
                    free(address);
                    free(data);
                    free(string);
                    fclose(file);
                    return;
                }
                i += strlen(string)-1;
                continue;
            }
            *(data+i+strlen(string)) = temp;
        }
        sprintf(Result,"-1\n");
        free(address);
        free(data);
        free(string);
        fclose(file);
        return;
    }

    //all
    if ( strcmp ( " -all\n",add_info+strlen(address)+push)==0){
        char temp;
        if ( strlen(data) <= strlen(string)){
            sprintf(Result,"-1\n");
            free(address);
            free(data);
            free(string);
            fclose(file);
            return;
        }
        for ( int i=0 ; i <= strlen(data) - strlen(string) ; i++){
            temp = *(data+i+strlen(string));
            *(data+i+strlen(string)) ='\0';
            if ( strcmp ( data+i , string ) == 0){
                *(data+i+strlen(string))=temp;
                //
                sprintf(Result+strlen(Result),"%d, ",i);
                i += strlen(string)-1;
                continue;
            }
            *(data+i+strlen(string)) = temp;
        }
        if ( strlen(Result) <= 2){
            strcat(Result,"-1\n");
            free(address);
            free(data);
            free(string);
            fclose(file);
            return;
        }
        *(Result+strlen(Result)-2) = '\n';
        *(Result+strlen(Result)-1) = '\0';
        free(address);
        free(data);
        free(string);
        fclose(file);
        return;
    }

    //byword
    if ( strcmp ( " -byword\n",add_info+strlen(address)+push)==0){
        int w_count=0;
        char temp;
        if ( strlen(data) <= strlen(string)){
            sprintf(Result,"0\n");
            free(address);
            free(data);
            free(string);
            fclose(file);
            return;
        }
        if ( (*data != ' ') && ( *data != '\t')){
            w_count=1;
        }

        for ( int i=0 ; i <= strlen(data) - strlen(string) ; i++){
            if ( ((*(data+i)==' ') || (*(data+i)=='\t'))  && (  (i==0) ||  ((*(data+i-1)!=' ')  && (*(data+i-1)!='\t')) )){
                w_count++;
                
            }

            temp = *(data+i+strlen(string));
            *(data+i+strlen(string)) ='\0';
            if ( strcmp ( data+i , string ) == 0){
                *(data+i+strlen(string))=temp;
                sprintf(Result,"%d\n",w_count);
                free(address);
                free(data);
                free(string);
                fclose(file);
                return;
            }
            *(data+i+strlen(string)) = temp;
        }
        sprintf(Result,"-1\n");
        free(address);
        free(data);
        free(string);
        fclose(file);
        return;
    }

    //at_byword
    if ( ((sscanf(add_info+strlen(address)+push," -at %d -byword%c",&at,&con)==2) ||(sscanf(add_info+strlen(address)+push," -byword -at %d%c",&at,&con)==2)) && (con == '\n')){ 
        char temp;
        int w_count=0;
        if ( (*data != ' ') && ( *data != '\t')){
            w_count=1;
        }
        int count=0;
        if ( strlen(data) <= strlen(string)){
            sprintf(Result,"-1\n");
            free(address);
            free(data);
            free(string);
            fclose(file);
            return;
        }

        for ( int i=0 ; i <= strlen(data) - strlen(string) ; i++){
            if ( ((*(data+i)==' ') || (*(data+i)=='\t'))  && (  (i==0) ||  ((*(data+i-1)!=' ')  && (*(data+i-1)!='\t')) )){
                w_count++;
                
            }
            temp = *(data+i+strlen(string));
            *(data+i+strlen(string)) ='\0';
            if ( strcmp ( data+i , string ) == 0){
                *(data+i+strlen(string))=temp;
                count++;
                if ( count == at){
                    sprintf(Result,"%d\n",w_count);
                    free(address);
                    free(data);
                    free(string);
                    fclose(file);
                    return;
                }
                i += strlen(string)-1;
                continue;
            }
            *(data+i+strlen(string)) = temp;
        }
        sprintf(Result,"-1\n");
        free(address);
        free(data);
        free(string);
        fclose(file);
        return;
    }

    //all byword
    if ( (strcmp ( " -all -byword\n",add_info+strlen(address)+push)==0) ||(strcmp(" -byword -all\n",add_info+strlen(address)+push) == 0)){
        char temp;
        int w_count=0;
        if ( (*data != ' ') && ( *data != '\t')){
            w_count=1;
        }
        if ( strlen(data) <= strlen(string)){
            sprintf(Result,"-1\n");
            free(address);
            free(data);
            free(string);
            fclose(file);
            return;
        }
        for ( int i=0 ; i <= strlen(data) - strlen(string) ; i++){
            if ( ((*(data+i)==' ') || (*(data+i)=='\t'))  && (  (i==0) ||  ((*(data+i-1)!=' ')  && (*(data+i-1)!='\t')) )){
                w_count++;
                
            }
            temp = *(data+i+strlen(string));
            *(data+i+strlen(string)) ='\0';
            if ( strcmp ( data+i , string ) == 0){
                *(data+i+strlen(string))=temp;
                //
                sprintf(Result+strlen(Result),"%d, ",w_count);
                i += strlen(string)-1;
                continue;
            }
            *(data+i+strlen(string)) = temp;
            
        }
        if ( strlen(Result) <= 2){
            strcat(Result,"-1\n");
            free(address);
            free(data);
            free(string);
            fclose(file);
            return;
        }
        *(Result+strlen(Result)-2) = '\n';
        *(Result+strlen(Result)-1) = '\0';
        free(address);
        free(data);
        free(string);
        fclose(file);
        return;
    }
    

    /////////////////////////////////////////
    sprintf(Result,"-error: undefined or illegal mode combination for find:%s" , add_info+strlen(address)+push );
    free(address);
    free(data);
    free(string);
    return;
}

void replace( char *String1 , char *string2 , char *add_info , char *Result){
    char *address = ( char *) malloc ( SIZE2);
    if (!( ( (*add_info != '\"')  && (sscanf( add_info , "%s" , address) == 1) ) || ( (*add_info == '\"') && ( sscanf(add_info , "\"%[^\"]%*c",address) == 1) ))){
        strcat(Result,"Bad address.\n");
        free(address);
        return;
    }
    FILE *file = fopen ( address+1 , "r");
    if ( file == NULL){
        strcat(Result,"-error: no such file.\n");
        free(address);
        fclose(file);
        return;
    }

    char *string1 =  ( char *)malloc(SIZE2);
    int cont=0;
    for ( int i=0 ; i< strlen(String1);i++){
        if ( (i<strlen(String1)-1) && (*(String1+i)=='\\') && (*(String1+i+1)=='\"')){
            *(string1+cont)='\"';
            cont++;
            i++;
            continue;
        }
        if ( *(String1+i)=='\"'){
            continue;
        }
        *(string1+cont)=*(String1+i);
        cont++;
    }
    *(string1+cont)='\0';
    int cont2=0;
    int push=0;
    if ( *add_info == '\"'){
        push = 2;
    }
    if ( s_no(string2) != 0){
        char *string3 = (char *)malloc(SIZE2);
        sprintf(string3,"\"%s\"",string2);
        strcpy(string2,string3);
        free(string3);
    }
    //no option
    if ( strlen(add_info) - strlen(address) <= 3 ){
        char *result= ( char *)malloc(SIZE2/100);
        find(string1,add_info,result);
        if ( (sscanf(result,"%d\n", &cont2) != 1) || ( cont2==-1)){
            sprintf(Result,"%s",result);
            free(address);
            fclose(file);
            free(string1);
            return;
        }

        *result='\0';
        fclose(file);
        Remove(address,1,cont2,strlen(string1),'f',result,1);
        *result='\0';
        Insert(address,string2,1,cont2,result,0);
        sprintf(Result,"success.\n");
        free(address);
        free(string1);
        return;
    }

    //at
    int at;
    char con;
    if ( (sscanf(add_info+strlen(address)+push," -at %d%c",&at,&con)==2) && (con == '\n')){
        char *result= ( char *)malloc(SIZE2/100);
        find(string1,add_info,result);
        if ( (sscanf(result,"%d\n", &cont2) != 1) || ( cont2==-1)){
            sprintf(Result,"%s",result);
            free(address);
            fclose(file);
            free(string1);
            return;
        }
        *result='\0';
        fclose(file);
        Remove(address,1,cont2,strlen(string1),'f',result,1);
        *result='\0';
        Insert(address,string2,1,cont2,result,0);
        sprintf(Result,"success.\n");
        free(address);
        free(string1);
        return;

    }

    //all
    if ( strcmp ( " -all\n",add_info+strlen(address)+push)==0){
        char *result= ( char *)malloc(SIZE2/100);
        *result='\0';
        find(string1,add_info,result);
        if ( (sscanf(result,"%d", &cont2) != 1) || ( cont2==-1)){
            sprintf(Result,"%s",result);
            free(address);
            fclose(file);
            free(string1);
            return;
        }
        fclose(file);
        Remove(address,1,cont2,strlen(string1),'f',result,1);
        *result='\0';
        Insert(address,string2,1,cont2,result,0);

        while (1){
            *result='\0';
            find(string1,add_info,result);
            if ( (sscanf(result,"%d ", &cont2) != 1) || ( cont2==-1)){
                break;
            }
            Remove(address,1,cont2,strlen(string1),'f',result,0);
            *result='\0';
            Insert(address,string2,1,cont2,result,0);
        }
        sprintf(Result,"success.\n");
        free(address);
        free(string1);
        free(result);
        return;

    }

    sprintf(Result,"-error: undefined or illegal mode combination for find:%s" , add_info+strlen(address)+push );
    free(address);
    free(string1);
    return;
}

void undo ( char *address , char * Result){
    FILE *file = fopen ( address , "r" );
    
    if ( file == NULL){
        sprintf(Result,"-error: bad address or no such file: %s\n",address);
        return;
    }

    char *backup_address = ( char *)malloc ( SIZE2/100);
    *backup_address='\0';
    
    strcat(backup_address,"backup/");
    strcat(backup_address,address);

    FILE *file2 = fopen(backup_address,"r");

    if ( file2 == NULL){
        sprintf(Result,"no history to undo.\n");
        return;
    }

    char *stream1 = ( char *)malloc(SIZE);
    char *stream2 = ( char *)malloc(SIZE);
    *stream1 = *stream2 = '\0';
    char *line = ( char *)malloc(SIZE2);
    *line = '\0';

    while ( fgets(line , SIZE2 , file) != NULL){
        strcat(stream1, line );
    }

    while ( fgets(line,SIZE2 , file2) != NULL){
        strcat(stream2,line);
    }

    fclose(file);
    fclose(file2);

    file = fopen ( address , "w");
    file2 = fopen ( backup_address , "w");

    fprintf(file , "%s", stream2);
    fprintf( file2 , "%s" , stream1);

    fclose(file);
    fclose(file2);

    free(stream1);
    free(stream2);
    free(line);
    free(backup_address);
    return;
}


/*****************************logic*****************************/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void insert_char ( char *string , char key , int pos){
    *(string + strlen(string) + 1) = '\0';
    for ( int i=strlen(string) ; i>pos ; i--){
        *(string + i) = *(string + i -1);
    }
    *(string + pos) = key;
}

int count_line( char *string){
    int count=1;
    for ( int i=0 ; i<strlen(string); i++){
        if ( *(string + i) == '\n'){
            count++;
        }
    }
    return count;
}

void remove_char (char *string , int pos){
    if ( pos == -1){
        return;
    }
    for ( int i=pos ; i<strlen(string)-1 ;i++){
        *(string+i) = *(string +  i +1);
    }
    *(string + strlen(string)-1) = '\0'; 
}

int chars_in_line ( char *string ,  int line){
    int n_l = 0;
    int position=0;
    for ( int i = 0 ; i<strlen(string) ; i++){
        if ( *(string+i) == '\n'){
            if ( n_l == line){
                return position;
            }
            n_l++;
            position=0;
            continue;
        }
        position++;
    }
    return position;
}

int index_of_line( char *string , int line){
    if ( line == 0){
        return 0;
    }
    int l_no=0;
    for ( int i=0 ; i<strlen(string) ; i++){
        if ( *(string+i) == '\n'){
            l_no++;
        }
        if ( l_no == line){
            return i+1;
        }
    }
    return ( strlen(string)-1);
}


void autoindent2( char * string){

    char *control;
    char *message = ( char *) malloc ( SIZE);
    char *line = ( char *) malloc ( SIZE);
    int counter=0;
    *line = '\0';
    *(message) = '\0';
    strcpy(message,string);

    ///dump
    for ( int i=0 ; i<strlen(message) ; i++ ){
        if ( (*(message+i) == '}') || (*(message+i) == '{') || (*(message+i) == '\n') ){
            
            counter = 1;
            while ( ((i - counter) >= 0) && (( *(message+i-counter) == ' ') || ( *(message+i-counter) == '\t') || (*(message+i-counter) == '\n'))){
                *(message+i-counter) = -1;
                counter++;
            }

            counter = 1;
            while ( ((i + counter) < strlen(message) ) && ( (*(message+i+counter) == '\t') || ( *(message+i+counter) == ' ') || ( *(message+i+counter) == '\n'))){
                *(message+i+counter) = -1;
                counter++;
            }

        }
    }
    ///finalize
    counter = 0;
    char last_char = -1;
    for ( int i = 0 ; i< strlen(message) ; i++){

        if ( *(message + i) == -1){
            continue;
        }

        if ( *(message+i) == '{'){
            if ( (last_char != -1) && ( last_char != ' ') && ( last_char != '\t') && (last_char != '\n')){
                //fputc(' ',file);
                sprintf(line+strlen(line)," ");
            }
            else{
                for ( int j=0 ; j < counter ; j++){
                //fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
                sprintf(line+strlen(line),"    ");
            }
            }
            sprintf(line+strlen(line),"{");
            //fputc( '{' , file );
            counter++;
            //fputc('\n',file);
            sprintf(line+strlen(line),"\n");
            last_char = '\n';
            // for ( int j=0 ; j < counter ; j++){
            //     fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
            // }
            //last_char = ' ';
            continue;

        }

        if ( *(message+i) == '}'){
            if ( (last_char != '\n') && (last_char != ' ') && (last_char != '\t')){
                //fputc('\n',file);
                sprintf(line+strlen(line),"\n");
            }
            counter--;
            for ( int j=0 ; j < counter ; j++){
                    //fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
                    sprintf(line+strlen(line),"    ");
            }

            //fputc('}',file);
            //fputc('\n',file);
            sprintf(line+strlen(line),"}\n");
            last_char='\n';
            // for ( int j=0 ; j < counter ; j++){
            //     fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
            //     last_char=' ';
            // }
            continue;
        }

        if (*(message+i) == '\n' ){
            //fputc('\n',file);
            sprintf(line+strlen(line),"\n");
            last_char='\n';
            // for ( int j=0 ; j < counter ; j++){
            //     fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
            // }
            // last_char=' ';
            continue;
        }

        if ( last_char == '\n'){
            for ( int j=0 ; j < counter ; j++){
                //fputc(' ', file);fputc(' ', file);fputc(' ', file);fputc(' ', file);
                sprintf(line+strlen(line),"    ");
            }
        }
        //fputc(*(message+i),file);
        sprintf(line+strlen(line),"%c",*(message+i));
        last_char= *(message+i);
    }

    strcpy ( string , line);
    free(line);
    free(message);
    return;
    
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(){
    int result,inp, int1 , int2 , int3 , int4;
    char *main_buffer=(char*)malloc(SIZE/100000);
    char *address=(char*)malloc(SIZE/10000);
    char *temp=(char *)malloc(SIZE/10000);
    char *temp2=(char *)malloc(SIZE/10000);

    char *clipboard=(char *)malloc(SIZE);
    *clipboard='\0';

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    char input,input2,input3,input4;
    int max_x ,max_y, Mode=0,key,Index=0;
    char *buffer = (char *)malloc(SIZE2);
    char *buffer2 = (char *)malloc(SIZE2);
    *buffer2 = '\0';
    /// 
    char *clocation = ( char *)malloc ( SIZE2/10);
    char *content = ( char *)malloc(SIZE);
    *clocation = *content='\0';

    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_color( 11 , 0 , 255 , 0);
    init_color(COLOR_BLACK , 40, 40 , 40);
    init_color(10, 555 , 555 , 555);
    init_color(12 , 90 , 90 ,90);
    init_pair(1, 10 , 0);
    init_pair(2 , COLOR_BLUE , 0);
    init_pair(3 , COLOR_RED , COLOR_CYAN);
    init_pair(4 , COLOR_RED , COLOR_WHITE);
    init_pair(5 , COLOR_BLACK , COLOR_YELLOW);
    init_pair(6 , COLOR_WHITE , 12);
    ///
    ///
    getmaxyx(stdscr ,max_y,max_x);
    WINDOW *left = newwin(max_y-2, 4 , 0,0);
    WINDOW *display = newwin(max_y-2,max_x-5, 0 , 5);
    WINDOW *command = newwin(1 , max_x , max_y-1 , 0);
    WINDOW *mode = newwin( 1 , 6 , max_y-2 , 0);
    WINDOW *Location = newwin ( 1 , (max_x - 6)* 3 / 4  - 2, max_y - 2 , 8);
    WINDOW *save = newwin ( 1 , 1 , max_y - 2 , 7);
    WINDOW *OUT = newwin ( 1 , (max_x - 6)/4 , max_y -2 , 3*(max_x+2)/4);
    refresh();
    wattron(mode , COLOR_PAIR(5));
    wattron(save , COLOR_PAIR(4));
    wbkgd(display,COLOR_PAIR(6));
    wbkgd(OUT , COLOR_PAIR(5));
    wbkgd(Location,COLOR_PAIR(5));
    keypad(command,TRUE);
    keypad(display,TRUE);
    keypad(mode,TRUE);
    keypad(OUT,TRUE);
    keypad(save,TRUE);
    keypad(Location,TRUE);
    //keypad(left,TRUE);
    ///
    wprintw(OUT,"salam");
    wprintw(mode , "NORMAL");
    wprintw( Location , "/root/123.txt");
    wprintw(save,"+");
    wrefresh(save);
    wrefresh(OUT);
    wrefresh(mode);
    wrefresh(Location);
    FILE *file;
    ///
    //printw ( "%d:%d\nsoyuz",max_y,max_x);
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int line_number=1;
    int line=0,pos = 0,start=0,end=0;
    char save_status;
    //
    ////////////////////////////////////////////////////////////
    // strcpy(clocation , "/root/file.txt");
    // file = fopen ( clocation+1 , "r");
    // save_status='-';
    // while ( fgets(buffer2 , SIZE2 , file) != NULL){
    //     //wprintw(display,"line:%s",buffer2);
    //     //wprintw(display,"ksfj:%s",(buffer2));
    //     //wrefresh(display);
    //     strcat(content,buffer2);
    //     line_number++;
    // }
    // fclose(file);
    *clocation = '\0';

    end = line_number-1-start;
    wclear(display);
    wprintw(display,"%s" , content+index_of_line(content,start));
    wrefresh(display);
    //wprintw(command,"%d:",(int)strlen(content));

    wattron(left,COLOR_PAIR(1));
    wclear(left);
    //if (strlen(content)>0){
        for ( int i=0 ; (i<end+1) && (i<max_y-2) ; i++){
                wprintw(left,"%4d",(i+1+start)%10000);
            }
    //}
    wattron(left,COLOR_PAIR(2));
    for(int i=end+1 ; i<max_y-2 ; i++){
        wprintw(left,"%-4c",'~');
    }
                
    wrefresh(left);
    //////////////////////////////
    
    wclear(command);
    wprintw(command , "%d:%d", line+start+1 , pos);
    wrefresh(command);

    wmove(display,line , pos);
    wrefresh(display);
    ////////////////////////////////////////////////////////////

    // wprintw(display,"%s" , content+index_of_line(content,start));
    // wrefresh(display);
    // //wprintw(command,"%d:",(int)strlen(content));
    // wattron(left,COLOR_PAIR(1));
    // for ( int i=0 ; i<end+1 ; i++){
    //     wprintw(left,"%3d",i+1+start);
    // }
    // wattron(left,COLOR_PAIR(2));
    // for(int i=end+1 ; i<max_y-2 ; i++){
    //     wprintw(left,"%-3c",'~');
    // }
    // wrefresh(left);
    
    while(1){
        wclear(mode);
        wrefresh(mode);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        if ( Mode == 0){

            //normal
            wprintw(mode,"NORMAL");
            wrefresh(mode);

            while(1){
                noecho();
                //////location
                wclear(Location);
                wprintw(Location,"%s",clocation);
                //wprintw(Location,"->%d",strlen(content));
                wrefresh(Location);
                ///////save status
                wprintw(save , "%c", save_status);
                wrefresh(save);
                ///////////update file
                end = line_number-1-start;
                wclear(display);
                wprintw(display,"%s" , content+index_of_line(content,start));
                wrefresh(display);
                //wprintw(command,"%d:",(int)strlen(content));

                wattron(left,COLOR_PAIR(1));
                wclear(left);
                //if (strlen(content)>0){
                    for ( int i=0 ; (i<end+1) && (i<max_y-2) ; i++){
                        wprintw(left,"%4d",(i+1+start)%10000);
                    }
                //}
                wattron(left,COLOR_PAIR(2));
                for(int i=end+1 ; i<max_y-2 ; i++){
                    wprintw(left,"%-4c",'~');
                }
                
                wrefresh(left);
                //////////////////////////////

                wclear(command);
                wprintw(command , "%d:%d", line+start+1 , pos);
                wrefresh(command);

                wmove(display,line , pos);
                wrefresh(display);
                key = wgetch(display);

                //change mode
                if ( key == 'i'){
                    Mode = 1;
                    break;    
                }

                if ( key == 'v'){
                    Mode=2;
                    break;
                }

                //auto-indent
                if ( key == '='){
                    line = pos = 0;
                    start=0;
                    autoindent2(content);
                    line_number = count_line(content);
                    continue;
                }
                //move
                if ( (key == KEY_LEFT) && ( pos > 0)){
                    pos--;
                    // wmove(display,line , pos);
                    // //wprintf(display,'{');
                    // wrefresh(display);
                    continue;
                }
                if ( (key == KEY_RIGHT) && ( pos < chars_in_line(content,line+start) )){
                    pos++;
                    // wmove(display,line , pos);
                    // wrefresh(display);
                    continue;
                }
                if ( (key == KEY_UP)){
                    if ( (start > 0) && (line==3)){
                        if ( chars_in_line(content,line+start-1) < pos ){
                            pos = chars_in_line(content,line+start-1);
                        }
                        start--;
                        continue;
                    }
                    if ( line > 0){
                        if ( chars_in_line(content,line+start-1) < pos ){
                            pos = chars_in_line(content,line+start-1);
                        }
                        line--;
                        continue;
                    }
                }
                if ( key == KEY_DOWN){
                    if ( ( end - max_y + 3 > 0 ) && (line == max_y - 6) ){
                        if ( chars_in_line(content,line+start+1) < pos ){
                            pos = chars_in_line(content,line+start+1);
                        }
                        start++;
                        continue;
                    }
                    if ( (line < max_y -3) && (line < end)){
                        if ( chars_in_line(content,line+start+1) < pos ){
                            pos = chars_in_line(content,line+start+1);
                        }
                        line++;
                        continue;
                    }
                }
                //
                if ( key == ':'){
                    echo();
                    wclear(command);
                    wrefresh(command);
                    wprintw(command , ":");
                    wgetstr(command, main_buffer);
                    strcat(main_buffer,"\n");
                    wclear(OUT);
                    wrefresh(OUT);
                    //phase1
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    
                    for ( int rocket = 0 ; rocket < 1 ; rocket++){
                    ////////////////////////////////////////////////////////////////////////

                    //open
                    input = *(main_buffer + strlen("open "));
                    *(main_buffer + strlen("open "))='\0';
                    if ( strcmp(main_buffer,"open ") == 0){
                        *(main_buffer + strlen("open ")) = input;
                        *(main_buffer+strlen(main_buffer)-1)='\0';

                        if ( strlen(main_buffer+strlen("open ")) < 4){
                            wprintw(OUT , "bad address.try again.\n");
                            wrefresh(OUT);
                            continue;
                        }
                        

                        if ( save_status == '+'){
                            strcat(main_buffer,"\n");
                            createfile(main_buffer+strlen("open "));
                            *(main_buffer+strlen(main_buffer)-1)='\0';
                            file = fopen ( main_buffer+strlen("open ")+1 , "r");
                            if ( file == NULL){
                                wclear(OUT);
                                wprintw(OUT,"bad address.\n");
                                wrefresh(OUT);
                                continue;
                            }

                            strcpy(clocation , main_buffer+strlen("open "));
                            //save_status='-';
                            line_number=0;
                            *content='\0';
                            *buffer2='\0';
                            pos=0;
                            line=0;
                            start=0;
                            while ( fgets(buffer2 , SIZE2 , file) != NULL){
                                //wprintw(display,"line:%s",buffer2);
                                //wprintw(display,"ksfj:%s",(buffer2));
                                //wrefresh(display);
                                strcat(content,buffer2);
                                line_number++;
                            }
                            // for ( int i=0 ; i<strlen(content); i++){
                            //     wprintw(OUT,"%d,",*(content+i));
                            //     wrefresh(OUT);
                            // }
                            fclose(file);
                            //wclear(OUT);
                            wprintw(OUT,"opened the file.\n");
                            wrefresh(OUT);
                            continue;
                        }

                        if ( strlen(clocation) == 0){
                            wclear(OUT);
                            wprintw(OUT,"no address to save current file.\n");
                            wrefresh(OUT);
                            continue;
                        }

                        ///////////save first
                        file = fopen ( clocation+1 , "w" );
                        if ( file == NULL){
                            wclear(OUT);
                            wprintw(OUT,"couldn't save open text.\n");
                            wrefresh(OUT);
                            continue;
                        }
                        fprintf( file , "%s" , content);
                        fclose(file);

                        strcat(main_buffer,"\n");
                        createfile(main_buffer+strlen("open "));
                        *(main_buffer+strlen(main_buffer)-1)='\0';
                        file = fopen ( main_buffer+strlen("open ")+1 , "r");

                        if ( file == NULL){
                            wclear(OUT);
                            wprintw(OUT,"bad address.\n");
                            wrefresh(OUT);
                            continue;
                        }

                        strcpy(clocation , main_buffer+strlen("open "));
                        //save_status='-';
                        line_number=0;
                        *content='\0';
                        *buffer2='\0';
                        save_status='+';
                        pos=0;
                        line=0;
                        start=0;
                        while ( fgets(buffer2 , SIZE2 , file) != NULL){
                            //wprintw(display,"line:%s",buffer2);
                            //wprintw(display,"ksfj:%s",(buffer2));
                            //wrefresh(display);
                            strcat(content,buffer2);
                            line_number++;
                        }
                        if (strlen(content)==0){
                            line_number=1;
                        }
                        wclear(OUT);
                        wprintw(OUT,"opened the file.\n");
                        wrefresh(OUT);
                        fclose(file);

                        // if ( (file = fopen ( main_buffer+strlen("open ")+1 , "w"))  == NULL){
                        //     wprintw(OUT , "bad address.use saveas with valid address.\n");
                        //     wrefresh(OUT);
                        //     continue;
                        // }

                        // strcpy( clocation , main_buffer+strlen("open "));
                        
                        // fprintf( file , "%s" , content);
                        // fclose(file);
                        // save_status = '+';
                        // wprintw(OUT , "saved\n");
                        // wrefresh(OUT);
                                
                        continue;
                    }
                    *(main_buffer + strlen("open ")) = input;

                    //save
                    if ( strcmp(main_buffer , "save\n") == 0){
                        if ( strlen(clocation) == 0){
                            wprintw(Location , "no address.enter the path.'ll delete possible existing content.\n");
                            wrefresh(Location);
                            wclear(command);
                            wprintw(command,"enter path:");
                            wrefresh(command);
                            wgetstr(command,clocation);
                            wclear(command);
                            wrefresh(command);
                            if ( strlen(clocation) < 4){
                                wprintw(OUT , "bad address.use saveas with valid address.\n");
                                wrefresh(OUT);
                                *clocation='\0';
                                continue;
                            }
                            strcat(clocation,"\n");
                            createfile(clocation);
                            *(clocation+strlen(clocation)-1)='\0';

                            if ( (file = fopen ( clocation+1 , "w"))  == NULL){
                                wprintw(OUT , "bad address.use saveas with valid address.\n");
                                wrefresh(OUT);
                                *clocation='\0';
                                continue;
                            }

                            fprintf( file , "%s" , content);
                            fclose(file);
                            save_status = '+';
                            wprintw(OUT , "saved\n");
                            wrefresh(OUT);
                            continue;

                        }
                        if ( (file = fopen ( clocation+1 , "w"))  == NULL){
                           wprintw(OUT , "error: bad address.\n");
                           wrefresh(OUT);
                           continue;
                        }

                        fprintf( file , "%s" , content);
                        fclose(file);
                        save_status = '+';
                        wprintw(OUT , "saved\n");
                        wrefresh(OUT);
                        continue;
                    }

                    //saveas
                    input = *(main_buffer + strlen("saveas "));
                    *(main_buffer + strlen("saveas "))='\0';
                    if ( strcmp(main_buffer,"saveas ") == 0){
                        *(main_buffer + strlen("saveas ")) = input;
                        //*(main_buffer+strlen(main_buffer)-1)='\0';
                        

                        if ( strlen(main_buffer+strlen("saveas ")) < 5){
                            wprintw(OUT , "bad address.use saveas with valid address.\n");
                            wrefresh(OUT);
                            continue;
                        }

                        createfile(main_buffer+strlen("saveas "));
                        *(main_buffer+strlen("saveas ")+strlen(main_buffer+strlen("saveas "))-1)='\0';

                        if ( (file = fopen ( main_buffer+strlen("saveas ")+1 , "w"))  == NULL){
                            wprintw(OUT , "bad address.use saveas with valid address.\n");
                            wrefresh(OUT);
                            continue;
                        }

                        strcpy( clocation , main_buffer+strlen("saveas "));
                        
                        fprintf( file , "%s" , content);
                        fclose(file);
                        save_status = '+';
                        wprintw(OUT , "saved\n");
                        wrefresh(OUT);
                                
                        continue;
                    }
                    *(main_buffer + strlen("saveas ")) = input;

                    ////////////////////////////////////////////////////////////////////////
                    //createfile
                    if ( (sscanf(main_buffer,"createfile %s ",temp) == 1) && ( strcmp(temp,"--file") == 0) && ( strlen(main_buffer) >= 24)){

                        result = createfile(main_buffer+18);

                        //send error to console
                        if ( result == 0){
                            wprintw(OUT,"-error: file already exists. nothing done.\n");
                        }
                        if ( result == 2){
                            wprintw(OUT,"-error: bad address.\n");
                        }
                        //

                        continue;
                    }

                    //cat
                    if ( (sscanf(main_buffer,"cat %s ",temp) == 1) && ( strcmp(temp,"--file") == 0) && ( strlen(main_buffer) >= 18)){

                        char *Result = (char*)malloc(SIZE2);
                        *(Result)='\0';

                        result = cat(main_buffer,Result);

                        //send error to console
                        if ( result == 0){
                            wprintw(OUT,"-error: no such file, or bad address.\n");
                            continue;
                        }


                        //send result to console
                        wprintw(OUT,"%s\n",Result);
                        free(Result);
                        //
                        continue;
                    }

                    //tree
                    if( (sscanf(main_buffer,"tree %d",&inp) == 1) ){

                        char *Result = (char*)malloc(SIZE2);
                        *(Result)='\0';

                        char *temp_add=(char *)malloc(15);
                        strcpy(temp_add,"./root");
                        tree(0,inp,temp_add,Result);
                        free(temp_add);


                        //*(Result+strlen(Result)-1)='\0';
                        //send result to console
                        wprintw(OUT,"%s",Result);
                        free(Result);
                        continue;
                        //

                    }

                    //compare
                    if ( ( (sscanf(main_buffer,"compare %s %s%c",temp,temp2,&input) == 3) && ( input == '\n') ) || ((sscanf(main_buffer,"compare %s \"%[^\"]%*c%c",temp,temp2,&input) == 3) && ( input == '\n') ) || ((sscanf(main_buffer,"compare \"%[^\"]%*c %s%c",temp,temp2,&input) == 3) && ( input == '\n') ) || ((sscanf(main_buffer,"compare \"%[^\"]%*c \"%[^\"]%*c%c",temp,temp2,&input) == 3) && ( input == '\n') )  ){

                        char *Result = (char*)malloc(SIZE2);
                        *(Result)='\0';

                        compare(temp,temp2,Result);

                        
                        //send result to console
                        wprintw(OUT,"%s",Result);
                        free(Result);
                        continue;
                    
                    }

                    //insert
                    if ( ( (sscanf(main_buffer,"insertstr --file %s%c--str%c",temp,&input,&input2)==3) && ((input2==' ') && (input==' '))) || ( (sscanf(main_buffer,"insertstr --file \"%[^\"]%*c%c--str%c",temp,&input,&input2)==3) && ((input==' ') && (input2==' ')) ) ){
                        //temp:valid address
                        int push=0;
                        if ((*(main_buffer+17) != '\"')){
                            push=17+strlen(temp)+7;
                        }
                        else if ((*(main_buffer+17) == '\"')){
                            push=18+strlen(temp)+1+7;
                        }

                        int index=0;
                        for ( int i=(strlen(main_buffer)-2) ; i>=push+2+5 ;i--){
                            if ( ((*(main_buffer+i)) == ' ') && ((*(main_buffer+i-1)) == 's') && ((*(main_buffer+i-2)) == 'o') && ((*(main_buffer+i-3)) == 'p') && ((*(main_buffer+i-4)) == '-') && ((*(main_buffer+i-5)) == '-') && ((*(main_buffer+i-6)) == ' ') ){
                                index=i+1;
                                break;
                            }
                        }
                        int line=0,pos=0;
                        sscanf(main_buffer+index,"%d:%d",&line,&pos);
                        char *text=(char*)malloc(SIZE);
                        *text='\0';
                        for ( int i=0; i<=index-push-8;i++ ){
                            (*(text+i)) = (*(main_buffer+push+i));
                        }
                        (*(text+index-push-7))='\0';

                        if (sscanf(main_buffer+index,"%d:%d",&line,&pos) == 2){
                            ///everything ready : temp , text , line , pos
                            //printf("%s\n%s\n%d:%d\n",temp,text,line,pos);
                            char *Result = (char *)malloc(SIZE2);
                            *Result ='\0';
                            Insert(temp,text,line,pos,Result,1);

                            wprintw(OUT,"%s",Result);
                            free(Result);
                            free(text);
                            continue;
                        }

                        ///
                        
                        free(text);
                    }

                    //remove
                    if ( ((sscanf(main_buffer,"removestr --file %s --pos %d:%d --size %d -%c%c",temp,&int1,&int2,&int3,&input,&input2)==6) && (input2 == '\n') ) || ((sscanf(main_buffer,"removestr --file \"%[^\"]%*c --pos %d:%d --size %d -%c%c",temp,&int1,&int2,&int3,&input,&input2)==6) && (input2 == '\n') )){
                        //printf("%s\n%d:%d   %d\n%c\n",temp,int1,int2,int3,input); ready
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';

                        Remove(temp,int1,int2,int3,input,Result,1);

                        //
                        wprintw(OUT,"%s",Result);
                        free(Result);
                        //
                        continue;
                    }

                    //copy
                    if ( ((sscanf(main_buffer,"copystr --file %s --pos %d:%d --size %d -%c%c",temp,&int1,&int2,&int3,&input,&input2)==6) && (input2 == '\n') ) || ((sscanf(main_buffer,"copystr --file \"%[^\"]%*c --pos %d:%d --size %d -%c%c",temp,&int1,&int2,&int3,&input,&input2)==6) && (input2 == '\n') )){
                        //printf("%s\n%d:%d   %d\n%c\n",temp,int1,int2,int3,input); ready
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';

                        copy(temp,int1,int2,int3,input,Result,clipboard);

                        //
                        wprintw(OUT,"%s",Result);
                        free(Result);
                        //
                        //
                        //printf("clipborad:\n%s\n",clipboard);
                        //
                        continue;
                    }

                    //cut: combinational operation 
                    //cutstr --file <> --pos 1:2 --size 2 -f/b
                    if ( ((sscanf(main_buffer,"cutstr --file %s --pos %d:%d --size %d -%c%c",temp,&int1,&int2,&int3,&input,&input2)==6) && (input2 == '\n') ) || ((sscanf(main_buffer,"cutstr --file \"%[^\"]%*c --pos %d:%d --size %d -%c%c",temp,&int1,&int2,&int3,&input,&input2)==6) && (input2 == '\n') )){
                        //printf("%s\n%d:%d   %d\n%c\n",temp,int1,int2,int3,input); ready
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';

                        copy(temp,int1,int2,int3,input,Result,clipboard);
                        Remove(temp,int1,int2,int3,input,Result,1);

                        //
                        wprintw(OUT,"%s",Result);
                        free(Result);
                        //
                        //
                        //printf("clipborad:\n%s\n",clipboard);
                        //
                        continue;
                    }

                    //paste: combinational operation
                    //pastestr --file <> --pos 1:2
                    if ( ((sscanf(main_buffer,"pastestr --file %s --pos %d:%d%c",temp,&int1,&int2,&input)==4) && (input == '\n') ) || ((sscanf(main_buffer,"pastestr --file \"%[^\"]%*c --pos %d:%d%c",temp,&int1,&int2,&input)==4) && (input == '\n') )){
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';

                        Insert(temp,clipboard,int1,int2,Result,1);

                        wprintw(OUT,"%s",Result);
                        free(Result);
                        continue;
                    }

                    //grep
                    // if ( sscanf(main_buffer,"grep --str %s --files %[^\n]%*c",temp,temp2) == 2 ){
                    //     char *Result = ( char *)malloc(SIZE2);
                    //     *Result = '\0';

                    //     Grep(temp,temp2,Result);

                    //     printf("%s",Result);
                    //     free(Result);
                    //     continue;
                    // }

                    //grep
                    input= *(main_buffer + strlen("grep --str "));
                    *(main_buffer + strlen("grep --str ")) = '\0';
                    if ( strcmp(main_buffer,"grep --str ") == 0){
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';
                        *(main_buffer + strlen("grep --str ")) = input;
                        int index=-1;
                        for ( int i = strlen(main_buffer)- 12 ; i >= 0 ; i--){
                            if (index != -1){break;}
                            input2 = *(main_buffer+i+8);
                            *(main_buffer+i+8)= '\0';
                            if ( strcmp( main_buffer + i, "--files ") == 0){
                                *(main_buffer+i+8)= input2;
                                if ( d_no(main_buffer+i+8) != 0){
                                    continue;
                                }
                                index = i+8;
                                input3 = *(main_buffer+i-1);
                                *(main_buffer+i -1) = '\0';
                                
                                Grep(main_buffer+strlen("grep --str "),main_buffer+index,Result);
                                //
                                *(main_buffer+i-1)=input3;
                                *(main_buffer)='\0';
                                printf("%s",Result);
                                break;

                            }
                            *(main_buffer+i+8)= input2;
                        }
                        if ( *Result != '\0'){free(Result);continue;}
                        free(Result);
                    }
                    *(main_buffer + strlen("grep --str ")) = input;
                    /////////////////////////////////////////////////////////
                    //grep -c
                    input= *(main_buffer + strlen("grep -c --str "));
                    *(main_buffer + strlen("grep -c --str ")) = '\0';
                    if ( strcmp(main_buffer,"grep -c --str ") == 0){
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';
                        *(main_buffer + strlen("grep -c --str ")) = input;
                        int index=-1;
                        for ( int i = strlen(main_buffer)- 12 ; i >= 0 ; i--){
                            if (index != -1){break;}
                            input2 = *(main_buffer+i+8);
                            *(main_buffer+i+8)= '\0';
                            if ( strcmp( main_buffer + i, "--files ") == 0){
                                *(main_buffer+i+8)= input2;
                                if ( d_no(main_buffer+i+8) != 0){
                                    continue;
                                }
                                index = i+8;
                                input3 = *(main_buffer+i-1);
                                *(main_buffer+i -1) = '\0';
                                
                                Grep2(main_buffer+strlen("grep -c --str "),main_buffer+index,Result);
                                //
                                *(main_buffer)='\0';
                                *(main_buffer+i-1)=input3;
                                wprintw(OUT,"%s",Result);
                                break;

                            }
                            *(main_buffer+i+8)= input2;
                        }
                        if ( *Result != '\0'){free(Result);continue;}
                        free(Result);
                    }
                    *(main_buffer + strlen("grep -c --str ")) = input;
                    /////////////////////
                    //grep -l
                    input= *(main_buffer + strlen("grep -l --str "));
                    *(main_buffer + strlen("grep -l --str ")) = '\0';
                    if ( strcmp(main_buffer,"grep -l --str ") == 0){
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';
                        *(main_buffer + strlen("grep -l --str ")) = input;
                        int index=-1;
                        for ( int i = strlen(main_buffer)- 12 ; i >= 0 ; i--){
                            if (index != -1){break;}
                            input2 = *(main_buffer+i+8);
                            *(main_buffer+i+8)= '\0';
                            if ( strcmp( main_buffer + i, "--files ") == 0){
                                *(main_buffer+i+8)= input2;
                                if ( d_no(main_buffer+i+8) != 0){
                                    continue;
                                }
                                index = i+8;
                                input3 = *(main_buffer+i-1);
                                *(main_buffer+i -1) = '\0';
                                
                                Grep3(main_buffer+strlen("grep -l --str "),main_buffer+index,Result);
                                //
                                *(main_buffer)='\0';
                                *(main_buffer+i-1)=input3;
                                wprintw(OUT,"%s",Result);
                                break;

                            }
                            *(main_buffer+i+8)= input2;
                        }
                        if ( *Result != '\0'){free(Result);continue;}
                        free(Result);
                    }
                    *(main_buffer + strlen("grep -l --str ")) = input;

                    //autoindent
                    if ( ((sscanf(main_buffer,"auto-indent --file %[^'\n']%*c",temp)==1) && (*temp != '\"')) || ((sscanf(main_buffer,"auto-indent --file \"%[^\n]%*c",temp) == 1 )&&( *(temp+strlen(temp)-1) == '\"'))){
                        if (*(temp+strlen(temp)-1) == '\"'){
                            *(temp+strlen(temp)-1) = '\0';
                        }
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';

                        autoindent(temp+1 , Result);

                        if ( *Result != '\0'){
                            wprintw(OUT,"%s",Result);
                        }
                        free(Result);
                        continue;
                    }

                    //undo
                    if ( ((sscanf(main_buffer,"undo --file %[^'\n']%*c",temp)==1) && (*temp != '\"')) || ((sscanf(main_buffer,"undo --file \"%[^\n]%*c",temp) == 1 )&&( *(temp+strlen(temp)-1) == '\"'))){
                        if (*(temp+strlen(temp)-1) == '\"'){
                            *(temp+strlen(temp)-1) = '\0';
                        }
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';

                        undo(temp+1 , Result);

                        if ( *Result != '\0'){
                            wprintw(OUT,"%s",Result);
                        }
                        free(Result);
                        continue;
                    }
                    //find
                    input= *(main_buffer + strlen("find --str "));
                    *(main_buffer + strlen("find --str ")) = '\0';
                    if ( strcmp(main_buffer,"find --str ") == 0){
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';
                        *(main_buffer + strlen("find --str ")) = input;
                        int index=-1;
                        for ( int i = strlen(main_buffer)- 11 ; i >= 0 ; i--){
                            if (index != -1){break;}
                            input2 = *(main_buffer+i+7);
                            *(main_buffer+i+7)= '\0';
                            if ( strcmp( main_buffer + i, "--file ") == 0){
                                *(main_buffer+i+7)= input2;
                                if ( d_no(main_buffer+i+7) != 0){
                                    continue;
                                }
                                index = i+7;
                                input3 = *(main_buffer+i-1);
                                *(main_buffer+i -1) = '\0';
                                //printf("%s-%s\n",main_buffer+strlen("find --str "),main_buffer+index);
                                find(main_buffer+strlen("find --str "),main_buffer+index,Result);
                                //printf("data: %s, %s",main_buffer+strlen("find --str "), main_buffer + index);
                                //
                                *(main_buffer+i-1)=input3;
                                *(main_buffer)='\0';
                                wprintw(OUT,"%s",Result);
                                break;

                            }
                            *(main_buffer+i+7)= input2;
                        }
                        if ( *Result != '\0'){free(Result);continue;}
                        free(Result);
                    }
                    *(main_buffer + strlen("find --str ")) = input;

                    //replace
                    input= *(main_buffer + strlen("replace --str1 "));
                    *(main_buffer + strlen("replace --str1 ")) = '\0';
                    if ( strcmp(main_buffer,"replace --str1 ") == 0){
                        char *Result = (char *)malloc(SIZE2);
                        *Result ='\0';
                        *(main_buffer + strlen("replace --str1 ")) = input;
                        int index=-1;
                        for ( int i = strlen(main_buffer)- 11 ; i >= 0 ; i--){
                            if (index != -1){break;}
                            input2 = *(main_buffer+i+7);
                            *(main_buffer+i+7)= '\0';
                            if ( strcmp( main_buffer + i, "--file ") == 0){
                                *(main_buffer+i+7)= input2;
                                if ( d_no(main_buffer+i+7) != 0){
                                    continue;
                                }
                                index = i+7;
                                input3 = *(main_buffer+i-1);
                                *(main_buffer+i -1) = '\0';

                                if (( *(main_buffer+strlen("replace --str1 ")) != '\"')   && ( sscanf( main_buffer + strlen("replace --str1 ") , "%s " , temp ) == 1) && ( strlen(main_buffer+strlen("replace --str1 ")) >= 10)){

                                    *(main_buffer+strlen("replace --str1 ") + strlen(temp)) = '\0';
                                    input4 = *(main_buffer+strlen("replace --str1 ") + strlen(temp) + 7 ) ;
                                    *(main_buffer+strlen("replace --str1 ") + strlen(temp) + 7 ) = '\0';
                                    if ( strcmp((main_buffer+strlen("replace --str1 ") + strlen(temp)) , "--str2")){
                                        *(main_buffer+strlen("replace --str1 ") + strlen(temp) + 7 ) = input4 ;
                                        //*(main_buffer+strlen("replace --str1 "))
                                        //*(main_buffer+strlen("replace --str1 ")+strlen(temp)+8)
                                        //*(main_buffer+index)
                                        //replace(...)
                                        replace((main_buffer+strlen("replace --str1 ")),(main_buffer+strlen("replace --str1 ")+strlen(temp)+8),main_buffer+index,Result);
                                        *(main_buffer+i-1)=input3;
                                        *(main_buffer)='\0';
                                        wprintw(OUT,"%s",Result);
                                        break;
                                    }
                                    *(main_buffer+strlen("replace --str1 ") + strlen(temp) + 7 ) = input4 ;
                                }

                                if ( (*(main_buffer+strlen("replace --str1 ")) == '\"') && ( strlen(main_buffer+strlen("replace --str1 ")) >= 10)){
                                    int end=-1;
                                    for ( int i=1 ; i<strlen(main_buffer+strlen("replace --str1 ")) ; i++){
                                        if ( (*(main_buffer+strlen("replace --str1 ")+i) == '\"') && ((*(main_buffer+strlen("replace --str1 ")+i-1) != '\\')) ){
                                            end=i;
                                            break;
                                        }
                                    }
                                    if ( end != -1){
                                        *(main_buffer+strlen("replace --str1 ")+end) = '\0';
                                        *(main_buffer+strlen("replace --str1 ")) = '\0';
                                        replace(main_buffer+strlen("replace --str1 ")+1 ,main_buffer+strlen("replace --str1 ")+strlen("--str2")+end+3 ,main_buffer+index , Result);
                                    }


                                }
                                //printf("%s-%s\n",main_buffer+strlen("replace --str1 "),main_buffer+index);
                                //replace(main_buffer+strlen("replace --str1 "),main_buffer+index,Result);
                                //printf("data: %s, %s",main_buffer+strlen("find --str "), main_buffer + index);
                                //
                                *(main_buffer+i-1)=input3;
                                *(main_buffer)='\0';
                                wprintw(OUT,"%s",Result);
                                break;

                            }
                            *(main_buffer+i+7)= input2;
                        }
                        if ( *Result != '\0'){free(Result);continue;}
                        free(Result);
                    }
                    *(main_buffer + strlen("replace --str1 ")) = input;



                    //invalid command to console
                    wprintw(OUT,"-invalid command\n");
                    //
                    }
                    wrefresh(OUT);
                    continue;
                }
                
            }
            continue;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if ( Mode == 1){
            //insert
            wprintw(mode,"INSERT");
            wrefresh(mode);
            while(1){
                noecho();
                //////location
                wclear(Location);
                wprintw(Location,"%s",clocation);
                wrefresh(Location);
                ///////save status
                wprintw(save , "%c", save_status);
                wrefresh(save);
                ///////////update file
                end = line_number-1-start;
                wclear(display);
                wprintw(display,"%s" , content+index_of_line(content,start));
                wrefresh(display);
                //wprintw(command,"%d:",(int)strlen(content));

                wattron(left,COLOR_PAIR(1));
                wclear(left);
                //if (strlen(content)>0){
                    for ( int i=0 ; (i<end+1) && (i<max_y-2) ; i++){
                        wprintw(left,"%4d",(i+1+start)%10000);
                    }
                //}
                wattron(left,COLOR_PAIR(2));
                for(int i=end+1 ; i<max_y-2 ; i++){
                    wprintw(left,"%-4c",'~');
                }
                
                wrefresh(left);
                //////////////////////////////

                wclear(command);
                wprintw(command , "%d:%d", line+start+1 , pos);
                wrefresh(command);

                wmove(display,line , pos);
                wrefresh(display);
                key = wgetch(display);

                //change mode
                if ( key == KEY_END){
                    Mode = 0;
                    break;    
                }

                //move
                if ( (key == KEY_LEFT) && ( pos > 0)){
                    pos--;
                    // wmove(display,line , pos);
                    // //wprintf(display,'{');
                    // wrefresh(display);
                    continue;
                }
                if ( (key == KEY_RIGHT) && ( pos < chars_in_line(content,line+start) )){
                    pos++;
                    // wmove(display,line , pos);
                    // wrefresh(display);
                    continue;
                }
                if((key == KEY_RIGHT) || (key == KEY_LEFT)){
                    continue;
                }
                if ( (key == KEY_UP)){
                    if ( (start > 0) && (line==3)){
                        if ( chars_in_line(content,line+start-1) < pos ){
                            pos = chars_in_line(content,line+start-1);
                        }
                        start--;
                        continue;
                    }
                    if ( line > 0){
                        if ( chars_in_line(content,line+start-1) < pos ){
                            pos = chars_in_line(content,line+start-1);
                        }
                        line--;
                        continue;
                    }
                    continue;
                }
                if ( key == KEY_DOWN){
                    if ( ( end - max_y + 3 > 0 ) && (line == max_y - 6) ){
                        if ( chars_in_line(content,line+start+1) < pos ){
                            pos = chars_in_line(content,line+start+1);
                        }
                        start++;
                        continue;
                    }
                    if ( (line < max_y -3) && (line < end)){
                        if ( chars_in_line(content,line+start+1) < pos ){
                            pos = chars_in_line(content,line+start+1);
                        }
                        line++;
                        continue;
                    }
                    continue;
                }
                //insert
                //if ( key == Enter)
                save_status='-';
                if ( key == KEY_BACKSPACE){
                    Index=index_of_line(content , line+start);
                    // for ( int i=0 ; i<line+start ; i++){
                    //     Index += (chars_in_line(content , i)+1);
                    // }
                    Index += pos;
                    if ( pos>0){
                        pos--;
                    }
                    else{
                        if ((line >= max_y-6) && (start>0)){
                            start--;
                            pos = chars_in_line(content,line+start);
                            line_number--;
                        }
                        else if( line+start>0){
                            pos = chars_in_line(content,line+start-1);
                            line--;
                            line_number--;
                        }
                    }
                    remove_char(content , Index-1);
                    continue;
                }
                if ( key == '\t'){
                    key = ' ';
                }

                Index=0;
                Index=index_of_line(content , line+start);
                // for ( int i=0 ; i<line+start ; i++){
                //     Index += (chars_in_line(content , i)+1);
                // }
                Index += pos;
                insert_char(content,key,Index);
                if ( key == '\n'){
                    if ( (line < max_y-6)  || (line_number < max_y -2)){
                        line++;
                        pos=0;
                        line_number++;
                        continue;
                    }
                    else{
                        pos=0;
                        start++;
                        line_number++;
                        continue;
                    }
                    continue;
                }
                pos++;
            }
            continue;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if ( Mode == 2){
            //visual
            wprintw(mode,"VISUAL");
            wrefresh(mode);
            while(1){
                noecho();
                //////location
                wclear(Location);
                wprintw(Location,"%s",clocation);
                wrefresh(Location);
                ///////save status
                wprintw(save , "%c", save_status);
                wrefresh(save);
                ///////////update file
                end = line_number-1-start;
                wclear(display);
                wprintw(display,"%s" , content+index_of_line(content,start));
                wrefresh(display);
                //wprintw(command,"%d:",(int)strlen(content));

                wattron(left,COLOR_PAIR(1));
                wclear(left);
                //if (strlen(content)>1){
                    for ( int i=0 ; (i<end+1) && (i<max_y-2) ; i++){
                        wprintw(left,"%4d",(i+1+start)%10000);
                    }
                //}
                wattron(left,COLOR_PAIR(2));
                for(int i=end+1 ; i<max_y-2 ; i++){
                    wprintw(left,"%-4c",'~');
                }
                
                wrefresh(left);
                //////////////////////////////

                wclear(command);
                wprintw(command , "%d:%d", line+start+1 , pos);
                wrefresh(command);

                wmove(display,line , pos);
                wrefresh(display);
                key = wgetch(display);

                //change mode
                if ( key == KEY_END){
                    Mode = 0;
                    break;    
                }

                
            }
            continue;
        }
    }

    endwin();
}