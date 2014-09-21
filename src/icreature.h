#pragma once

namespace NStep {

struct TSpec;
class ICreature {
public:
    virtual void Place(PhysicsWorld& w) = 0;
    virtual void Step() = 0;
    virtual float GetCenterX() {
        return 0;
    }
    virtual const TSpec* GetSpec() {
        return 0;
    }
    virtual shared_ptr<ICreature> New(float mut) {
        return shared_ptr<ICreature>();
    }
};

shared_ptr<ICreature> LoadCreature(const string& s);
void SaveCreature(const string& s, ICreature* c);
ICreature* NewCreature();

}
