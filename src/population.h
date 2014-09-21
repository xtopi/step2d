#pragma once
#include "saveload.h"
#include "cr_parts.h"
#include "creature.h"
#include <fstream>

namespace NStep {

typedef vector<shared_ptr<ICreature>> TCreatures;
typedef vector<TCreatureMetadata> TMetas;

class TPopulationBase {
private:
    string Filename;
    ofstream File;
    TMetas Metas;
private:
    void LoadMetadata();
public:
    TPopulationBase(string file);
    ~TPopulationBase() {
        File.flush();
        File.close();
    }
    void SaveCreature(const TSpec& cr, const TCreatureMetadata& meta);
    void SaveCreature(ICreature* cr, const TCreatureMetadata& meta) {
        SaveCreature(*cr->GetSpec(), meta);
    }
    size_t GetWinner();
    shared_ptr<ICreature> Get(size_t i);
};

struct TCreatureEntry {
    shared_ptr<ICreature> Creature;
    TCreatureMetadata Meta;
    size_t Index;

    TCreatureEntry()
        : Index(0)
    {}
};

class TPopulation
{
public:
    size_t PopulationSize;
    size_t SelectionSize;

    size_t Pos;
    size_t NSame;

    bool NextCycle;

    typedef vector<TCreatureEntry> TEntries;
    TEntries Creatures;
    TPopulationBase Base;

public:
    TPopulation(size_t size, size_t sel)
        : PopulationSize(size)
        , SelectionSize(sel)
        , Pos(0)
        , NSame(0)
        , NextCycle(true)
        , Base("crs/trace.crs")
    {
        Init();
    }
    void Init(); // set random creatures
    void Mutate();
    shared_ptr<ICreature> GetNextCreature();
    void Restart() {
        Pos = 0;
    }
    void SetScore(size_t score) {
        TCreatureMetadata& meta = Creatures[Pos - 1].Meta;
        meta.X = score;
        Dlog << "run " << (Pos - 1) << " of " << meta.Name << " is at " << score << endl;
    }
    void SaveCreature() {
        Base.SaveCreature(Creatures[Pos - 1].Creature.get(), Creatures[Pos - 1].Meta);
    }
    bool IsNextCycle() const {
        return NextCycle;
    }
};

}
