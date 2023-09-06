#include <iostream>
#include <yuJson/json.hpp>


int main()
{

    /*
    * build
    */
    auto arr_build = yuJson::Array({ "key", "value" });
    std::cout << arr_build.Print() << std::endl << std::endl;

    auto obj_build = yuJson::Object({"key", "value"});
    std::cout << obj_build.Print() << std::endl << std::endl;
    
    yuJson::Json json{ 
        "int", 123, 
        "arr", yuJson::Array({ "1", 2 }),
        "obj", { "key", "value" },
        "str", "2333" 
    };
    std::cout << json.Print() << std::endl << std::endl;

    /*
    * value get, set, convert
    */
    std::cout << json["int"].Int() << std::endl;
    std::cout << json["arr"][0].String() << std::endl;
    std::cout << json["obj"]["key"].String() << std::endl;

    json["obj"]["key"] = { "sub_key", "sub_value" };
    std::cout << json["obj"]["key"].Print() << std::endl;


    std::cout << json["str"].ConvertToInt() << std::endl;

    json["str"] = "111";
    std::cout << json["str"].ConvertToInt() << std::endl;
    
    std::cout << json["int"].ConvertToString() << std::endl;

    json["int"] = 10000;
    std::cout << json["int"].ConvertToString() << std::endl;



    /*
    * serialization and deserialization
    */
    auto invalid_test = yuJson::Parse("{a}");
    if (invalid_test.IsValid()) {
        std::cout << "valid json text" << std::endl << std::endl;
    }
    else {
        std::cout << "invalid json text" << std::endl << std::endl;
    }
    auto valid_test = yuJson::Parse("{\"test\":123}");
    if (invalid_test.IsValid()) {
        std::cout << "valid" << std::endl << std::endl;
    }

    json["cccccc"] = "qaq";
    json["obj"]["aa"] = { 1,2,3,4 };
    std::cout << json.Print() << std::endl << std::endl;

    json = yuJson::Parse(R"({ "qvq":     ["233", 123, "emm", {"qa\q":"sb", "emmm":true}] })");
    std::cout << json.Print() << std::endl << std::endl;

    std::cout << (json.Find("qvq") ? "exist" : "non-existent") << std::endl << std::endl;
    std::cout << (json.Find("qaq") ? "exist" : "non-existent") << std::endl << std::endl;

    std::cout << json["qvq"][3].Print() << std::endl << std::endl;


    /*
    * iterator
    */
    std::cout << json.Print() << std::endl << std::endl;
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
}
