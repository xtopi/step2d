#include "icreature.h"
#include "creature.h"
#include "cr_parts.h"
#include "saveload.h"

namespace NStep {

class TFirstCreature
    : public ICreature
{
    Body MyBody;

    virtual void Place(PhysicsWorld& pworld) {
        b2World& world = pworld.get_b2();

        b2BodyDef bdef;
        bdef.type = b2_dynamicBody;
        bdef.position = b2Vec2(5, 3);
        bdef.angle = 0.05;
        // bdef.gravityScale = 1;
        // bdef.active = true;
        // bdef.fixedRotation = false;
        b2Body* body = world.CreateBody(&bdef);

        vector<b2Vec2> vertices;
        vertices.push_back(b2Vec2(-0.5,0));
        vertices.push_back(b2Vec2(0.5,0));
        vertices.push_back(b2Vec2(0,1));

        b2PolygonShape shape;
        shape.Set(&vertices[0], vertices.size());
        Dlog << "shape radius: " << shape.GetType() << endl;

        b2FixtureDef fdef;
        fdef.shape = &shape;

        fdef.density = 10;
        fdef.friction = 1;
        fdef.restitution = 0.2;
        body->CreateFixture(&fdef);

    }
    virtual void Step() {
    }
};


class TSecondCreature
    : public ICreature
{
public:
    TCreatureDesc Desc;
    TPlacedParts Placed;
    size_t NStep;
    TSecondCreature()
        : NStep(0)
    {
        // MakeRandomCreature(1, Desc);
        // MakeRandomCreature(7, Desc);
        MakeRandomCreature(7, Desc);
        // MakeSimpleCreature(1, Desc);
    }

    virtual void Place(PhysicsWorld& world) {
        PlaceCreatureDesc(world.get_b2(), Desc, Placed);
    }

    virtual void Step() {
        if (++NStep == 100)
            Stand();
    }
private:
    void Stand() {
        for (auto& j: Placed.Joints) {
            b2RevoluteJoint& joint = (b2RevoluteJoint&)*j;
            joint.SetLimits(-0.1, 0.1);
            joint.EnableLimit(true);
        }
        Dlog << "STAND" << endl;
    }
};

class TThirdCreature
: public ICreature
{
public:
    TCreatureDesc Desc;
    TPlacedParts Placed;
    size_t NStep;
    vector<THarmonicLaw> Taxis;

    TThirdCreature()
        : NStep(0)
    {
        const size_t njoints = 5;
        MakeRandomCreature(njoints, Desc);
        Taxis.resize(njoints);
        for (auto& t: Taxis)
            t = MakeRandomHarmonic();
    }

    virtual void Place(PhysicsWorld& world) {
        PlaceCreatureDesc(world.get_b2(), Desc, Placed);
    }

    virtual void Step() {
        if (++NStep < 100)
            return;
        Go();
    }

    void Go() {
        for (size_t i = 0; i < Placed.Joints.size(); ++i) {
            auto& j = Placed.Joints[i];
            b2RevoluteJoint& joint = (b2RevoluteJoint&)*j;
            // joint.SetMotorSpeed(Taxis[i].Get(NStep - 100));
            // joint.GetLowerLimit();
            // joint.GetUpperLimit();
            ApplySpeed(joint, Taxis[i].Get(NStep) * (joint.GetUpperLimit() - joint.GetLowerLimit()) + joint.GetLowerLimit());
        }
    }
};

class TFourthCreature
: public ICreature
{
public:
    TSpec Spec;
    size_t NStep;
    TPlacedParts Placed;

    TFourthCreature()
        : NStep(0)
    {
        MakeRandomSpec(Spec);

        string str;
        Save(Spec, str);
        cout.write(str.data(), str.size());
    }
    TFourthCreature(const string& data)
        : NStep(0)
    {
        Dlog << "loading creature..." << endl;
        Load(Spec, data);
    }

    virtual void Place(PhysicsWorld& world) {
        Placed = TPlacedParts();
        PlaceCreatureDesc(world.get_b2(), Spec.Desc, Placed);
    }
    virtual void Step() {
        if (++NStep < 100)
            return;
        MoveByMotion(Placed.Joints, Spec.Motion, NStep);
    }
    virtual float GetCenterX() {
        return Placed.Bodies.front()->GetPosition().x;
    }
    virtual const TSpec* GetSpec() {
        return &Spec;
    }
};



class TFifthCreature
: public ICreature
{
public:
    TSpec Spec;
    size_t NStep;
    TPlacedParts Placed;

    TFifthCreature()
        : NStep(0)
    {
        MakeRandomSpec(Spec);
    }
    TFifthCreature(const string& data)
        : NStep(0)
    {
        Load(Spec, data);
    }
    TFifthCreature(const TFifthCreature& p)
        : Spec(p.Spec)
        , NStep(0)
    {
    }

    virtual void Place(PhysicsWorld& world) {
        Placed = TPlacedParts();
        PlaceCreatureDesc(world.get_b2(), Spec.Desc, Placed);
        NStep = 0;
    }
    virtual void Step() {
        if (++NStep < 60)
            return;
        MoveByMotion(Placed.Joints, Spec.Motion, NStep);
    }
    virtual float GetCenterX() {
        return Placed.Bodies.front()->GetPosition().x;
    }
    virtual const TSpec* GetSpec() {
        return &Spec;
    }
    virtual shared_ptr<ICreature> New(float mut) {
        TFifthCreature* child = new TFifthCreature(*this);
        Modify(child->Spec, mut);
        return shared_ptr<ICreature>(child);
    }

};



ICreature* NewCreature() {
    // return new TFirstCreature();
    // return new TSecondCreature();
    // return new TThirdCreature();
    // return new TFourthCreature();
    return new TFifthCreature();
}

shared_ptr<ICreature> LoadCreature(const string& s) {
    return shared_ptr<ICreature>(new TFifthCreature(s));
}
void SaveCreature(string& s, ICreature* c) {
    TFifthCreature& f = (TFifthCreature&)*c;
    Save(f.Spec, s);
}


}
