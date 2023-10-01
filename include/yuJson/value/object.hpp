#ifndef YUJSON_VALUE_OBJECT_HPP_
#define YUJSON_VALUE_OBJECT_HPP_

#include <stdexcept>

#include <yuJson/value/value.hpp>

namespace yuJson {
class Json;
namespace value {
class ObjectValue : public ValueInterface {
public:
    ObjectValue() noexcept { }
    ObjectValue(ObjectValue&& obj) noexcept : obj_(std::move(obj.obj_)) { }
    ~ObjectValue() noexcept { }

    ObjectValue(const ObjectValue&) = delete;
    void operator=(const ObjectValue&) = delete;

    virtual ValueType Type() const noexcept {
        return ValueType::kObject;
    }

    ValuePtrtMap& GetMap() noexcept {
        return obj_;
    }

    ValuePtr& At(const std::string& key) {
        return obj_.at(key);
    }

    ValuePtr& operator[](const std::string& key) {
        return obj_[key];
    }

    bool Exist(const std::string& key) noexcept {
        return obj_.find(key) != obj_.end();
    }

    void Set(const std::string& key, ValuePtr value) {
        auto& it = obj_.operator[](key);
        it = std::move(value);
    }

    void Delete(const std::string& key) noexcept {
        obj_.erase(key);
    }

private:
    ValuePtrtMap obj_;
};

using ObjectPtr = std::unique_ptr<ObjectValue>;
} // namespace value
} // namespace yuJson

#endif // YUJSON_VALUE_OBJECT_HPP_