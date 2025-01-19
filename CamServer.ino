#include <WiFi.h> // For Connecting ESP32-Cam to WiFi Network
#include "esp_camera.h" //For Pins and Configuration of ESP32-Cam's Camera Initialization.
#include "my_app_httpd.hpp"

// Wifi Credentials
// const char *ssid = "MJProject";
// const char *password = "HumanoidRobo";

const char *ssid = "Sabin2g@ClassicTech";
const char *password = "19161214Sabin@";

#define CAMERA_MODEL_AI_THINKER


//Camera Pins Definition for CAMERA_MODEL_AI_THINKER Module which has PSRAM taken from camera_pins.h library.
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27
//Following 8 pins will connect to camera for image data transfer.
#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22
#define LED_GPIO_NUM   4 // 4 for flash led or 33 for normal led




//Function Declarations
void setupWifi();
void startMyCameraServer();
// void 


//Setup Code that runs only once
void setup()
{
// Setup Serial Communication at rate of 115200 bauds/sec
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
// Welcome Text
  Serial.println("Welcome to Humanoid Robo Project");



//Camera Configuration
camera_config_t config; //Camera configuration structure object from esp_camera.h. It will hold all the configuration for the camera.
config.ledc_channel = LEDC_CHANNEL_0; //LED Control Channel that will be used for the camera's XCLK signal is set to channel 0.
config.ledc_timer = LEDC_TIMER_0; // LEDC Timer to be used is set ot timer 0.
// These 8 pins d0-d7 of the camera module is interfaces with the GPIO PINS y2-y9(as defined above) of the ESP32-Cam for Image Data Transfer.
config.pin_d0 = Y2_GPIO_NUM;
config.pin_d1 = Y3_GPIO_NUM;
config.pin_d2 = Y4_GPIO_NUM;
config.pin_d3 = Y5_GPIO_NUM;
config.pin_d4 = Y6_GPIO_NUM;
config.pin_d5 = Y7_GPIO_NUM;
config.pin_d6 = Y8_GPIO_NUM;
config.pin_d7 = Y9_GPIO_NUM;

config.pin_xclk = XCLK_GPIO_NUM;  // Specifies the GPIO pin for XCLK signal, which is the external clock signal for the camera.
config.pin_pclk = PCLK_GPIO_NUM;  // Specifies the GPIO pin for PCLK signal, which is the pixel clock signal used to synchronize data transfers.
config.pin_vsync = VSYNC_GPIO_NUM;  // Specifies the GPIO pin for VSYNC signal, which indicates the start of a new frame.
config.pin_href = HREF_GPIO_NUM;  //  Specifies the GPIO pin for HREF signal, which indicates the start of a new line of pixels.
// Following 2 lines assign the GPIO pins for the SCCB (Serial Camera Control Bus) SDA (data) and SCL (clock) signals, which are used for camera configuration.
config.pin_sccb_sda = SIOD_GPIO_NUM;
config.pin_sccb_scl = SIOC_GPIO_NUM;

config.pin_pwdn = PWDN_GPIO_NUM;  // GPIO pin for PWDN signal which powers the camera.
config.pin_reset = RESET_GPIO_NUM;  // GPIO pin for the reset signal, which is used to reset the camera.
config.xclk_freq_hz = 20000000; // Frequency of the XCLK signal in hertz. Here, it is set to 20 MHz.
config.frame_size = FRAMESIZE_UXGA; // The resolution of the frames captured by the camera. FRAMESIZE_UXGA corresponds to 1600x1200 pixels.

// The format of the image pixels. PIXFORMAT_JPEG is used for streaming JPEG images, while PIXFORMAT_RGB565 can be used for face detection/recognition.
config.pixel_format = PIXFORMAT_JPEG;  // For streaming
// config.pixel_format = PIXFORMAT_RGB565; // for face detection/recognition
config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;  // The frame grab mode. CAMERA_GRAB_WHEN_EMPTY means the camera will grab a new frame when the buffer is empty.
config.fb_location = CAMERA_FB_IN_PSRAM;  // The location of the frame buffer. CAMERA_FB_IN_PSRAM means the frame buffer will be stored in PSRAM (external RAM).
config.jpeg_quality = 12; // The quality of the JPEG images captured by the camera. The value ranges from 0 (highest quality) to 63 (lowest quality). Here, it is set to 12.
config.fb_count = 1;  // The number of frame buffers to be used. Here it is set to 1.

if (config.pixel_format == PIXFORMAT_JPEG) {  // If the pixel format is JPEG
    if (psramFound()) { //  If PSRAM is available, it sets a higher JPEG quality (10), uses two frame buffers, and sets the grab mode to CAMERA_GRAB_LATEST.
        config.jpeg_quality = 10;
        config.fb_count = 2;
        config.grab_mode = CAMERA_GRAB_LATEST;
    } else {  // If PSRAM is not available, it limits the frame size to FRAMESIZE_SVGA (800x600 pixels) and sets the frame buffer location to DRAM (internal RAM).
        config.frame_size = FRAMESIZE_SVGA;
        config.fb_location = CAMERA_FB_IN_DRAM;
    }
}
else {  // If the pixel format is not JPEG
    // Best option for face detection/recognition
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3 // If the target is ESP32-S3, it uses two frame buffers.
    config.fb_count = 2;
#endif
}



// Camera Initialization
  esp_err_t err = esp_camera_init(&config); // structure to hold the camera errors.
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();  //Get a pointer to the image sensor control structure //@return pointer to the sensor
  s->set_framesize(s, FRAMESIZE_HVGA);  //setting camera frame size to default HVGA 480 x 320
  s->set_hmirror(s, 1); //Creating H-Mirror by default
  s->set_wb_mode(s, 1); //Setting wb_mode to sunny (1=sunny, 2 = cloudy, 3= office , 4 = home)
  
  // Function to setup wifi connection.
  setupWifi();

  // Starting the camera server
  startMyCameraServer();
}

//code that runs continuously in the loop after setup code executes
void loop(){
  // enable_led(true);
  // delay(1000);
  // Serial.println("On");
  // led_duty = 255;
  // delay(1000);
  // Serial.println("Off");
  // led_duty = 0;
}

//Function Implementations

//Function to setup wifi connection
void setupWifi()
{
  //Begin Wifi Instance
  WiFi.begin(ssid, password);

  //Setting Up static IP address
  if (!WiFi.config(
        // IPAddress(192, 168, 137, 14), // ESP's IP address : 192.168.137.14
        // IPAddress(192, 168, 137, 1), // Gateway or Router's IP Address : 192.168.137.1
        // IPAddress(255, 255, 255, 0), // Subnet Mask 255.255.255.0 for class C Network
        // IPAddress(8, 8, 8, 8) // DNS server

        IPAddress(192, 168, 254, 15), // ESP's IP address : 192.168.137.14
        IPAddress(192, 168, 254, 254), // Gateway or Router's IP Address : 192.168.137.1
        IPAddress(255, 255, 255, 0), // Subnet Mask 255.255.255.0 for class C Network
        IPAddress(192,168,254,254) // DNS server
      )) {
    Serial.println("Failed to set static IP");
  } 
  else
  {
    Serial.println("Static IP Address Setup Correctly");
  }

  Serial.printf("Connecting to the wifi with ssid %s\n", ssid);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected to WiFi network %s with IP Address:", ssid);
  Serial.println(WiFi.localIP());
  Serial.println(CONFIG_HTTPD_WS_SUPPORT);

}

void startMyCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 16;

  httpd_uri_t index_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = my_index_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    ,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t status_uri = {
    .uri = "/status",
    .method = HTTP_GET,
    .handler = status_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    ,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = NULL
#endif
  };

  httpd_uri_t cmd_uri = {
    .uri = "/control",
    .method = HTTP_GET,
    .handler = cmd_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    ,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = NULL
#endif
  };

httpd_uri_t stream_uri = {
    .uri = "/stream",
    .method = HTTP_GET,
    .handler = stream_handler,
    .user_ctx = NULL
#ifdef CONFIG_HTTPD_WS_SUPPORT
    ,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = NULL
#endif
  };

//   httpd_uri_t capture_uri = {
//     .uri = "/capture",
//     .method = HTTP_GET,
//     .handler = capture_handler,
//     .user_ctx = NULL
// #ifdef CONFIG_HTTPD_WS_SUPPORT
//     ,
//     .is_websocket = true,
//     .handle_ws_control_frames = false,
//     .supported_subprotocol = NULL
// #endif
//   };

//   httpd_uri_t bmp_uri = {
//     .uri = "/bmp",
//     .method = HTTP_GET,
//     .handler = bmp_handler,
//     .user_ctx = NULL
// #ifdef CONFIG_HTTPD_WS_SUPPORT
//     ,
//     .is_websocket = true,
//     .handle_ws_control_frames = false,
//     .supported_subprotocol = NULL
// #endif
//   };

//   httpd_uri_t xclk_uri = {
//     .uri = "/xclk",
//     .method = HTTP_GET,
//     .handler = xclk_handler,
//     .user_ctx = NULL
// #ifdef CONFIG_HTTPD_WS_SUPPORT
//     ,
//     .is_websocket = true,
//     .handle_ws_control_frames = false,
//     .supported_subprotocol = NULL
// #endif
//   };

//   httpd_uri_t reg_uri = {
//     .uri = "/reg",
//     .method = HTTP_GET,
//     .handler = reg_handler,
//     .user_ctx = NULL
// #ifdef CONFIG_HTTPD_WS_SUPPORT
//     ,
//     .is_websocket = true,
//     .handle_ws_control_frames = false,
//     .supported_subprotocol = NULL
// #endif
//   };

//   httpd_uri_t greg_uri = {
//     .uri = "/greg",
//     .method = HTTP_GET,
//     .handler = greg_handler,
//     .user_ctx = NULL
// #ifdef CONFIG_HTTPD_WS_SUPPORT
//     ,
//     .is_websocket = true,
//     .handle_ws_control_frames = false,
//     .supported_subprotocol = NULL
// #endif
//   };

//   httpd_uri_t pll_uri = {
//     .uri = "/pll",
//     .method = HTTP_GET,
//     .handler = pll_handler,
//     .user_ctx = NULL
// #ifdef CONFIG_HTTPD_WS_SUPPORT
//     ,
//     .is_websocket = true,
//     .handle_ws_control_frames = false,
//     .supported_subprotocol = NULL
// #endif
//   };

//   httpd_uri_t win_uri = {
//     .uri = "/resolution",
//     .method = HTTP_GET,
//     .handler = win_handler,
//     .user_ctx = NULL
// #ifdef CONFIG_HTTPD_WS_SUPPORT
//     ,
//     .is_websocket = true,
//     .handle_ws_control_frames = false,
//     .supported_subprotocol = NULL
// #endif
//   };

  ra_filter_init(&ra_filter, 20);

#if CONFIG_ESP_FACE_RECOGNITION_ENABLED
  recognizer.set_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "fr");

  // load ids from flash partition
  recognizer.set_ids_from_flash();
#endif
  log_i("Starting web server on port: '%d'", config.server_port);
  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &cmd_uri);
    httpd_register_uri_handler(camera_httpd, &status_uri);
    // httpd_register_uri_handler(camera_httpd, &capture_uri);
    // httpd_register_uri_handler(camera_httpd, &bmp_uri);

    // httpd_register_uri_handler(camera_httpd, &xclk_uri);
    // httpd_register_uri_handler(camera_httpd, &reg_uri);
    // httpd_register_uri_handler(camera_httpd, &greg_uri);
    // httpd_register_uri_handler(camera_httpd, &pll_uri);
    // httpd_register_uri_handler(camera_httpd, &win_uri);
  }

  config.server_port += 1;
  config.ctrl_port += 1;
  log_i("Starting stream server on port: '%d'", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

void setupMyLedFlash(int pin) {
#if CONFIG_LED_ILLUMINATOR_ENABLED
  ledcAttach(pin, 5000, 8);
#else
  log_i("LED flash is disabled -> CONFIG_LED_ILLUMINATOR_ENABLED = 0");
#endif
}

static esp_err_t my_index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    sensor_t *s = esp_camera_sensor_get();
    if (s != NULL)
    {
      return httpd_resp_send(req, (const char *)my_index_html_gz, my_index_gz_len);
    }
    else
    {
        log_e("Camera sensor not found");
        return httpd_resp_send_500(req);
    }
}

// static esp_err_t my_stream_handler(httpd_req_t *req)
// {
//     camera_fb_t *fb = NULL;
//     struct timeval _timestamp;
//     esp_err_t res = ESP_OK;
//     size_t _jpg_buf_len = 0;
//     uint8_t *_jpg_buf = NULL;
//     char *part_buf[128];
// #if CONFIG_ESP_FACE_DETECT_ENABLED
// #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//     bool detected = false;
//     int64_t fr_ready = 0;
//     int64_t fr_recognize = 0;
//     int64_t fr_encode = 0;
//     int64_t fr_face = 0;
//     int64_t fr_start = 0;
// #endif
//     int face_id = 0;
//     size_t out_len = 0, out_width = 0, out_height = 0;
//     uint8_t *out_buf = NULL;
//     bool s = false;
// #if TWO_STAGE
//     HumanFaceDetectMSR01 s1(0.1F, 0.5F, 10, 0.2F);
//     HumanFaceDetectMNP01 s2(0.5F, 0.3F, 5);
// #else
//     HumanFaceDetectMSR01 s1(0.3F, 0.5F, 10, 0.2F);
// #endif
// #endif

//     static int64_t last_frame = 0;
//     if (!last_frame)
//     {
//         last_frame = esp_timer_get_time();
//     }

//     res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
//     if (res != ESP_OK)
//     {
//         return res;
//     }

//     httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
//     httpd_resp_set_hdr(req, "X-Framerate", "60");

// #if CONFIG_LED_ILLUMINATOR_ENABLED
//     isStreaming = true;
//     enable_led(true);
// #endif

//     while (true)
//     {
// #if CONFIG_ESP_FACE_DETECT_ENABLED
// #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//         detected = false;
// #endif
//         face_id = 0;
// #endif

//         fb = esp_camera_fb_get();
//         if (!fb)
//         {
//             log_e("Camera capture failed");
//             res = ESP_FAIL;
//         }
//         else
//         {
//             _timestamp.tv_sec = fb->timestamp.tv_sec;
//             _timestamp.tv_usec = fb->timestamp.tv_usec;
// #if CONFIG_ESP_FACE_DETECT_ENABLED
// #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//             fr_start = esp_timer_get_time();
//             fr_ready = fr_start;
//             fr_encode = fr_start;
//             fr_recognize = fr_start;
//             fr_face = fr_start;
// #endif
//             if (!detection_enabled || fb->width > 400)
//             {
// #endif
//                 if (fb->format != PIXFORMAT_JPEG)
//                 {
//                     bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
//                     esp_camera_fb_return(fb);
//                     fb = NULL;
//                     if (!jpeg_converted)
//                     {
//                         log_e("JPEG compression failed");
//                         res = ESP_FAIL;
//                     }
//                 }
//                 else
//                 {
//                     _jpg_buf_len = fb->len;
//                     _jpg_buf = fb->buf;
//                 }
// #if CONFIG_ESP_FACE_DETECT_ENABLED
//             }
//             else
//             {
//                 if (fb->format == PIXFORMAT_RGB565
// #if CONFIG_ESP_FACE_RECOGNITION_ENABLED
//                     && !recognition_enabled
// #endif
//                 )
//                 {
// #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//                     fr_ready = esp_timer_get_time();
// #endif
// #if TWO_STAGE
//                     std::list<dl::detect::result_t> &candidates = s1.infer((uint16_t *)fb->buf, {(int)fb->height, (int)fb->width, 3});
//                     std::list<dl::detect::result_t> &results = s2.infer((uint16_t *)fb->buf, {(int)fb->height, (int)fb->width, 3}, candidates);
// #else
//                     std::list<dl::detect::result_t> &results = s1.infer((uint16_t *)fb->buf, {(int)fb->height, (int)fb->width, 3});
// #endif
// #if CONFIG_ESP_FACE_DETECT_ENABLED && ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//                     fr_face = esp_timer_get_time();
//                     fr_recognize = fr_face;
// #endif
//                     if (results.size() > 0)
//                     {
//                         fb_data_t rfb;
//                         rfb.width = fb->width;
//                         rfb.height = fb->height;
//                         rfb.data = fb->buf;
//                         rfb.bytes_per_pixel = 2;
//                         rfb.format = FB_RGB565;
// #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//                         detected = true;
// #endif
//                         draw_face_boxes(&rfb, &results, face_id);
//                     }
//                     s = fmt2jpg(fb->buf, fb->len, fb->width, fb->height, PIXFORMAT_RGB565, 80, &_jpg_buf, &_jpg_buf_len);
//                     esp_camera_fb_return(fb);
//                     fb = NULL;
//                     if (!s)
//                     {
//                         log_e("fmt2jpg failed");
//                         res = ESP_FAIL;
//                     }
// #if CONFIG_ESP_FACE_DETECT_ENABLED && ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//                     fr_encode = esp_timer_get_time();
// #endif
//                 }
//                 else
//                 {
//                     out_len = fb->width * fb->height * 3;
//                     out_width = fb->width;
//                     out_height = fb->height;
//                     out_buf = (uint8_t *)malloc(out_len);
//                     if (!out_buf)
//                     {
//                         log_e("out_buf malloc failed");
//                         res = ESP_FAIL;
//                     }
//                     else
//                     {
//                         s = fmt2rgb888(fb->buf, fb->len, fb->format, out_buf);
//                         esp_camera_fb_return(fb);
//                         fb = NULL;
//                         if (!s)
//                         {
//                             free(out_buf);
//                             log_e("To rgb888 failed");
//                             res = ESP_FAIL;
//                         }
//                         else
//                         {
// #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//                             fr_ready = esp_timer_get_time();
// #endif

//                             fb_data_t rfb;
//                             rfb.width = out_width;
//                             rfb.height = out_height;
//                             rfb.data = out_buf;
//                             rfb.bytes_per_pixel = 3;
//                             rfb.format = FB_BGR888;

// #if TWO_STAGE
//                             std::list<dl::detect::result_t> &candidates = s1.infer((uint8_t *)out_buf, {(int)out_height, (int)out_width, 3});
//                             std::list<dl::detect::result_t> &results = s2.infer((uint8_t *)out_buf, {(int)out_height, (int)out_width, 3}, candidates);
// #else
//                             std::list<dl::detect::result_t> &results = s1.infer((uint8_t *)out_buf, {(int)out_height, (int)out_width, 3});
// #endif

// #if CONFIG_ESP_FACE_DETECT_ENABLED && ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//                             fr_face = esp_timer_get_time();
//                             fr_recognize = fr_face;
// #endif

//                             if (results.size() > 0)
//                             {
// #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//                                 detected = true;
// #endif
// #if CONFIG_ESP_FACE_RECOGNITION_ENABLED
//                                 if (recognition_enabled)
//                                 {
//                                     face_id = run_face_recognition(&rfb, &results);
// #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//                                     fr_recognize = esp_timer_get_time();
// #endif
//                                 }
// #endif
//                                 draw_face_boxes(&rfb, &results, face_id);
//                             }
//                             s = fmt2jpg(out_buf, out_len, out_width, out_height, PIXFORMAT_RGB888, 90, &_jpg_buf, &_jpg_buf_len);
//                             free(out_buf);
//                             if (!s)
//                             {
//                                 log_e("fmt2jpg failed");
//                                 res = ESP_FAIL;
//                             }
// #if CONFIG_ESP_FACE_DETECT_ENABLED && ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//                             fr_encode = esp_timer_get_time();
// #endif
//                         }
//                     }
//                 }
//             }
// #endif
//         }
//         if (res == ESP_OK)
//         {
//             res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
//         }
//         if (res == ESP_OK)
//         {
//             size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART, _jpg_buf_len, _timestamp.tv_sec, _timestamp.tv_usec);
//             res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
//         }
//         if (res == ESP_OK)
//         {
//             res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
//         }
//         if (fb)
//         {
//             esp_camera_fb_return(fb);
//             fb = NULL;
//             _jpg_buf = NULL;
//         }
//         else if (_jpg_buf)
//         {
//             free(_jpg_buf);
//             _jpg_buf = NULL;
//         }
//         if (res != ESP_OK)
//         {
//             log_e("Send frame failed");
//             break;
//         }
//         int64_t fr_end = esp_timer_get_time();

// #if CONFIG_ESP_FACE_DETECT_ENABLED && ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//         int64_t ready_time = (fr_ready - fr_start) / 1000;
//         int64_t face_time = (fr_face - fr_ready) / 1000;
//         int64_t recognize_time = (fr_recognize - fr_face) / 1000;
//         int64_t encode_time = (fr_encode - fr_recognize) / 1000;
//         int64_t process_time = (fr_encode - fr_start) / 1000;
// #endif

//         int64_t frame_time = fr_end - last_frame;
//         frame_time /= 1000;
// #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
//         uint32_t avg_frame_time = ra_filter_run(&ra_filter, frame_time);
// #endif
//         log_i(
//             "MJPG: %uB %ums (%.1ffps), AVG: %ums (%.1ffps)"
// #if CONFIG_ESP_FACE_DETECT_ENABLED
//             ", %u+%u+%u+%u=%u %s%d"
// #endif
//             ,
//             (uint32_t)(_jpg_buf_len), (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time, avg_frame_time, 1000.0 / avg_frame_time
// #if CONFIG_ESP_FACE_DETECT_ENABLED
//             ,
//             (uint32_t)ready_time, (uint32_t)face_time, (uint32_t)recognize_time, (uint32_t)encode_time, (uint32_t)process_time, (detected) ? "DETECTED " : "", face_id
// #endif
//         );
//     }

// #if CONFIG_LED_ILLUMINATOR_ENABLED
//     isStreaming = false;
//     enable_led(false);
// #endif

//     return res;
// }
