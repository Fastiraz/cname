/*
 ===============================================================================
 Name        : cname.c
 Author      : Fastiraz
 Version     : 1.1
 Copyright   : Your copyright notice
 Description : A simple tool that allow us to find usernames across social networks.
 Compile     : gcc -o cname cname.c -lcurl
 Usage       : ./cname -n Fastiraz -w data.json
 ===============================================================================
 */

/*============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
/*============================================================================*/

/*============================================================================*/
#define MAX_WORD_LEN 100
#define MAX_URL_LEN 200
/*============================================================================*/

/*============================================================================*/
void remove_n(char *str);
void banner();
void info();
void start();
void end(int *count);
void help();
void geturl(char *datalist, char ***list, int *list_size, char *name);
char *delchar(char *line);
int check(char *url, int *count);
/*============================================================================*/

/*============================================================================*/
int main(int argc, char **argv)
{
    // Set default values
    char *name = "Fastiraz";
    char *datalist = "data.json";
    char **list = NULL;
    int list_size = 0;
    int count = 0;

    // Parse the command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            name = argv[++i];
        } else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
            datalist = argv[++i];
        } else {
            help();
            return 1;
        }
    }

    banner();
    info(name, datalist);
    start();

    /* C0de */
    geturl(datalist, &list, &list_size, name);
    for(int i=0; i < list_size; i++){
        remove_n(list[i]);
        check(list[i], &count);
    }
    free(list);
    end(&count);

    return 0;
}
/*============================================================================*/

/*============================================================================*/
void remove_n(char *str){
    int len = strlen(str);
    // Loop through the string and replace \n with \0
    for (int i = 0; i < len; i++) {
        if (str[i] == '\n') {
            str[i] = '\0';
            break;
        }
    }
}
/*============================================================================*/

/*============================================================================*/
void banner(){
  printf("\n\n===============================================================\n");
  printf("CName v1.0.1\n");
  printf("by Fastiraz\n");
  printf("===============================================================\n");
}
/*============================================================================*/

/*============================================================================*/
void info(char *name, char *wdl){
  printf("[+] Name:\t\t\t%s\n", name);
  printf("[+] Threads:\t\t\t1\n");
  printf("[+] Datalist:\t\t\t%s\n", wdl);
  printf("[+] Status codes:\t\t200,204,301,302,307,401,403\n");
  printf("[+] User Agent:\t\t\tCName/1.0.0\n");
  printf("[+] Timeout:\t\t\t10s\n");
}
/*============================================================================*/

/*============================================================================*/
void start(){
  // Get current calendar time
  time_t current_time = time(NULL);

  // Convert to local time
  struct tm* local_time = localtime(&current_time);

  // Create a buffer to hold the date and time string
  char date_time_str[32];

  // Use strftime to format the date and time
  strftime(date_time_str, 32, "%Y/%m/%d %H:%M:%S", local_time);

  // Print the date and time
  printf("===============================================================\n");
  printf("%s Starting CName...\n", date_time_str);
  printf("===============================================================\n");
}
/*============================================================================*/

/*============================================================================*/
void end(int *count){
  // Get current calendar time
  time_t current_time = time(NULL);

  // Convert to local time
  struct tm* local_time = localtime(&current_time);

  // Create a buffer to hold the date and time string
  char date_time_str[32];

  // Use strftime to format the date and time
  strftime(date_time_str, 32, "%Y/%m/%d %H:%M:%S", local_time);

  // Print the date and time
  printf("\n===============================================================\n");
  printf("%s Finished [%d tested]\n", date_time_str, *count);
  printf("===============================================================\n\n\n");
}
/*============================================================================*/

/*============================================================================*/
void help(){
  printf("\nFLAGS:\n");
  printf("\t-v : Verbose output (errors)\n");
  printf("\t-w : Custom json file that contain data to test\n");
  printf("\nEXAMPLES:\n");
  printf("\tUsage :\t./cname Fastiraz -j data.json\n");
}
/*============================================================================*/

/*============================================================================*/
void geturl(char *datalist, char ***list, int *list_size, char *name){
    char *start, *end;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE *fp = fopen(datalist, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }
    while ((read = getline(&line, &len, fp)) != -1) {
        if(strstr(line, "\"url\":") != NULL){
            sprintf(strstr(line, "{}"), "%s\n", name); // Replace '{}' by the username
            for(int i=0; i<14; i++){ // For each characters before the url
                line[i] = ' '; // Replace char to get '              https://www.github.com/Fastiraz'
            }
            line = delchar(line); // Remove the space before url
            *list = (char**) realloc(*list, (*list_size + 1) * sizeof(char*)); // Allocate space for one more string
            (*list)[(*list_size)++] = strdup(line); // Add the new string to the array and increment size
        }
    }

    fclose(fp);
    if (line) {
        free(line);
    }
}
/*============================================================================*/

/*============================================================================*/
// Function to delete n characters
char *delchar(char *line){
    char ch = ' ';
    int len = strlen(line);
    for(int i=0; i<len; i++){
		if(line[i] == ch){
			for(int j=i; j<len; j++)
			{
				line[j] = line[j + 1];
			}
			len--;
			i--;	
		} 
	}
    return line;
}
/*============================================================================*/

/*============================================================================*/
int check(char *url, int *count){
  CURL* curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl) {
    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Set request method to HEAD
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    // Perform request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      fprintf(stderr, "%s\t | \x1B[35m%s\x1B[0m\n", curl_easy_strerror(res), url);
    } else {
      // Get status code
      long status_code;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
      if (status_code >= 200 && status_code < 300) {
        // Page exists
        printf("Username exists\t\t\t");
        printf(" | \033[0;32m%s\033[0m\n", url);
        (*count)++;
      } else {
        // Page does not exist
        printf("Username does not exist\t\t");
        printf(" | \033[0;31m%s\033[0m\n", url);
        (*count)++;
      }
    }

    // Clean up
    curl_easy_cleanup(curl);
  }
  return 0;
}
/*============================================================================*/
