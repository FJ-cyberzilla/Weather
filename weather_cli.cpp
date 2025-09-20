#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <vector>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <chrono>
#include <thread>
#include <memory>
#include <unordered_map>
#include <ctime>
#include <regex>

using json = nlohmann::json;

// Enhanced ANSI escape codes with 256-color support
namespace Colors {
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string ITALIC = "\033[3m";
    const std::string UNDERLINE = "\033[4m";
    const std::string BLINK = "\033[5m";
    const std::string REVERSE = "\033[7m";
    const std::string RESET = "\033[0m";
    
    // Enhanced color palette
    const std::string RED = "\033[38;5;196m";
    const std::string ORANGE = "\033[38;5;208m";
    const std::string YELLOW = "\033[38;5;226m";
    const std::string GREEN = "\033[38;5;82m";
    const std::string CYAN = "\033[38;5;87m";
    const std::string BLUE = "\033[38;5;75m";
    const std::string PURPLE = "\033[38;5;141m";
    const std::string PINK = "\033[38;5;213m";
    const std::string GRAY = "\033[38;5;246m";
    const std::string WHITE = "\033[38;5;255m";
    const std::string DARK_GRAY = "\033[38;5;240m";
    
    // Gradient colors for weather conditions
    const std::string SUNNY = "\033[38;5;226m";
    const std::string CLOUDY = "\033[38;5;250m";
    const std::string RAINY = "\033[38;5;75m";
    const std::string SNOWY = "\033[38;5;255m";
    const std::string STORMY = "\033[38;5;93m";
    
    // Background colors
    const std::string BG_BLUE = "\033[48;5;18m";
    const std::string BG_DARK = "\033[48;5;236m";
    const std::string BG_RED = "\033[48;5;88m";
    const std::string BG_GREEN = "\033[48;5;22m";
    const std::string BG_YELLOW = "\033[48;5;94m";
}

// Configuration class for better organization
class Config {
public:
    static std::string API_KEY;
    static std::string BASE_URL;
    static int TIMEOUT;
    static std::string USER_AGENT;
    static std::string CONFIG_FILE;
    
    static bool load_config();
    static bool save_config();
};

std::string Config::API_KEY = "f807e87175ee4ff98f551941252009";
std::string Config::BASE_URL = "https://api.weatherapi.com/v1/";
int Config::TIMEOUT = 15;
std::string Config::USER_AGENT = "WeatherCLI-Pro/2.0";
std::string Config::CONFIG_FILE = "weather_cli_config.json";

bool Config::load_config() {
    std::ifstream file(CONFIG_FILE);
    if (file.is_open()) {
        try {
            json config;
            file >> config;
            if (config.contains("api_key")) API_KEY = config["api_key"];
            if (config.contains("timeout")) TIMEOUT = config["timeout"];
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

bool Config::save_config() {
    std::ofstream file(CONFIG_FILE);
    if (file.is_open()) {
        json config;
        config["api_key"] = API_KEY;
        config["timeout"] = TIMEOUT;
        file << config.dump(4);
        return true;
    }
    return false;
}

// Enhanced logging system
class Logger {
private:
    static std::string log_file;
    static bool logging_enabled;

public:
    enum Level { DEBUG, INFO, WARNING, ERROR };
    
    static void enable_logging(bool enable = true) { logging_enabled = enable; }
    static void log(Level level, const std::string& message);
    static void debug(const std::string& message) { log(DEBUG, message); }
    static void info(const std::string& message) { log(INFO, message); }
    static void warning(const std::string& message) { log(WARNING, message); }
    static void error(const std::string& message) { log(ERROR, message); }
};

std::string Logger::log_file = "weather_cli.log";
bool Logger::logging_enabled = false;

void Logger::log(Level level, const std::string& message) {
    if (!logging_enabled) return;
    
    std::ofstream file(log_file, std::ios::app);
    if (file.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::string level_str;
        switch (level) {
            case DEBUG: level_str = "DEBUG"; break;
            case INFO: level_str = "INFO"; break;
            case WARNING: level_str = "WARNING"; break;
            case ERROR: level_str = "ERROR"; break;
        }
        
        file << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") 
             << " [" << level_str << "] " << message << std::endl;
    }
}

// Enhanced HTTP client with retry logic and caching
class HttpClient {
private:
    static std::unordered_map<std::string, std::pair<json, std::chrono::steady_clock::time_point>> cache;
    static const std::chrono::minutes CACHE_DURATION;

public:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    static json make_request(const std::string& url, bool use_cache = true, int retries = 3);
    static void clear_cache() { cache.clear(); }
};

std::unordered_map<std::string, std::pair<json, std::chrono::steady_clock::time_point>> HttpClient::cache;
const std::chrono::minutes HttpClient::CACHE_DURATION(5);

size_t HttpClient::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

json HttpClient::make_request(const std::string& url, bool use_cache, int retries) {
    // Check cache first
    if (use_cache && cache.find(url) != cache.end()) {
        auto& [cached_data, timestamp] = cache[url];
        auto now = std::chrono::steady_clock::now();
        if (now - timestamp < CACHE_DURATION) {
            Logger::debug("Using cached data for: " + url);
            return cached_data;
        } else {
            cache.erase(url);
        }
    }

    std::string response_string;
    CURL* curl = curl_easy_init();
    if (!curl) {
        Logger::error("Failed to initialize curl");
        return nullptr;
    }

    // Enhanced curl options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, Config::USER_AGENT.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, Config::TIMEOUT);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res;
    int attempt = 0;
    
    while (attempt < retries) {
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) break;
        
        attempt++;
        Logger::warning("Request attempt " + std::to_string(attempt) + " failed: " + curl_easy_strerror(res));
        
        if (attempt < retries) {
            std::this_thread::sleep_for(std::chrono::seconds(attempt * 2)); // Exponential backoff
        }
    }

    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        Logger::error("Request failed after " + std::to_string(retries) + " attempts: " + curl_easy_strerror(res));
        return nullptr;
    }

    try {
        json data = json::parse(response_string);
        if (data.contains("error")) {
            Logger::error("API Error: " + data["error"]["message"].get<std::string>());
            return nullptr;
        }
        
        // Cache successful response
        if (use_cache) {
            cache[url] = {data, std::chrono::steady_clock::now()};
        }
        
        return data;
    } catch (const json::parse_error& e) {
        Logger::error("JSON parsing error: " + std::string(e.what()));
        return nullptr;
    }
}

// Enhanced utility functions
class Utils {
public:
    static std::string url_encode(const std::string& str);
    static std::string trim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::string format_time(const std::string& time_str);
    static std::string get_weather_icon(const std::string& condition);
    static std::string get_temperature_color(double temp);
    static std::string get_aqi_color(int aqi);
    static void animate_loading(const std::string& message, int duration_ms = 2000);
    static bool is_valid_ip(const std::string& ip);
    static std::string get_current_time();
};

std::string Utils::url_encode(const std::string& str) {
    std::string encoded_str = "";
    for (char c : str) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded_str += c;
        } else if (c == ' ') {
            encoded_str += "%20";
        } else {
            std::ostringstream oss;
            oss << '%' << std::setw(2) << std::setfill('0') << std::hex << (int)(unsigned char)c;
            encoded_str += oss.str();
        }
    }
    return encoded_str;
}

std::string Utils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

std::string Utils::get_weather_icon(const std::string& condition) {
    std::string lower_condition = condition;
    std::transform(lower_condition.begin(), lower_condition.end(), lower_condition.begin(), ::tolower);
    
    if (lower_condition.find("sunny") != std::string::npos || lower_condition.find("clear") != std::string::npos) {
        return "☀️";
    } else if (lower_condition.find("cloud") != std::string::npos) {
        return "☁️";
    } else if (lower_condition.find("rain") != std::string::npos || lower_condition.find("drizzle") != std::string::npos) {
        return "🌧️";
    } else if (lower_condition.find("snow") != std::string::npos) {
        return "❄️";
    } else if (lower_condition.find("storm") != std::string::npos || lower_condition.find("thunder") != std::string::npos) {
        return "⛈️";
    } else if (lower_condition.find("fog") != std::string::npos || lower_condition.find("mist") != std::string::npos) {
        return "🌫️";
    }
    return "🌤️";
}

std::string Utils::get_temperature_color(double temp) {
    if (temp >= 35) return Colors::RED;
    else if (temp >= 25) return Colors::ORANGE;
    else if (temp >= 15) return Colors::YELLOW;
    else if (temp >= 5) return Colors::GREEN;
    else if (temp >= -5) return Colors::CYAN;
    else return Colors::BLUE;
}

std::string Utils::get_aqi_color(int aqi) {
    switch(aqi) {
        case 1: return Colors::GREEN;
        case 2: return Colors::YELLOW;
        case 3: return Colors::ORANGE;
        case 4: return Colors::RED;
        case 5: case 6: return Colors::PURPLE;
        default: return Colors::GRAY;
    }
}

void Utils::animate_loading(const std::string& message, int duration_ms) {
    const std::vector<std::string> frames = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    auto start = std::chrono::steady_clock::now();
    int frame_idx = 0;
    
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
           std::chrono::steady_clock::now() - start).count() < duration_ms) {
        std::cout << "\r" << Colors::CYAN << frames[frame_idx] << " " << message << Colors::RESET << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        frame_idx = (frame_idx + 1) % frames.size();
    }
    std::cout << "\r" << std::string(message.length() + 5, ' ') << "\r";
}

bool Utils::is_valid_ip(const std::string& ip) {
    std::regex ip_regex(R"(^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)");
    return std::regex_match(ip, ip_regex);
}

std::string Utils::get_current_time() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Enhanced display functions
class Display {
public:
    static void banner();
    static void section_header(const std::string& title);
    static void key_value(const std::string& key, const std::string& value, const std::string& color = Colors::CYAN);
    static void progress_bar(int percentage, const std::string& label = "");
    static void weather_card(const json& data, const std::string& location);
    static void hourly_forecast(const json& data);
    static void seven_day_forecast(const json& data);
    static void menu();
    static void clear_screen();
    static void error_message(const std::string& message);
    static void success_message(const std::string& message);
    static void warning_message(const std::string& message);
    static void info_message(const std::string& message);
};

void Display::banner() {
    std::cout << Colors::BG_BLUE << Colors::PURPLE << R"(
  ╔══════════════════════════════════════════════════════════════╗
  ║  ╦ ╦┌─┐┌─┐┌─┐┬ ┬┌─┐┬─┐  ╔═╗┬  ┬  ╔═╗┬─┐┌─┐  ┌┬┐ ┬ ┬  ┬┌─┐  ║
  ║  ║║║├┤ ├─┤│  ├─┤├┤ ├┬┘  ║  │  ║  ╠═╝├┬┘│ │   │ ┬│└┬┘ │ │  ║
  ║  ╚╩╝└─┘┴ ┴└─┘┴ ┴└─┘┴└─  ╚═╝┴─┘┴  ╩  ┴└─└─┘  ─┴─ ┴ ┴  ┴└─┘  ║
  ╚══════════════════════════════════════════════════════════════╝
)" << Colors::RESET << std::endl;
    std::cout << Colors::BG_BLUE << Colors::CYAN << "      🌍 Professional Weather Intelligence Platform v2.0      " << Colors::RESET << std::endl;
    std::cout << Colors::BG_BLUE << Colors::YELLOW << "      ──────────────────────────────────────────────────      " << Colors::RESET << "\n\n";
    std::cout << Colors::DIM << "Current time: " << Utils::get_current_time() << Colors::RESET << "\n\n";
}

void Display::section_header(const std::string& title) {
    std::string border(title.length() + 4, '═');
    std::cout << "\n" << Colors::BG_DARK << Colors::BOLD << Colors::BLUE;
    std::cout << "╔" << border << "╗" << std::endl;
    std::cout << "║ " << title << " ║" << std::endl;
    std::cout << "╚" << border << "╝" << Colors::RESET << std::endl;
}

void Display::key_value(const std::string& key, const std::string& value, const std::string& color) {
    std::cout << " " << Colors::BOLD << Colors::GRAY << std::setw(18) << std::left << key + ":" 
              << Colors::RESET << color << value << Colors::RESET << std::endl;
}

void Display::progress_bar(int percentage, const std::string& label) {
    const int bar_width = 50;
    int filled = (percentage * bar_width) / 100;
    
    std::cout << " " << Colors::BOLD << label << ": " << Colors::RESET;
    std::cout << "[";
    
    for (int i = 0; i < bar_width; ++i) {
        if (i < filled) {
            std::cout << Colors::GREEN << "█" << Colors::RESET;
        } else {
            std::cout << Colors::DARK_GRAY << "░" << Colors::RESET;
        }
    }
    
    std::cout << "] " << Colors::BOLD << percentage << "%" << Colors::RESET << std::endl;
}

void Display::weather_card(const json& data, const std::string& location) {
    if (!data.contains("current")) return;
    
    auto& current = data["current"];
    std::string condition = current["condition"]["text"];
    double temp = current["temp_c"];
    
    std::cout << "\n╭─────────────────────────────────────╮" << std::endl;
    std::cout << "│ " << Colors::BOLD << Colors::CYAN << std::setw(33) << std::left << location << Colors::RESET << " │" << std::endl;
    std::cout << "├─────────────────────────────────────┤" << std::endl;
    std::cout << "│ " << Utils::get_weather_icon(condition) << " " << condition << std::setw(25) << " " << "│" << std::endl;
    std::cout << "│ " << Colors::BOLD << Utils::get_temperature_color(temp) << std::setw(33) << std::left 
              << (std::to_string(static_cast<int>(temp)) + "°C") << Colors::RESET << " │" << std::endl;
    std::cout << "│ Feels like " << Utils::get_temperature_color(current["feelslike_c"]) 
              << static_cast<int>(current["feelslike_c"].get<double>()) << "°C" << Colors::RESET << std::setw(18) << " " << "│" << std::endl;
    std::cout << "╰─────────────────────────────────────╯" << std::endl;
}

void Display::hourly_forecast(const json& data) {
    if (!data.contains("forecast") || data["forecast"]["forecastday"].empty()) return;
    
    section_header("24-HOUR FORECAST");
    
    auto& hours = data["forecast"]["forecastday"][0]["hour"];
    std::cout << "\n";
    
    // Header
    std::cout << Colors::BOLD << std::setw(6) << "Time" << std::setw(12) << "Condition" 
              << std::setw(8) << "Temp" << std::setw(8) << "Rain%" << std::setw(10) << "Wind" << Colors::RESET << std::endl;
    std::cout << std::string(44, '─') << std::endl;
    
    // Show next 12 hours
    for (int i = 0; i < std::min(12, static_cast<int>(hours.size())); i++) {
        auto& hour = hours[i];
        std::string time = hour["time"].get<std::string>().substr(11, 5); // Extract HH:MM
        std::string condition = hour["condition"]["text"];
        double temp = hour["temp_c"];
        int rain_chance = hour["chance_of_rain"];
        double wind = hour["wind_kph"];
        
        std::cout << Colors::CYAN << std::setw(6) << time << Colors::RESET
                  << std::setw(12) << (condition.length() > 10 ? condition.substr(0, 10) : condition)
                  << Utils::get_temperature_color(temp) << std::setw(6) << static_cast<int>(temp) << "°" << Colors::RESET
                  << Colors::BLUE << std::setw(6) << rain_chance << "%" << Colors::RESET
                  << Colors::GRAY << std::setw(8) << static_cast<int>(wind) << "kph" << Colors::RESET << std::endl;
    }
}

void Display::seven_day_forecast(const json& data) {
    if (!data.contains("forecast")) return;
    
    section_header("7-DAY FORECAST");
    
    auto& forecast_days = data["forecast"]["forecastday"];
    
    std::cout << "\n" << Colors::BOLD;
    std::cout << std::setw(12) << "Date" << std::setw(15) << "Condition" 
              << std::setw(8) << "High" << std::setw(8) << "Low" 
              << std::setw(8) << "Rain%" << std::setw(10) << "Wind" << Colors::RESET << std::endl;
    std::cout << std::string(61, '─') << std::endl;
    
    for (const auto& day : forecast_days) {
        std::string date = day["date"].get<std::string>().substr(5); // Remove year
        std::string condition = day["day"]["condition"]["text"];
        double max_temp = day["day"]["maxtemp_c"];
        double min_temp = day["day"]["mintemp_c"];
        int rain_chance = day["day"]["daily_chance_of_rain"];
        double max_wind = day["day"]["maxwind_kph"];
        
        std::cout << Colors::CYAN << std::setw(12) << date << Colors::RESET
                  << std::setw(15) << (condition.length() > 13 ? condition.substr(0, 13) : condition)
                  << Utils::get_temperature_color(max_temp) << std::setw(6) << static_cast<int>(max_temp) << "°" << Colors::RESET
                  << Utils::get_temperature_color(min_temp) << std::setw(6) << static_cast<int>(min_temp) << "°" << Colors::RESET
                  << Colors::BLUE << std::setw(6) << rain_chance << "%" << Colors::RESET
                  << Colors::GRAY << std::setw(8) << static_cast<int>(max_wind) << "kph" << Colors::RESET << std::endl;
    }
}

void Display::menu() {
    std::cout << "\n╭─────────────────────── MAIN MENU ───────────────────────╮" << std::endl;
    std::cout << "│                                                         │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "1." << Colors::RESET << " 🌤️  Current Weather & Today's Forecast              │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "2." << Colors::RESET << " 📅  7-Day Extended Forecast                        │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "3." << Colors::RESET << " 🕒  24-Hour Detailed Forecast                      │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "4." << Colors::RESET << " 💨  Air Quality Index & Pollution Data            │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "5." << Colors::RESET << " ⚠️   Weather Alerts & Warnings                     │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "6." << Colors::RESET << " 🌿  Pollen & Allergen Information                 │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "7." << Colors::RESET << " 🌍  IP Geolocation & Weather                       │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "8." << Colors::RESET << " ⚙️   Settings & Configuration                       │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "9." << Colors::RESET << " 📄  Export Weather Report                          │" << std::endl;
    std::cout << "│ " << Colors::BOLD << "0." << Colors::RESET << " 🚪  Exit Application                               │" << std::endl;
    std::cout << "│                                                         │" << std::endl;
    std::cout << "╰─────────────────────────────────────────────────────────╯" << std::endl;
    std::cout << "\n" << Colors::BOLD << Colors::PURPLE << "Select an option (0-9): " << Colors::RESET;
}

void Display::clear_screen() {
    std::cout << "\033[2J\033[1;1H";
}

void Display::error_message(const std::string& message) {
    std::cout << "\n" << Colors::BG_RED << Colors::WHITE << " ❌ ERROR " << Colors::RESET 
              << " " << Colors::RED << message << Colors::RESET << std::endl;
}

void Display::success_message(const std::string& message) {
    std::cout << "\n" << Colors::BG_GREEN << Colors::WHITE << " ✅ SUCCESS " << Colors::RESET 
              << " " << Colors::GREEN << message << Colors::RESET << std::endl;
}

void Display::warning_message(const std::string& message) {
    std::cout << "\n" << Colors::BG_YELLOW << Colors::WHITE << " ⚠️  WARNING " << Colors::RESET 
              << " " << Colors::YELLOW << message << Colors::RESET << std::endl;
}

void Display::info_message(const std::string& message) {
    std::cout << "\n" << Colors::CYAN << "ℹ️  " << message << Colors::RESET << std::endl;
}

// Enhanced weather service class
class WeatherService {
public:
    static json get_current_weather(const std::string& location);
    static json get_forecast(const std::string& location, int days = 7);
    static json get_hourly_forecast(const std::string& location);
    static json get_air_quality(const std::string& location);
    static json get_alerts(const std::string& location);
    static json get_ip_lookup(const std::string& ip);
    static bool export_weather_report(const std::string& location, const std::string& filename);
};

json WeatherService::get_current_weather(const std::string& location) {
    std::string url = Config::BASE_URL + "current.json?key=" + Config::API_KEY + 
                     "&q=" + Utils::url_encode(location) + "&aqi=yes";
    return HttpClient::make_request(url);
}

json WeatherService::get_forecast(const std::string& location, int days) {
    std::string url = Config::BASE_URL + "forecast.json?key=" + Config::API_KEY + 
                     "&q=" + Utils::url_encode(location) + "&days=" + std::to_string(days) + 
                     "&aqi=yes&alerts=yes";
    return HttpClient::make_request(url);
}

json WeatherService::get_hourly_forecast(const std::string& location) {
    return get_forecast(location, 2); // Get 2 days for 48-hour forecast
}

json WeatherService::get_air_quality(const std::string& location) {
    std::string url = Config::BASE_URL + "current.json?key=" + Config::API_KEY + 
                     "&q=" + Utils::url_encode(location) + "&aqi=yes";
    return HttpClient::make_request(url);
}

json WeatherService::get_alerts(const std::string& location) {
    std::string url = Config::BASE_URL + "forecast.json?key=" + Config::API_KEY + 
                     "&q=" + Utils::url_encode(location) + "&alerts=yes";
    return HttpClient::make_request(url);
}

json WeatherService::get_ip_lookup(const std::string& ip) {
    std::string url = Config::BASE_URL + "ip.json?key=" + Config::API_KEY + "&q=" + ip;
    return HttpClient::make_request(url);
}

bool WeatherService::export_weather_report(const std::string& location, const std::string& filename) {
    json data = get_forecast(location, 7);
    if (data.is_null()) return false;
    
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << "Weather Report for " << location << std::endl;
    file << "Generated: " << Utils::get_current_time() << std::endl;
    file << "=================================================" << std::endl;
    
    if (data.contains("current")) {
        auto& current = data["current"];
        file << "\nCURRENT CONDITIONS:" << std::endl;
        file << "Temperature: " << current["temp_c"] << "°C" << std::endl;
        file << "Condition: " << current["condition"]["text"] << std::endl;
        file << "Feels Like: " << current["feelslike_c"] << "°C" << std::endl;
        file << "Humidity: " << current["humidity"] << "%" << std::endl;
        file << "Wind: " << current["wind_kph"] << " km/h " << current["wind_dir"] << std::endl;
    }
    
    if (data.contains("forecast")) {
        file << "\n7-DAY FORECAST:" << std::endl;
        for (const auto& day : data["forecast"]["forecastday"]) {
            file << day["date"] << ": " 
                 << day["day"]["condition"]["text"] << " | "
                 << "High: " << day["day"]["maxtemp_c"] << "°C | "
                 << "Low: " << day["day"]["mintemp_c"] << "°C" << std::endl;
        }
    }
    
    return true;
}

// Enhanced weather display functions
class WeatherDisplay {
public:
    static void show_current_weather(const std::string& location);
    static void show_extended_forecast(const std::string& location);
    static void show_hourly_forecast(const std::string& location);
    static void show_air_quality(const std::string& location);
    static void show_weather_alerts(const std::string& location);
    static void show_pollen_data(const std::string& location);
    static void show_ip_lookup(const std::string& ip);
    static void show_detailed_current(const json& data, const std::string& location);
    static void show_astronomy_data(const json& data);
};

void WeatherDisplay::show_current_weather(const std::string& location) {
    Utils::animate_loading("Fetching current weather data");
    
    json data = WeatherService::get_forecast(location, 1);
    if (data.is_null()) {
        Display::error_message("Failed to fetch weather data for " + location);
        return;
    }
    
    Display::clear_screen();
    Display::banner();
    
    // Weather card
    Display::weather_card(data, location);
    
    // Detailed current conditions
    show_detailed_current(data, location);
    
    // Today's forecast
    if (data.contains("forecast") && !data["forecast"]["forecastday"].empty()) {
        auto& today = data["forecast"]["forecastday"][0];
        Display::section_header("TODAY'S FORECAST");
        
        Display::key_value("Max Temperature", std::to_string(static_cast<int>(today["day"]["maxtemp_c"].get<double>())) + "°C", 
                          Utils::get_temperature_color(today["day"]["maxtemp_c"]));
        Display::key_value("Min Temperature", std::to_string(static_cast<int>(today["day"]["mintemp_c"].get<double>())) + "°C", 
                          Utils::get_temperature_color(today["day"]["mintemp_c"]));
        Display::key_value("Condition", today["day"]["condition"]["text"], Colors::GREEN);
        Display::key_value("Precipitation", std::to_string(today["day"]["totalprecip_mm"].get<double>()) + " mm", Colors::BLUE);
        Display::key_value("Max Wind", std::to_string(static_cast<int>(today["day"]["maxwind_kph"].get<double>())) + " km/h", Colors::CYAN);
        
        Display::progress_bar(today["day"]["daily_chance_of_rain"].get<int>(), "Chance of Rain");
        Display::progress_bar(today["day"]["daily_chance_of_snow"].get<int>(), "Chance of Snow");
        
        // Astronomy data
        show_astronomy_data(data);
    }
}

void WeatherDisplay::show_detailed_current(const json& data, const std::string& location) {
    if (!data.contains("current")) return;
    
    auto& current = data["current"];
    
    Display::section_header("DETAILED CONDITIONS");
    
    // Temperature section
    Display::key_value("Temperature", std::to_string(static_cast<int>(current["temp_c"].get<double>())) + "°C", 
                      Utils::get_temperature_color(current["temp_c"]));
    Display::key_value("Feels Like", std::to_string(static_cast<int>(current["feelslike_c"].get<double>())) + "°C", 
                      Utils::get_temperature_color(current["feelslike_c"]));
    
    // Wind section
    Display::key_value("Wind Speed", std::to_string(static_cast<int>(current["wind_kph"].get<double>())) + " km/h", Colors::CYAN);
    Display::key_value("Wind Direction", current["wind_dir"].get<std::string>(), Colors::CYAN);
    Display::key_value("Wind Gust", std::to_string(static_cast<int>(current["gust_kph"].get<double>())) + " km/h", Colors::CYAN);
    
    // Atmospheric conditions
    Display::key_value("Pressure", std::to_string(static_cast<int>(current["pressure_mb"].get<double>())) + " mb", Colors::BLUE);
    Display::key_value("Humidity", std::to_string(current["humidity"].get<int>()) + "%", Colors::BLUE);
    Display::key_value("Visibility", std::to_string(static_cast<int>(current["vis_km"].get<double>())) + " km", Colors::BLUE);
    Display::key_value("UV Index", std::to_string(static_cast<int>(current["uv"].get<double>())), Colors::ORANGE);
    
    // Progress bars for key metrics
    Display::progress_bar(current["humidity"].get<int>(), "Humidity");
    Display::progress_bar(std::min(100, static_cast<int>(current["uv"].get<double>() * 10)), "UV Index");
}

void WeatherDisplay::show_astronomy_data(const json& data) {
    if (!data.contains("forecast") || data["forecast"]["forecastday"].empty()) return;
    
    auto& astro = data["forecast"]["forecastday"][0]["astro"];
    
    Display::section_header("ASTRONOMY");
    
    Display::key_value("Sunrise", astro["sunrise"].get<std::string>(), Colors::ORANGE);
    Display::key_value("Sunset", astro["sunset"].get<std::string>(), Colors::ORANGE);
    Display::key_value("Moonrise", astro["moonrise"].get<std::string>(), Colors::PURPLE);
    Display::key_value("Moonset", astro["moonset"].get<std::string>(), Colors::PURPLE);
    Display::key_value("Moon Phase", astro["moon_phase"].get<std::string>(), Colors::PURPLE);
    
    int moon_illumination = astro["moon_illumination"].get<int>();
    Display::progress_bar(moon_illumination, "Moon Illumination");
}

void WeatherDisplay::show_extended_forecast(const std::string& location) {
    Utils::animate_loading("Fetching extended forecast");
    
    json data = WeatherService::get_forecast(location, 7);
    if (data.is_null()) {
        Display::error_message("Failed to fetch forecast data for " + location);
        return;
    }
    
    Display::clear_screen();
    Display::banner();
    
    Display::seven_day_forecast(data);
    
    // Additional statistics
    if (data.contains("forecast")) {
        Display::section_header("FORECAST SUMMARY");
        
        auto& days = data["forecast"]["forecastday"];
        double avg_high = 0, avg_low = 0, total_rain = 0;
        int rainy_days = 0;
        
        for (const auto& day : days) {
            avg_high += day["day"]["maxtemp_c"].get<double>();
            avg_low += day["day"]["mintemp_c"].get<double>();
            total_rain += day["day"]["totalprecip_mm"].get<double>();
            if (day["day"]["daily_chance_of_rain"].get<int>() > 50) rainy_days++;
        }
        
        avg_high /= days.size();
        avg_low /= days.size();
        
        Display::key_value("Avg High", std::to_string(static_cast<int>(avg_high)) + "°C", Utils::get_temperature_color(avg_high));
        Display::key_value("Avg Low", std::to_string(static_cast<int>(avg_low)) + "°C", Utils::get_temperature_color(avg_low));
        Display::key_value("Total Rain", std::to_string(static_cast<int>(total_rain)) + " mm", Colors::BLUE);
        Display::key_value("Rainy Days", std::to_string(rainy_days) + "/" + std::to_string(days.size()), Colors::BLUE);
    }
}

void WeatherDisplay::show_hourly_forecast(const std::string& location) {
    Utils::animate_loading("Fetching hourly forecast");
    
    json data = WeatherService::get_hourly_forecast(location);
    if (data.is_null()) {
        Display::error_message("Failed to fetch hourly data for " + location);
        return;
    }
    
    Display::clear_screen();
    Display::banner();
    
    Display::hourly_forecast(data);
}

void WeatherDisplay::show_air_quality(const std::string& location) {
    Utils::animate_loading("Fetching air quality data");
    
    json data = WeatherService::get_air_quality(location);
    if (data.is_null()) {
        Display::error_message("Failed to fetch air quality data for " + location);
        return;
    }
    
    Display::clear_screen();
    Display::banner();
    
    Display::section_header("AIR QUALITY INDEX - " + location);
    
    if (data.contains("current") && data["current"].contains("air_quality")) {
        auto& aqi = data["current"]["air_quality"];
        
        int us_epa = aqi["us-epa-index"].get<int>();
        std::string aqi_level, health_advice;
        
        switch(us_epa) {
            case 1: aqi_level = "Good"; health_advice = "Air quality is satisfactory"; break;
            case 2: aqi_level = "Moderate"; health_advice = "Acceptable for most people"; break;
            case 3: aqi_level = "Unhealthy for Sensitive Groups"; health_advice = "Sensitive individuals should limit outdoor activities"; break;
            case 4: aqi_level = "Unhealthy"; health_advice = "Everyone should limit outdoor activities"; break;
            case 5: aqi_level = "Very Unhealthy"; health_advice = "Avoid outdoor activities"; break;
            case 6: aqi_level = "Hazardous"; health_advice = "Emergency conditions - stay indoors"; break;
            default: aqi_level = "Unknown"; health_advice = "Data unavailable";
        }
        
        Display::key_value("AQI Level", std::to_string(us_epa) + " - " + aqi_level, Utils::get_aqi_color(us_epa));
        Display::key_value("Health Advice", health_advice, Colors::YELLOW);
        
        std::cout << std::endl;
        Display::progress_bar(us_epa * 16, "Air Quality Index");
        
        Display::section_header("POLLUTANT BREAKDOWN");
        
        Display::key_value("Carbon Monoxide", std::to_string(static_cast<int>(aqi["co"].get<double>())) + " µg/m³", Colors::CYAN);
        Display::key_value("Nitrogen Dioxide", std::to_string(static_cast<int>(aqi["no2"].get<double>())) + " µg/m³", Colors::CYAN);
        Display::key_value("Ozone", std::to_string(static_cast<int>(aqi["o3"].get<double>())) + " µg/m³", Colors::CYAN);
        Display::key_value("Sulphur Dioxide", std::to_string(static_cast<int>(aqi["so2"].get<double>())) + " µg/m³", Colors::CYAN);
        Display::key_value("PM 2.5", std::to_string(static_cast<int>(aqi["pm2_5"].get<double>())) + " µg/m³", Colors::CYAN);
        Display::key_value("PM 10", std::to_string(static_cast<int>(aqi["pm10"].get<double>())) + " µg/m³", Colors::CYAN);
    } else {
        Display::warning_message("Air quality data not available for this location");
    }
}

void WeatherDisplay::show_weather_alerts(const std::string& location) {
    Utils::animate_loading("Checking weather alerts");
    
    json data = WeatherService::get_alerts(location);
    if (data.is_null()) {
        Display::error_message("Failed to fetch alerts for " + location);
        return;
    }
    
    Display::clear_screen();
    Display::banner();
    
    Display::section_header("WEATHER ALERTS - " + location);
    
    if (data.contains("alerts") && !data["alerts"]["alert"].empty()) {
        auto& alerts = data["alerts"]["alert"];
        
        for (size_t i = 0; i < alerts.size(); ++i) {
            const auto& alert = alerts[i];
            
            std::cout << "\n╭── Alert " << (i + 1) << " ──────────────────────────────────────────╮" << std::endl;
            std::cout << "│ " << Colors::BOLD << Colors::RED << "⚠️  " << alert["headline"].get<std::string>() << Colors::RESET << std::endl;
            std::cout << "├─────────────────────────────────────────────────────────────┤" << std::endl;
            std::cout << "│ " << Colors::BOLD << "Severity: " << Colors::RESET << Colors::ORANGE << alert["severity"].get<std::string>() << Colors::RESET << std::endl;
            std::cout << "│ " << Colors::BOLD << "Areas: " << Colors::RESET << alert["areas"].get<std::string>() << std::endl;
            std::cout << "│ " << Colors::BOLD << "Expires: " << Colors::RESET << alert["expires"].get<std::string>() << std::endl;
            std::cout << "│ " << Colors::BOLD << "Description: " << Colors::RESET << std::endl;
            
            // Word wrap the description
            std::string desc = alert["desc"].get<std::string>();
            std::vector<std::string> words = Utils::split(desc, ' ');
            std::string line = "│ ";
            
            for (const auto& word : words) {
                if (line.length() + word.length() > 57) {
                    std::cout << line << std::endl;
                    line = "│ " + word + " ";
                } else {
                    line += word + " ";
                }
            }
            if (line.length() > 2) std::cout << line << std::endl;
            
            std::cout << "╰─────────────────────────────────────────────────────────────╯" << std::endl;
        }
    } else {
        Display::success_message("No weather alerts for " + location + " - All clear! 🌤️");
    }
}

void WeatherDisplay::show_pollen_data(const std::string& location) {
    Utils::animate_loading("Fetching pollen information");
    
    json data = WeatherService::get_forecast(location, 3);
    if (data.is_null()) {
        Display::error_message("Failed to fetch pollen data for " + location);
        return;
    }
    
    Display::clear_screen();
    Display::banner();
    
    Display::section_header("POLLEN & ALLERGEN DATA - " + location);
    
    // Note: WeatherAPI doesn't provide detailed pollen data, so we'll show what's available
    Display::info_message("Detailed pollen data requires a specialized API. Showing available air quality metrics.");
    
    if (data.contains("current") && data["current"].contains("air_quality")) {
        auto& aqi = data["current"]["air_quality"];
        int aqi_level = aqi["us-epa-index"].get<int>();
        
        std::string pollen_estimate;
        if (aqi_level <= 2) pollen_estimate = "Low";
        else if (aqi_level <= 3) pollen_estimate = "Moderate";
        else if (aqi_level <= 4) pollen_estimate = "High";
        else pollen_estimate = "Very High";
        
        Display::key_value("Estimated Pollen", pollen_estimate, Utils::get_aqi_color(aqi_level));
        Display::key_value("Air Quality", std::to_string(aqi_level) + "/6", Utils::get_aqi_color(aqi_level));
        
        Display::progress_bar(aqi_level * 16, "Allergen Risk Level");
        
        std::cout << "\n" << Colors::YELLOW << "💡 Tip: ";
        if (aqi_level <= 2) {
            std::cout << "Good conditions for outdoor activities!";
        } else if (aqi_level <= 3) {
            std::cout << "Sensitive individuals should monitor symptoms.";
        } else {
            std::cout << "Consider staying indoors and using air purifiers.";
        }
        std::cout << Colors::RESET << std::endl;
    } else {
        Display::warning_message("Pollen data not available for this location");
    }
}

void WeatherDisplay::show_ip_lookup(const std::string& ip) {
    Utils::animate_loading("Looking up IP information");
    
    json data = WeatherService::get_ip_lookup(ip);
    if (data.is_null()) {
        Display::error_message("Failed to lookup IP: " + ip);
        return;
    }
    
    Display::clear_screen();
    Display::banner();
    
    Display::section_header("IP GEOLOCATION LOOKUP");
    
    Display::key_value("IP Address", data["ip"].get<std::string>(), Colors::CYAN);
    Display::key_value("Type", data["type"].get<std::string>(), Colors::CYAN);
    Display::key_value("Country", data["country_name"].get<std::string>(), Colors::GREEN);
    Display::key_value("Region", data["region"].get<std::string>(), Colors::GREEN);
    Display::key_value("City", data["city"].get<std::string>(), Colors::GREEN);
    Display::key_value("Latitude", std::to_string(data["lat"].get<double>()), Colors::YELLOW);
    Display::key_value("Longitude", std::to_string(data["lon"].get<double>()), Colors::YELLOW);
    Display::key_value("Timezone", data["tz_id"].get<std::string>(), Colors::PURPLE);
    Display::key_value("Local Time", data["localtime"].get<std::string>(), Colors::PURPLE);
    
    // Show weather for this location
    std::string location = data["city"].get<std::string>() + ", " + data["country_name"].get<std::string>();
    
    Display::info_message("Fetching weather for detected location...");
    Utils::animate_loading("Loading weather data", 1000);
    
    json weather_data = WeatherService::get_current_weather(location);
    if (!weather_data.is_null()) {
        Display::weather_card(weather_data, location);
    }
}

// Settings and configuration management
class Settings {
public:
    static void show_settings_menu();
    static void configure_api_key();
    static void configure_units();
    static void toggle_logging();
    static void clear_cache();
    static void export_settings();
    static void import_settings();
};

void Settings::show_settings_menu() {
    Display::clear_screen();
    Display::banner();
    
    Display::section_header("SETTINGS & CONFIGURATION");
    
    std::cout << "\n" << Colors::BOLD << "Current Configuration:" << Colors::RESET << std::endl;
    Display::key_value("API Key", Config::API_KEY.substr(0, 8) + "...", Colors::GRAY);
    Display::key_value("Timeout", std::to_string(Config::TIMEOUT) + " seconds", Colors::CYAN);
    Display::key_value("Logging", Logger::logging_enabled ? "Enabled" : "Disabled", 
                      Logger::logging_enabled ? Colors::GREEN : Colors::RED);
    
    std::cout << "\n╭────────── Settings Menu ──────────╮" << std::endl;
    std::cout << "│                                    │" << std::endl;
    std::cout << "│ 1. 🔑 Configure API Key            │" << std::endl;
    std::cout << "│ 2. 📊 Toggle Logging               │" << std::endl;
    std::cout << "│ 3. 🗑️  Clear Cache                  │" << std::endl;
    std::cout << "│ 4. 📤 Export Settings              │" << std::endl;
    std::cout << "│ 5. 📥 Import Settings              │" << std::endl;
    std::cout << "│ 6. ⬅️  Back to Main Menu           │" << std::endl;
    std::cout << "│                                    │" << std::endl;
    std::cout << "╰────────────────────────────────────╯" << std::endl;
    
    std::cout << "\n" << Colors::BOLD << Colors::PURPLE << "Select option (1-6): " << Colors::RESET;
    
    std::string choice;
    std::getline(std::cin, choice);
    choice = Utils::trim(choice);
    
    if (choice == "1") configure_api_key();
    else if (choice == "2") toggle_logging();
    else if (choice == "3") clear_cache();
    else if (choice == "4") export_settings();
    else if (choice == "5") import_settings();
    else if (choice == "6") return;
    else {
        Display::error_message("Invalid choice");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        show_settings_menu();
    }
}

void Settings::configure_api_key() {
    std::cout << "\n" << Colors::CYAN << "Enter new API key (or press Enter to keep current): " << Colors::RESET;
    std::string new_key;
    std::getline(std::cin, new_key);
    
    if (!new_key.empty()) {
        Config::API_KEY = new_key;
        if (Config::save_config()) {
            Display::success_message("API key updated successfully");
        } else {
            Display::error_message("Failed to save configuration");
        }
    }
}

void Settings::toggle_logging() {
    Logger::enable_logging(!Logger::logging_enabled);
    Display::success_message("Logging " + std::string(Logger::logging_enabled ? "enabled" : "disabled"));
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Settings::clear_cache() {
    HttpClient::clear_cache();
    Display::success_message("Cache cleared successfully");
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Settings::export_settings() {
    if (Config::save_config()) {
        Display::success_message("Settings exported to " + Config::CONFIG_FILE);
    } else {
        Display::error_message("Failed to export settings");
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void Settings::import_settings() {
    if (Config::load_config()) {
        Display::success_message("Settings imported successfully");
    } else {
        Display::error_message("Failed to import settings");
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// Enhanced main application class
class WeatherApp {
private:
    bool running;
    
    std::string get_location_input();
    std::string get_ip_input();
    void handle_export_report();
    
public:
    WeatherApp() : running(true) {}
    
    void initialize();
    void run();
    void shutdown();
};

void WeatherApp::initialize() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    Config::load_config();
    Logger::enable_logging(true);
    Logger::info("Weather CLI Pro v2.0 started");
}

std::string WeatherApp::get_location_input() {
    std::cout << "\n" << Colors::BOLD << Colors::CYAN << "🌍 Enter location (city, coordinates, or postal code): " << Colors::RESET;
    std::string location;
    std::getline(std::cin, location);
    return Utils::trim(location);
}

std::string WeatherApp::get_ip_input() {
    std::cout << "\n" << Colors::BOLD << Colors::CYAN << "🌐 Enter IP address (or 'auto' for your IP): " << Colors::RESET;
    std::string ip;
    std::getline(std::cin, ip);
    ip = Utils::trim(ip);
    return ip.empty() || ip == "auto" ? "auto" : ip;
}

void WeatherApp::handle_export_report() {
    std::string location = get_location_input();
    if (location.empty()) {
        Display::error_message("Location cannot be empty");
        return;
    }
    
    std::cout << "\n" << Colors::CYAN << "📄 Enter filename (default: weather_report.txt): " << Colors::RESET;
    std::string filename;
    std::getline(std::cin, filename);
    if (filename.empty()) filename = "weather_report.txt";
    
    Utils::animate_loading("Generating weather report");
    
    if (WeatherService::export_weather_report(location, filename)) {
        Display::success_message("Weather report exported to " + filename);
    } else {
        Display::error_message("Failed to export weather report");
    }
}

void WeatherApp::run() {
    while (running) {
        Display::clear_screen();
        Display::banner();
        
        Display::menu();
        
        std::string choice;
        std::getline(std::cin, choice);
        choice = Utils::trim(choice);
        
        if (choice == "0" || choice == "exit" || choice == "quit") {
            running = false;
            break;
        }
        
        std::string location;
        
        try {
            if (choice == "1") {
                location = get_location_input();
                if (!location.empty()) {
                    WeatherDisplay::show_current_weather(location);
                } else {
                    Display::error_message("Location cannot be empty");
                }
            }
            else if (choice == "2") {
                location = get_location_input();
                if (!location.empty()) {
                    WeatherDisplay::show_extended_forecast(location);
                } else {
                    Display::error_message("Location cannot be empty");
                }
            }
            else if (choice == "3") {
                location = get_location_input();
                if (!location.empty()) {
                    WeatherDisplay::show_hourly_forecast(location);
                } else {
                    Display::error_message("Location cannot be empty");
                }
            }
            else if (choice == "4") {
                location = get_location_input();
                if (!location.empty()) {
                    WeatherDisplay::show_air_quality(location);
                } else {
                    Display::error_message("Location cannot be empty");
                }
            }
            else if (choice == "5") {
                location = get_location_input();
                if (!location.empty()) {
                    WeatherDisplay::show_weather_alerts(location);
                } else {
                    Display::error_message("Location cannot be empty");
                }
            }
            else if (choice == "6") {
                location = get_location_input();
                if (!location.empty()) {
                    WeatherDisplay::show_pollen_data(location);
                } else {
                    Display::error_message("Location cannot be empty");
                }
            }
            else if (choice == "7") {
                std::string ip = get_ip_input();
                if (!ip.empty()) {
                    WeatherDisplay::show_ip_lookup(ip);
                } else {
                    Display::error_message("IP address cannot be empty");
                }
            }
            else if (choice == "8") {
                Settings::show_settings_menu();
                continue; // Don't wait for enter after settings
            }
            else if (choice == "9") {
                handle_export_report();
            }
            else {
                Display::error_message("Invalid choice. Please select 0-9.");
            }
            
            if (choice != "8") { // Don't pause after settings menu
                std::cout << "\n" << Colors::GRAY << "Press Enter to return to main menu..." << Colors::RESET;
                std::cin.ignore();
            }
            
        } catch (const std::exception& e) {
            Logger::error("Exception in main loop: " + std::string(e.what()));
            Display::error_message("An error occurred: " + std::string(e.what()));
            std::cout << "\n" << Colors::GRAY << "Press Enter to continue..." << Colors::RESET;
            std::cin.ignore();
        }
    }
}

void WeatherApp::shutdown() {
    Logger::info("Weather CLI Pro v2.0 shutting down");
    curl_global_cleanup();
    Display::clear_screen();
    
    // Goodbye animation
    std::vector<std::string> goodbye_frames = {
        "🌤️", "⛅", "🌥️", "☁️", "🌦️", "🌧️", "⛈️", "🌩️", "🌨️", "❄️"
    };
    
    std::cout << "\n" << Colors::BOLD << Colors::CYAN << "Thank you for using Weather CLI Pro!" << Colors::RESET << std::endl;
    std::cout << Colors::GREEN;
    
    for (const auto& frame : goodbye_frames) {
        std::cout << "\r" << frame << " Have a great day! " << frame << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    std::cout << "\r" << Colors::BOLD << "🌈 Weather CLI Pro - Stay informed, stay safe! 🌈" << Colors::RESET << std::endl;
    std::cout << Colors::GRAY << "Version 2.0 - Professional Weather Intelligence" << Colors::RESET << std::endl;
}

// Enhanced main function with error handling
int main(int argc, char* argv[]) {
    try {
        WeatherApp app;
        app.initialize();
        
        // Command line argument support
        if (argc > 1) {
            std::string location;
            for (int i = 1; i < argc; ++i) {
                if (i > 1) location += " ";
                location += argv[i];
            }
            
            // Quick weather lookup from command line
            Display::clear_screen();
            Display::banner();
            Display::info_message("Quick lookup for: " + location);
            
            WeatherDisplay::show_current_weather(location);
            
            std::cout << "\n" << Colors::GRAY << "Press Enter to continue to main menu..." << Colors::RESET;
            std::cin.ignore();
        }
        
        app.run();
        app.shutdown();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << Colors::RED << "Fatal error: " << e.what() << Colors::RESET << std::endl;
        Logger::error("Fatal error: " + std::string(e.what()));
        return 1;
    } catch (...) {
        std::cerr << Colors::RED << "Unknown fatal error occurred" << Colors::RESET << std::endl;
        Logger::error("Unknown fatal error occurred");
        return 1;
    }
}
