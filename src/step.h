#include "icreature.h"

namespace NStep {

class TStep {
public:
    enum EKind {
        Block,
        Floor,
        Rand,
        Stairs,
        Fract
    };

private:
    EKind Kind;
    unique_ptr<PhysicsWorld> World;
    Body Ground;
    shared_ptr<ICreature> Creature;
    size_t Scale;
private:
    void AddWorld();
    void AddGround(EKind kind);
    void AddBlockGround();
    void AddStairs();
public:
    TStep(EKind kind = Block)
        : Kind(kind)
        , Scale(100)
    {
    }
    void Initialize();
    PhysicsWorld& GetWorld() {
        return *World.get();
    }
    void Load(const string& s);
    void NewExperiment();
    void StartExperiment();
    bool StepExperiment();
    size_t GetProgress();
    ICreature* GetCreature() {
        return Creature.get();
    }
    void SetCreature(shared_ptr<ICreature> creature) {
        Creature = creature;
    }
    void SetScale(size_t s) {
        Scale = s;
    }
    size_t GetScale() const {
        return Scale;
    }
    void FlipScale() {
        if (Scale == 100)
            Scale = 10;
        else
            Scale = 100;
    }
};

}

