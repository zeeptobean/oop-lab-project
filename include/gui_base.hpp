#pragma once

class IUIAbstract {
    public:
    virtual void draw() = 0;
    virtual ~IUIAbstract() = default;
};