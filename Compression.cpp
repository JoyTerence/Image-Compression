#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <cmath>
#include <fstream>

#define PI 3.14159265

using namespace cv;
using namespace std;


int min(int a, int b)
{
	return (a < b) ? a : b;
}
int max(int a, int b)
{
	return (a > b) ? a : b;
}


class compression
{
	//Contains namely Mat Image for storing for 3 channel pixel values of the original image
	Mat image, inputBlock, Channel[3];
	int rows, cols;
	Mat compressed_image;
	Mat decompressed_image;
public:
	compression()
	{
		string image_path;
		cout << "Image Name\n";
		cin >> image_path;
		image = imread(image_path);
		//width of the Image 
		rows = image.rows;
		//Height of the Image
		cols = image.cols;
		//Mat to store final Compressed Image
		compressed_image = Mat::zeros(rows - rows % 8, cols - cols % 8, CV_8UC3);
	}

	void zig_zag(int zigzag[], int result[][8])
	{
		//Entropy Coding of the Mat done by zig_zag traversal of the Matrix and storing the values so traversed into single D array 'zig_zag'
		int m, n;
		m = n = 8;
		int j = 0;
		for (int i = 0; i < m + n - 1; i++){
			if (i % 2 == 1){
				for (int y = min(i, n - 1); y >= max(0, i - m + 1); y--){
					zigzag[j++] = result[i - y][y];
				}
			}
			else{
				for (int x = min(i, m - 1); x >= max(0, i - n + 1); x--){
					zigzag[j++] = result[x][i - x];
				}
			}
		}
	}
	void compress()
	{
		//for each r g and b component of the image
		for (int rgb = 0; rgb < 3; rgb++)
		{
			//Scans till last mutiple of 8 in row as well as column
			//have to modify to consider even the left pixel parts in row and column
			for (int l = 0; l <= rows - rows % 8 - 8; l += 8)
			{
				for (int m = 0; m <= cols - cols % 8 - 8; m += 8)
				{
					ofstream file; 															//Stores the final compressed values
					cout << "ROWS : " << l << " COLS : " << m << endl;
					Mat roi_img(image(cvRect(m, l, 8, 8)));									//8*8 block extracted from Image
					split(roi_img, Channel);												//8*8 block split into different channels (R,G,B)
					Channel[rgb].convertTo(inputBlock, CV_8S);								//Copying into Mat data structure of suitable type
					file.open("encoded.txt", fstream::app);
					Mat Q = (Mat_<double>(8, 8) << 16, 11, 10, 16, 24, 40, 51, 61,			//Base Quantization Matrix 
						12, 12, 14, 19, 26, 58, 60, 55,
						14, 13, 16, 24, 40, 57, 69, 56,
						14, 17, 22, 29, 51, 87, 80, 62,
						18, 22, 37, 56, 68, 109, 103, 77,
						24, 35, 55, 64, 81, 104, 113, 92,
						49, 64, 78, 87, 103, 121, 120, 101,
						72, 92, 95, 98, 112, 100, 103, 99);
					Mat generatedMatrix(8, 8, CV_64FC1);
					Mat MeanBlock(8, 8, CV_8S);

					// Subtracting 128 from each pixel of 8*8 block to get the values centered around 0
					add(inputBlock, -128, MeanBlock);

					//DCT performed on 8*8 Block 
					double K = 1 / 4.0;
					double mul_fact = PI / 16;
					double alpha_X, alpha_Y;
					double sum = 0, cosx, cosy;
					for (int i = 0; i < 8; i++)
					{
						for (int j = 0; j < 8; j++)
						{
							alpha_X = alpha_Y = 1;
							if (i == 0)
								alpha_X = 1 / sqrt(2);
							if (j == 0)
								alpha_Y = 1 / sqrt(2);
							for (int x = 0; x < 8; x++)
							{
								for (int y = 0; y < 8; y++)
								{
									cosx = cos((2 * x + 1) * i * mul_fact);
									cosy = cos((2 * y + 1) * j * mul_fact);
									sum = sum + (double)(MeanBlock.at<schar>(x, y)) * cosx * cosy;
								}
							}
							generatedMatrix.at<double>(i, j) = K * alpha_X * alpha_Y * sum;
							sum = 0;
						}
					}

					//Divide element wise by Quantization Matrix
					compressed_image = generatedMatrix / Q;
					int result[8][8];
					int result_zig_zag[64];

					//Storing the Mat into 2D Result Array
					for (int i = 0; i < 8; i++){
						for (int j = 0; j < 8; j++) {
							result[i][j] = (int)compressed_image.at<double>(i, j);
						}
					}
					//zig_zag returns result_zig_zag containing Matrix values read in zig_zag manner
					zig_zag(result_zig_zag, result);

					//Finding the the begining part of consecutive zeros at the end
					int zero_index = 63;
					for (int i = 63; i > 0; i--)
					{
						if (result_zig_zag[i] != 0)
							break;
						else
							zero_index = i;
					}
					//writing the compressed values into file with last consecutive zeros at the end skipped
					for (int i = 0; i < zero_index; i++)
						file << result_zig_zag[i] << " ";
					file << "\n";
				}
			}
		}
		return;
	}
	void zig_zag_reader(vector<double> inputs, Mat inputblock)
	{
		//converts the vector of double numbers read in zig_zag manner in right form into inputblock
		int j = 0;
		int m = 8, n = 8;
		for (int i = 0; i < m + n - 1; i++)
		{
			if (i % 2 == 1)
			{
				for (int y = min(i, n - 1); y >= max(0, i - m + 1); y--){
					inputblock.at<double>(i - y, y) = (double)inputs[j++];
				}
			}
			else
			{
				for (int x = min(i, m - 1); x >= max(0, i - n + 1); x--){
					inputblock.at<double>(x, i - x) = (double)inputs[j++];
				}
			}
		}
	}
	void decompressor()
	{
		ifstream file1;
		file1.open("encoded.txt");						// Read the file
		double After_DCT[8][8];
		vector<double> Array;
		vector<double>::iterator it;
		Mat Q = (Mat_<double>(8, 8) << 16, 11, 10, 16, 24, 40, 51, 61,				//Base Quantization Matrix

			12, 12, 14, 19, 26, 58, 60, 55,
			14, 13, 16, 24, 40, 57, 69, 56,
			14, 17, 22, 29, 51, 87, 80, 62,
			18, 22, 37, 56, 68, 109, 103, 77,
			24, 35, 55, 64, 81, 104, 113, 92,
			49, 64, 78, 87, 103, 121, 120, 101,
			72, 92, 95, 98, 112, 100, 103, 99);

		int k = 0, n = 0;

		Mat channel[3];
		vector<Mat> final;
		split(decompressed_image, channel);											//Splits the compressed image into its respective channels

		// Initializing respective channels with value 0
		channel[0] = Mat::zeros(rows - rows % 8, cols - cols % 8, CV_8UC1);
		channel[1] = Mat::zeros(rows - rows % 8, cols - cols % 8, CV_8UC1);
		channel[2] = Mat::zeros(rows - rows % 8, cols - cols % 8, CV_8UC1);
		int r = 0, c = 0;
		Mat inputblock(8, 8, CV_64FC1);
		int i = 0, j = 0;
		int count = 0;

		int rgb = 0;
		string numbers;
		while (!file1.eof()) {
			//Reading a line of numbers from the encoded file
			count++;
			getline(file1, numbers);
			vector<double> inputs;
			istringstream in(numbers);
			copy(istream_iterator<double>(in), istream_iterator<double>(), back_inserter(inputs));
			vector<double>::iterator it;

			//Adding 0 to fill the places where the last consecutive zeros were removed in compress() method
			for (int i = inputs.size(); i < 64; i++)
				inputs.push_back(0);
			//get the right matrix from numbers in zig_zag form
			zig_zag_reader(inputs, inputblock);
			Mat Perform_DCT(8, 8, CV_64FC1);
			Perform_DCT = inputblock.mul(Q);
			double K = 1 / 4.0;
			double mul_fact = PI / 16;
			double alpha_u, alpha_v;
			double sum = 0, cosx, cosy;

			//Perform Inverse DCT
			for (int x = 0; x < 8; x++)
			{
				for (int y = 0; y < 8; y++)
				{
					for (int u = 0; u < 8; u++)
					{
						for (int v = 0; v < 8; v++)
						{
							alpha_u = alpha_v = 1;
							if (u == 0)
								alpha_u = 1 / sqrt(2);
							if (v == 0)
								alpha_v = 1 / sqrt(2);
							cosx = cos((2 * x + 1) * u * mul_fact);
							cosy = cos((2 * y + 1) * v * mul_fact);
							sum = sum + alpha_u * alpha_v * Perform_DCT.at<double>(u, v)* cosx * cosy;
						}
					}
					After_DCT[x][y] = K * sum;
					sum = 0;
				}
			}

			// Making the matrix to be centered around 128 from being centered around 0
			for (int x = 0; x < 8; x++){
				for (int y = 0; y < 8; y++)
					After_DCT[x][y] += 128;
			}

			Mat A = Mat(8, 8, CV_8UC1);

			//Converting to Mat form
			for (int x = 0; x < 8; x++){
				for (int y = 0; y < 8; y++)
					A.at<uchar>(x, y) = (int)After_DCT[x][y];
			}

			//Fill the 8*8 blocks into respective channel of final Decompressed Image
			A.copyTo(channel[rgb](Rect(c, r, 8, 8)));


			//Keeping track of row and column number
			c += 8;
			if (c == (cols - cols % 8)){
				r += 8;
				c = 0;
			}


			if (r >= (rows - rows % 8))
			{
				c = r = 0;
				rgb++;													//next channel in RGB
				if (rgb == 3)
				{
					cout << "END" << endl;
					file1.close();
					final.push_back(channel[0]);
					final.push_back(channel[1]);
					final.push_back(channel[2]);
					//Merging all the channels into compressed_image
					merge(final, decompressed_image);
					//Display of compressed Image
					imshow("Compressed", decompressed_image);
					//Display of original Image
					imshow("Original", image);
					waitKey(0);
					return;
				}
			}
			cout << c << " " << r << " Channel no : " << rgb + 1 << " row : " << count << endl;
		}
	}
};


int main()
{
	//Create a object of compression class
	compression Object;
	cout << "Enter : \n1 to compress\n2 to Decompress\n3 to Exit\n";
	int choice;
	cin >> choice;
	switch (choice)
	{
	case 1:
		//Call Compressing member function
		Object.compress();
		break;
	case 2:
		//Call Decompressing member function
		Object.decompressor();
		break;
	case 3:
		exit(0);
	}
	return 0;
}
