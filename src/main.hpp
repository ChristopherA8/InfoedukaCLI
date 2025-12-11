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

using namespace std;
using namespace ftxui;

const string APP_TITLE = "Infoeduka";

struct ClassSession {
    std::string datum;
    std::string terminPocetak;
    std::string terminKraj;
    int terminTrajanje;
    std::string dvorana;
    std::string url;
    std::string nastavnik;
    std::string tip;
    std::string predmet;
};

void from_json(const nlohmann::json& j, ClassSession& c) {
    j.at("datum").get_to(c.datum);
    j.at("terminPocetak").get_to(c.terminPocetak);
    j.at("terminKraj").get_to(c.terminKraj);
    j.at("terminTrajanje").get_to(c.terminTrajanje);
    j.at("dvorana").get_to(c.dvorana);
    j.at("url").get_to(c.url);
    j.at("nastavnik").get_to(c.nastavnik);
    j.at("tip").get_to(c.tip);
    j.at("predmet").get_to(c.predmet);
}