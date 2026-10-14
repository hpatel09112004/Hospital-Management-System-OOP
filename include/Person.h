#pragma once
// Abstract base class for every human in the system.
//   - Encapsulation: data is protected, changed only through validating setters.
//   - Abstraction:   getRole() and serialize() are pure virtual, so Person
//                    can never be instantiated directly.
//   - Static member: population_ counts live Person objects.

#include <iosfwd>
#include <string>

class Person {
protected:
    int id_;
    std::string name_;
    int age_;
    std::string phone_;

public:
    Person(int id, const std::string& name, int age, const std::string& phone);
    virtual ~Person();

    // People are unique records, so copying is disabled.
    Person(const Person&) = delete;
    Person& operator=(const Person&) = delete;

    int getId() const { return id_; }
    const std::string& getName() const { return name_; }
    int getAge() const { return age_; }
    const std::string& getPhone() const { return phone_; }

    void setName(const std::string& name);
    void setAge(int age);
    void setPhone(const std::string& phone);

    virtual std::string getRole() const = 0;
    virtual std::string serialize() const = 0;
    virtual void display(std::ostream& os) const;

    static int population() { return population_; }

    friend std::ostream& operator<<(std::ostream& os, const Person& person);

private:
    static int population_;
};
