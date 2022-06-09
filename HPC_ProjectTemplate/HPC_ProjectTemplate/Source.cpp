#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <mpi.h>
#include <msclr\marshal_cppstd.h>
#include <ctime>
#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>

using namespace std;
using namespace msclr::interop;

int* inputImage(int* w, int* h, System::String^ imagePath) //put the size of image in w & h
{
	int* input;
	int OriginalImageWidth, OriginalImageHeight;

	//************************* Read Image and save it to local arrays *************************
	System::Drawing::Bitmap BM(imagePath);

	OriginalImageWidth = BM.Width;
	OriginalImageHeight = BM.Height;
	*w = BM.Width;
	*h = BM.Height;

	int* Red = new int[BM.Height * BM.Width];
	int* Green = new int[BM.Height * BM.Width];
	int* Blue = new int[BM.Height * BM.Width];
	input = new int[BM.Height * BM.Width];

	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);

			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;

			input[i * BM.Width + j] = ((c.R + c.B + c.G) / 3); // Gray scale value equals the average of RGB values

		}
	}

	return input;
}

void createImage(int* image, int width, int height, int index)
{
	System::Drawing::Bitmap MyNewImage(width, height);

	for (int i = 0; i < MyNewImage.Height; i++)
	{
		for (int j = 0; j < MyNewImage.Width; j++)
		{
			//i * OriginalImageWidth + j
			if (image[i * width + j] < 0)
			{
				image[i * width + j] = 0;
			}
			if (image[i * width + j] > 255)
			{
				image[i * width + j] = 255;
			}

			System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i * MyNewImage.Width + j], image[i * MyNewImage.Width + j], image[i * MyNewImage.Width + j]);
			MyNewImage.SetPixel(j, i, c);
		}
	}

	MyNewImage.Save("..//Data//Output//outputRes" + index + ".png");
	std::cout << "Result Image Saved " << index << endl;
}

void parallelLowPassFilter(int* imageData, int ImageWidth, int ImageHeight, int kernelSize, int index, int rank, int size)
{
	if (rank == 0)
	{
		int* ResultedImage = new int[(ImageHeight - (kernelSize - 1)) * (ImageWidth - (kernelSize - 1))];
		MPI_Status status;
		int counter1 = 0;
		int counter2 = 0;


		int sizeloop = (ImageHeight - (kernelSize - 1)) / (size - 1);
		int finalloop = 0;
		if ((ImageHeight - (kernelSize - 1)) % (size - 1) != 0)
		{
			sizeloop++;
			finalloop = (ImageHeight - (kernelSize - 1)) % (size - 1);
		}
		else
			finalloop = (size - 1);


		for (int i = 0; i < sizeloop - 1; i++)
		{
			for (int i = 1; i < size; i++)
			{
				MPI_Send(&imageData[counter1], ImageWidth * kernelSize, MPI_INT, i, 0, MPI_COMM_WORLD);
				counter1 += ImageWidth;
			}
			for (int i = 1; i < size; i++)
			{
				MPI_Recv(&ResultedImage[counter2], ImageWidth - (kernelSize - 1), MPI_INT, i, 0, MPI_COMM_WORLD, &status);
				counter2 += ImageWidth - (kernelSize - 1);
			}
		}

		for (int i = 1; i < finalloop + 1; i++)
		{
			MPI_Send(&imageData[counter1], ImageWidth * kernelSize, MPI_INT, i, 0, MPI_COMM_WORLD);
			counter1 += ImageWidth;
		}
		for (int i = 1; i < finalloop + 1; i++)
		{
			MPI_Recv(&ResultedImage[counter2], ImageWidth - (kernelSize - 1), MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			counter2 += ImageWidth - (kernelSize - 1);
		}
		for (int i = 0; i < 16; i++)
		{
			int min = 0, max =255;
			int Current_img = ResultedImage[i];
			int img_normalized =max *(Current_img - min) / (max - min);
			if (kernelSize >= 7) {
				ResultedImage[i] = img_normalized / (kernelSize - 2);
			}
			else {
				ResultedImage[i] = img_normalized / (kernelSize / 2);
			}
		}

		createImage(ResultedImage, ImageWidth - (kernelSize - 1), ImageHeight - (kernelSize - 1), index);
	}
	else if (rank < size)
	{
		double kernel = 1.0 / (kernelSize * kernelSize);

		int sizeloop = (ImageHeight - (kernelSize - 1)) / (size - 1);
		if ((ImageHeight - (kernelSize - 1)) % (size - 1) != 0)
		{
			sizeloop++;
			if (rank <= (ImageHeight - (kernelSize - 1)) % (size - 1))
				sizeloop++;
		}


		for (int i = 0; i < sizeloop - 1; i++)
		{

			int* RecivedLocalArr = new int[ImageWidth * kernelSize];
			int* ResultedLocalArr = new int[ImageWidth - (kernelSize - 1)];
			MPI_Status status;
			MPI_Recv(RecivedLocalArr, ImageWidth * kernelSize, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

			for (int i = 0; i < ImageWidth - (kernelSize - 1); i++)
			{
				int calcutedValue = 0;
				int index = i;
				int checkCounter = 0;

				for (int j = 0; j < kernelSize * kernelSize; j++)
				{
					calcutedValue += RecivedLocalArr[index] * kernel;
					checkCounter++;
					index++;
					if (checkCounter == kernelSize)
					{
						index += ImageWidth - kernelSize;
						checkCounter = 0;
					}
				}

				ResultedLocalArr[i] = calcutedValue;
			}

			MPI_Send(ResultedLocalArr, ImageWidth - (kernelSize - 1), MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
	}
}

void sequentialLowPassFilter(int* imageData, int ImageWidth, int ImageHeight, int kernelSize, int index, int rank, int size)
{
	if (rank == 0)
	{
		int* ResultedImage = new int[(ImageHeight - (kernelSize - 1)) * (ImageWidth - (kernelSize - 1))];
		int counter1 = 0;
		int counter2 = 0;
		double kernel = 1.0 / (kernelSize * kernelSize);

		for (int i = 0; i < (ImageHeight - (kernelSize - 1)); i++)
		{
			int* RecivedLocalArr = new int[ImageWidth * kernelSize];

			for (int j = 0; j < ImageWidth * kernelSize; j++)
			{
				RecivedLocalArr[j] = imageData[counter1 + j];
			}

			for (int j = 0; j < ImageWidth - (kernelSize - 1); j++)
			{
				int calcutedValue = 0;
				int index = j;
				int checkCounter = 0;

				for (int k = 0; k < kernelSize * kernelSize; k++)
				{

					calcutedValue += RecivedLocalArr[index] * kernel;
					checkCounter++;
					index++;
					if (checkCounter == kernelSize)
					{
						index += ImageWidth - kernelSize;
						checkCounter = 0;
					}
				}

				ResultedImage[counter2] = calcutedValue;
				counter2++;
			}
			counter1 += ImageWidth;
		}

		createImage(ResultedImage, ImageWidth - (kernelSize - 1), ImageHeight - (kernelSize - 1), index);
	}
}

int main()
{
	MPI_Init(NULL, NULL);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int ImageWidth = 4, ImageHeight = 4;
	int start_s, stop_s, TotalTime = 0;

	System::String^ imagePath;
	std::string img;

	img = "../Data/Test Cases/5N.png";

	imagePath = marshal_as<System::String^>(img);
	int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);

	start_s = clock();

	//------------------------------------- Low Pass Filter -------------------------------------

	//sequentialLowPassFilter(imageData, ImageWidth, ImageHeight, 3, 0, rank, size);
	//mpiexec -n 1 HPC_ProjectTemplate

	//parallelLowPassFilter(imageData, ImageWidth, ImageHeight, 3, 0, rank, size);
	//mpiexec HPC_ProjectTemplate

	//-------------------------------------------------------------------------------------------

	stop_s = clock();
	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	std::cout << "time: " << TotalTime << endl;

	std::free(imageData);
	MPI_Finalize();
	return 0;
}
