# yuJson
C++ Json

# 特性
- header-only
- 精致小巧，接口友好

# 快速入门
``` C++
#include <iostream>

#include <yuJson/json.hpp>

int main() {
	// Parse
	auto json_parse = yuJson::Parse(R"({"key":["arr_ele", 1, "abc", {"efg":"string", "emmm":true}]})");
	std::cout << json_parse.Print(true) << std::endl << std::endl;

	// Dump
	yuJson::Json json_dump { 
		"int", 123, 
		"arr", yuJson::Array({ "1", 2 }),
		"obj", { "key", "value" },
		"str", "2333" 
	};
	std::cout << json_dump.Print() << std::endl << std::endl;

	return 0;
}
```

# 更多示例
参考test.cpp