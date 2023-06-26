#include <iostream>
#include <yuJson/json.hpp>


int main()
{
    auto obj = yuJson::Json::MakeObject();
    std::cout << obj.Print() << std::endl << std::endl;

    yuJson::Json json{ "awdwd", yuJson::Json({ "aa", 123 }), "emmm", 3333};
    json["cccccc"] = "qaq";
    json["awdwd"]["aa"] = { 1,2,3,4 };
    std::cout << json.Print() << std::endl << std::endl;

    json = {1, 2};
    std::cout << json.Print() << std::endl << std::endl;

    json.Parse(R"({ "qvq":   ["233", 123, "emm", {"qa\q":"sb", "emmm":true}] })");
    std::cout << json.Print() << std::endl << std::endl;
    
    std::cout << (json.Find("qvq") ? "exist" : "non-existent") << std::endl << std::endl;
    std::cout << (json.Find("qaq") ? "exist" : "non-existent") << std::endl << std::endl;

    std::cout << json["qvq"][3].Print() << std::endl << std::endl;

    //std::cout << json["qvq"][3]["emmm"].GetBoolean() << std::endl << std::endl;
    std::cout << json.Print() << std::endl << std::endl;
}
