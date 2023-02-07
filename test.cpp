#include <iostream>

#include <yuJson/json.hpp>


int main()
{
    using yuJson::Json;
    using namespace yuJson::value;

    Json json;

    json.Parse("s");

    json.Parse(R"({ "qv\"q":   ["233", 123, "emm", {"qaq":"s\"b", "emmm":true}] })");
    std::cout << json.Print() << std::endl;
    
    json.GetObject().Set("sb", 233);

    auto& val = json.GetObject().Get("2b");
    std::cout << (val.IsString() ? val.ToString().Get() : "对象中不存在该键值对") << std::endl;

    std::cout << json.GetObject().Get("sb").ToNumber().GetInt() << std::endl;
    std::cout << json.Print() << std::endl;

    auto obj = CreateObject();
    obj->Set("adw", std::move(json.GetObject()));

    json.Set(std::move(obj));
    std::cout << json.Print() << std::endl;

    json.Set(nullptr);
    std::cout << json.Print() << std::endl;
}
