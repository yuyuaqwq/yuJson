# yuJson
C++ Json数据交互库

# 特性
- header-only
- 接口友好
- 仅1.2k代码

# 引用
仅需包含json.hpp文件

# 快速入门
``` C++
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
```

# 更多示例
参考test.cpp