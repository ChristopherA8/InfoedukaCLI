#include "main.hpp"
#include "token.hpp"

std::future<nlohmann::json> fetchDataAsync(const std::string& url) {
    return std::async(std::launch::async, [url]() {
      auto response = cpr::Get(
        cpr::Url{url},
        cpr::Cookies{{"PHPSESSID", TOKEN}}
      );
      return nlohmann::json::parse(response.text);
    });
}

Component Text(const std::string& t) {
  return Renderer([t] { return text(t) | borderEmpty; });
}

int main() {
  cout << "\033]0;" << APP_TITLE << "\a" << std::flush;
  auto futureJson = fetchDataAsync("https://student.algebra.hr/digitalnareferada/api/student/raspored/tjedni");
  nlohmann::json j = futureJson.get();
  vector<ClassSession> sessions = j["data"].get<vector<ClassSession>>();

  auto screen = ScreenInteractive::Fullscreen();

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //\/ Content
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  auto schedule = Renderer([] {
    return hbox({
      window(text(" Monday "), {
        vbox({
          Text("Basics of Computer Networks")->Render(),
          Text("Room: B1")->Render(),
          Text("10:30-12:00")->Render()
        })
      }),
      window(text(" Tuesday "), {
        vbox({
          Text("Basics of Computer Networks")->Render(),
          Text("Room: B1")->Render(),
          Text("10:30-12:00")->Render()
        })
      }),
      window(text(" Wednesday "), {
        vbox({
          Text("Basics of Computer Networks")->Render(),
          Text("Room: B1")->Render(),
          Text("10:30-12:00")->Render()
        })
      }),
      window(text(" Thursday "), {
        vbox({
          Text("Basics of Computer Networks")->Render(),
          Text("Room: B1")->Render(),
          Text("10:30-12:00")->Render()
        })
      }),
      window(text(" Friday "), {
        vbox({
          Text("Basics of Computer Networks")->Render(),
          Text("Room: B1")->Render(),
          Text("10:30-12:00")->Render()
        })
      }),
    });
  });

  // State:
  int size = 15;
  int size_min = 15;
  int size_max = 80;

  auto classesSplit = ResizableSplit({
    .main = Renderer([] { return text("Left") | center; }),
    .back = Renderer([] { return text("Right") | center; }),
    .direction = Direction::Left,
    .main_size = &size,
    .min = &size_min,
    .max = &size_max,
  });

  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
  //\/ Menu
  //\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

  vector<string> tab_values{
      "Schedule", "Classes", "Grades", "Attendance",
  };
  int tab_selected = 0;

  auto tab_content = Container::Tab({
    schedule,
    classesSplit,
    Text(sessions[0].datum),
    Text("Attendance"),
  }, &tab_selected);

  auto option = MenuOption::HorizontalAnimated();
  auto menu = Menu(&tab_values, &tab_selected, option);
  auto container = Container::Vertical({ menu, tab_content });

  auto renderer = Renderer(container, [&] {
    return vbox({
                      menu->Render(),
                      tab_content->Render() | flex,
                  });
  });
 
  screen.Loop(renderer);
}