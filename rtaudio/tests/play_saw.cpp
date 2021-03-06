/******************************************/
/*
  play_saw.c
  by Gary P. Scavone, 2001

  Play sawtooth waveforms of distinct frequency.
  Takes number of channels and sample rate as
  input arguments.  Uses blocking functionality.
*/
/******************************************/

#include "RtAudio.h"
#include <iostream>

/*
typedef signed long  MY_TYPE;
#define FORMAT RTAUDIO_SINT24
#define SCALE  2147483647.0

typedef char  MY_TYPE;
#define FORMAT RTAUDIO_SINT8
#define SCALE  127.0
*/

typedef signed short  MY_TYPE;
#define FORMAT RTAUDIO_SINT16
#define SCALE  32767.0

/*
typedef signed long  MY_TYPE;
#define FORMAT RTAUDIO_SINT32
#define SCALE  2147483647.0

typedef float  MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32
#define SCALE  1.0

typedef double  MY_TYPE;
#define FORMAT RTAUDIO_FLOAT64
#define SCALE  1.0
*/

#define BASE_RATE 0.005
#define TIME   4.0

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  std::cout << "\nuseage: play_saw N fs <device>\n";
  std::cout << "    where N = number of channels,\n";
  std::cout << "    fs = the sample rate,\n";
  std::cout << "    and device = the device to use (default = 0).\n\n";
  exit(0);
}

int main(int argc, char *argv[])
{
  int chans, fs, buffer_size, device = 0;
  long frames, counter = 0, i, j;
  MY_TYPE *buffer;
  RtAudio *audio = 0;
  double *data = 0;

  // minimal command-line checking
  if (argc != 3 && argc != 4 ) usage();

  chans = (int) atoi(argv[1]);
  fs = (int) atoi(argv[2]);
  if ( argc == 4 )
    device = (int) atoi(argv[3]);

  // Open the realtime output device
  buffer_size = 512;
  try {
    audio = new RtAudio(device, chans, 0, 0,
                        FORMAT, fs, &buffer_size, 4);
  }
  catch (RtError &error) {
    error.printMessage();
    exit(EXIT_FAILURE);
  }

  frames = (long) (fs * TIME);
  data = (double *) calloc(chans, sizeof(double));

  try {
    buffer = (MY_TYPE *) audio->getStreamBuffer();
    audio->startStream();
  }
  catch (RtError &error) {
    error.printMessage();
    goto cleanup;
  }

  std::cout << "\nPlaying for " << TIME << " seconds ... buffer size = " << buffer_size << "." << std::endl;
  while (counter < frames) {
    for (i=0; i<buffer_size; i++) {
      for (j=0; j<chans; j++) {
        buffer[i*chans+j] = (MY_TYPE) (data[j] * SCALE);
        data[j] += BASE_RATE * (j+1+(j*0.1));
        if (data[j] >= 1.0) data[j] -= 2.0;
      }
    }

    try {
      audio->tickStream();
    }
    catch (RtError &error) {
      error.printMessage();
      goto cleanup;
    }

    counter += buffer_size;
  }

  try {
    audio->stopStream();
  }
  catch (RtError &error) {
    error.printMessage();
  }

 cleanup:
  audio->closeStream();
  delete audio;
  if (data) free(data);

  return 0;
}
