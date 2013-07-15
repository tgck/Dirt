#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <lo/lo.h>
#include <sys/types.h>
#include <math.h>

#include "server.h"
#include "audio.h"

void error(int num, const char *m, const char *path);

int trigger_handler(const char *path, const char *types, lo_arg **argv,
                    int argc, void *data, void *user_data);

int generic_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data);

/**/

void error(int num, const char *msg, const char *path) {
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
}

/**/

int generic_handler(const char *path, const char *types, lo_arg **argv,
		    int argc, void *data, void *user_data) {
    int i;
    
    printf("path: <%s>\n", path);
    for (i=0; i<argc; i++) {
      printf("arg %d '%c' ", i, types[i]);
      lo_arg_pp(types[i], argv[i]);
      printf("\n");
    }
    printf("\n");

    return 1;
}

/**/

int kriole_handler(const char *path, const char *types, lo_arg **argv,
                   int argc, void *data, void *user_data) {

  double when = (double) argv[0]->i + ((double) argv[1]->i / 1000000.0);
  float duration = argv[2]->f;
  float pitch_start = argv[3]->f;
  float pitch_stop = argv[4]->f;
  
  audio_kriole(when, duration, pitch_start, pitch_stop);

  return(0);
}

/**/

int play_handler(const char *path, const char *types, lo_arg **argv,
                 int argc, void *data, void *user_data) {

  /* lo_timetag ts = lo_message_get_timestamp(data); */

  double when = (double) argv[0]->i + ((double) argv[1]->i / 1000000.0);

  char *sample_name = strdup((char *) argv[2]);

  float offset = argv[3]->f;
  float start = argv[4]->f;
  float end  = argv[5]->f;
  float speed  = argv[6]->f;
  float pan  = argv[7]->f;
  float velocity  = argv[8]->f;
  char *vowel_s = (char *) argv[9];
  float cutoff = argv[10]->f;
  float resonance = argv[11]->f;
  float accellerate = argv[12]->f;
  float shape = argv[13]->f;
  int kriole_chunk = argv[14]->i;
  
  int vowelnum = -1;
  
  switch(vowel_s[0]) {
  case 'a': case 'A': vowelnum = 0; break;
  case 'e': case 'E': vowelnum = 1; break;
  case 'i': case 'I': vowelnum = 2; break;
  case 'o': case 'O': vowelnum = 3; break;
  case 'u': case 'U': vowelnum = 4; break;
  }
  //printf("vowel: %s num: %d\n", vowel_s, vowelnum);

  audio_play(when,
             sample_name,
             offset,
             start, 
             end,
             speed,
             pan,
             velocity,
             vowelnum,
             cutoff,
             resonance,
             accellerate,
             shape,
             kriole_chunk
             );
  return 0;
}

/**/

extern int server_init(void) {
  lo_server_thread st = lo_server_thread_new("7771", error);

  lo_server_thread_add_method(st, "/play", "iisffffffsffffi",
                              play_handler, 
                              NULL
                             );

  lo_server_thread_add_method(st, "/kriole", "iifff",
                              kriole_handler, 
                              NULL
                             );
  lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);
  lo_server_thread_start(st);
  
  return(1);
}

extern void osc_send_pitch(float starttime, unsigned int chunk, 
			   float pitch, float flux, float centroid) {
  static lo_address t = NULL;
  static int pid = 0;
  if (t == NULL) {
    t = lo_address_new(NULL, "6010");
  }
  if (pid == 0) {
    pid = (int) getpid();
  }
  //printf("send [%d] %f\n", chunk, pitch);
  // pid, starttime, chunk, v_pitch, v_flux, v_centroid
  lo_send(t, "/chunk", "ififff", 
          pid,
          starttime,
          (int) chunk,
          pitch,
          flux,
	  centroid
          );
  
}


extern void osc_send_play(double when, int lowchunk, float pitch, float flux, float centroid) {
  static lo_address t = NULL;
  static int pid = 0;
  if (t == NULL) {
    t = lo_address_new(NULL, "6010");
  }
  if (pid == 0) {
    pid = (int) getpid();
  }
  printf("play [%d] %f\n", lowchunk, pitch);
  // pid, starttime, chunk, v_pitch, v_flux, v_centroid
  lo_send(t, "/play", "iiiifff", 
          pid,
          (int) when,
          (int) ((when - floor(when)) * 1000000.0),
          lowchunk,
          pitch,
          flux,
	  centroid
          );
  
}

