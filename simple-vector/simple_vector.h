#pragma once
#include "array_ptr.h"

#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <iterator>

class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity_to_reserve) : capacity_(capacity_to_reserve) {}
    size_t GetCapacity() const { return capacity_; }
private:
    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;
    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) :SimpleVector(size, Type{})
    {}
    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) :
        items_(size),
        size_(size),
        capacity_(size) {
        std::fill(this->begin(), this->end(), value);
    }

    SimpleVector(ReserveProxyObj x) {
        SimpleVector tmp(x.GetCapacity());
        tmp.Clear();
        swap(tmp);
        capacity_ = x.GetCapacity();
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) :
        items_(init.size()),
        size_(init.size()),
        capacity_(init.size()) {
        auto it = init.begin();
        for (size_t i = 0; i < init.size(); ++i) {
            items_[i] = *it;
            ++it;
        }
    }

    SimpleVector(const SimpleVector& other) {
        SimpleVector tmp(other.size_);
        tmp.capacity_ = other.capacity_;
        size_t j = 0;
        for (auto it = other.begin(); it != other.end(); ++it) {
            tmp.items_[j] = *it;
            ++j;
        }
        swap(tmp);
    }
    SimpleVector(SimpleVector&& other) {
        swap(other);
        other.capacity_ = other.size_ = 0;
        delete other.items_.Release();          
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector copy_rhs(rhs);
            swap(copy_rhs);
        }
        return *this;
    }
    SimpleVector& operator=(SimpleVector&& rhs) noexcept {
        if (this != &rhs) {
            swap(rhs);
            delete rhs.items_.Release();
            rhs.size_ = rhs.capacity_ = 0;
        }
        return *this;
    }


    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        // Напишите тело самостоятельно
        return size_;
    }
    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity_;
    }
    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_==0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        return items_[index];
    }
    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return items_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("");
        }
        return items_[index];
    }
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("");
        }
        return items_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size >= capacity_) {
            ArrayPtr<Type> new_arr(new_size);
            std::move(this->begin(), this->end(), new_arr.Get());
            new_arr.swap(items_);
            for (size_t i = size_; i < new_size; ++i)
            {
                items_[i] = Type{};
            }
            capacity_ = new_size;
        }
        else if(new_size >= size_){
            for (size_t i = size_; i < new_size; ++i)
            {
                items_[i] = Type{};
            }
        }
        size_ = new_size;
        
    }

    void swap(SimpleVector& other) noexcept {
        items_.swap(other.items_);
        size_t old_size = size_;
        size_t old_capacity = capacity_;

        size_ = other.size_;
        capacity_ = other.capacity_;

        other.size_ = old_size;
        other.capacity_ = old_capacity;
    }

   
    Iterator begin() noexcept {
        return &items_[0];
    }
    Iterator end() noexcept {
        return &items_[size_];
    }
    ConstIterator begin() const noexcept {
        return &items_[0];
    }
    ConstIterator end() const noexcept {
        return &items_[size_];
    }
    ConstIterator cbegin() const noexcept {
        return &items_[0];
    }
    ConstIterator cend() const noexcept {
        return &items_[size_];
    }

    
    void PopBack() noexcept {
        if (size_ == 0) {
            return;
        }
        --size_;
    }
    
    void PushBack(const Type& value) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_arr(new_capacity);
            std::move(this->begin(), this->end(), new_arr.Get());
            new_arr.swap(items_);
            capacity_ = new_capacity;
           
        }
            items_[size_] = value;
            ++size_;
    }
    void PushBack(Type&& value) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_arr(new_capacity);
            std::move(this->begin(), this->end(), new_arr.Get());
            items_.swap(new_arr);
            capacity_ = new_capacity;
        }
        items_[size_] = std::move(value);
        ++size_;
    }
   
    Iterator Erase(ConstIterator pos) {
        auto it = begin() + std::distance(cbegin(), pos);;
        std::move((it + 1), end(), it);
        --size_;
        return Iterator(pos);
    }
    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t h = pos - begin();
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_array(new_capacity);
            std::move(this->begin(), this->end(), new_array.Get());
            items_.swap(new_array);
            capacity_ = new_capacity;
        }
        std::move_backward(items_.Get() + h, items_.Get() + size_, items_.Get() + size_ + 1);
        items_[h] = value;
        ++size_;
        return &items_[h];
    }
    Iterator Insert(ConstIterator pos,Type&& value) {
        size_t h = pos - begin();
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
            ArrayPtr<Type> new_array(new_capacity);
            std::move(this->begin(), this->end(), new_array.Get());
            items_.swap(new_array);
            capacity_ = new_capacity;
        }
        std::move_backward(items_.Get() + h, items_.Get() + size_, items_.Get() + size_ + 1);
        items_[h] = std::move(value);
        ++size_;
        return &items_[h];
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            ArrayPtr<Type> new_array(new_capacity);
            std::move(this->begin(), this->end(), new_array.Get());
            items_.swap(new_array);
            capacity_ = new_capacity;
        }
        
    }
    

private:
    ArrayPtr<Type> items_;

    size_t size_=0;
    size_t capacity_=0;
};




template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}
template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return !(rhs < lhs);
}
template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {

    return rhs<lhs;
}
template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
   
    return !(lhs < rhs);
}