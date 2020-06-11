#include "fe_region_growth.h"

short arr_in_img[BREADTH][LENGTH] = {{0}};
short arr_out_img[BREADTH + 2][LENGTH + 2];

void getData(short arr_out_img[BREADTH + 2][LENGTH + 2], int i, int j, int star_num, int x_sum[], int y_sum[], int pixel_sum[], int num_pixels[])
{
	// base case
	if(arr_out_img[j][i] <= THRESHOLD)
		return;
	
	// keeping track of the sums
	x_sum[star_num] += (int)arr_out_img[j][i]*i;
	y_sum[star_num] += (int)arr_out_img[j][i]*j;
	pixel_sum[star_num] += (int)arr_out_img[j][i];
	num_pixels[star_num] += 1;
	arr_out_img[j][i] = 0;

	// recursive calls
	getData(arr_out_img, i - 1, j, star_num, x_sum, y_sum, pixel_sum, num_pixels);
	getData(arr_out_img, i + 1, j, star_num, x_sum, y_sum, pixel_sum, num_pixels);
	getData(arr_out_img, i, j - 1, star_num, x_sum, y_sum, pixel_sum, num_pixels);
	getData(arr_out_img, i, j + 1, star_num, x_sum, y_sum, pixel_sum, num_pixels);

	return;
}

void padZeroes(short arr_in_img[BREADTH][LENGTH], short arr_out_img[BREADTH + 2][LENGTH + 2])
{
	for(int i = 0; i < BREADTH + 2; i++)
		for(int j = 0; j < LENGTH + 2; j++)
			if((i == 0) | (j==0) | (i == (BREADTH + 1)) | (j == (LENGTH + 1)))
				arr_out_img[i][j] = 0;
			else
				arr_out_img[i][j] = arr_in_img[i-1][j-1];
}

void regionGrowth(short arr_in_img[BREADTH][LENGTH], short arr_out_img[BREADTH + 2][LENGTH + 2], double centroids_st[MAX_STARS][3])
{
	int star_num = 0;
	padZeroes(arr_in_img, arr_out_img);
	int x_sum[MAX_STARS] = {0};
	int y_sum[MAX_STARS] = {0};
	int pixel_sum[MAX_STARS] = {0};
	int num_pixels[MAX_STARS] = {0};
	for(int j = 1; j < BREADTH + 1; j += SKIP_PIXELS)
		for(int i = 1; i < LENGTH + 1; i += SKIP_PIXELS)
			if(arr_out_img[j][i] > THRESHOLD)
			{
				getData(arr_out_img, i, j, star_num, x_sum, y_sum, pixel_sum, num_pixels);
				star_num++;
			}
	ofstream out;
	out.open("centroid_data.csv");
	out<<"x_sum, y_sum, pixel_sum, num_pixels\n";
	for(int i = 0; i < MAX_STARS; i++)
	{
		out<<x_sum[i]<<","<<y_sum[i]<<","<<pixel_sum[i]<<","<<num_pixels[i]<<"\n";
	}
	out.close();
	int valid_stars = 0;
	
	ofstream cen;
	cen.open("centroids.csv");
	cen<<"ID, x_cen, y_cen\n";
	for(int k = 0; k < star_num; k++)
	{
		if((num_pixels[k] < STAR_MAX_PIXEL) & (num_pixels[k] >= STAR_MIN_PIXEL))
		{
			valid_stars++;
			centroids_st[k][0] = valid_stars;
			centroids_st[k][1] = (double)x_sum[k]/(double)pixel_sum[k] - ((double)(LENGTH/2) + 0.5);
			centroids_st[k][2] = -1*((double)y_sum[k]/(double)pixel_sum[k] - ((double)(BREADTH/2) + 0.5));
			cen<<centroids_st[k][0]<<","<<centroids_st[k][1]<<","<<centroids_st[k][2]<<"\n";
		}
	}
	cen.close();
	return;
}

int main()
{
    double centroids_st[MAX_STARS][3];
	ifstream file;
    file.open("image.txt");
	for(int i = 0; i < BREADTH; i++)
		for(int j = 0; j < LENGTH; j++)
			file >> arr_in_img[i][j];
	auto start = high_resolution_clock::now();

	regionGrowth(arr_in_img, arr_out_img, centroids_st);

	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(stop - start);
	cout<<"Time taken: " <<duration.count()<<" ms";
}
