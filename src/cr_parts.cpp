#include "cr_parts.h"

namespace NStep {

TPartDesc MakeRandomPart() {
    TPartDesc part;
    part.Xb = rnd() * 0.9 + 0.1;
    part.Xc = rnd() * part.Xb;
    part.Yc = rnd() * 0.9 + 0.1;
    return part;
}


TJointDesc MakeRandomJoint(const TPartPos& src, const TPartPos& dst) {
    TJointDesc joint;
    joint.Src = src;
    joint.Dst = dst;
    /*
    joint.Min = (rnd() * 2 - 1) * PI * 2;
    joint.Max = (rnd() * 2 - 1) * PI * 2;
    if (joint.Min > joint.Max)
        swap(joint.Min, joint.Max);
    joint.Norm = rnd() * (joint.Max - joint.Min) + joint.Min;
    */
    return joint;
}

void MakeRandomCreature(size_t njoints, TCreatureDesc& result) {
    result.Parts.push_back(MakeRandomPart());
    result.Attach.push_back(TPartPos(0, 0));
    result.Attach.push_back(TPartPos(0, 1));
    result.Attach.push_back(TPartPos(0, 2));
    for (size_t i = 0; i < njoints; ++i) {
        // select random attachement point
        size_t attpos = rnd(result.Attach.size());
        TPartPos pos = result.Attach[attpos];
        result.Attach.erase(result.Attach.begin() + attpos);

        size_t npart = result.Parts.size();
        result.Parts.push_back(MakeRandomPart());
        result.Attach.push_back(TPartPos(npart, 1));
        result.Attach.push_back(TPartPos(npart, 2));

        result.Joints.push_back(MakeRandomJoint(pos, TPartPos(npart, 0)));
    }
}

void MakeSimpleCreature(size_t njoints, TCreatureDesc& result) {
    TPartDesc p1 = {1, 0, 1};
    TPartDesc p2 = {1, 0 ,1};
    result.Parts = {p1, p2};
    TJointDesc j = { {0, 2}, {1, 0}, 0, 0, 0};
    result.Joints = {j};
}

b2Body* PlacePart(b2World& world, const TPartDesc& pdesc, const b2Vec2& pos, float rot) {
    // Dlog << "place part " << pdesc << endl;
    b2BodyDef bdef;
    bdef.type = b2_dynamicBody;
    bdef.position = pos;
    bdef.angle = rot;
    // bdef.gravityScale = 1;
    // bdef.active = true;
    // bdef.fixedRotation = false;
    b2Body* body = world.CreateBody(&bdef);

    vector<b2Vec2> vertices;
    vertices.push_back(b2Vec2(0, 0));
    vertices.push_back(b2Vec2(pdesc.Xb, 0));
    vertices.push_back(b2Vec2(pdesc.Xc, pdesc.Yc));

    b2PolygonShape shape;
    shape.Set(&vertices[0], vertices.size());
    // Dlog << "shape mass: " << shape. << endl;

    b2FixtureDef fdef;
    fdef.shape = &shape;

    fdef.density = 10;
    fdef.friction = 1;
    fdef.restitution = 0.2;
    body->CreateFixture(&fdef);
    return body;
}

void DoPlaceCreaturePart(b2World& world, TPlacedParts& placed, const TPartDesc& p, const TJointDesc& j, const TPartDesc& n) {
    b2Body* pbody = placed.Bodies[j.Src.first];
    size_t ppos = j.Src.second;

    b2Vec2 vert = p.GetVertex(ppos);
    b2Vec2 v1 = pbody->GetWorldPoint(vert);
    const float adir = p.GetAngleDirection(ppos);
    const float ddir = adir + pbody->GetAngle();
    const float vrot = ddir - n.GetAngleA() / 2;
    const b2Vec2 v2(0.1, 0);
    const b2Rot rot(ddir);
    // const b2Vec2 dvec = b2Mul(rot, v2);
    // calculate angle
    b2Body* nbody = PlacePart(world, n, v1, vrot);
    placed.Bodies.push_back(nbody);
    /*
    Dlog << "connecting " << p << " pos " << ppos << " to " << n << " pos 0 " << " at " << v1 << endl;
    Dlog << "vertex " << vert << " is at " << v1 << ", body is at " << pbody->GetPosition() << endl;
    Dlog << "vertex dir: " << adir << "(" << adir / PI << "pi)" << ", pdir: " << pbody->GetAngle() << ", new A: " << n.GetAngleA() << ", vrot: " << vrot << endl;
    Dlog << "vrot = " << vrot << "(" << vrot / PI << "pi)" << ", disp: " << dvec << endl;
    */
    b2RevoluteJointDef jdef;
    // jdef.collideConnected = true;
    jdef.collideConnected = true;
    jdef.bodyA = pbody;
    jdef.bodyB = nbody;
    jdef.localAnchorA = vert;
    jdef.localAnchorB = b2Vec2(0, 0);
    jdef.referenceAngle = vrot - pbody->GetAngle();
    jdef.lowerAngle = -1 * (p.GetAngleA() + n.GetAngleA());
    jdef.upperAngle = p.GetAngleA() + n.GetAngleA();
    jdef.enableLimit = false;
    jdef.enableLimit = true;
    jdef.motorSpeed = 0;
    jdef.maxMotorTorque = 50;
    jdef.enableMotor = true;

    // jdef.Initialize(pbody, nbody, v1);
    // set angle
    b2Joint* joint = world.CreateJoint(&jdef);
    placed.Joints.push_back(joint);
}

void PlaceCreatureDesc(b2World& world, const TCreatureDesc& cdesc, TPlacedParts& res) {
    // Dlog << "================== place =======================" << endl;
    b2Vec2 placePoint(2, 2.5);
    res.Bodies.push_back(PlacePart(world, cdesc.Parts[0], placePoint, 0));

    // next joint always binds some place on known body to the place 0 on unknown
    for (auto& joint: cdesc.Joints) {
        // Dlog << "joint: " << joint << endl;
        TPartPos s = joint.Src;
        TPartPos d = joint.Dst;
        assert(s.first < res.Bodies.size());
        assert(d.first == res.Bodies.size());
        DoPlaceCreaturePart(world, res, cdesc.Parts[s.first], joint, cdesc.Parts[d.first]);

    }
}

THarmonicLaw MakeRandomHarmonic() {
    THarmonicLaw ret;
    ret.Phase = rnd();
    ret.Amp = rnd();
    // ret.Period = 30 + rnd() * 240;
    ret.Period = 180;
    return ret;
}

void ApplySpeed(b2RevoluteJoint& joint, float desiredAngle) {
    b2Rot rot(desiredAngle - joint.GetJointAngle());
    float angle = rot.GetAngle();
    float speed = angle;
    joint.SetMotorSpeed(speed * 2);
}

void MoveJoints(vector<b2Joint*>& joints, vector<float> desiredAngles) {
    for (size_t i = 0; i < joints.size(); ++i) {
        auto& j = joints[i];
        b2RevoluteJoint& joint = (b2RevoluteJoint&)*j;
        ApplySpeed(joint, desiredAngles[i] * (joint.GetUpperLimit() - joint.GetLowerLimit()) + joint.GetLowerLimit());
    }
}

void MoveByMotion(vector<b2Joint*>& joints, TMotion& motion, size_t step) {
    vector<float> positions;
    positions.reserve(motion.size());
    for (auto& m: motion)
        positions.push_back(m->Get(step));
    MoveJoints(joints, positions);
}

void MakeRandomSpec(TSpec& spec) {
    const auto njoints = TDefs::NumParts - 1;
    MakeRandomCreature(njoints, spec.Desc);
    spec.Motion.resize(njoints);
    for (TLawPtr& p: spec.Motion)
        p.reset(new THarmonicLaw(MakeRandomHarmonic()));
}

void Modify(NStep::TSpec& spec, float amount) {
    for (TPartDesc& part: spec.Desc.Parts) {
        // if (Rare()) part.Xb += SmallDist();
        // if (Rare()) part.Xc += SmallDist();
        // if (Rare()) part.Yc += SmallDist();
        part.Xb *= (1 + SmallDist() * 20 * amount);
        part.Xc *= (1 + SmallDist() * 20 * amount);
        part.Yc *= (1 + SmallDist() * 20 * amount);
    }
    for (TLawPtr& motion: spec.Motion) {
        motion = motion->Modify(amount);
    }
}


template <> size_t Add(TLawPtr& s, char* buf, bool write) {
    // use factory
    if (!write)
        s.reset(new THarmonicLaw);
    size_t n = s->Add(buf, write);
    return n;
}

template <> size_t Add(TPartDesc& p, char* buf, bool write) {
    size_t n = Add(p.Xb, buf, write);
    n += Add(p.Xc, buf + n, write);
    n += Add(p.Yc, buf + n, write);
    return n;
}

template <> size_t Add(TJointDesc& j, char* buf, bool write) {
    size_t n = Add(j.Src, buf, write);
    n += Add(j.Dst, buf + n, write);
    return n;
}

template <> size_t Add(TCreatureDesc& c, char* buf, bool write) {
    size_t n = Add(c.Parts, buf, write);
    n += Add(c.Joints, buf + n, write);
    return n;
}

template <> size_t Add(TSpec& s, char* buf, bool write) {
    size_t n = Add(s.Desc, buf, write);
    n += Add(s.Motion, buf + n, write);
    return n;
}

void Save(const TSpec& spec, string& s) {
    s.resize(1024);
    size_t n = Add((TSpec&)spec, &s[0], true);
    s.resize(n);
}
void Load(TSpec& spec, const string& s) {
    Add(spec, (char*)&s[0], false);
}


}
