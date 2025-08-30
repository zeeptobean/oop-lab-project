#pragma once

#include "user.hpp"
#include "../sha256.hpp"

#include <fstream>
#include <map>
#include <memory>
#include <string>
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

class UserDatabase {
    private:
    std::string userDataFile;
    UserDatabase();
    ~UserDatabase();

    std::map<uint64_t, std::unique_ptr<User>> userMap;

    User* initUserJson(const nlohmann::json& jsonData);

    public:
    static UserDatabase& get();
    UserDatabase(const UserDatabase&) = delete;
    UserDatabase& operator=(const UserDatabase&) = delete;

    User* login(const std::string& email, const std::string& password);
    User* query(uint64_t userId);
    std::string queryUserPasswordHash(const std::string& email);
    bool loadFile(const std::string& filename);
    void writeFile();
};