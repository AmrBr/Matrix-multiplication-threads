#include <iostream>
#include <pthread.h>
#include <string.h>
#include <fstream>
#include <time.h>

int M[1024][1024];
int N[1024][1024];
int result1[1024][1024];
int result2[1024][1024];

using namespace std;

// Matrix Multiplication By Element
void *mulMatrixElement(void *arg)
{
    int *arguments = (int *)arg;
    int i = arguments[0];
    int j = arguments[1];
    for (int k = 0; k < arguments[2]; k++)
    {
        result1[i][j] += M[i][k] * N[k][j];
    }
    return NULL;
}

// Matrix Multiplication by Row
void *mulMatrixRow(void *arg)
{
    int *arguments = (int *)arg;
    int i = arguments[0];
    for (int j = 0; j < arguments[1]; j++)
    {
        for (int k = 0; k < arguments[2]; k++)
        {
            result2[i][j] += M[i][k] * N[k][j];
        }
    }
    return NULL;
}

int main()
{
    // Getting file name as string from the user
    string fileName;
    int rows, columns, rows1, columns1, i, j, k;
    cout << "Enter Input File Name: ";
    cin >> fileName;
    fileName = fileName + ".txt";

    // Opening input file to read input data
    ifstream inputFile;
    inputFile.open(fileName);

    // Reading number of rows and columns of the first matrix from the file
    if (inputFile.is_open())
    {
        inputFile >> rows >> columns;
        getchar();
    }

    // Storing data of the first matrix read from the file into the Matrix M
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < columns; j++)
        {
            inputFile >> M[i][j];
        }
    }

    // Reading number of rows and columns of the second matrix from the file
    inputFile >> rows1 >> columns1;

    // Storing data of the Second matrix read from the file into the Matrix N
    for (i = 0; i < rows1; i++)
    {
        for (j = 0; j < columns1; j++)
        {
            inputFile >> N[i][j];
        }
    }
    inputFile.close();

    // Opening output file to write output data
    ofstream outputFile;
    outputFile.open("output.txt");

    // Making sure that the matrices dimensions are compatible for multiplication
    if (columns != rows1)
    {
        outputFile << "Wrong Matrix Dimensions, Multiplication is not Possible!" << endl;
        cout << "Dimensions Error!!" << endl;
        return 0;
    }

    // Calculating Number of threads for each case:
    // By elements --> Number of threads equal number of elements in the output matrix
    int numberOfThreads1 = (rows * columns1) - 1;

    // By row --> Number of threads equal number of rows in the output matrix
    int numberOfThreads2 = rows - 1;

    // Creating array of threads for the first case
    pthread_t thread[numberOfThreads1];

    // Array row_col_mul[] Contains data required by the function to multiply the given matrices
    // row_col_mul[0] --> identifies the Row number of the first matrix
    // row_col_mul[1] --> identifies the Column number of the second matrix
    // row_col_mul[2] --> identifies the number of multiplications required in each thread
    int row_col_mul[3];
    row_col_mul[2] = columns;

    // Calculate time of the processor untill now
    time_t time = clock();

    // Creating threads each calling function mulMatrixElement with argument row_col_mul[]
    // Each thread waits untill the previous thread finishes execution
    for (i = 0; i < rows; i++)
    {
        row_col_mul[0] = i;
        for (j = 0; j < columns1; j++)
        {
            row_col_mul[1] = j;
            pthread_create(&thread[i + j], NULL, mulMatrixElement, &row_col_mul);
            pthread_join(thread[i + j], NULL);
        }
    }

    // Calculate execution time of the threads and the multiplication process
    time = clock() - time;

    // Convert time calculated to seconds
    double totalTime1 = ((double)time) / CLOCKS_PER_SEC;

    // Write Result of the "Multiplication by Element" into the output file
    outputFile << "By Element" << endl;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < columns1; j++)
        {
            outputFile << result1[i][j] << " ";
        }
        outputFile << endl;
    }
    outputFile << "Time: " << totalTime1 << endl
               << endl;

    // Multiplication By Rows:
    // Creating Second array of threads
    pthread_t thread2[numberOfThreads2];

    row_col_mul[1] = columns1;
    time_t time2 = clock();

    // Creating threads each calling function mulMatrixRow with argument row_col_mul[]
    // Each thread waits untill the previous thread finishes execution
    for (i = 0; i < rows; i++)
    {
        row_col_mul[0] = i;
        pthread_create(&thread2[i], NULL, mulMatrixRow, &row_col_mul);
        pthread_join(thread2[i], NULL);
    }

    // Calculating Time in seconds
    time2 = clock() - time2;
    double totalTime2 = ((double)time2) / CLOCKS_PER_SEC;

    // Write Result of the "Multiplication by Row" into the output file
    outputFile << "By Row" << endl;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < columns1; j++)
        {
            outputFile << result2[i][j] << " ";
        }
        outputFile << endl;
    }
    outputFile << "Time: " << totalTime2 << endl;
    outputFile.close();

    cout << "Process Done Successfully.." << endl;
    return 0;
}
