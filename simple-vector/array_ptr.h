#include <cstdlib>
#include <utility>

template <typename Type>
class ArrayPtr {
public:
    ArrayPtr() = default;

   

    explicit ArrayPtr(size_t size) {
        if (size) {
            Type* tmp = new Type[size];
            raw_ptr_ = tmp;
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept : raw_ptr_(raw_ptr) {}

    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) noexcept {
        delete[] raw_ptr_;
        raw_ptr_ = exchange(other.raw_ptr_, nullptr);
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr& operator=(ArrayPtr&& other) noexcept {
            delete[] raw_ptr_;
            raw_ptr_ = exchange(other.raw_ptr_, nullptr);
    }

    [[nodiscard]] Type* Release() noexcept {
        Type* ret = raw_ptr_;
        raw_ptr_ = nullptr;
        return ret;
    }

    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    explicit operator bool() const {
        return (raw_ptr_) ? true : false;
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }


private:
    Type* raw_ptr_ = nullptr;
};