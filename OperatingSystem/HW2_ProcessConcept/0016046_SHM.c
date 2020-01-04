#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>

int main(int argc, char* argv[])
{
    srand(time(NULL));

    /* Standard I/O */
    FILE* ParentFile;
	FILE* ChildFile;
	char ParentFileName[50]={0};
	char ChildFileName[50]={0};


    /* Shared Memory */
    int shm;
    shm=shmget(IPC_PRIVATE, 5*sizeof(int), IPC_CREAT|0666);

    int* buff;
    /* buff[0]=x-axis
       buff[1]=y-axis
       buff[2]=W-Flag
       buff[3]=Parent Movement Count
       buff[4]=Child  Movement Count */

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
        sprintf(ChildFileName, "%d_SHM.txt", getpid());
        ChildFile=fopen(ChildFileName, "w");
        /* STD I/O: F=0 */
        fprintf(ChildFile, "0\n");

        while(!gameover)
        {
            /* SHM: pop Parent movement */
            buff=(int*)shmat(shm, 0, 0);
            while(buff[3]==0);
            buff[0]=buff[0];
            buff[1]=buff[1];
            buff[2]=buff[2];
            buff[3]--;
            buff[4]=buff[4];

            if(buff[2]==0)
            {   /* Parent Lose */
                gameover=1;
                /* STD I/O: W=1 */
                fprintf(ChildFile,"1\n");

                //printf("Child WIN.\n");
                shmdt(buff);
                break;
            }
            else
            {
                x=buff[0];
                y=buff[1];
                shmdt(buff);
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
                /* SHM: push Child movement */
                buff=(int*)shmat(shm, 0, 0);
                while(buff[4]==1);
                buff[0]=x;
                buff[1]=y;
                buff[2]=2;
                buff[3]=buff[3];
                buff[4]++;
                shmdt(buff);
                /* STD I/O: x y */
                fprintf(ChildFile,"%d %d\n", x,y);

                //printf("%d %d\n", x,y);
            }
            else
            {   /* spaces are all full
                   Child Lose          */
                gameover=1;
                /* SHM: push W=0 */
                buff=(int*)shmat(shm, 0, 0);
                while(buff[4]==1);
                buff[0]=0;
                buff[1]=0;
                buff[2]=0;
                buff[3]=buff[3];
                buff[4]++;
                shmdt(buff);
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
        sprintf(ParentFileName,"%d_SHM.txt",getpid());
        ParentFile=fopen(ParentFileName,"w");
        /* STD I/O: F=1 */
        fprintf(ParentFile,"1\n");

        x=rand()%50;
        y=rand()%50;
        T[x][y]=1;

        //printf("Parent turn : ");
        // SHM: push Parent 1st movement
        buff=(int*)shmat(shm, 0, 0);
        buff[0]=x;
        buff[1]=y;
        buff[2]=2;
        buff[3]=1;
        buff[4]=0;
        shmdt(buff);
        /* STD I/O: x y */
        fprintf(ParentFile,"%d %d\n",x,y);

        //printf("%d %d\n",x,y);

        while(!gameover)
        {
            /* SHM: pop Child movement */
            buff=(int*)shmat(shm, 0, 0);
            while(buff[4]==0);
            buff[0]=buff[0];
            buff[1]=buff[1];
            buff[2]=buff[2];
            buff[3]=buff[3];
            buff[4]--;

            if(buff[2]==0)
            {   /* Child Lose */
                gameover=1;
                /* STD I/O: W=1 */
                fprintf(ParentFile,"1\n");

                //printf("Parent WIN.\n");
                shmdt(buff);
                break;
            }
            else
            {
                x=buff[0];
                y=buff[1];
                shmdt(buff);
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
                /* SHM: push Parent movement */
                buff=(int*)shmat(shm, 0, 0);
                while(buff[3]==1);
                buff[0]=x;
                buff[1]=y;
                buff[2]=2;
                buff[3]++;
                buff[4]=buff[4];
                shmdt(buff);
                /* STD I/O: x y */
                fprintf(ParentFile,"%d %d\n", x,y);

                //printf("%d %d\n", x,y);
            }
            else
            {   /* spaces are all full
                   Parent Lose         */
                gameover=1;
                /* SHM: push W=0 */
                buff=(int*)shmat(shm, 0, 0);
                while(buff[3]==1);
                buff[0]=0;
                buff[1]=0;
                buff[2]=0;
                buff[3]++;
                buff[4]=buff[4];
                shmdt(buff);
                /* STD I/O: W=0 */
                fprintf(ParentFile,"0\n");

                //printf("Parent LOSE.\n");
                break;
            }
        }

        fclose(ParentFile);

		shmctl(shm, IPC_RMID, 0);
	}

    return 0;

}
