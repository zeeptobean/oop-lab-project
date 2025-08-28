#pragma once

class RecommendationService {
    private:
    RecommendationService() = default;
    ~RecommendationService() = default;
    public:
    static RecommendationService& get() {
        static RecommendationService instance;
        return instance;
    }
    RecommendationService(const RecommendationService&) = delete;
    RecommendationService& operator=(const RecommendationService&) = delete;

    std::vector<uint64_t> getRecommendations(uint64_t userId) {
        return std::vector<uint64_t>();
    }
};