#pragma once

class IUIAbstract {
    protected:
    static std::string ;
    public:
    virtual void draw() = 0;
    virtual ~IUIAbstract() = default;
};