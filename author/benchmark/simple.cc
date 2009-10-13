#include "../../nanowww.h"
#include <iostream>
#include <curl/curl.h>

class Benchmark {
private:
    struct timeval start_at, end_at;
public:
    Benchmark() {
        gettimeofday(&start_at, NULL);
    }
    void end() {
        gettimeofday(&end_at, NULL);
    }
    double elapsed() {
        return end_at.tv_sec + end_at.tv_usec / 1000000.0
                - (start_at.tv_sec + start_at.tv_usec / 1000000.0);
    }
};

typedef struct {
  char* data;   // response data from server
  size_t size;  // response size of data
} MEMFILE;

MEMFILE*
memfopen() {
  MEMFILE* mf = (MEMFILE*) malloc(sizeof(MEMFILE));
  mf->data = NULL;
  mf->size = 0;
  return mf;
}

void
memfclose(MEMFILE* mf) {
  if (mf->data) free(mf->data);
  free(mf);
}

size_t
memfwrite(char* ptr, size_t size, size_t nmemb, void* stream) {
  MEMFILE* mf = (MEMFILE*) stream;
  int block = size * nmemb;
  if (!mf->data)
    mf->data = (char*) malloc(block);
  else
    mf->data = (char*) realloc(mf->data, mf->size + block);
  if (mf->data) {
    memcpy(mf->data + mf->size, ptr, block);
    mf->size += block;
  }
  return block;
}

char*
memfstrdup(MEMFILE* mf) {
  char* buf = (char*)malloc(mf->size + 1);
  memcpy(buf, mf->data, mf->size);
  buf[mf->size] = 0;
  return buf;
}

void bench_curl(const char *url) {
    char error[256];
    MEMFILE* mf = memfopen();
    CURL * curl = curl_easy_init();
    assert(curl);
    assert(curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &error) == 0);
    assert(curl_easy_setopt(curl, CURLOPT_URL, url) == 0);
    assert(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, memfwrite) == 0);
    assert(curl_easy_setopt(curl, CURLOPT_WRITEDATA, mf) == 0);
    if (curl_easy_perform(curl) == CURLE_OK) {
    } else {
        std::cout << error << std::endl;
    }
    curl_easy_cleanup(curl);
}

void bench_nanowww(const char *url) {
    nanowww::Client www;
    nanowww::Response res;
    assert(www.send_get(&res, url));
}

void dump_version() {
    printf("-- version\n");
    printf("nanowww: %s\n", nanowww::version());
    printf("libcurl: %s\n", curl_version_info(CURLVERSION_NOW)->version);
}

int main(int argc, char **argv) {
    assert(argc == 2 && "Usage: a.out port");
    dump_version();

    const int N = 10000;

    const char *url = argv[1];

    printf("-- result\n");
    {
        Benchmark b;
        for (int i=0; i<N; i++) {
            bench_curl(url);
        }
        b.end();
        std::cout << "libcurl: " << b.elapsed() << std::endl;
    }

    {
        Benchmark b;
        for (int i=0; i<N; i++) {
            bench_nanowww(url);
        }
        b.end();
        std::cout << "nanowww: " << b.elapsed() << std::endl;
    }
}

