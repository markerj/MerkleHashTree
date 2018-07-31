/*
 * John Marker
 * Merkle Hash Tree
 * 7/25/2018
*/

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

int numfiles=0;
char hashes[500][50];
char tophashes[][50];
int numtophashes =0;

char * readFile(char *);
void hash(char *);
void tree(char[][50], int);

struct node {
    char data[50];
    struct node *right;
    struct node *left;
};

int main()
{
    int userInput;
    char *filenames[500];

    //give user instructions and get user input
    while(1)
    {
        printf("         Merkle Hash Tree\n");
        printf("-------------------------------------\n");
        printf("         1: Add new file(s)\n");
        printf("         2: Compute top hash\n");
        printf("         3: Start new tree\n");
        printf("         4: Exit\n");
        printf("--------------------------------------\n");
        scanf("%d", &userInput);

        if(userInput==1)
        {
            printf("Enter file names separated by commas (e.g. file1.txt,file2.txt,file3.txt)\n");
            fflush(stdin);
            int length=0;
            char buffer[2000];
            char *aux;
            scanf("%s",&buffer);
            aux=strtok(buffer, ",");
            while(aux)
            {
                filenames[length]= aux;
                length++;
                aux=strtok(NULL, ",");
            }
            for(int i=0; i<length; i++)
            {
                //read contents of file into filedata
                char *filedata = readFile(filenames[i]);

                //hash filedata
                if(strcmp(filedata,"") != 0)
                {
                    hash(filedata);
                }
            }
        }
        if(userInput==2)
        {
            tree(hashes,numfiles);
            numtophashes++;
        }
        if(userInput==3)
        {
            int i = 0;
            printf("Removing hashes\n");
            while(i < numfiles)
            {
                // printf("Removing the following hash: %s\n", hashes[i]);
                memset(hashes[i],'\0',50);
                i++;
            }
            numfiles = 0;
        }
        if(userInput==4)
        {
            break;
        }
    }
    return 0;
}

//store the contents of a file in a buffer
char * readFile(char *filename)
{
    fflush(stdin);
    if( access( filename, F_OK ) == -1 )
    {
        printf("File doesn't exist\n");
        return "";
    }

    char *source = NULL;
    FILE *fp = fopen(filename, "rt");
    /* Go to the end of the file. */
    if (fseek(fp, 0L, SEEK_END) == 0)
    {
        long bufsize = ftell(fp);
        if (bufsize == -1) {  }
        source = malloc(sizeof(char) * (bufsize + 1));
        if (fseek(fp, 0L, SEEK_SET) != 0) {  }
        size_t newLen = fread(source, sizeof(char), bufsize, fp);
        if ( ferror( fp ) != 0 )
        {
            fputs("Error reading file", stderr);
        }
        else
        {
            source[newLen++] = '\0';
        }
    }
    fclose(fp);
    return source;
}

//compute the has value of the given file content
void hash(char *filedata)
{
    char hashedfile[50];
    char hexhashedfile[50];
    SHA1(hashedfile,filedata,strlen(filedata));
    int i;

    printf("Adding the following hash: ");
    for(i=0; i<20; i++)
    {
        printf("%02x", (int) hashedfile[i]&0xff);
        sprintf(hexhashedfile + (2*i),"%02x", hashedfile[i]&0xff);
    }
    printf("\n");
    numfiles++;
    strcpy(hashes[numfiles-1],hexhashedfile);
}

//recursively concatenate and hash provided hashes at incremental depths of the tree.
void tree(char hashes_param[][50], int numnodes)
{
    char hashcombined[200][50];
    int count = 0;
    printf("In tree function\n");
    if(numnodes==0)
    {
        printf("There are no files to hash\n");
        return 1;
    }
    if(numnodes==1)
    {
        printf("Top Hash is: %s\n", hashes_param[0]);
        strcpy(tophashes[numtophashes], hashes_param[0]);
        return 1;
    }

    int i = 0;
    while(i<numnodes)
    {
        if(i == numnodes -1)
        {
            //odd one out
            strcpy(hashcombined[count],hashes_param[numnodes-1]);
            count++;

            printf("Odd hash out: %s\n",hashes_param[numnodes-1]);
            char hashedresult[50];
            char hexhashedresult[50];
            SHA1(hashedresult,hashes_param[numnodes-1],strlen(hashes_param[numnodes-1]));
            fflush(stdin);
            for(int i=0; i<20; i++)
            {
                sprintf(hexhashedresult + (2*i),"%02x", hashedresult[i]&0xff);
            }
            printf("\n");
            return tree(hashcombined,count);
        }
        else
        {
            char *result = malloc(strlen(hashes_param[i]) + strlen(hashes_param[i+1]) + 1);
            strcpy(result, hashes_param[i]);
            strcat(result, hashes_param[i+1]);
            printf("Concatenated hash: %s",result);

            char hashedresult[50];
            char hexhashedresult[50];
            SHA1(hashedresult,result,strlen(result));
            fflush(stdin);
            for(int i=0; i<20; i++)
            {
                sprintf(hexhashedresult + (2*i),"%02x", hashedresult[i]&0xff);
            }
            printf("\n");
            printf("Hashed concatenated hash: %s\n",hexhashedresult);
            strcpy(hashcombined[count],hexhashedresult);
            count++;
            i+=2;
        }

    }
    printf("\n");
    return tree(hashcombined,count);
}
