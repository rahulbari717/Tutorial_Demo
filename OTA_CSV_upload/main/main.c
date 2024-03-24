#include <stdio.h>
#include "esp_crt_bundle.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "s3_auth_header.h"
#include "protocol_examples_common.h"
#include "esp_https_ota.h"

static char *TAG = "AWS S3";

#define AWS_ACCESS_KEY "AKIAU6GDXX5NFKLTXEN2"                        // your IAM access key
#define AWS_ACCESS_SECRET "wK58HKjDxph0TAyPMnRG7ZKIaC1rdIGjxnwnSGDw" // your IAM access secret

#define URL "https://my-esp32-ota-bucket.s3.ap-south-1.amazonaws.com/Test.csv" // your file URL to create and read
#define URL_CSV "https://my-esp32-ota-bucket.s3.ap-south-1.amazonaws.com/rahul.csv"
#define CSV_CANONICAL_URI "/rahul.csv" // your file name

#define HOST "my-esp32-ota-bucket.s3.amazonaws.com" // your bucket host
#define REGION "ap-south-1"                         // your bucket region
#define CANONICAL_URI "/Test.csv"                   // your file name

#define OTA_URL "https://my-esp32-ota-bucket.s3.ap-south-1.amazonaws.com/hello-world.bin" // once you compile this code, upload the binary to your bucket and change this URL for OTA
#define OTA_CANONICAL_URI "/hello-world.bin"                                              // the name of the binary file for OTA

#define file_path "files/rahul.csv"
esp_err_t on_client_data(esp_http_client_event_t *evt)
{
    if (evt->event_id == HTTP_EVENT_ON_DATA)
        ESP_LOGI(TAG, "Length=%d data %.*s\n", evt->data_len, evt->data_len, (char *)evt->data);
    return ESP_OK;
}

/************************ Wifi connection  *******************************/
void wifi_connect()
{
    nvs_flash_init();
    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
    ESP_LOGI(TAG, "Wifi is connected");
}
/************************ printmyfile ***************************/
void printmyfile()
{
    extern const unsigned char rahul[] asm("_binary_rahul_csv_start");
    printf("rahu = %s\n", rahul);
}
/************************ UPLOAD UPDATE A FILE ***************************/
/* void update_file()
{
    esp_http_client_config_t esp_http_client_config = {
        .url = URL,
        .method = HTTP_METHOD_PUT,
        .event_handler = on_client_data,
        .crt_bundle_attach = esp_crt_bundle_attach};
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);

    char *content = "Hello AWS From ESP32!";
    s3_params_t s3_params = {
        .access_key = AWS_ACCESS_KEY,
        .secret_key = AWS_ACCESS_SECRET,
        .host = HOST,
        .region = REGION,
        .canonical_uri = CANONICAL_URI,
        .content = content,
        .method = "PUT",
        .should_get_time = true};

    http_client_set_aws_header(client, &s3_params);
    esp_http_client_set_post_field(client, content, strlen(content));
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET status = %d, content_length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    ESP_LOGI(TAG, "Uplaod completed...");
    esp_http_client_cleanup(client);
}
 */
/************************ UPDATE CSV FILE ***************************/

// Function to read the content of a file
char *read_file_content(const char *filename, size_t *filesize)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Failed to open file %s\n", filename);
        return NULL;
    }

    // Get the file size
    fseek(fp, 0, SEEK_END);
    *filesize = ftell(fp);
    rewind(fp);

    // Allocate memory for file content
    char *content = (char *)malloc(*filesize + 1);
    if (content == NULL)
    {
        fclose(fp);
        printf("Failed to allocate memory for file content\n");
        return NULL;
    }

    // Read file content into the buffer
    size_t bytes_read = fread(content, 1, *filesize, fp);
    if (bytes_read != *filesize)
    {
        fclose(fp);
        free(content);
        printf("Failed to read file %s. Read %zu bytes instead of expected %zu\n", filename, bytes_read, *filesize);

        return NULL;
    }
    content[*filesize] = '\0'; // Null-terminate the string

    fclose(fp);
    return content;
}

void upload_csv_to_s3()
{
    // Read the content of the CSV file
    size_t filesize;
    char *content = read_file_content("files/rahul.csv", &filesize);
    if (content == NULL)
    {
        printf("Failed to read CSV file\n");
        return;
    }

    // Print the content for debugging
    printf("CSV content:\n%s\n", content);

    // Configure the HTTP client
    esp_http_client_config_t esp_http_client_config = {
        .url = URL_CSV,
        .method = HTTP_METHOD_PUT,
        .event_handler = on_client_data,           // Event handler function
        .crt_bundle_attach = esp_crt_bundle_attach // Certificate bundle to attach
    };
    esp_http_client_handle_t client = esp_http_client_init(&esp_http_client_config);
    if (client == NULL)
    {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return;
    }
    ESP_LOGI(TAG, "HTTP client initialized");

    ESP_LOGI(TAG, "Content: %s", content);

    // Set up S3 parameters
    s3_params_t s3_params = {
        .access_key = AWS_ACCESS_KEY,
        .secret_key = AWS_ACCESS_SECRET,
        .host = HOST,
        .region = REGION,
        .canonical_uri = CSV_CANONICAL_URI,
        .content = content,
        .method = "PUT",        // HTTP method
        .should_get_time = true // Whether to fetch time
    };

    ESP_LOGI(TAG, "S3 parameters configured");

    // Set AWS header
    http_client_set_aws_header(client, &s3_params);
    ESP_LOGI(TAG, "AWS header set");

    // Set POST field
    esp_http_client_set_post_field(client, content, strlen(content));
    ESP_LOGI(TAG, "POST field set");

    // Perform HTTP request
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP PUT request successful");
        ESP_LOGI(TAG, "HTTP status code: %d", esp_http_client_get_status_code(client));
        ESP_LOGI(TAG, "Content length: %lld", esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP PUT request failed: %s", esp_err_to_name(err));
    }
    ESP_LOGI(TAG, "HTTP request completed");
    // Free the dynamically allocated memory
    free(content);
    // Cleanup HTTP client
    esp_http_client_cleanup(client);
    ESP_LOGI(TAG, "HTTP client cleaned up");
}

void app_main(void)
{
    wifi_connect();

    /*     printf("update file\n");
        update_file(); */
    printf("upload csv file\n");
    // Call upload_csv_to_s3() with the file path of the CSV file
    upload_csv_to_s3();
    printmyfile();
}
