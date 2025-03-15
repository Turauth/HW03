#include <iostream>
#include<curl/curl.h>
#include<json/json.h>
#include <json/reader.h>
#include <json/writer.h>
#include <json/value.h>
#include <string>
#include <cmath>

// The write_callback function should return the total size of the data processed in the data transfer from the server back
// to the client (https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html; 
// https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c; ChatGPT).
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    // This first line was taken from the stack overflow link, and ChatGPT explained that the first line of this cb. fun.
    // essentially uses the append() function to add the chunk of data received from the server to end of userp std::string
    // object.
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main()
{
// Send an HTTP request to the web server.
    // Initialize a curl object using a pointer of type CURL (https://curl.se/libcurl/c/curl_easy_init.html; ChatGPT).
    // curl_easy_init() calls some sort of pointer from a libcurl data structure that needs to be assigned to another
    // pointer (* curl) because it itself returns pointer.
    CURL *curl = curl_easy_init();

    // Declare variable res of type CURLcode (ChatGPT). "CURLcode is an enumeration (enum) type defined in the 
    // libcurl library. It is used to store the return value of various libcurl functions" (ChatGPT). In C++, 
    // an enumeration is a compound data type whose values are restricted to a set of named symbolic constants" 
    // (https://www.learncpp.com/cpp-tutorial/unscoped-enumerations/).
    CURLcode res;

    // Prepare a string to store the data transferred from the server 
    // (https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c).
    std::string readBuffer;

    // Add the target URL to the curl object (https://curl.se/libcurl/c/curl_easy_init.html; 
    // https://www.weatherapi.com/docs/). The curl_easy_setopt configures the CURLOPT_URL option of the CURL handle
    // (ChatGPT).
    curl_easy_setopt(curl, CURLOPT_URL, 
    "http://api.weatherapi.com/v1/current.json?key=f2e0b6d400b34048aa021935251303&q=37321");

    // The curl_easy perform function will return the weather data, but the write_callback function and the 
    // CURLOPT_WRITEFUNCTION and CURLOPT_WRITEDATA options will be used to store that data in a string
    // (https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html; 
    // https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c; ChatGPT).
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);    

    // Send an HTTP request to the web server and receive a response back in the form of a JSON file. 
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    // Parse readBuffer so it can be treated like a Json::Value object and store certain fields as strings.
    // (https://github.com/open-source-parsers/jsoncpp/wiki; 
    // https://stackoverflow.com/questions/31121378/json-cpp-how-to-initialize-from-string-and-get-string-value; ChatGPT).
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( readBuffer.c_str(), root );
    if ( !parsingSuccessful )
    {
        std::cout << "Failed to parse"
            << reader.getFormattedErrorMessages();
        return 0;
    }

    std::string last_updated = root["current"].get("last_updated", "Default value." ).asString();
    std::string temp_f = root["current"].get("temp_f", "Default value." ).asString();
    std::string feelslike_f = root["current"].get("feelslike_f", "Default value." ).asString();
    std::string condition_text = root["current"]["condition"].get("text", "Default value." ).asString();
    std::string precip_in = root["current"].get("precip_in", "Default value." ).asString();
    std::string cloud = root["current"].get("cloud", "Default value." ).asString();

    // Output the strings with values rounded to two decimal places (https://www.geeksforgeeks.org/substring-in-cpp/;
    // https://www.weatherapi.com/docs/).
    std::cout << "Current Weather in Dayton, TN, 37321" << std::endl;
    std::cout << "Last Updated: " << last_updated << std::endl;
    std::cout << "Temperature: " << temp_f.substr(0,5) << " °F" << std::endl;
    std::cout << "Feels like: " << feelslike_f.substr(0,4) << " °F" << std::endl;
    std::cout << "Condition: " << condition_text << std::endl;
    std::cout << "Precipitation: " << precip_in.substr(0,5) << " in." << std::endl;
    std::cout << "Cloud cover: " << cloud << "%" << std::endl;

return 0;
}