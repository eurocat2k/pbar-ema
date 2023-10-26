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

const double PREDICT_WEIGHT = 0.4;

double predict_next(double last_prediction, double actual) {
  return ((last_prediction * (1.0 - PREDICT_WEIGHT)) + (actual * PREDICT_WEIGHT));
}

typedef struct {
  size_t bytes;
} dataentry;

void update_pbar(int p, statusinfo *sinfo) {
	fprintf(stdout, "\r[");
	int nleft = p;
	int numch = (PROG_BAR_LENGTH * p) / 100;
	for (int i = 0; i < PROG_BAR_LENGTH; i++) {
		if (numch-- > 0) {
			fprintf(stdout,"#");
		} else {
			fprintf(stdout, " ");
		}
	}
	fprintf(stdout, "] %3d %% *(bytes: %ld)", p, sinfo->total_bytes);
	fflush(stdout);
}

size_t got_data(dataentry *ent, statusinfo *sinfo) {
    size_t bytes = 0;
    for (size_t i = 0; i < ent->bytes; i++) {
		sinfo->total_bytes += i;
		sinfo->current_bytes = bytes;
		bytes = i;
		long urls_left = sinfo->total_urls - sinfo->url_so_far;
		long estimated_current = sinfo->exp_bytes_per_url;
		if (sinfo->current_bytes > sinfo->exp_bytes_per_url) {
				estimated_current = sinfo->current_bytes * 4 / 3;
		}
		double guess_next_prediction =
			predict_next(sinfo->exp_bytes_per_url, estimated_current);
		long estimated_total =
			sinfo->total_bytes +
			(estimated_current - sinfo->current_bytes) +
			((urls_left - 1) * guess_next_prediction);

		long percentdone = sinfo->total_bytes * 100 / estimated_total;
		// usleep(120);
		update_pbar(percentdone, sinfo);
    }
    // long urls_left = sinfo->total_urls - sinfo->url_so_far;
    // long estimated_current = sinfo->exp_bytes_per_url;
    // if (sinfo->current_bytes > sinfo->exp_bytes_per_url) {
    //     estimated_current = sinfo->current_bytes * 4 / 3;
    // }
    // double guess_next_prediction = predict_next(sinfo->exp_bytes_per_url, estimated_current);
    // long estimated_total = sinfo->total_bytes + (estimated_current - sinfo->current_bytes) + ((urls_left - 1) * guess_next_prediction);

    // long percentdone = sinfo->total_bytes * 100 / estimated_total;
	// // usleep(120);
    // update_pbar(percentdone, sinfo);
    return bytes;
}

bool get_data_entry(dataentry *ent, statusinfo *sinfo) {
    got_data(ent, sinfo);
    usleep(500000);
    return true;
}

int main() {
	dataentry data[] = {{.bytes = 21234}, {.bytes = 34324}, {.bytes = 22134}, {.bytes =114435}, {.bytes = 27643}};
	statusinfo sinfo = {0};
    int num_entries = sizeof(data) / sizeof(data[0]);
    sinfo.total_urls = num_entries - 1;
    sinfo.exp_bytes_per_url = 10000000;
    update_pbar(0, &sinfo);
    for (int i = 0; i < num_entries; i++) {
        // update_pbar(i);
        // usleep(20000);
        sinfo.current_bytes = 0;
		get_data_entry(&data[i], &sinfo);
		// update_pbar(((i + 1) * 100) / num_entries, &sinfo);
		sinfo.url_so_far++;
		sinfo.exp_bytes_per_url = predict_next(sinfo.exp_bytes_per_url, sinfo.current_bytes);
    }
    fprintf(stdout, " Done\n");
	return 0;
}
