#include <iostream>
#include <string>
#include <curl/curl.h>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// -----------------------------
// WriteCallback to receive HTTP response
// -----------------------------
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// -----------------------------
// fetchURL to call TMDB API
// -----------------------------
string fetchURL(const string& url) {
    CURL* curl = curl_easy_init();
    string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);  // Optional: skip SSL verification
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        }

        curl_easy_cleanup(curl);
    } else {
        cerr << "Failed to initialize CURL." << endl;
    }

    return response;
}

// -----------------------------
// Display CLI menu
// -----------------------------
void showMenu() {
    cout << "\n====== TMDB CLI TOOL ======\n";
    cout << "1. Search Movie\n";
    cout << "2. Trending Movies\n";
    cout << "3. Exit\n";
    cout << "Choose an option: ";
}

// -----------------------------
// Search movies from TMDB
// -----------------------------
void searchMovie(const string& apiKey) {

    string movie;
    cout << "Enter movie name: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush input buffer
    getline(cin, movie);

    // Basic URL encoding (spaces and a few special chars)
    auto urlEncode = [](const string& str) {
        string encoded;
        char hex[4];
        for (unsigned char c : str) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                encoded += c;
            } else if (c == ' ') {
                encoded += '+';
            } else {
                snprintf(hex, sizeof(hex), "%%%02X", c);
                encoded += hex;
            }
        }
        return encoded;
    };

    string encodedMovie = urlEncode(movie);
    string url = "https://api.themoviedb.org/3/search/movie?api_key=" + apiKey + "&query=" + encodedMovie;
    string result = fetchURL(url);

    try {
        auto data = json::parse(result);

        if (!data.contains("results") || data["results"].empty()) {
            cout << "No results found.\n";
            return;
        }

        for (auto& m : data["results"]) {
            cout << "\n-----------------------------\n";
            cout << "Title: " << m.value("title", "N/A") << endl;
            cout << "Release Date: " << m.value("release_date", "Unknown") << endl;
            cout << "Overview: " << m.value("overview", "No overview available.") << endl;
            cout << "Rating: " << m.value("vote_average", 0.0) << "\n";
        }

    } catch (const json::exception& e) {
        cerr << "JSON parsing error: " << e.what() << endl;
    }
}

// -----------------------------
// Main Program
// -----------------------------
int main() {
    string apiKey = "your API key;  // Replace with your own TMDB API key
    int choice = 0;
    while (true) {
        showMenu();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Try again.\n";
            continue;
        }
        switch (choice) {
            case 1:
                searchMovie(apiKey);
                break;
            case 2:
                cout << "Trending movies coming soon...\n";
                break;
            case 3:
                cout << "Exiting...\n";
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}
