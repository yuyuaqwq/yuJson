#include <iostream>

#include <yuJson/json.hpp>


int main()
{
    using yuJson::Json;
    using namespace yuJson::value;

    Json json;

    json.Parse("s");

    json.Parse(R"({ "qvq":   ["233", 123, "emm", {"qaq":"sb", "emmm":true}] })");
    std::cout << json.Print() << std::endl;

    json.GetObject().Set("sb", 233);
    std::cout << json.GetObject().Get("sb").ToNumber().GetInt() << std::endl;
    std::cout << json.Print() << std::endl;

    json.Set("233");
    std::cout << json.Print() << std::endl;

    json.Set(nullptr);
    std::cout << json.Print() << std::endl;
}
