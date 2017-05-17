
#include "Prac4.h"
#include <cmath>

void insertionsort(int *tempArray)
{
  int u,v,temp;
  temp = 0;
  for (u=3; u<243+3; u+=3)
  {
    v = u;
    while (v > 0 && tempArray[v-3] > tempArray[v])
    {
      temp = tempArray[v-3];
      tempArray[v - 3] = tempArray[v];
      tempArray[v] = temp;
      v-=3;
    }
    v = u+1;
    while (v > 1 && tempArray[v-3] > tempArray[v])
    {
      temp = tempArray[v-3];
      tempArray[v - 3] = tempArray[v];
      tempArray[v] = temp;
      v-=3;
    }
    v = u+2;
    while (v > 2 && tempArray[v-3] > tempArray[v])
    {
      temp = tempArray[v-3];
      tempArray[v - 3] = tempArray[v];
      tempArray[v] = temp;
      v-=3;
    }
  }
}

void Master(){
 int  j;
 int returnVal[1];  //Simple single entry Return Buffer
 int sendVal[3];    //Simple 3-entry Send Buffer (Segment Size, X Width, Y Height)
 MPI_Status stat;
 if(!Input.Read("Data/greatwall.jpg")){
  printf("Cannot read image\n");
  return;
 }
 if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return;
 int yDivident = std::floor((float)(Input.Height)/(numprocs-1));
 int totalSize = Input.Height;
 char buff [yDivident* Input.Width * Input.Components];
 char byteBuf[yDivident][Input.Width * Input.Components];
 for(j = 1; j < numprocs; j++){
  sendVal[0] = (j < numprocs-1 ? yDivident * Input.Width * Input.Components: totalSize * Input.Width * Input.Components);
  sendVal[1] = Input.Width * Input.Components;
  sendVal[2] = yDivident;

  int k =0;
  int p =0;
  int bufCount = 0;
  for (k = yDivident*(j-1); k < yDivident*(j);k+=1){
    for (p = 0;p < Input.Width * Input.Components;p+=3){
        byteBuf[bufCount][p + 0] = Input.Rows[k][p + 0];
        byteBuf[bufCount][p + 1] = Input.Rows[k][p + 1];
        byteBuf[bufCount][p + 2] = Input.Rows[k][p + 2];

    }bufCount+=1;

  }
  printf("0: We have %d slaves\n", numprocs-1);
  MPI_Send(sendVal, 3, MPI_INT, j, TAG, MPI_COMM_WORLD);          //Send Dimensions
  MPI_Recv(returnVal, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat); //Get Response - Prevents blocking
  MPI_Send(byteBuf, yDivident * Input.Width * Input.Components, MPI_CHAR, j, TAG, MPI_COMM_WORLD); //Send Data
  MPI_Recv(returnVal, 1, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat); //Get Resonse
  printf("0: Slave %d started\n", j);
  totalSize-=yDivident;
 }
 for(j = 1; j < numprocs; j++){
  MPI_Recv(byteBuf, yDivident * Input.Width * Input.Components, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat); //Ger Result
  printf("0: Slave %d Finished\n", j);
  int k =0;
  int p =0;
  int bufCount = 0;
  for (k = yDivident*(j-1); k < yDivident*(j);k+=1){  //Recombine
    for (p = 0;p < Input.Width * Input.Components;p+=3){
      Output.Rows[k][p + 0] = byteBuf[bufCount][p + 0];
      Output.Rows[k][p + 1] = byteBuf[bufCount][p + 1];
      Output.Rows[k][p + 2] = byteBuf[bufCount][p + 2];

    }bufCount+=1;
  }


  printf("0: Slave %d Reassembled\n", j);
 }

 if(!Output.Write("Data/Output.jpg")){
  printf("Cannot write image\n");
  return;
 }
}
//------------------------------------------------------------------------------

void Slave(int ID){
 // Start of "Hello World" example..............................................
 int  size[3];
 char idstr[32];
 char buff [BUFSIZE];

 MPI_Status stat;

 // receive from rank 0 (master):
 // This is a blocking receive, which is typical for slaves.
 //int MPI_Send(void *data_to_send, int send_count, MPI_Datatype send_type, int destination_ID, int tag, MPI_Comm comm);
 MPI_Recv(size, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
 char buffIn [size[2]][size[1]];
 char buffOut [size[2]][size[1]];
 MPI_Send(size, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);


 MPI_Recv(buffIn, size[0], MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);

 MPI_Send(size, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);
 //
    int x, y, u, v, a, b, i,j, temp;
    double tempValue;
    bool MidCheck = true;
     tempValue = 0;

    int tempArray[243];
    for(y = 0; y < size[2]; y++){
     for(x = 0; x < size[1]; x+=3){

       a = 0;
       b = 0;
       i=0;
       for (v = y-4; v< y+5;v++)
       {
         for (u = x-12; u < x+13; u+=3)
         {
           if (v < 0 | u < 0 | v >= size[2] | u >= size[1])
           {
             tempArray[i] = buffIn[y][x];
             tempArray[i+1] = buffIn[y][x+1];
             tempArray[i+2] = buffIn[y][x+2];
           }else
           {
             tempArray[i] = buffIn[v][u];
             tempArray[i+1] = buffIn[v][u+1];
             tempArray[i+2] = buffIn[v][u+2];

           }
           i+=3;
         }
       }
       //BubbleSort
       //bubblesort(tempArray);
       insertionsort(tempArray);
       buffOut[y][x] = tempArray[120];
       buffOut[y][x+1] = tempArray[121];
       buffOut[y][x+2] = tempArray[122];

     }
    }




 MPI_Send(buffOut, size[0], MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
 // End of "Hello World" example................................................
}
//------------------------------------------------------------------------------

int main(int argc, char** argv){
 int myid;

 // MPI programs start with MPI_Init
 MPI_Init(&argc, &argv);

 // find out how big the world is
 MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

 // and this processes' rank is
 MPI_Comm_rank(MPI_COMM_WORLD, &myid);

 // At this point, all programs are running equivalently, the rank
 // distinguishes the roles of the programs, with
 // rank 0 often used as the "master".
 if(myid == 0) Master();
 else          Slave (myid);

 // MPI programs end with MPI_Finalize
 MPI_Finalize();
 return 0;
}
//------------------------------------------------------------------------------
