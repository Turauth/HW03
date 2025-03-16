#include <iostream>
#include<curl/curl.h>
#include<json/json.h>
#include <json/reader.h>
#include <json/writer.h>
#include <json/value.h>
#include <string>
#include <cmath>
#include <fstream>
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

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

// Write another callback function to get information that will be needed to download the graphical form (the icon)
// (https://proxiesapi.com/articles/downloading-images-from-urls-in-c).
    size_t writeData(void* ptr, size_t size, size_t nmemb, FILE* stream)
    {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

int main(int argc, char *argv[])
{
// This is the main part of the homework.
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
    std::cout << "Temperature: " << temp_f.substr(0,4) << " °F" << std::endl;
    std::cout << "Feels like: " << feelslike_f.substr(0,4) << " °F" << std::endl;
    std::cout << "Condition: " << condition_text << std::endl;
    std::cout << "Precipitation: " << precip_in.substr(0,5) << " in." << std::endl;
    std::cout << "Cloud cover: " << cloud << "%" << std::endl;

// What follows is the bonus part of the homework.
    // Notice that the server returned part of a link to the icon for the current weather condition
    // (ChatGPT).
    std::string icon = root["current"]["condition"].get("icon","Default value.").asString();

    // Complete the link by combining strings (https://www.learncpp.com/cpp-tutorial/stdstring-appending/).
    std::string url{"https:"};
    url.append(icon);

    // Initialize a string for the name of the icon file that will be downloaded 
    // (https://proxiesapi.com/articles/downloading-images-from-urls-in-c).
    std::string filename = "status_icon.png";

    // Send another request to the HTTP web server and receive another response.
        // Initialize another curl object using a pointer of type CURL (https://curl.se/libcurl/c/curl_easy_init.html; ChatGPT;
        // https://proxiesapi.com/articles/downloading-images-from-urls-in-c). curl_easy_init() calls some sort of pointer 
        // from a libcurl data structure that needs to be assigned to another
        // pointer (* curl) because it itself returns pointer.
        CURL* curl2 = curl_easy_init();
        if (curl2)
        {
            // The response of the web server will be written to file.
            FILE* fp = fopen(filename.c_str(), "wb");

            // // Add the target URL to the curl object (https://curl.se/libcurl/c/curl_easy_init.html; 
            // https://www.weatherapi.com/docs/; https://proxiesapi.com/articles/downloading-images-from-urls-in-c). The 
            // curl_easy_setopt configures the CURLOPT_URL option of the CURL handle (ChatGPT).
            curl_easy_setopt(curl2, CURLOPT_URL, url.c_str());

            // The curl_easy perform function will return the weather data, but the write_callback function and the 
            // CURLOPT_WRITEFUNCTION and CURLOPT_WRITEDATA options will be used to store that data in a file
            // (https://curl.se/libcurl/c/CURLOPT_WRITEFUNCTION.html; 
            // https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c; 
            // https://proxiesapi.com/articles/downloading-images-from-urls-in-c; ChatGPT).
            curl_easy_setopt(curl2, CURLOPT_WRITEFUNCTION, writeData);
            curl_easy_setopt(curl2, CURLOPT_WRITEDATA, fp);
            
            //// Send an HTTP request to the web server and receive a response back.
            CURLcode res = curl_easy_perform(curl2);
            curl_easy_cleanup(curl2);
            fclose(fp);

            // If the image failed to download, return an appropriate error 
            // (https://proxiesapi.com/articles/downloading-images-from-urls-in-c).
            if (res != CURLE_OK)
            {
                std::cout << "Failed to download image: " << curl_easy_strerror(res) << std::endl;
            }
        }
        
        // Display the downloaded icon.
        QApplication a(argc, argv);
        QGraphicsScene scene;
        QGraphicsView view(&scene);
        QGraphicsPixmapItem item(QPixmap("./status_icon.png"));
        scene.addItem(&item);
        view.show();
        return a.exec();
}