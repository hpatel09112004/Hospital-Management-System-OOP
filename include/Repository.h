#pragma once
// Generic, type-safe in-memory store (class template + Repository pattern).
// Works for any T that has `int getId() const`.
//   Repository<Doctor>, Repository<Patient>, Repository<Appointment>

#include <algorithm>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Exceptions.h"

template <typename T>
class Repository {
private:
    std::map<int, std::shared_ptr<T>> items_;
    std::string entityName_;

public:
    explicit Repository(std::string entityName) : entityName_(std::move(entityName)) {}

    void add(const std::shared_ptr<T>& item) {
        if (!item) throw ValidationException("cannot store an empty " + entityName_);
        const int id = item->getId();
        if (items_.count(id)) {
            throw ValidationException(entityName_ + " ID " + std::to_string(id) +
                                      " already exists");
        }
        items_[id] = item;
    }

    std::shared_ptr<T> get(int id) const {
        auto it = items_.find(id);
        if (it == items_.end()) {
            throw NotFoundException(entityName_ + " with ID " + std::to_string(id));
        }
        return it->second;
    }

    bool exists(int id) const { return items_.count(id) > 0; }

    void remove(int id) {
        if (items_.erase(id) == 0) {
            throw NotFoundException(entityName_ + " with ID " + std::to_string(id));
        }
    }

    std::vector<std::shared_ptr<T>> all() const {
        std::vector<std::shared_ptr<T>> out;
        out.reserve(items_.size());
        for (const auto& entry : items_) out.push_back(entry.second);
        return out;
    }

    // Member function template: filter with any callable (lambda, functor...).
    template <typename Predicate>
    std::vector<std::shared_ptr<T>> findIf(Predicate predicate) const {
        std::vector<std::shared_ptr<T>> out;
        for (const auto& entry : items_) {
            if (predicate(*entry.second)) out.push_back(entry.second);
        }
        return out;
    }

    int maxId() const { return items_.empty() ? 0 : items_.rbegin()->first; }
    std::size_t size() const { return items_.size(); }
    bool empty() const { return items_.empty(); }
};
