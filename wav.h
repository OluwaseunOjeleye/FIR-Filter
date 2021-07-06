#ifndef WAVFILE_H
#define WAVFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct wav_hdr{
    char    file_id[4]; 				//"RIFF"
    int32_t file_size; 		
    char    file_type[4]; 				//"WAVE"
    char    fmt_id[4]; 				//"fmt "
    int32_t fmt_size; 	
    int16_t audio_format;
    int16_t num_channels;
    int32_t sample_rate;
    int32_t byte_rate;
    int16_t block_align;
    int16_t bits_per_sample;
} Header;

typedef struct chunk_hdr{
	char    id[4];					//"data"
    int32_t data_size;
} Chunk;

typedef struct wav{
	Header header;			
    Chunk  chunk;

    int16_t *samples;
    int     no_samples;
} WAV;


WAV read_wav_file(const char* fileName){
	printf("====== WAV Input File Info ======\n");
    FILE *f = fopen(fileName, "rb");

    //Reading WAV Header
    WAV wav_file;
    fread(&wav_file.header, sizeof(wav_file.header), 1, f);
    printf("Sample Rate: %dHz\n", wav_file.header.sample_rate);

    //Reading Data Chunk
    while (1){
        fread(&wav_file.chunk, sizeof(wav_file.chunk), 1, f);
        if (*(int32_t*)&wav_file.chunk.id == 0x61746164) break;
        fseek(f, wav_file.chunk.data_size, SEEK_CUR);	//skip chunk data bytes
    }

    int sample_size     = wav_file.header.bits_per_sample / 8;
    wav_file.no_samples = wav_file.chunk.data_size * 8 / wav_file.header.bits_per_sample;	//Number of samples
    printf("No of samples: %d\n", wav_file.no_samples);

    wav_file.samples    = (int16_t*)malloc(wav_file.no_samples * sizeof(int16_t));
    memset(wav_file.samples, 0, sizeof(int16_t) * wav_file.no_samples);

    //Reading Data
    for (int i=0; i < wav_file.no_samples; i++){
        fread(&wav_file.samples[i], sample_size, 1, f);
    }

    fclose(f);

    return wav_file;
}

void save_wav_file(WAV wav, const char* filename){
    printf("====== WAV Output File Info ======\n");
    printf("Sample Rate: %dHz\n", wav.header.sample_rate);
    printf("No of samples: %d\n", wav.no_samples);

    FILE *f = fopen(filename, "wb");
	if(!f) return;

	fwrite(&wav.header,sizeof(wav.header), 1, f);
	fwrite(&wav.chunk,sizeof(wav.chunk), 1, f);

	fwrite(wav.samples, sizeof(int16_t), wav.no_samples, f);

	int file_length = ftell(f);
	int data_length = file_length - (sizeof(wav.header) + sizeof(wav.chunk));
	fseek(f, (sizeof(wav.header) + sizeof(wav.chunk)) - sizeof(int), SEEK_SET);
	fwrite(&data_length, sizeof(data_length), 1, f);

	int riff_length = file_length - 8;
	fseek(f, 4, SEEK_SET);
	fwrite(&riff_length, sizeof(riff_length), 1, f);


	fclose(f);
    return;
}

#endif