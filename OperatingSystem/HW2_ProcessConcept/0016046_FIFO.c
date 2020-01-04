#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include<unistd.h>
#include<fcntl.h>

#define BUFFER_SIZE 6

int main(int argc, char* argv[])
{
    srand(time(NULL));

    /* Standard I/O */
	FILE* ParentFile;
	FILE* ChildFile;
	char ParentFileName[50]={0};
	char ChildFileName[50]={0};

    /* FIFO */
	const char fifo[5]="fifo";
    int fd;
    mkfifo(fifo, 0666);

    char buff[BUFFER_SIZE]={0};

    /* Game */
    int gameover=0;
    /* T=0 -> empty
       T=1 -> full  */
    int T[52][52]={0};
    int i;
    for(i=0; i<52; i++)
    {
        T[ 0][ i]=1;
        T[51][ i]=1;
        T[ i][ 0]=1;
        T[ i][51]=1;
    }
    int x;
    int y;
    int space[4];
    /* space[4]={ T[x]  [y-1] up
                , T[x+1][y]   right
                , T[x]  [y+1] down
                , T[x-1][y]   left}; */

	pid_t cpid;
    cpid=fork();

	if(cpid<0)
	{
	}
	else if(cpid==0)
	{   /* Child  Process -> Player 2 */
        /* STD I/O */
        sprintf(ChildFileName, "%d_FIFO.txt", getpid());
        ChildFile=fopen(ChildFileName, "w");
        /* STD I/O: F=0 */
        fprintf(ChildFile, "0\n");

        while(!gameover)
        {
            /* FIFO: pop Parent movement */
            fd=open(fifo, O_RDONLY);
            read(fd, buff, sizeof(char)*BUFFER_SIZE);
            close(fd);

            if(strlen(buff)==1)
            {   /* Parent Lose */
                gameover=1;
                /* STD I/O: W=1 */
                fprintf(ChildFile, "1\n");

                //printf("Child WIN.\n");
                break;
            }
            else
            {
                char* xc=strtok(buff, " ");
                char* yc=strtok(NULL, " ");
                x=atoi(xc);
                y=atoi(yc);
            }

            T[x][y]=1;

            space[0]=T[x][y-1];
            space[1]=T[x+1][y];
            space[2]=T[x][y+1];
            space[3]=T[x-1][y];

            if(space[0]+space[1]+space[2]+space[3]!=4)
            {   /* spaces are not all full */
                int i;
                /* until the chosen space is empty */
                for(i=rand()%4; space[i]!=0; i=rand()%4);

                if(i==0){
                    x=x;
                    y=y-1;
                }
                else if(i==1){
                    x=x+1;
                    y=y;
                }
                else if(i==2){
                    x=x;
                    y=y+1;
                }
                else{
                    x=x-1;
                    y=y;
                }

                T[x][y]=1;

                //printf("Child  turn : ");
                /* FIFO: push Child movement */
                fd=open(fifo, O_WRONLY);
                sprintf(buff, "%d %d", x,y);
                write(fd, buff, sizeof(char)*(strlen(buff)+1));
                close(fd);
                /* STD I/O: x y */
                fprintf(ChildFile,"%d %d\n", x,y);

                //printf("%d %d\n", x,y);
            }
            else
            {   /* spaces are all full
                   Child Lose          */
                gameover=1;

                /* FIFO: push W=0 */
                fd=open(fifo, O_WRONLY);
                sprintf(buff, "0");
                write(fd, buff, sizeof(char)*(strlen(buff)+1));
                close(fd);
                /* STD I/O: W=0 */
                fprintf(ChildFile,"0\n");

                //printf("Child LOSE.\n");
                break;
            }
        }

        fclose(ChildFile);
	}
	else
    {   /* Parent Process -> Player 1 */
        /* STD I/O */
        sprintf(ParentFileName,"%d_FIFO.txt",getpid());
        ParentFile=fopen(ParentFileName,"w");
        /* STD I/O: F=1 */
        fprintf(ParentFile,"1\n");

        x=rand()%50;
        y=rand()%50;
        T[x][y]=1;

        //printf("Parent turn : ");
        // FIFO: push Parent 1st movement
        fd=open(fifo, O_WRONLY);
        sprintf(buff, "%d %d", x,y);
        write(fd, buff, sizeof(char)*(strlen(buff)+1));
        close(fd);
        /* STD I/O: x y */
        fprintf(ParentFile,"%d %d\n",x,y);

        //printf("%d %d\n",x,y);

        while(!gameover)
        {
            /* FIFO: pop Child movement */
            fd=open(fifo, O_RDONLY);
            read(fd, buff, sizeof(char)*BUFFER_SIZE);
            close(fd);

            if(strlen(buff)==1)
            {   /* Child Lose */
                gameover=1;
                /* STD I/O: W=1 */
                fprintf(ParentFile,"1\n");

                //printf("Parent WIN.\n");
                break;
            }
            else
            {
                char* xc=strtok(buff, " ");
                char* yc=strtok(NULL, " ");
                x=atoi(xc);
                y=atoi(yc);
            }

            T[x][y]=1;

            space[0]=T[x][y-1];
            space[1]=T[x+1][y];
            space[2]=T[x][y+1];
            space[3]=T[x-1][y];

            if(space[0]+space[1]+space[2]+space[3]!=4)
            {   /* spaces are not all full */
                int i;
                /* until the chosen space is empty */
                for(i=rand()%4; space[i]!=0; i=rand()%4);

                if(i==0){
                    x=x;
                    y=y-1;
                }
                else if(i==1){
                    x=x+1;
                    y=y;
                }
                else if(i==2){
                    x=x;
                    y=y+1;
                }
                else{
                    x=x-1;
                    y=y;
                }

                T[x][y]=1;

                //printf("Parent turn : ");
                /* FIFO: push Parent movement */
                fd=open(fifo, O_WRONLY);
                sprintf(buff, "%d %d", x,y);
                write(fd, buff, sizeof(char)*(strlen(buff)+1));
                close(fd);
                /* STD I/O: x y */
                fprintf(ParentFile,"%d %d\n", x,y);

                //printf("%d %d\n", x,y);
            }
            else
            {   /* spaces are all full
                   Parent Lose         */
                gameover=1;
                /* FIFO: push W=0 */
                fd=open(fifo, O_WRONLY);
                sprintf(buff,"0");
                write(fd, buff, sizeof(char)*(strlen(buff)+1));
                close(fd);
                /* STD I/O: W=0 */
                fprintf(ParentFile,"0\n");

                //printf("Parent LOSE.\n");
                break;
            }
        }

        fclose(ParentFile);

		remove(fifo);
	}

    return 0;
}
