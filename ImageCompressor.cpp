
#include "afxwin.h"

#include "ImageCompressor.h"
#include "Hash.h"

#include <iostream>
#include <math.h>
#include <amp.h>
using namespace concurrency;

#define pi 3.141592654

const int QuantizationMatrixForLuminance[8][8] =
{
	{ 16, 11, 10, 16, 24, 40, 51, 61 },
	{ 12, 12, 14, 19, 26, 58, 60, 55 },
	{ 14, 13, 16, 24, 40, 57, 69, 56 },
	{ 14, 17, 22, 29, 51, 87, 80, 62 },
	{ 18, 22, 37, 56, 68, 109, 103, 77 },
	{ 24, 35, 55, 64, 81, 104, 113, 92 },
	{ 49, 64, 78, 87, 103, 121, 120, 101 },
	{ 72, 92, 95, 98, 112, 100, 103, 99 }
};

const int ZigZagArray[8][8] =
{
	{ 0, 1, 5, 6, 14, 15, 27, 28 },
	{ 2, 4, 7, 13, 16, 26, 29, 42 },
	{ 3, 8, 12, 17, 25, 30, 41, 43},
	{ 9, 11, 18, 24, 31, 40, 44, 53},
	{ 10, 19, 23, 32, 39, 45, 52, 54},
	{ 20, 22, 33, 38, 46, 51, 55, 60},
	{ 21, 34, 37, 47, 50, 56, 59, 61},
	{ 35, 36, 48, 49, 57, 58, 62, 63}
};

Hash ZigZagArrayHashTable;
float LookupTable[8][8];

// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
void JPEGCompressor::DCTCompression(MyImage &original, float compressedStream[])
{
	//test code
	/*compressed.setHeight(original.getHeight());
	compressed.setWidth(original.getWidth());
	compressed.allocateMemoryforData();
	for (int i = 0; i < original.getHeight(); i++)
		for (int j = 0; j < original.getWidth(); j++)
			for (int k = 0; k < 3; k++)
			{
				compressed.setValue(i, j, k, original.getValue(i, j, k));
			}*/
	

	float imgBlock[8][8], DCTBlock[8][8];
	int quantizedBlock[8][8];
	int streamArr[64];

	initializeZigZagArrayHashTable();
	initializeDCTFormulaPart();

	int iter = 0;
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < original.getHeight(); i = i + 8)
		{
			for (int j = 0; j < original.getWidth(); j = j + 8)
			{
				for (int p = 0; p < 8; p++)
					for (int q = 0; q < 8; q++)
						imgBlock[p][q] = 1.0 * original.getValue((i + p), (j + q), k);

				/*if (i == 0 && j == 0 && k == 0)
					for (int p = 0; p < 8; p++)
						for (int q = 0; q < 8; q++)
							std::cout << imgBlock[p][q] << std::endl;*/

				imgBlock_to_DCTBlock(imgBlock, DCTBlock);
				//quantizeBlock(DCTBlock, quantizedBlock);
				//entropyEncoder(quantizedBlock, streamArr);
				entropyEncoder(DCTBlock, streamArr);

				for (int streamIndex = 0; streamIndex < 64; streamIndex++)
					compressedStream[iter++] = streamArr[streamIndex];

				//for (int p = 0; p < 8; p++)
				//	for (int q = 0; q < 8; q++)
				//		compressedStream[iter++] = DCTBlock[p][q];

			}
		}
	}
}

void JPEGCompressor::DCTDecompression(float compressedStream[], MyImage &decompressed, int m)
{
	float imgBlock[8][8], DCTBlock[8][8];
	int quantizedBlock[8][8];
	//float streamArr[64];
	int streamArr[64];

	int iter = 0;
	for (int k = 0; k < 3; k++)
	{
		for (int i = 0; i < decompressed.getHeight(); i += 8)
		{
			for (int j = 0; j < decompressed.getWidth(); j += 8)
			{
				for (int streamIndex = 0; streamIndex < 64; streamIndex++)
					streamArr[streamIndex] = compressedStream[iter++];

				//for (int p = 0; p < 8; p++)
				//	for (int q = 0; q < 8; q++)
				//		DCTBlock[p][q] = compressedStream[iter++];

				//entropyDecoder(streamArr, quantizedBlock, m);
				entropyDecoder(streamArr, DCTBlock, m);
				//deQuantizeBlock(quantizedBlock, DCTBlock);
				DCTBlock_to_imgBlock(DCTBlock, imgBlock);

				for (int p = 0; p < 8; p++)
					for (int q = 0; q < 8; q++)
						decompressed.setValue((i + p), (j + q), k, (int)imgBlock[p][q]);


			}
		}
	}
}

void JPEGCompressor::imgBlock_to_DCTBlock(float imgBlock[8][8], float DCTBlock[8][8])
{
	float sum = 0, Cu, Cv;
	for (int u = 0; u < 8; u++)
	{
		for (int v = 0; v < 8; v++)
		{
			sum = 0.0;
			if (u == 0)
				Cu = 1.0 / sqrt(2.0);
			else
				Cu = 1.0;
			if (v == 0)
				Cv = 1.0 / sqrt(2.0);
			else
				Cv = 1.0;

			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					sum += imgBlock[x][y] * cos(((2 * x + 1) * u * pi) / 16) * cos(((2 * y + 1) * v * pi) / 16);
					//sum += imgBlock[x][y] * LookupTable[u][x] * LookupTable[v][y];
				} // for y
			} // for x

			DCTBlock[u][v] = (1.0 / 4) * Cu * Cv * sum;
		} // for v
	} // for u
} // method end


void JPEGCompressor::quantizeBlock(float DCTBlock[8][8], int quantizedBlock[8][8])
{
	for (int u = 0; u < 8; u++)
	{
		for (int v = 0; v < 8; v++)
		{
			quantizedBlock[u][v] = (int)(round(DCTBlock[u][v] / QuantizationMatrixForLuminance[u][v]));
		} // for v
	} // foru
}


void JPEGCompressor::entropyEncoder(float quantizedBlock[8][8], int streamArr[64])
{
	int p, q;
	for (int i = 0; i < 64; i++)
	{
		ZigZagArrayHashTable.FindIndex(i, p, q);
		streamArr[i] = (int)quantizedBlock[p][q];
	}
}


void JPEGCompressor::entropyDecoder(int streamArr[64], float quantizedBlock[8][8], int m)
{
	int p, q;
	for (int i = 0; i < 64; i++)
	{
		if (i < m)
		{
			ZigZagArrayHashTable.FindIndex(i, p, q);
			quantizedBlock[p][q] = 1.0 * streamArr[i];
		}
		else
		{
			ZigZagArrayHashTable.FindIndex(i, p, q);
			quantizedBlock[p][q] = 0.0;
		}
	}
}

void JPEGCompressor::deQuantizeBlock(int quantizedBlock[8][8], float DCTBlock[8][8])
{
	for (int u = 0; u < 8; u++)
	{
		for (int v = 0; v < 8; v++)
		{
			DCTBlock[u][v] = (float)(quantizedBlock[u][v] * QuantizationMatrixForLuminance[u][v]);
		} // for v
	} // foru
}

void JPEGCompressor::DCTBlock_to_imgBlock(float DCTBlock[8][8], float imgBlock[8][8])
{
	float sum = 0, Cu, Cv;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			sum = 0.0;
			
			for (int u = 0; u < 8; u++)
			{
				for (int v = 0; v < 8; v++)
				{
					if (u == 0)
						Cu = 1.0 / sqrt(2.0);
					else
						Cu = 1.0;
					if (v == 0)	
						Cv = 1.0 / sqrt(2.0);
					else
						Cv = 1.0;
					
					sum += Cu * Cv * DCTBlock[u][v] * cos(((2 * x + 1) * u * pi) / 16) * cos(((2 * y + 1) * v * pi) / 16);
					//sum += Cu * Cv * DCTBlock[u][v] * LookupTable[u][x] * LookupTable[v][y];
				} // for y
			} // for x

			imgBlock[x][y] = (1.0 / 4) * sum;
			if (imgBlock[x][y] < 0.0)	imgBlock[x][y] = 0.0;
			if (imgBlock[x][y] > 255.0) imgBlock[x][y] = 255.0;
		} // for v
	} // for u
}

void JPEGCompressor::initializeZigZagArrayHashTable()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			ZigZagArrayHashTable.AddItem(ZigZagArray[i][j], i, j);
		}
	}
}


void JPEGCompressor::DWTCompression(MyImage &original, float compressedStream[])
{
	float **transformMatrixLeft;
	float **transformMatrixRight;
	float **imgBlockMatrix;
	float **intermediate;
	float ***compressedStreamMatrix;
	float **comp;
	int size;

	//Allocate memory for compressedStreamMatrix
	compressedStreamMatrix = new float **[512];
	for (int i = 0; i < 512; i++)
		compressedStreamMatrix[i] = new float*[512];
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
			for (int k = 0; k < 3; k++)
				compressedStreamMatrix[i][j] = new float[3];

	//copy image in compressedStreamMatrix[512][512][3]
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
			for (int k = 0; k < 3; k++)
				compressedStreamMatrix[i][j][k] = (float)original.getValue(i, j, k);

	//master loop
	for (int exp = 9; exp > 0; exp--)
	{
		size = pow(2, exp);

		transformMatrixLeft = new float *[size];
		transformMatrixRight = new float *[size];
		imgBlockMatrix = new float *[size];
		intermediate = new float *[size];
		comp = new float *[size];

		for (int i = 0; i < size; i++)
		{
			transformMatrixLeft[i] = new float[size];	//transformMatrixLeft[cap][cap]
			transformMatrixRight[i] = new float[size];	//transformMatrixRight[cap][cap]
			imgBlockMatrix[i] = new float[size];	//imgBlockMatrix[cap][cap]
			intermediate[i] = new float[size];	//intermediate[cap][cap]
			comp[i] = new float[size];	//comp[cap][cap]
		}

		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				transformMatrixLeft[i][j] = 0.0;

		int index = 0;
		for (int i = 0; i < size / 2; i++)
		{
			transformMatrixLeft[i][index] = 0.5;
			transformMatrixLeft[i + (size / 2)][index++] = -0.5;
			transformMatrixLeft[i][index] = 0.5;
			transformMatrixLeft[i + (size / 2)][index++] = 0.5;
		}

		TransposeMatrix(transformMatrixLeft, transformMatrixRight, size, size);

		for (int k = 0; k < 3; k++)
		{
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					imgBlockMatrix[i][j] = compressedStreamMatrix[i][j][k];

			MultiplyMatrices(transformMatrixLeft, imgBlockMatrix, intermediate, size, size);
			MultiplyMatrices(intermediate, transformMatrixRight, comp, size, size);

			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					compressedStreamMatrix[i][j][k] = comp[i][j];
		}

		delete comp;
		delete intermediate;
		delete imgBlockMatrix;
		delete transformMatrixRight;
		delete transformMatrixLeft;
	}

	int iter = 0;
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
			for (int k = 0; k < 3; k++)
				compressedStream[iter++] = compressedStreamMatrix[i][j][k];

	delete compressedStreamMatrix;
}


void JPEGCompressor::DWTDecompression(float compressedStream[], MyImage &decompressed, int m)
{
	float **transformMatrixLeft;
	float **transformMatrixRight;
	float **imgBlockMatrix;
	float **intermediate;
	float ***compressedStreamMatrix;
	float ***compressedStreamMatrix_zeroCopy;
	float **comp;
	int size;


	//Allocate memory for compressedStreamMatrix
	compressedStreamMatrix = new float **[512];
	compressedStreamMatrix_zeroCopy = new float **[512];
	for (int i = 0; i < 512; i++)
	{
		compressedStreamMatrix[i] = new float*[512];
		compressedStreamMatrix_zeroCopy[i] = new float*[512];
	}
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
			for (int k = 0; k < 3; k++)
			{
				compressedStreamMatrix[i][j] = new float[3];
				compressedStreamMatrix_zeroCopy[i][j] = new float[3];
			}

	int iter = 0;
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
			for (int k = 0; k < 3; k++)
			{
				compressedStreamMatrix[i][j][k] = compressedStream[iter++];
				compressedStreamMatrix_zeroCopy[i][j][k] = 0;
			}

	/////////////////////////////////////////////

	//CODE FOR COEFFICIENT APPROXIMATION

	int level = ceil(log2(m) / 2);
	int upperLimit;

	if (m > 0)
		for (int k = 0; k < 3; k++)
			compressedStreamMatrix_zeroCopy[0][0][k] = compressedStreamMatrix[0][0][k];
	if (m > 1)
		for (int k = 0; k < 3; k++)
			compressedStreamMatrix_zeroCopy[0][1][k] = compressedStreamMatrix[0][1][k];
	if (m > 2)
		for (int k = 0; k < 3; k++)
			compressedStreamMatrix_zeroCopy[1][0][k] = compressedStreamMatrix[1][0][k];
	if (m > 3)
		for (int k = 0; k < 3; k++)
			compressedStreamMatrix_zeroCopy[1][1][k] = compressedStreamMatrix[1][1][k];

	int coeff_left = m - pow(2, (2*level));

	if (coeff_left == 0)	upperLimit = level;
	else					upperLimit = level - 1;
	
	if (m > 4)
	{
		for (int lvl = 2; lvl <= upperLimit; lvl++)
		{
			size = pow(2, lvl);

			//HL block
			for (int i = 0; i < (size / 2); i++)
			{
				for (int j = (size / 2); j < size; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						compressedStreamMatrix_zeroCopy[i][j][k] = compressedStreamMatrix[i][j][k];
					}
				}
			}

			//LH block
			for (int i = (size / 2); i < size; i++)
			{
				for (int j = 0; j < (size / 2); j++)
				{
					for (int k = 0; k < 3; k++)
					{
						compressedStreamMatrix_zeroCopy[i][j][k] = compressedStreamMatrix[i][j][k];
					}
				}
			}

			//HH block
			for (int i = (size / 2); i < size; i++)
			{
				for (int j = (size / 2); j < size; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						compressedStreamMatrix_zeroCopy[i][j][k] = compressedStreamMatrix[i][j][k];
					}
				}
			}
		}

		size = pow(2, level);
		bool flagHL = false, flagLH = false, flagHH = false;
		//HL block
		for (int i = 0; i < (size / 2); i++)
		{
			if (flagHL != true)
			{
				for (int j = (size / 2); j < size; j++)
				{
					if (coeff_left > 0)
					{
						for (int k = 0; k < 3; k++)
						{
							compressedStreamMatrix_zeroCopy[i][j][k] = compressedStreamMatrix[i][j][k];
						}
					}
					else
					{
						flagHL = true;
						break;
					}
					coeff_left--;
				}
			}
			else
				break;
		}

		//LH block
		for (int i = (size / 2); i < size; i++)
		{
			if (flagHL != true)
			{
				for (int j = 0; j < (size / 2); j++)
				{
					if (coeff_left > 0)
					{
						for (int k = 0; k < 3; k++)
						{
							compressedStreamMatrix_zeroCopy[i][j][k] = compressedStreamMatrix[i][j][k];
						}
					}
					else
					{
						flagLH = true;
						break;
					}
					coeff_left--;
				}
			}
			else
				break;
		}

		//HH block
		for (int i = (size / 2); i < size; i++)
		{
			if (flagHL != true)
			{
				for (int j = (size / 2); j < size; j++)
				{
					if (coeff_left > 0)
					{
						for (int k = 0; k < 3; k++)
						{
							compressedStreamMatrix_zeroCopy[i][j][k] = compressedStreamMatrix[i][j][k];
						}
					}
					else
					{
						flagHH = true;
						break;
					}
					coeff_left--;
				}
			}
			else
				break;
		}
	}

	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
			for (int k = 0; k < 3; k++)
				compressedStreamMatrix[i][j][k] = compressedStreamMatrix_zeroCopy[i][j][k];

	/////////////////////////////////////////////

		
	//master loop
	for (int exp = 1; exp <= 9; exp++)
	{
		size = pow(2, exp);

		transformMatrixLeft = new float *[size];
		transformMatrixRight = new float *[size];
		imgBlockMatrix = new float *[size];
		intermediate = new float *[size];
		comp = new float *[size];

		for (int i = 0; i < size; i++)
		{
			transformMatrixLeft[i] = new float[size];	//transformMatrixLeft[cap][cap]
			transformMatrixRight[i] = new float[size];	//transformMatrixRight[cap][cap]
			imgBlockMatrix[i] = new float[size];	//imgBlockMatrix[cap][cap]
			intermediate[i] = new float[size];	//intermediate[cap][cap]
			comp[i] = new float[size];	//comp[cap][cap]
		}

		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				transformMatrixLeft[i][j] = 0.0;

		int index = 0;
		for (int i = 0; i < size / 2; i++)
		{
			transformMatrixLeft[i][index] = 1.0;
			transformMatrixLeft[i + (size / 2)][index++] = -1.0;
			transformMatrixLeft[i][index] = 1.0;
			transformMatrixLeft[i + (size / 2)][index++] = 1.0;
		}

		TransposeMatrix(transformMatrixLeft, transformMatrixRight, size, size);

		for (int k = 0; k < 3; k++)
		{
			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					imgBlockMatrix[i][j] = compressedStreamMatrix[i][j][k];

			MultiplyMatrices(transformMatrixRight, imgBlockMatrix, intermediate, size, size);
			MultiplyMatrices(intermediate, transformMatrixLeft, comp, size, size);

			for (int i = 0; i < size; i++)
				for (int j = 0; j < size; j++)
					compressedStreamMatrix[i][j][k] = comp[i][j];
		}

		delete comp;
		delete intermediate;
		delete imgBlockMatrix;
		delete transformMatrixRight;
		delete transformMatrixLeft;
	}
	
	for (int i = 0; i < 512; i++)
		for (int j = 0; j < 512; j++)
			for (int k = 0; k < 3; k++)
				decompressed.setValue(i,j,k,(int)compressedStreamMatrix[i][j][k]);

	delete compressedStreamMatrix;
	delete compressedStreamMatrix_zeroCopy;
}

void JPEGCompressor::Matrix2Dto1D(float *_2DMatrix[], float _1DMatrix[], int rows, int cols)
{
	int iter = 0;
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			_1DMatrix[iter++] = _2DMatrix[i][j];
}

void JPEGCompressor::Matrix1Dto2D(float _1DMatrix[], float *_2DMatrix[], int rows, int cols)
{
	int iter = 0;
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			_2DMatrix[i][j] = _1DMatrix[iter++];
}

void JPEGCompressor::TransposeMatrix(float *_src[], float *_transpose[], int rows, int cols)
{
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			_transpose[i][j] = _src[j][i];
}

void JPEGCompressor::MultiplyMatrices(float *left[], float *right[], float *result[], int rows, int cols)
{
	for (int i = 0; i < rows; i++){
		for (int j = 0; j < cols; j++){
			result[i][j] = 0;
			for (int k = 0; k < cols; k++){
				result[i][j] = result[i][j] + (left[i][k] * right[k][j]);
			}
		}
	}
}

void JPEGCompressor::initializeDCTFormulaPart()
{
	for (int u = 0; u < 8; u++)
		for (int x = 0; x < 8; x++)
			LookupTable[u][x] = cos((2 * x + 1) * u * pi) / 16;
}