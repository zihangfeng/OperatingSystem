//Zihang Feng
//jfeng9

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <math.h>
#include <ctime>

using namespace std;


unsigned char *addr;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

unsigned char range = 255;
unsigned char* addr_of_range = NULL;

unsigned char best_max = 0;
unsigned char* addr_of_best_max = NULL;

unsigned char sum_of_difference_value = 255;
unsigned char* addr_of_sum_of_difference_value = NULL;

double best_standard_deviation = 255*255;
unsigned char* addr_of_best_standard_deviation = NULL;

double best_standard_deviation_in_difference = 255*255;
unsigned char* addr_of_best_standard_deviation_in_difference = NULL;

struct thread_info{
	unsigned char* start;
	unsigned char*  end;
};


void best_range_function(unsigned char *starting_address, unsigned char *ending_address)
{
		unsigned char smallest_value = 255;
		unsigned char largest_value = 0;
		unsigned char current_range = 255;
		unsigned char current;
		for(unsigned char *current_address = starting_address; current_address < ending_address; current_address++)
		{
			current = *current_address;
			if(current < smallest_value)
				smallest_value = current; 
			if(current > largest_value)
				largest_value = current;
		}
		current_range = largest_value - smallest_value;
		

		// lock the value being share
		pthread_mutex_lock(&mutex1);
		if(current_range < range)
		{	
			range = current_range;
			addr_of_range = starting_address;
		}
		pthread_mutex_unlock(&mutex1);		
}

void absolute_max(unsigned char *starting_address, unsigned char *ending_address)
{
                unsigned char current_max = 0;
		unsigned char current;
		unsigned char next;
		unsigned char absolute_value;

                for(unsigned char *current_address = starting_address; current_address < ending_address; current_address++)
                {
                        current = *current_address;
			next = *(current_address + 1);
			absolute_value = (unsigned char)abs(current - next);
                        if(current_max < absolute_value)
                                current_max = absolute_value;
                }


                // lock the value being share
                pthread_mutex_lock(&mutex1);
                if(current_max > best_max)
		        {
                        best_max = current_max;    
                        addr_of_best_max = starting_address;
                }
                pthread_mutex_unlock(&mutex1);  
}


void sum_the_difference(unsigned char *starting_address, unsigned char *ending_address)
{
                unsigned char current_sum = 0;
		unsigned char current;
		unsigned char next;
		unsigned char absolute_value;

                for(unsigned char *current_address = starting_address; current_address < ending_address; current_address++)
                {
                        current = *current_address;
                        next = *(current_address + 1);
                        absolute_value = (unsigned char)abs(current - next);
                        current_sum += absolute_value;
                }

                // lock the value being share 
                pthread_mutex_lock(&mutex1);
		if(current_sum < sum_of_difference_value)
		{
                	sum_of_difference_value = current_sum;
			addr_of_sum_of_difference_value = starting_address;
		}
                pthread_mutex_unlock(&mutex1);
}

double compute_mean(unsigned char *starting_address, unsigned char * ending_address)
{
	double mean = 0.0;
	for(unsigned char *current_address = starting_address; current_address < ending_address; current_address++)
	{
		mean += *current_address;
	}
	mean /= 80.0;
	if( mean == 0.0)
		mean = 1.0;

	return mean;
}
void standard_deviation(unsigned char *starting_address, unsigned char *ending_address, double mean)
{
                double current_sum_deviation = 0;
                for(unsigned char *current_address = starting_address; current_address < ending_address; current_address++)
                {
			current_sum_deviation += ((double)*current_address - mean)*((double)*current_address - mean);
                }
		double standard_deviation = sqrt(current_sum_deviation/80.0);

                // lock the value being share
                pthread_mutex_lock(&mutex1);
		if(standard_deviation < best_standard_deviation)
		{
                	best_standard_deviation = standard_deviation;
			addr_of_best_standard_deviation = starting_address;
		}
                pthread_mutex_unlock(&mutex1);
}

double compute_mean_in_difference(unsigned char *starting_address, unsigned char* ending_address)
{
	double mean_in_difference = 0.0;
	for(unsigned char *current_address = starting_address; current_address < ending_address; current_address++)
		mean_in_difference += *current_address;

	mean_in_difference /= 80.0;
	
	if(mean_in_difference == 0.0)
		mean_in_difference = 1.0;

	return mean_in_difference;	
} 

void standard_deviation_in_difference(unsigned char *starting_address, unsigned char *ending_address, double mean_in_difference)
{
                double current_deviation_in_difference = 0;
		double current;
		double next;
                for(unsigned char *current_address = starting_address; current_address < ending_address; current_address++)
                {
                        current = (double)*current_address;
                        next = (double)*(current_address + 1);
                        current_deviation_in_difference += (current - next - mean_in_difference)*(current - next - mean_in_difference);
                }

		double standard_deviation_in_difference = sqrt(current_deviation_in_difference/80.0);
                // lock the value being share
                pthread_mutex_lock(&mutex1);
		if(standard_deviation_in_difference < best_standard_deviation_in_difference)
                {
			best_standard_deviation_in_difference = standard_deviation_in_difference;
			addr_of_best_standard_deviation_in_difference = starting_address;
		}
                pthread_mutex_unlock(&mutex1);
}


void *start_routine(void *argv)
{
        //printf("start = %u\nend = %u\n", ((struct thread_info*)argv)->start, ((struct thread_info*)argv)->end);

        for(unsigned char* i = ((struct thread_info*)argv)->start + 40; i < ((struct thread_info*)argv)->end; i += 1)
        {
		unsigned char *starting_address = i - 40;
		best_range_function(starting_address, i);
		absolute_max(starting_address, i);
		sum_the_difference(starting_address, i);
		double mean = compute_mean(starting_address, i);
		standard_deviation(starting_address, i, mean);
		double mean_in_difference = compute_mean_in_difference(starting_address, i);
		standard_deviation_in_difference(starting_address, i, mean_in_difference);
	}

}

void print_value(unsigned char *best_address)
{
unsigned char graph_array[8][40];
	for(int i = 0; i < 8; i++)
        	for(int j = 0; j < 40; j++)
                	graph_array[i][j] = ' ';

	for(int i = 0; i < 40; i++)
        	graph_array[best_address[i]/32][i] = 'x';

	for(int i = 0; i < 8; i++)
	{
		printf("| ");
        	for(int j = 0; j < 40; j++)
                	printf("%c", graph_array[i][j]);
        	printf("\n");
	}

printf("|_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _");
printf("\n");

}

int main(int argc, char** argv)
{

if(argc < 2)
{
printf("Error: need more arguments.\n");
exit(-1);
}

int fd = open(argv[1],O_RDONLY);
if(fd == -1)
{
printf("The File you have entered does not exist.\n");
exit(-1);
}

printf("Zihang Feng\n");
printf("jfeng9\n");


int nThread;

if(argc < 3 || atoi(argv[2]) == 0 || atoi(argv[2]) < 0)
	nThread = 1;
else		
	nThread = atoi(argv[2]);

// get the size from the given file
struct stat filestatus;
stat(argv[1], &filestatus);
int size_of_file = filestatus.st_size;
if(size_of_file == 0)
{
	printf("No file being load or the file is empty\n.");
	exit(-1);
}

if((addr = (unsigned char*)mmap(NULL, size_of_file, PROT_READ, MAP_PRIVATE, fd, 0)) == (unsigned char*)-1)
{
printf("mmap function causes problem.\n");
exit(-1);
}


int mod = size_of_file%nThread;
int chunk_size = size_of_file/nThread;
struct thread_info *thread_value = new struct thread_info[nThread];
pthread_t *thread_array = new pthread_t[nThread];

unsigned char *current = addr;

int start = 0, end = 0;

clock_t begin = clock();
 
for(int i = 0; i < nThread; i++)
{ 
	pthread_t n;			
	thread_value[i].start = addr + (i * chunk_size);
	thread_value[i].end = addr + ((i + 1) * chunk_size);
	if(i == nThread - 1)
		thread_value[i].end += mod;
	
	if(pthread_create(&thread_array[i], NULL, &start_routine, (void *)&thread_value[i]) != 0)
	{
		printf("Creating the thread causes problems.\n");
		exit(-1);
	}
}

for(int i = 0; i < nThread; i++)
{
	pthread_join(thread_array[i],NULL);
}


printf("best range valuen is %u\n", range);
printf("it's at address %u\n", addr_of_range - addr);
print_value(addr_of_range);
printf("\n");

printf("best max value is %u\n", best_max);
printf("it's at address %u\n", addr_of_best_max - addr);
print_value(addr_of_best_max);
printf("\n");

printf("sum of the difference is %u\n", sum_of_difference_value);
printf("it's at address %u\n", addr_of_sum_of_difference_value - addr);
print_value(addr_of_sum_of_difference_value);
printf("\n");

printf("the best standard deviation of the data is %f\n", best_standard_deviation);
printf("it's at address %u\n", addr_of_best_standard_deviation - addr);
print_value(addr_of_best_standard_deviation);
printf("\n");

printf("the best standard deviation in differece of the data is %f\n", best_standard_deviation_in_difference);
printf("it's at address %u\n", addr_of_best_standard_deviation_in_difference - addr);
print_value(addr_of_best_standard_deviation_in_difference);
printf("\n");

delete[] thread_value;
delete[] thread_array;

clock_t stop = clock();
double elapsed_secs = double(stop - begin) / CLOCKS_PER_SEC;

printf("The number of threads for this simuation is %d\n", nThread);
printf("The amound of time it took is: %f\n", elapsed_secs);
return 0;
}
