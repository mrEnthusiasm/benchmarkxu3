#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include <stdbool.h>
#define DATA_NAME "recording.raw"
#define PATTERN_NAME "pattern.raw"

int main()
{
//get file data
	FILE *datastream_file = fopen(DATA_NAME, "rb");
	FILE *pattern_file = fopen(PATTERN_NAME, "rb");
	if(datastream_file == NULL)
	{
		printf("%s not found\n",DATA_NAME);
		return 0;
	}
	if(pattern_file == NULL)
	{
		printf("%s not found\n",PATTERN_NAME);
		return 0;
	}

//read file data into arrays
	struct stat st;
	stat(PATTERN_NAME, &st);
	int pattern_size=(st.st_size/8);
	double pattern_array[pattern_size];
	stat(DATA_NAME, &st);
	int datastream_size=(st.st_size/8);
	double datastream_array[st.st_size/8];
	float temp_datastream_array[datastream_size];
	fread(pattern_array, sizeof(double), pattern_size, pattern_file);
	fread(temp_datastream_array, sizeof(float), datastream_size, 
		datastream_file);
	fclose(datastream_file);
	fclose(pattern_file);

//Find average value of pattern and datastream
	int i;
	double sum=0;
	double pattern_avg;
	for( i=0; i<pattern_size; i++)
	{
		sum += pattern_array[i];
	}
	pattern_avg = (sum / (double)pattern_size);
	printf("average value in pattern: %f\n",pattern_avg);
	sum=0;
	double datastream_avg;
	float tmp_val;
	double double_val;
	int fail_count=0;

	for( i=0; i<datastream_size; i++) //remove NaN and INFINITY
	{	
		if(temp_datastream_array[i] != temp_datastream_array[i])
		{
			fail_count++;
			continue;
		}
		if(temp_datastream_array[i] == INFINITY || temp_datastream_array[i] 				== -INFINITY)
		{
			fail_count++;
			continue;
		}
		tmp_val = temp_datastream_array[i];
		double_val = (double)tmp_val;
		datastream_array[i]=double_val;
		
		sum += double_val;
	}
	datastream_avg = sum / ((double)datastream_size-(double)fail_count);
//Find CC
	printf("started CC calculation\n");
	double output_array[datastream_size];

	int j;
	double numerator=0;
	double denom_x=0;
	double denom_y=0;
	double denominator;
//i is the size of the delay
	for( i=0; i< (datastream_size - pattern_size); i++)
	{
//j is the current index
		for( j=0; j<pattern_size; j++)
		{
			numerator += (datastream_array[i+j] - datastream_avg) *
				(pattern_array[j] - pattern_avg);
			denom_x += (pattern_array[j]-pattern_avg) * (pattern_array
				[j]-pattern_avg);
			denom_y += (datastream_array[i+j]-datastream_avg) * 
			(datastream_array[i+j]-datastream_avg);
		}
		denominator = sqrt(denom_x * denom_y);

		if(denominator < 0.0000001 || numerator < 0.0000001)
		{
			output_array[i] = 0;
		}
		else
		{
			output_array[i] = numerator / denominator;
		}
		denom_x = 0;
		denom_y = 0;
		denominator = 0;
		numerator = 0;
	}
	return 0;
}
