#include <iostream>
#include <yuJson/json.hpp>


int main()
{

    auto arr = yuJson::Array({ "emm", "123" });
    std::cout << arr.Print() << std::endl << std::endl;

    auto obj = yuJson::Object({"emm", "123"});
    std::cout << obj.Print() << std::endl << std::endl;
    
    yuJson::Json test = {
        "emm",  yuJson::Array({ "hello", {"world", "??"}})
    };


    //auto& qqq = obj.GetObject().At("abc");

    //obj["aa"]["bb"];

    std::cout << obj["str"].ConvertToInt(100) << std::endl;

    obj["str"] = "111";
    std::cout << obj["str"].ConvertToInt(100) << std::endl;
    
    std::cout << obj["int"].ConvertToString("emm") << std::endl;

    obj["int"] = 10000;
    std::cout << obj["int"].ConvertToString("emm") << std::endl;


    yuJson::Json json{ "awdwd", { "aa", 123 }, "emmm", 3333};
    json["cccccc"] = "qaq";
    json["awdwd"]["aa"] = { 1,2,3,4 };
    std::cout << json.Print() << std::endl << std::endl;

    auto cac = yuJson::Parse("{a}");
    if (cac.IsValid()) {
        std::cout << "valid" << std::endl << std::endl;
    }
    for (auto& sub_json : json) {
        std::cout << sub_json.key() << ":";
        sub_json.value() = "??";
        std::cout << sub_json.value().Print() << std::endl << std::endl;
    }

    json = {1, 2};
    std::cout << json.Print() << std::endl << std::endl;

    for (auto& sub_json : json) {
        std::cout << sub_json.value().Print() << std::endl << std::endl;
    }

    json = yuJson::Parse(R"({ "qvq":     ["233", 123, "emm", {"qa\q":"sb", "emmm":true}] })");
    std::cout << json.Print() << std::endl << std::endl;
    
    std::cout << (json.Find("qvq") ? "exist" : "non-existent") << std::endl << std::endl;
    std::cout << (json.Find("qaq") ? "exist" : "non-existent") << std::endl << std::endl;

    std::cout << json["qvq"][3].Print() << std::endl << std::endl;

    

    //std::cout << json["qvq"][3]["emmm"].GetBoolean() << std::endl << std::endl;
    std::cout << json.Print() << std::endl << std::endl;
}
