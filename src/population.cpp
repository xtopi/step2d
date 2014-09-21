#include "population.h"

namespace NStep {

size_t TPopulationBase::GetWinner() {
    size_t max = 0;
    size_t maxpos = 0;
    size_t cnt = 0;
    for (auto& meta: Metas) {
        if (meta.X > max) {
            max = meta.X;
            maxpos = cnt;
        }
        ++cnt;
    }
    return maxpos;
}

shared_ptr<ICreature> TPopulationBase::Get(size_t i) {
    TCreatureMetadata& meta = Metas[i];
    string fc = GetFile(Filename);
    fc = fc.substr(meta.Data);
    return LoadCreature(fc);
}

void TPopulationBase::LoadMetadata() {
    string fc = GetFile(Filename);
    Dlog << "read file of " << fc.size() << " bytes" << endl;
    size_t n = 0;
    size_t cnt = 0;

    while(n != fc.size()) {
        // char mark[3] = "\0\0";
        Assert(fc.substr(n, 3) == "===", "not mark: " + fc.substr(n, n + 3));
        n += 3;
        const size_t sstart = n;

        string s;
        n += Add(s, (char*)fc.data() + n, false);
        // Dlog << "read string of " << s.size() << " bytes" << endl;
        TCreatureMetadata meta;
        size_t n2 = Add(meta, (char*)s.data(), false);

        size_t off = sstart + sizeof(size_t) + n2;
        Assert(fc[off] == s[n2], "data pos incorrect");
        meta.Data = off;

        Metas.push_back(meta);

        Dlog << "creature metadata, x=" << meta.X << ", gen: " << meta.Name << ", cnt=" << cnt << endl;

        ++cnt;
    }

}

TPopulationBase::TPopulationBase(string file)
    : Filename(file)
{
    LoadMetadata();
    File.open(file.c_str(), ios::app);
    Assert(!!File, "cannot write to " + file);
}
void TPopulationBase::SaveCreature(const TSpec& cr, const TCreatureMetadata& meta) {
    string s(1024, ' ');
    size_t n = Add((TCreatureMetadata&)meta, &s[0], true);
    // todo: add to metadata
    n += Add((TSpec&)cr, &s[0] + n, true);
    s.resize(n);
    File << "===";
    File.write((char*)&n, sizeof(n));
    File.write(s.data(), n);
    // Dlog << "saved " << n << " bytes (with envelope " << n + 3  + sizeof(n) << ")"  << endl;
}

void TPopulation::Init() {
    Creatures.resize(PopulationSize);
    for (auto& p: Creatures) {
        p.Creature.reset(NewCreature());
        p.Meta.Name = string() + char(rnd(26) + 'A');
    }
    NSame = 0;
    Pos = 0;
}
shared_ptr<ICreature> TPopulation::GetNextCreature() {
    if (Pos == PopulationSize) {
        Mutate();
        Pos = SelectionSize;
        NextCycle = true;
    }
    else
        NextCycle = false;
    return Creatures[Pos++].Creature;
}

bool EntryLess(const TCreatureEntry& l, const TCreatureEntry& r) {
    return l.Meta.X > r.Meta.X;
}

void TPopulation::Mutate() {
    TEntries sorted(Creatures);
    sort(sorted.begin(), sorted.end(), &EntryLess);

    bool same = true;
    for (size_t i = 0; i < SelectionSize; ++i) {
        if (Creatures[i].Creature != sorted[i].Creature)
            same = false;
        Creatures[i] = sorted[i];
    }
    if (same)
        ++NSame;
    else
        NSame = 0;

    Dlog << "=== end run ===" << endl;
    for (size_t i = 0; i < SelectionSize; ++i)
        Dlog << ' ' << i << " " << sorted[i].Meta.Name << " at " << sorted[i].Meta.X << endl;

    float mut = powf(1.2, -NSame);
    Dlog << " nsame = " << NSame << ", mut = " << mut << endl;
    for (size_t i = SelectionSize; i < Creatures.size(); ++i) {
        TCreatureEntry& e = Creatures[i % SelectionSize];
        char letter = (e.Index < 26 ? 'A' + e.Index : (e.Index < 26 * 2 ? 'a' + e.Index - 26 : '0' + e.Index - 26 * 2));
        TCreatureMetadata nmeta;
        nmeta.Name = e.Meta.Name + letter;
        ++e.Index;

        Creatures[i].Creature = e.Creature->New(mut);
        Creatures[i].Meta = nmeta;
        Creatures[i].Index = 0;
    }
}

}
