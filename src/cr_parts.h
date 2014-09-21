#pragma once
#include "saveload.h"

namespace NStep {

inline int randomint() {
    int i;
    static clan::Random rand;
    rand.get_random_bytes((unsigned char*)(&i), sizeof(i));
    return i;
}

inline size_t rnd(size_t max) {
    int i = randomint();
    return std::abs(i) % max;
}

inline float rnd() {
    const float r = (rnd(10000000) / 10000000.f);
    // Dlog << r << endl;
    return r;
}

inline float Atan2(float y, float x) {
    const float r = atan2f(y, x);
    if (r < 0)
        return r + 2 * PI;
    return r;
}

inline bool Rare() {
    return rnd() < 0.1;
}

inline float SmallDist() {
    return rnd() * 0.02 - 0.01;
}


struct angle {
    const float& f;
    angle(const float& ff): f(ff) {}
};

inline ostream& operator<<(ostream& os, const angle& a) {
    return os << a.f / PI << "pi";
}

struct TPartDesc {
    float Xb, Xc, Yc;
    b2Vec2 GetVertex(size_t n) const {
        switch(n) {
            case 0:
                return b2Vec2(0, 0);
            case 1:
                return b2Vec2(Xb, 0);
            case 2:
                return b2Vec2(Xc, Yc);
            default:
                assert(false);
        }
        return b2Vec2(0, 0);
    }
    float GetAngleA() const {
        return Atan2(Yc, Xc);
    }
    float GetAngleB() const {
        return Atan2(Yc, Xb - Xc);
    }
    float GetAngleC() const {
        return PI - GetAngleA() - GetAngleB();
    }
    float GetAngleDirection(size_t n) const {
        switch(n) {
            case 0:
                return PI + GetAngleA() / 2;
            case 1:
                return -GetAngleB() / 2;
            case 2:
                // Dlog << "get angle dir 2 - A=" << angle(GetAngleA()) << ", C=" << angle(GetAngleC()) << endl;
                return GetAngleA() + GetAngleC() / 2;
            default:
                assert(false);
        }
        return 0;
    }
};

typedef pair<size_t, size_t> TPartPos;

struct TJointDesc {
    TPartPos Src, Dst;
    float Min, Max, Norm;
};

struct TCreatureDesc {
    vector<TPartDesc> Parts;
    vector<TJointDesc> Joints;
    vector<TPartPos> Attach;
};

struct TPlacedParts {
    vector<b2Body*> Bodies;
    vector<b2Joint*> Joints;
};

inline ostream& operator<<(ostream& os, const TPartDesc& pd) {
    return os << "part xb=" << pd.Xb << " xc=" << pd.Xc << " yc=" << pd.Yc;
}

template <typename A, typename B>
ostream& operator<<(ostream& os, const pair<A, B> & p) {
    return os << '(' << p.first << ',' << p.second << ')';
}

inline ostream& operator<<(ostream& os, const TJointDesc& jd) {
    return os << "joint " << jd.Src << " -> " << jd.Dst;
}

inline ostream& operator<<(ostream& os, const b2Vec2& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

// TPartDesc MakeRandomPart();
// TJointDesc MakeRandomJoint(const TPartPos& src, const TPartPos& dst);
void MakeRandomCreature(size_t njoints, TCreatureDesc& result);
void PlaceCreatureDesc(b2World& world, const TCreatureDesc& cdesc, TPlacedParts& res);

/////// Motion

class IMotionLaw {
public:
    virtual float Get(size_t time) = 0;
    virtual size_t Add(char* buf, bool write) = 0;
    virtual shared_ptr<IMotionLaw> Modify(float amount) = 0;
};

typedef shared_ptr<IMotionLaw> TLawPtr;
typedef vector<TLawPtr> TMotion;

class THarmonicLaw
    : public IMotionLaw
{
public:
    float Phase;
    float Amp;
    float Period;

    virtual float Get(size_t time) {
        return cos((time / Period + Phase) * 2 * PI);
    }
    virtual size_t Add(char* buf, bool write) {
        size_t n = NStep::Add(Phase, buf, write);
        n += NStep::Add(Amp, buf + n, write);
        n += NStep::Add(Period, buf + n, write);
        return n;
    }
    virtual shared_ptr<IMotionLaw> Modify(float amount) {
        THarmonicLaw* n = new THarmonicLaw(*this);
        // if (Rare())
            // n->Phase += SmallDist() * 5 * amount;
            n->Phase += SmallDist() * 5 * amount;
            // if (Rare())
            n->Amp *= (1 + SmallDist() * 5 * amount);
        // if (n->Amp < 0)
        //     n->Amp = 0;
        if (Rare())
            n->Period *= (1 + rnd() * 5 * amount);
        if (n->Period < 30)
            n->Period = 30;
        return TLawPtr(n);
    }
};

struct TSpec {
    TCreatureDesc Desc;
    TMotion Motion;
};

void Save(const TSpec& spec, string& s);
void Load(TSpec& spec, const string& s);

void MakeRandomSpec(TSpec& spec);
THarmonicLaw MakeRandomHarmonic();
void ApplySpeed(b2RevoluteJoint& joint, float desiredAngle);
void MoveByMotion(vector<b2Joint*>& joints, TMotion& motion, size_t step);
void Modify(TSpec& spec, float amount);

}
