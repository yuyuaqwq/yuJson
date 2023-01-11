#include <iostream>

#include <yuJson/json.hpp>


int main()
{
	using yuJson::Json;
	using yuJson::value::String;
	using yuJson::value::Number;

	Json json;

	json.Parse(R"({ "qvq":   ["233", 123, "emm", {"qaq":"sb", "emmm":true}] })");
	std::cout << json.Print() << std::endl;

	json.Get()->ToObject()->Insert("sb", std::make_unique<String>("qtmd"));
	std::cout << json.Print() << std::endl;

	json.Get()->ToObject()->Set("sb", std::make_unique<Number>(233));

	std::cout << json.Get()->ToObject()->At("sb").ToNumber()->GetFloat() << std::endl;

	std::cout << json.Print() << std::endl;

	json.Set(std::make_unique<String>("233"));
	std::cout << json.Print() << std::endl;
}
