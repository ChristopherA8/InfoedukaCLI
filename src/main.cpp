#include "main.hpp"

Component Text(const std::string &t) {
  return Renderer([t] { return text(t) | borderEmpty; });
}

int main() {
  cout << "\033]0;" << APP_TITLE << "\a" << std::flush;
  auto scheduleJson =
      fetchDataAsync("https://student.algebra.hr/digitalnareferada/api/student/"
                     "raspored/tjedni");
  auto classesJson = fetchDataAsync(
      "https://student.algebra.hr/digitalnareferada/api/student/predmeti");
  nlohmann::json j = scheduleJson.get();
  nlohmann::json c = classesJson.get();
  // vector<ClassSession> sessions = j["data"].get<vector<ClassSession>>();

  auto screen = ScreenInteractive::Fullscreen();

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //\/ Schedule
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  std::vector<std::string> weekdays = {"Monday",   "Tuesday", "Wednesday",
                                       "Thursday", "Friday",  "Saturday",
                                       "Sunday"};

  // Group entries by day (starting from today)
  std::map<int, std::vector<nlohmann::json>> week_schedule;

  // --- Determine today ---
  time_t t_today = today();

  // --- Determine end of week (Sunday 23:59:59) ---
  std::tm tm = *std::localtime(&t_today);
  int days_until_sunday = 7 - tm.tm_wday; // wday: 0=Sun
  tm.tm_hour = 23;
  tm.tm_min = 59;
  tm.tm_sec = 59;
  tm.tm_mday += days_until_sunday;
  time_t t_end_week = std::mktime(&tm);

  // --- Process JSON ---
  for (auto &entry : j["data"]) {
    time_t t = parseDate(entry["datum"]);

    if (t < t_today)
      continue;
    if (t > t_end_week)
      continue;

    std::tm tm2 = *std::localtime(&t);

    int weekday_index = tm2.tm_wday;         // 0=Sun
    weekday_index = (weekday_index + 6) % 7; // make Mon=0

    week_schedule[weekday_index].push_back(entry);
  }

  // --- Sort events within each day by time ---
  for (auto &[day, events] : week_schedule) {
    std::sort(events.begin(), events.end(),
              [](const nlohmann::json &a, const nlohmann::json &b) {
                return a["terminPocetak"] < b["terminPocetak"];
              });
  }
  // Build FTXUI Renderer
  auto schedule_renderer = Renderer([=, &week_schedule] {
    std::vector<Element> columns;

    for (int i = 0; i < 5; ++i) { // Monday-Friday
      std::vector<Element> items;
      if (week_schedule.count(i)) {
        for (auto &entry : week_schedule.at(i)) {
          std::string subject = entry["predmet"];
          std::string room = entry["dvorana"];
          std::string time = std::string(entry["terminPocetak"]) + "-" +
                             std::string(entry["terminKraj"]);

          items.push_back(Text(subject)->Render());
          items.push_back(Text("Room: " + room)->Render());
          items.push_back(Text(time)->Render());
          items.push_back(separator()); // optional separator
        }
      } else {
        items.push_back(Text("No events")->Render());
      }

      columns.push_back(
          window(text(" " + weekdays[i] + " "), vbox(std::move(items))) | flex);
    }

    return hbox(std::move(columns)) | flex;
  });

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //\/ Classes
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  std::vector<ClassInfo> classes;

  for (auto &year : c["data"]) {
    for (auto &godina : year["godine"]) {
      for (auto &p : godina["predmeti"]) {
        ClassInfo c;

        c.idPredmet = p["idPredmet"];
        c.predmet = p["predmet"];
        c.sifra = p["sifra"];
        c.ects = p["ects"];
        c.potpis = p["potpis"];

        // ocjena may be "" or number
        if (p["ocjena"].is_number()) {
          c.ocjena = p["ocjena"].get<int>();
        } else {
          c.ocjena = std::nullopt;
        }

        c.ocjenaOpisno = p["ocjenaOpisno"];
        c.ocjenaDatum = p["ocjenaDatum"].is_null()
                            ? ""
                            : p["ocjenaDatum"].get<std::string>();
        c.priznat = p["priznat"];

        classes.push_back(std::move(c));
      }
    }
  }

  std::vector<std::string> entries;
  for (const auto &c : classes) {
    entries.push_back(c.predmet + " (" + c.sifra + ")");
  }

  int selected = 0;
  MenuOption class_menu_option;
  class_menu_option.on_enter = [] {};
  auto class_menu = Menu(&entries, &selected, class_menu_option);

  auto class_page = Renderer([&] {
    if (classes.empty())
      return text("No class data available");

    const auto &c = classes[selected];

    return vbox({
               text(c.predmet) | bold,
               text("Code: " + c.sifra),
               separator(),

               text("ECTS: " + std::to_string(c.ects)),
               text("Signed (potpis): " + std::string(c.potpis ? "Yes" : "No")),
               text("Recognized: " + std::string(c.priznat ? "Yes" : "No")),
               separator(),

               text("Grade: " + (c.ocjena ? std::to_string(*c.ocjena) : "—")),
               text("Grade (text): " +
                    (c.ocjenaOpisno.empty() ? "—" : c.ocjenaOpisno)),
               text("Grade date: " +
                    (c.ocjenaDatum.empty() ? "—" : c.ocjenaDatum)),
           }) |
           border;
  });

  // State:
  int size = 15;
  int size_min = 15;
  int size_max = 80;

  auto classesSplit = ResizableSplit({
      .main = class_menu, // ✅ container
      .back = class_page, // ✅ container
      .direction = Direction::Left,
      .main_size = &size,
      .min = &size_min,
      .max = &size_max,
  });

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //\/ Attendance
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //\/ Grades
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //\/ Menu
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  vector<string> tab_values{
      "Schedule",
      "Classes",
      "Grades",
      "Attendance",
  };
  int tab_selected = 0;

  auto tab_content = Container::Tab(
      {
          schedule_renderer,
          classesSplit,
          Text("Grades"),
          Text("Attendance"),
      },
      &tab_selected);

  auto option = MenuOption::HorizontalAnimated();
  auto menu = Menu(&tab_values, &tab_selected, option);
  auto container = Container::Vertical({menu, tab_content});

  auto renderer = Renderer(container, [&] {
    return vbox({
        menu->Render(),
        tab_content->Render() | flex,
    });
  });

  screen.Loop(renderer);
}