// Written by Edwin Villanueva
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

// intention: run `curl -4 https://api.ipify.org` every 10 minutes
// - if this command returns output that is different than IP saved to file,
//   - send the new IP to cloudflare
int main(int agrc, char** argv) {

  // init
  char* cfToken = getenv("CF_API_TOKEN");
  if (cfToken == NULL) {
    printf("CF_API_TOKEN not set!\n\n");
    return 1;
  }

  // to be filled in by other values
  char *cfRequestTemplate = getenv("CF_UPDATE_REQUEST");
  if (cfRequestTemplate == NULL) {
    printf("CF_UPDATE_REQUEST not set!\n\n");
    return 1;
  }

  CURL *curl;
  CURLcode result;

  // get current public IP
  char prevIp[16];
  char currentIp[16];
  char tempJSON[512];
  char command[30];
  char* cfRecordRequest = getenv("CF_CURRENT_IP");

  strcpy(command, "curl -4 https://api.ipify.org");
  FILE *output = popen(command, "r"); 

  cJSON *ipJSON = NULL; // parse json
  FILE *cfIP = popen(cfRecordRequest, "r");

  fgets(currentIp, 16, output);
  fgets(tempJSON, 512, cfIP);

  ipJSON = cJSON_Parse(tempJSON);
  if (ipJSON == NULL) {
    printf("CloudFlare DNS retrieval has failed...");
    return -1;
  }
  cJSON *resultLayer = cJSON_GetObjectItemCaseSensitive(ipJSON, "result");
  cJSON* cfResponseItem = cJSON_GetObjectItemCaseSensitive(resultLayer, "content");
  if (!cJSON_IsString(cfResponseItem) || cfResponseItem->valuestring == NULL) {
    printf("IP value not found on CloudFlare.");
    return -2;
  }

  // remove newline characters from both IP addresses
  snprintf(prevIp, sizeof(prevIp), "%s", cfResponseItem->valuestring);
  currentIp[strcspn(currentIp, "\r\n")] = '\0';
  prevIp[strcspn(cfResponseItem->valuestring, "\r\n")] = '\0';

  // check whether IPs match
  if (strcmp(currentIp, prevIp) == 0) {
    printf("both IPs are the same!");
    pclose(output);
    return 0;  //  - return safely
  }

  // if IPs do NOT match
  else {
    char* cfRequest = getenv("CF_UPDATE_REQUEST");
    if (cfRequest == NULL) {
      printf("CF_UPDATE_REQUEST not set!\n\n");
      return 1;
    };
    // add new IP to the request
    char request[2048];

    int written = snprintf(
        request,
        sizeof(request),
        "%s%s\\\"}\"",
        cfRequest,
        currentIp
    );

    if (written < 0 || (size_t)written >= sizeof(request)) {
        fprintf(stderr, "Request buffer too small!\n");
        return 1;
    }

    if (setenv("PUBLIC_IP", currentIp, 1) != 0) {
        perror("setenv");
        return 1;
    }

    int status = system(cfRequest); // send cloudflare the new IP
    FILE *overwriteIp = fopen("ip.txt", "w");
    fprintf(overwriteIp, "%s", currentIp);
    fclose(overwriteIp);
  }

  // close files and return
  pclose(output);
  return 0;
}
