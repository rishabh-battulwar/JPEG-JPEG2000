#ifndef IMAGE_COMPRESSOR
#define IMAGE_COMPRESSOR

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "afxwin.h"

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "Image.h"

class JPEGCompressor
{

private:
	
	
public:
	
	

	void	DCTCompression(MyImage &original, float compressedStream[]);
	void	imgBlock_to_DCTBlock(float imgBlock[8][8], float DCTBlock[8][8]);
	void	quantizeBlock(float DCTBlock[8][8], int quantizedBlock[8][8]);
	//void	entropyEncoder(int quantizedBlock[8][8], int streamArr[64]);
	void	entropyEncoder(float quantizedBlock[8][8], int streamArr[64]);

	void	DCTDecompression(float compressedStream[], MyImage &decompressed, int m);
	//void	entropyDecoder(int streamArr[64], int quantizedBlock[8][8], int m);
	void	entropyDecoder(int streamArr[64], float quantizedBlock[8][8], int m);
	void	deQuantizeBlock(int quantizedBlock[8][8], float DCTBlock[8][8]);
	void	DCTBlock_to_imgBlock(float DCTBlock[8][8], float imgBlock[8][8]);

	void	initializeZigZagArrayHashTable();
	void	initializeDCTFormulaPart();

	void	DWTCompression(MyImage &original, float compressedStream[]);

	void	DWTDecompression(float compressedStream[], MyImage &decompressed, int m);

	void	Matrix2Dto1D(float *[], float[], int rows, int cols);
	void	Matrix1Dto2D(float[], float *[], int rows, int cols);
	void	TransposeMatrix(float *[], float *[], int rows, int cols);
	void	MultiplyMatrices(float *[], float *[], float *[], int rows, int cols);

};




#endif //IMAGE_COMPRESSOR
