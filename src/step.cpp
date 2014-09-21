#include "step.h"
#include <Box2D/Box2D.h>

namespace NStep {

void TStep::AddWorld() {
    PhysicsWorldDescription phys_desc;
    phys_desc.set_timestep(1.0f/60.f);
    // phys_desc.set_timestep(1.0f/6000.f);
    // phys_desc.set_timestep(1.0f/60000.f);
    phys_desc.set_gravity(0.0f, 10.0f);
    phys_desc.set_sleep(true);
    phys_desc.set_physic_scale(Scale);

    phys_desc.set_velocity_iterations(12);
    phys_desc.set_position_iterations(16);

    World.reset(new PhysicsWorld(phys_desc));
}

void TStep::Initialize() {
    AddWorld();
    AddGround(Kind);
}

void TStep::NewExperiment() {
    Creature.reset(NewCreature());
}
void TStep::StartExperiment() {
    Initialize();
    if (!Creature)
        Creature.reset(NewCreature());
    Creature->Place(GetWorld());
}
bool TStep::StepExperiment() {
    Creature->Step();
    GetWorld().step();
    // Dlog << "step\n";
    return true;
}
size_t TStep::GetProgress() {
    // Dlog << "pos x = " << Creature->GetCenterX() << endl;
    float x = Creature->GetCenterX();
    if (x < 0)
        return 0;
    return x * 100;
}

void TStep::Load(const string& s) {
    Creature = LoadCreature(s);
}

void TStep::AddGround(EKind kind) {
    if (kind == Block)
        AddBlockGround();
    else if (kind == Stairs)
        AddStairs();
    else
        throw runtime_error("unsupported ground type: ");
}

void TStep::AddStairs() {
    b2World& world = World->get_b2();

    b2BodyDef bdef;
    bdef.type = b2_staticBody;
    bdef.position = b2Vec2(0, 5);
    b2Body* body = world.CreateBody(&bdef);

    // vector<b2Vec2> vertices;
    // vertices.push_back(b2Vec2(-10, 0));
    float xstart = 2, ystart = 2.5;
    float x = xstart, y = ystart;
    float xsize = 1, ysize = 0.2, ymarg = 0.5;
    for(int nsteps = 0; nsteps < 200; ++nsteps) {
        // float px = x, py = y;
        x += xsize;
        y -= ysize * ymarg;

        b2PolygonShape shape;
        shape.SetAsBox(xsize / 2, ysize / 2, b2Vec2(x - xsize / 2, y - ysize / 2), 0);
        // shape.
        b2FixtureDef fdef;
        fdef.shape = &shape;
        fdef.friction = 2.0f;
        body->CreateFixture(&fdef);
    }

    b2PolygonShape shape;
    float xleft = 3;
    shape.SetAsBox((xstart + xleft) / 2, ysize / 2, b2Vec2((xstart - xleft) / 2, ystart - ysize / 2), 0);
    // shape.
    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.friction = 2.0f;
    body->CreateFixture(&fdef);

}


/*
// chain is too tunnel-prone
void TStep::AddStairs() {
    b2World& world = World->get_b2();

    b2BodyDef bdef;
    bdef.type = b2_staticBody;
    bdef.position = b2Vec2(0, 5);
    b2Body* body = world.CreateBody(&bdef);

    vector<b2Vec2> vertices;
    vertices.push_back(b2Vec2(-10, 0));
    float x = 0, y = 0, step = 0.1;
    for(size_t nsteps = 0; nsteps < 200; ++nsteps) {
        vertices.push_back(b2Vec2(x, y));
        if (nsteps % 2)
            y -= step;
        else
            x += step * 5;
    }
    b2ChainShape shape;
    shape.CreateChain(&vertices[0], vertices.size());

    b2FixtureDef fdef;
    fdef.shape = &shape;
    fdef.friction = 2.0f;
    fdef.restitution = 0.0f;
    body->CreateFixture(&fdef);
}
*/
void TStep::AddBlockGround()
{
    PhysicsContext pc = World->get_pc();

    BodyDescription ground_desc(GetWorld());
    ground_desc.set_position(Vec2f(500.f, 520.f));
    // ground_desc.set_angle(clan::Angle(5.0, clan::angle_degrees));
    // ground_desc.set_type(body_static);
    ground_desc.set_type(body_kinematic);

    ground_desc.set_active(true);
    ground_desc.set_gravity_scale(0);
    ground_desc.set_linear_velocity(clan::Vec2f());
    // ground_desc.set_angular_velocity(clan::Angle(0.1f, clan::angle_degrees));

    Ground = Body(pc, ground_desc);

    //Setup ground fixture
    PolygonShape ground_shape(GetWorld());
    ground_shape.set_as_box(700.f,20.0f);

    FixtureDescription fixture_desc(GetWorld());
    fixture_desc.set_shape(ground_shape);
    fixture_desc.set_friction(2.0f);
    fixture_desc.set_density(1000.0f);

    Fixture ground_fixture(pc, Ground, fixture_desc);
}


}
