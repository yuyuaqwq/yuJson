# yuJson
C++ Json

# 特性
- header-only
- 仅 `1k+` 代码行，适合学习
- 要求C++14

# 快速入门
``` C++
#include <iostream>

#include <yuJson/json.hpp>

int main() {
    using Json = yuJson::Json;
	// Parse
	auto json_parse = Json::Parse(R"({"key":["arr_ele", 1, "abc", {"efg":"string", "emmm":true}]})");
	std::cout << json_parse.Print(true) << std::endl << std::endl;

	// Dump
	yuJson::Json json_dump { 
		"int", 123, 
		"arr", Json::Array({ "1", 2 }),
		"obj", { "key", "value" },
		"str", "2333" 
	};
	std::cout << json_dump.Print() << std::endl << std::endl;

	return 0;
}
```

# 更多示例
参考test.cpp