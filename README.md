# 🌤️ Weather CLI Pro

<div align="center">

[![Weather CLI Pro](https://img.shields.io/badge/Weather-CLI%20Pro-blue?style=for-the-badge&logo=weather&logoColor=white)](https://github.com/FJ-cyberzilla/Weather)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen?style=flat-square)](https://github.com/FJ-cyberzilla/Weather)
[![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows%20%7C%20Termux-lightgrey?style=flat-square)](https://github.com/FJ-cyberzilla/Weather)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square&logo=cplusplus)](https://isocpp.org/)
[![WeatherAPI](https://img.shields.io/badge/WeatherAPI-Powered-orange?style=flat-square)](https://weatherapi.com/)

**Professional Weather Intelligence Platform for Command Line**

*Stay informed, stay safe with comprehensive weather data at your fingertips*

</div>
Professional Weather Intelligence Platform for Command Line

Stay informed, stay safe with comprehensive weather data at your fingertips

</div>

---

✨ Features

🌍 Comprehensive Weather Data

· Current Conditions - Real-time weather with detailed metrics
· 7-Day Extended Forecast - Week-ahead predictions
· 24-Hour Detailed Forecast - Hourly breakdown
· Weather Alerts & Warnings - Safety notifications
· Air Quality Index - Pollution levels and health advice
· Pollen Information - Allergen data for sensitive individuals

🎨 Beautiful Interface

· Rich 256-Color Display - Weather-condition-specific colors
· Unicode Icons & Symbols - Visual weather representations
· Progress Bars - Humidity, UV index, air quality visualization
· Animated Loading - Smooth user experience
· Responsive Layout - Optimized for all terminal sizes

⚡ Advanced Features

· Smart Caching - 5-minute cache to reduce API calls
· Retry Logic - Exponential backoff for network resilience
· IP Geolocation - Automatic location detection
· Export Reports - Generate weather reports to files
· Configuration Management - Customizable settings
· Comprehensive Logging - Debug and error tracking

📱 Multi-Platform Support

· Linux - Native support
· macOS - Full compatibility
· Windows - Cross-platform ready
· Termux - Android terminal support
· WSL - Windows Subsystem for Linux

🚀 Quick Start

Prerequisites

· C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2019+)
· libcurl development libraries
· nlohmann/json library

Installation

🐧 Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev nlohmann-json3-dev

# Clone and build
git clone https://github.com/FJ-cyberzilla/Weather.git
cd Weather
make
sudo make install
```

🍎 macOS

```bash
# Install dependencies with Homebrew
brew install curl nlohmann-json

# Clone and build
git clone https://github.com/FJ-cyberzilla/Weather.git
cd Weather
make
make install
```

📱 Termux (Android)

```bash
# Install dependencies
pkg update && pkg upgrade
pkg install clang libcurl nlohmann-json

# Clone and build
git clone https://github.com/FJ-cyberzilla/Weather.git
cd Weather
chmod +x build.sh
./build.sh
```

🪟 Windows

```bash
# Using MSYS2/MinGW-w64
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-curl mingw-w64-x86_64-nlohmann-json

# Clone and build
git clone https://github.com/FJ-cyberzilla/Weather.git
cd Weather
make
```

📖 Usage

Command Line Interface

```bash
# Interactive mode
weather-cli

# Quick lookup
weather-cli "New York"
weather-cli "Tokyo, Japan"
weather-cli "40.7128,-74.0060"  # Coordinates
weather-cli "10001"             # Postal code
```

Menu Options

1. 🌤️ Current Weather & Today's Forecast
2. 📅 7-Day Extended Forecast
3. 🕒 24-Hour Detailed Forecast
4. 💨 Air Quality Index & Pollution Data
5. ⚠️ Weather Alerts & Warnings
6. 🌿 Pollen & Allergen Information
7. 🌍 IP Geolocation & Weather
8. ⚙️ Settings & Configuration
9. 📄 Export Weather Report

🔧 Configuration

API Key Setup

1. Get a free API key from WeatherAPI.com
2. Run the application and go to Settings (option 8)
3. Configure API Key (option 1)
4. Enter your API key

Configuration File

The app creates weather_cli_config.json with your settings:

```json
{
    "api_key": "your_api_key_here",
    "timeout": 15,
    "enable_logging": true
}
```

📊 Screenshots

```
╔══════════════════════════════════════════════════════════════╗
║  ╦ ╦┌─┐┌─┐┌─┐┬ ┬┌─┐┬─┐  ╔═╗┬  ┬  ╔═╗┬─┐┌─┐  ┌┬┐ ┬ ┬  ┬┌─┐  ║
║  ║║║├┤ ├─┤│  ├─┤├┤ ├┬┘  ║  │  ║  ╠═╝├┬┘│ │   │ ┬│└┬┘ │ │  ║
║  ╚╩╝└─┘┴ ┴└─┘┴ ┴└─┘┴└─  ╚═╝┴─┘┴  ╩  ┴└─└─┘  ─┴─ ┴ ┴  ┴└─┘  ║
╚══════════════════════════════════════════════════════════════╝
      🌍 Professional Weather Intelligence Platform v2.0
      ──────────────────────────────────────────────────

╭─────────────────────────────────────╮
│ New York, NY                        │
├─────────────────────────────────────┤
│ ☀️ Clear                            │
│ 22°C                                │
│ Feels like 24°C                     │
╰─────────────────────────────────────╯
```

🛠️ Building from Source

Manual Build

```bash
# Basic build
g++ -std=c++17 -o weather-cli weather_cli.cpp -lcurl -pthread

# With optimizations
g++ -std=c++17 -O2 -o weather-cli weather_cli.cpp -lcurl -pthread

# Debug build
g++ -std=c++17 -g -DDEBUG -o weather-cli-debug weather_cli.cpp -lcurl -pthread
```

Using Make

```bash
make                # Build release version
make debug          # Build debug version
make install        # Install to system
make clean          # Clean build files
make uninstall      # Remove from system
```

Using CMake

```bash
mkdir build && cd build
cmake ..
make
sudo make install
```

🔍 API Reference

WeatherAPI.com Endpoints Used

· Current Weather: /v1/current.json
· Forecast: /v1/forecast.json
· Weather Alerts: /v1/forecast.json?alerts=yes
· Air Quality: /v1/current.json?aqi=yes
· IP Lookup: /v1/ip.json

Supported Location Formats

· City names: "London", "New York"
· City, Country: "Tokyo, Japan"
· Coordinates: "40.7128,-74.0060"
· Postal codes: "10001", "SW1A 1AA"
· IP addresses: "8.8.8.8" or "auto"

🤝 Contributing

We welcome contributions! Please see our Contributing Guidelines.

Development Setup

```bash
git clone https://github.com/FJ-cyberzilla/Weather.git
cd Weather
git checkout -b feature/your-feature-name

# Make your changes
# Test thoroughly
# Submit a pull request
```

Code Style

· C++17 standard
· Consistent indentation (4 spaces)
· Comprehensive error handling
· Clear variable names
· Detailed comments for complex logic

📋 Roadmap

Version 2.1

· Historical weather data
· Weather maps integration
· Custom notification system
· Multiple location watchlist
· Weather trends analysis

Version 2.2

· Plugin system
· Themes and customization
· Database caching
· API key rotation
· Weather widget generation

Version 3.0

· Web dashboard
· Mobile app companion
· Weather prediction ML models
· Social weather sharing
· Advanced analytics

🫆 Troubleshooting

Common Issues

Build Errors

```bash
# Missing dependencies
sudo apt install libcurl4-openssl-dev nlohmann-json3-dev

# Compiler too old
g++ --version  # Should be 7.0 or higher
```

Runtime Issues

```bash
# Network connectivity
curl -I https://api.weatherapi.com/v1/current.json

# Permissions
chmod +x weather-cli

# Missing config
rm weather_cli_config.json  # Will recreate with defaults
```

API Issues

· Verify API key is correct
· Check API quota limits
· Ensure location format is supported

📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

🙏 Acknowledgments

· WeatherAPI.com for providing comprehensive weather data
· nlohmann/json for excellent JSON parsing
· libcurl for reliable HTTP requests
· The open-source community for inspiration and support

📞 Support

· Issues: GitHub Issues
· Email: fj.cyberzilla@example.com
· Documentation: Wiki

---

<div align="center">

Made with ⚡ for the terminal enthusiasts

https://img.shields.io/github/stars/FJ-cyberzilla/Weather?style=social
https://img.shields.io/github/forks/FJ-cyberzilla/Weather?style=social
https://img.shields.io/github/followers/FJ-cyberzilla?style=social

Weather CLI Pro - Professional Weather Intelligence Platform

</div>
