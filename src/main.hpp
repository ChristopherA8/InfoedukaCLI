#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <future>

#include "token.hpp"

using namespace std;
using namespace ftxui;

const string APP_TITLE = "Infoeduka";

std::future<nlohmann::json> fetchDataAsync(const std::string &url) {
  return std::async(std::launch::async, [url]() {
    auto response = cpr::Get(cpr::Url{url}, cpr::Cookies{{"PHPSESSID", TOKEN}});
    return nlohmann::json::parse(response.text);
  });
}

struct ClassInfo {
    int idPredmet;
    std::string predmet;
    std::string sifra;
    int ects;
    bool potpis;
    std::optional<int> ocjena;  // empty = null or ""
    std::string ocjenaOpisno;
    std::string ocjenaDatum;
    bool priznat;
};

// Helper: convert "dd.mm.yyyy." string to time_t
time_t parseDate(const std::string& s) {
    std::tm tm = {};
    sscanf(s.c_str(), "%d.%d.%d.", &tm.tm_mday, &tm.tm_mon, &tm.tm_year);
    tm.tm_mon -= 1;       // months 0-11
    tm.tm_year -= 1900;   // years since 1900
    return std::mktime(&tm);
}

// Helper: get today as time_t
time_t today() {
    auto now = std::chrono::system_clock::now();
    std::time_t tnow = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&tnow);
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
    return std::mktime(&tm);
}