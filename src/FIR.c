#include "wav.h"
#include <math.h>

#define PI 3.14159265358979

/*
	Function: convolve 		- For convolving a 1-D array of samples with a filter

	inputs: data 			- array of samples 
			filter  		- array of filter values
			data_size		- number of samples
			filter_size		- filter size
	output: result			- array of processed samples
*/
int16_t *convolve(const int16_t *data, const float *filter, int data_size, int filter_size){

	int len 		= data_size + filter_size - 1;
	int16_t *result = (int16_t*)malloc(len * sizeof(int16_t));

	for(int i=0; i<len; i++){

		result[i] = 0;

		for(int j=0; j<filter_size; j++){
			if((i-j < 0) || (i-j>data_size)) continue;

			result[i] += (int16_t)(data[i-j] * filter[j]);
		}
	}
	return result;
}


/*
	Function 1: lowpass_filter	- For computing the nth lowpass filter weight
	Function 2: highpass_filter	- For computing the nth highpass filter weight

	inputs: m 			- filter order 
			n  			- position of weight to compute
			ft			- normalised transition frequency
	output: result		- nth filter weight
*/
float lowpass_filter(float m, int n, float ft){
	if(n == (int)(m/2)) return 2 * ft;
	return sin(2 * PI * ft * (n - (m/2.))) / (float)(PI * (n - (m/2.)));
}

float highpass_filter(float m, int n, float ft){
	if(n == (m/2)) return 1 - (2 * ft);
	return -1 * sin(2 * PI * ft * (n - (m/2.))) / (float)(PI * (n - (m/2.)));
}

/*
	Function: compute_filter	- For computing the filter's weights based on filter type and window type

	inputs: F_cut_off 			- Cut-Off Frequency 
			F_sampling  		- Sampling Frequency
			filter_size			- filter size
			filter_type			- "-lowpass" or "-highpass"
			window_type			- "-rectangular", "-hamming", "-hanning", or "-blackman"
	output: result				- array of filter weight
*/
float *compute_filter(int F_cut_off, int F_sampling, int filter_size, const char *filter_type, const char *window_type){

	float *filter 	= (float*)malloc(filter_size * sizeof(float));
	float m 		= filter_size - 1;	// m is the filter order
	float F_t 		= F_cut_off / (float)F_sampling;

	for(int n=0; n<filter_size; n++){

		//Default Filter type - lowpass filter
		if (strcmp(filter_type, "-highpass") == 0) 	filter[n] = highpass_filter(m, n, F_t);
		else 										filter[n] = lowpass_filter(m, n, F_t);

		//Window Type
		if(strcmp(window_type, "-hamming") == 0){	//For Hamming Window Type
			filter[n] *= (0.54 - (0.46*cos(2 * PI * n / m)));
		}
		else if(strcmp(window_type, "-hanning") == 0){	//For Hanning Window Type
			filter[n] *= (0.5 - (0.5*cos(2 * PI * n / m)));
		}
		else if(strcmp(window_type, "-blackman") == 0){ //For Blackman Window Type
			filter[n] *= ((0.42 - (0.5*cos(2 * PI * n / m))) + (0.08*cos(4 * PI * n / m)));
		}
		else{// Use default window type - Rectangular window
			//filter[n] *= 1;
		}
	}

	return filter;
}


/*
	Function: FIR_Filter 	- FIR Filter Function

	inputs: audio 			- wav datatype 
			cut_off 		- cut-off frequency
			filter_length	- length of the filter
			type			- Window type - Rectangular, Hamming Window, etc.
	output: result			- WAV datatype
*/
WAV FIR_Filter(WAV audio, int cut_off, int filter_length, const char *filter_type, const char *window_type){
	WAV result = audio;
	
	//filter size constraint
	int filter_size = filter_length; 
	if (filter_length%2 == 0) filter_size++;

	//Computing Weights of Filter
	float *filter = compute_filter(cut_off, result.header.sample_rate, filter_size, filter_type, window_type);

	//Convolution of data and filter
	result.samples = convolve(result.samples, filter, result.no_samples, filter_size);
	result.no_samples += filter_size - 1;
	return result;
}

//main function
int main(int argc, char *argv[]){

	//default values
	char *audio_file 	= "wavfiles/Test0.wav";	// default wav file path
	char *filter_type	= "-lowpass";			// default filter type
	int cut_off_freq 	= 22000;				// default cut off frequency
	int filter_size 	= 21;					// default filter length
	char *window_type	= "-rectangular";		// default window type

	if(argc >= 2){
		audio_file		= argv[1];
		filter_type 	= argv[2];
		cut_off_freq 	= atoi(argv[3]);
		filter_size		= atoi(argv[4]);
		window_type 	= argv[5];
	}

	WAV wav = read_wav_file(audio_file);

	WAV filtered_wav = FIR_Filter(wav, cut_off_freq, filter_size, filter_type, window_type);

	save_wav_file(filtered_wav, "output.wav");
}