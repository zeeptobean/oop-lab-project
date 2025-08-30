#include "core/user_database.hpp"

UserDatabase::UserDatabase() = default;
UserDatabase::~UserDatabase() = default;

UserDatabase& UserDatabase::get() {
    static UserDatabase instance;
    return instance;
}

User* UserDatabase::initUserJson(const json& jsonData) {
    if(!jsonData.contains("type")) {
        return nullptr;
    }
    User *user = nullptr;
    std::string userType = jsonData.at("type").get<std::string>();
    if(userType == "undergraduate") {
        user = new UniversityUser();
        user->borrowingPolicy = BorrowingPolicyFactory::getUndergraduatePolicy();
    } else if(userType == "postgraduate") {
        user = new UniversityUser();
        user->borrowingPolicy = BorrowingPolicyFactory::getPostgraduatePolicy();
    } else if(userType == "teacher") {
        user = new UniversityUser();
        user->borrowingPolicy = BorrowingPolicyFactory::getPremiumPolicy();
    } else if(userType == "staff") {
        user = new UniversityUser();
        user->borrowingPolicy = BorrowingPolicyFactory::getNormalPolicy();
    } else if(userType == "premium") {
        user = new User();
        user->borrowingPolicy = BorrowingPolicyFactory::getPremiumPolicy();
    } else {
        user = new User();
        user->borrowingPolicy = BorrowingPolicyFactory::getNormalPolicy();
    }
    user->type = userType;

    try {
        user->internalId = jsonData.at("id").get<uint64_t>();
        user->name = jsonData.at("name").get<std::string>();
        user->dob = Timestamp(jsonData.at("dob").get<std::string>());
        user->nid = jsonData.at("nid").get<std::string>();
        user->email = jsonData.at("email").get<std::string>();
        user->address = jsonData.at("address").get<std::string>();
        user->phone = jsonData.at("phone").get<std::string>();
        user->image = jsonData.at("image").get<std::string>();
        user->passwordHash = jsonData.at("passwordHash").get<std::string>();
        user->isSuspended = jsonData.value("isSuspended", false);
        if(user->isSuspended) user->suspensionEnd = Timestamp(jsonData.at("suspendEnd").get<std::string>());

        std::vector<uint64_t> favoriteBookVec = jsonData.at("favoriteBookId").get<std::vector<uint64_t>>();
        user->favoriteBookId.clear();
        for(auto& bookId : favoriteBookVec) {
            user->favoriteBookId.insert(bookId);
        }

        UniversityUser* uuser = dynamic_cast<UniversityUser*>(user);
        if(uuser) {
            uuser->workId = jsonData.at("workId").get<std::string>();
            uuser->department = jsonData.at("department").get<std::string>();
        }
    } catch(...) {
        delete user;
        return nullptr;
    }

    if(userMap.find(user->internalId) == userMap.end()) {
        userMap[user->internalId] = std::unique_ptr<User>(user);
    } else {
        std::cerr << "User ID " << user->internalId << " duplicated. Do not add" << std::endl;
        delete user;
        return nullptr;
    }

    return user;
}

User* UserDatabase::login(const std::string& email, const std::string& password) {
    std::string passwordHash = sha256(password);
    for(auto const& [userId, userPtr] : userMap) {
        if(userPtr->email == email && userPtr->passwordHash == passwordHash) {
            return userPtr.get();
        }
    }
    return nullptr;
}

User* UserDatabase::query(uint64_t userId) {
    auto it = userMap.find(userId);
    if(it != userMap.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool UserDatabase::loadFile(const std::string& filename) {
    std::ifstream jsonFile(filename);
    if (!jsonFile) return false;
    this->userDataFile = filename;

    json jsonData;
    try {
        jsonFile >> jsonData;
        for(auto& item : jsonData.at("users")) {
            User *newUser = initUserJson(item);
            (void)newUser;
        }
    } catch (const std::exception& e) {
        return false;
    }
    jsonFile.close();
    return true;
}

std::string UserDatabase::queryUserPasswordHash(const std::string& email) {
    for(auto const& [userId, userPtr] : userMap) {
        if(userPtr->email == email) {
            return userPtr->passwordHash;
        }
    }
    return "";
}

void UserDatabase::writeFile() {
    json jsonData;
    jsonData["users"] = json::array();
    for(auto const& [userId, userPtr] : userMap) {
        json userJson;
        userJson["id"] = userPtr->internalId;
        userJson["name"] = userPtr->name;
        userJson["dob"] = userPtr->dob.toString();
        userJson["nid"] = userPtr->nid;
        userJson["email"] = userPtr->email;
        userJson["address"] = userPtr->address;
        userJson["phone"] = userPtr->phone;
        userJson["image"] = userPtr->image;
        userJson["type"] = userPtr->type;
        userJson["passwordHash"] = userPtr->passwordHash;
        userJson["isSuspended"] = userPtr->isSuspended;
        if(userPtr->isSuspended) {
            userJson["suspendEnd"] = userPtr->suspensionEnd.toString();
        }

        std::vector<uint64_t> favoriteBookVec(userPtr->favoriteBookId.begin(), userPtr->favoriteBookId.end());
        userJson["favoriteBookId"] = favoriteBookVec;

        UniversityUser* uuser = dynamic_cast<UniversityUser*>(userPtr.get());
        if(uuser) {
            userJson["workId"] = uuser->workId;
            userJson["department"] = uuser->department;
        }

        jsonData["users"].push_back(userJson);
    }

    std::ofstream jsonFile(this->userDataFile);
    if(!jsonFile) {
        return;
    }
    jsonFile << jsonData.dump(4);
    jsonFile.close();
}