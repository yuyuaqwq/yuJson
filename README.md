# yuJson
基于递归下降的Json解析库

# 使用
仅需包含json.hpp文件

#入门
```C++
// 创建json中的对象
yuJson::Json obj = { "key","value", "rmb",99999 };
obj["key"] = "test";
obj["rmb"] = { "www","qvq" };
std::cout << obj.Print() << std::endl;

// 创建json中的列表
yuJson::Json json = { 1, 2, 3, 4, "hello world" };

// 如何分辨创建的是对象还是列表？
// 初始化列表中元素数量%2==0，且key对应槽为string则以创建对象

```
