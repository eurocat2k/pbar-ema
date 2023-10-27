#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

const int PROG_BAR_LENGTH = 30;

typedef struct {
  long total_bytes;
  long total_expected;
  double exp_bytes_per_url;
  long current_bytes;
  long url_so_far;
  long total_urls;
} statusinfo;

typedef struct {
  size_t bytes;
} dataentry;

const double PREDICT_WEIGHT = 0.3;

void update_pbar(int p, statusinfo *sinfo);
double predict_next(double last_prediction, double actual);
size_t got_data(dataentry *ent, statusinfo *sinfo);

double predict_next(double last_prediction, double actual) {
  double predict = ((last_prediction * (1.0 - PREDICT_WEIGHT)) + (actual * PREDICT_WEIGHT));
#ifdef DEBUG
  fprintf(stdout, "  prediction> last: %f, actual: %f, new: %f\n", last_prediction, actual, predict);
#endif
  return predict;
}

#ifndef DEBUG
void update_pbar(int p, statusinfo *sinfo) {
	printf("\r[");
	int nleft = p;
	int numch = (PROG_BAR_LENGTH * p) / 100;
	for (int i = 0; i < PROG_BAR_LENGTH; i++) {
		if (numch-- > 0) {
			printf("#");
		} else {
			printf(" ");
		}
	}
	printf("] %3d %% (bytes: %ld)", p, sinfo->total_bytes);
	fflush(stdout);
}
#else
void update_pbar(int p, statusinfo *sinfo) {return;}
#endif

size_t got_data(dataentry *ent, statusinfo *sinfo) {
    size_t bytes = 0;
    sinfo->current_bytes += ent->bytes;
    bytes += ent->bytes;
    long urls_left = sinfo->total_urls - sinfo->url_so_far;
    long estimated_current = sinfo->exp_bytes_per_url;
    if (sinfo->current_bytes > sinfo->exp_bytes_per_url) {
        estimated_current = sinfo->current_bytes * (4 / 3);
    }
    double guess_next_prediction = predict_next(sinfo->exp_bytes_per_url, estimated_current);
    long estimated_total = sinfo->total_bytes + (estimated_current - sinfo->current_bytes) + ((urls_left - 1) * guess_next_prediction);

    if (urls_left) {
      // align expected final
      double est_final = predict_next((double) sinfo->total_bytes, (double)estimated_total);
      if (est_final > sinfo->total_bytes) {
	estimated_total = (long)est_final;
      }
    }

    long percentdone = sinfo->total_bytes * 100 / estimated_total;
#ifdef DEBUG
    fprintf(stderr, " [[urls_left: %ld, total: %ld, current: %ld, expected: %ld, guessed: %f, estimated: %ld, progress: %d %%]]\n", urls_left, sinfo->total_bytes, sinfo->current_bytes, estimated_current, guess_next_prediction, estimated_total, percentdone);
#endif    
    usleep(100);
    update_pbar(percentdone, sinfo);
    return bytes;
}

bool get_data_entry(dataentry *ent, statusinfo *sinfo) {
    //sinfo->total_bytes += ent->bytes;
    sinfo->current_bytes = 0;
    dataentry byte = {.bytes = 1};
    for (size_t i = 0; i < ent->bytes; i++) {
      size_t size = got_data(&byte, sinfo);
      sinfo->total_bytes += size;
#ifdef DEBUG
      fprintf(stderr, "entry: {.bytes = %ld }\n", ent->bytes);
#else
      //usleep(10000);
#endif
    }
    return true;
}

int main() {
    dataentry data[] = {{.bytes = 21234}, {.bytes = 34324}, {.bytes = 22134}, {.bytes =114435}, {.bytes = 27643}};
    statusinfo sinfo = {0};
    int num_entries = sizeof(data) / sizeof(data[0]);
    sinfo.total_urls = num_entries;
    sinfo.exp_bytes_per_url = 1000;
    update_pbar(0, &sinfo);
    for (int i = 0; i < num_entries; i++) {
        sinfo.current_bytes = 0;
	sinfo.url_so_far;
	get_data_entry(&data[i], &sinfo);
	//sinfo.url_so_far++;
	sinfo.exp_bytes_per_url = predict_next(sinfo.exp_bytes_per_url, sinfo.current_bytes);
	sinfo.url_so_far += 1;
#ifdef DEBUG
	fprintf(stderr, "- sinfo: {.current_bytes = %ld, .url_so_far = %ld, .exp_bytes_per_url = %f}\n",sinfo.current_bytes, sinfo.url_so_far, sinfo.exp_bytes_per_url);
#endif
    }
    update_pbar(100, &sinfo);
    fprintf(stdout, " Done [total: %ld]\n", sinfo.total_bytes);
    return 0;
}
