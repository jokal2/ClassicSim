#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include <QDebug>
#include <queue>

#include <Items.h>

#include "CharacterEncoder.h"
#include "CharacterSpells.h"
#include "CharacterStats.h"
#include "CharacterTalents.h"
#include "ClassicSimControl.h"
#include "EnabledBuffs.h"
#include "EncounterEnd.h"
#include "EncounterStart.h"
#include "Equipment.h"
#include "EquipmentDb.h"
#include "GeneralBuffs.h"
#include "NumberCruncher.h"
#include "Orc.h"
#include "Race.h"
#include "RaidControl.h"
#include "RandomAffixes.h"
#include "Rotation.h"
#include "RotationModel.h"
#include "SimSettings.h"
#include "SimulationThreadPool.h"
#include "Target.h"
#include "Warrior.h"
#include "Weapon.h"

class SimRunQueue {
  std::string name;
  std::queue<std::function<void(void)>> queue;

public:
  SimRunQueue(std::string name) : name{std::move(name)} {}

  void logName() { qDebug() << "\n\n" << name.c_str(); }
  void push(std::function<void(void)> &&run) { queue.push(run); }

  std::queue<std::function<void(void)>> &&getQueue() {
    return std::move(queue);
  }
};

class CommandLineControl : public ClassicSimControl {
  Q_OBJECT
public:
  CommandLineControl(QObject *parent = nullptr) : ClassicSimControl(parent) {}

  struct Setup {
    Items head;
    Items neck;
    Items shoulders;
    Items back;

    Items chest;
    Items wrist;
    Items mainhand;
    Items offhand;
    Items ranged;
    Items gloves;
    Items belt;
    Items legs;
    Items boots;
    Items ring1;
    Items ring2;
    Items trinket1;
    Items trinket2;

    int talent_index;
    int buff_index;

    void set_impale_prot() { talent_index = 0; }

    void set_fury_prot() { talent_index = 1; }

    void set_full_worldbuffs() { buff_index = 0; }

    void set_no_worldbuffs() { buff_index = 1; }
  };

  void set_gear_from_setup(Setup s) {
    auto e = current_char->get_equipment();
    e->set_mainhand(s.mainhand);
    e->set_offhand(s.offhand);
    e->set_ranged(s.ranged);
    e->set_head(s.head);
    e->set_neck(s.neck);
    e->set_shoulders(s.shoulders);
    e->set_back(s.back);
    e->set_chest(s.chest);
    e->set_wrist(s.wrist);
    e->set_gloves(s.gloves);
    e->set_belt(s.belt);
    e->set_legs(s.legs);
    e->set_boots(s.boots);
    e->set_ring1(s.ring1);
    e->set_ring2(s.ring2);
    e->set_trinket1(s.trinket1);
    e->set_trinket2(s.trinket2);
  }

public:
  std::queue<std::shared_ptr<SimRunQueue>> queues;
  std::queue<std::function<void(void)>> sim_runs;

  void pop_next_queue() {
    if (queues.size() == 0)
      return;

    auto srq = queues.front();
    sim_runs = srq->getQueue();
    srq->logName();
    queues.pop();
  }

  void run_next_sim() override {
    if (sim_runs.size() == 0)
      pop_next_queue();

    if (sim_runs.size() == 0)
      return;

    auto next = sim_runs.front();
    next();
    sim_runs.pop();
  }

  void set_fury_prot() { current_char->get_talents()->set_current_index(1); }

  void set_prot() { current_char->get_talents()->set_current_index(0); }

  void set_protection_rotation() {
    rotation_model->set_information_index(5);
    rotation_model->select_rotation();
    current_char->set_is_tanking(true);
  }

#define define_run_with(X)                                                     \
  void run_with_##X(SimRunQueue *srq, Setup setup, Items item,                 \
                    std::string log) {                                         \
    if (setup.X == item)                                                       \
      return;                                                                  \
    setup.X = item;                                                            \
    run_with_setup(srq, setup, log);                                           \
  }

#define run_with(slot, ITEM) run_with_##slot(srq.get(), setup, ITEM, #ITEM);

  define_run_with(mainhand);
  define_run_with(offhand);
  define_run_with(ranged);
  define_run_with(head);
  define_run_with(neck);
  define_run_with(shoulders);
  define_run_with(back);
  define_run_with(chest);
  define_run_with(wrist);
  define_run_with(gloves);
  define_run_with(belt);
  define_run_with(legs);
  define_run_with(boots);
  define_run_with(ring1);
  define_run_with(ring2);
  define_run_with(trinket1);
  define_run_with(trinket2);

#define mainhand(ITEM) run_with(mainhand, ITEM)
#define offhand(ITEM) run_with(offhand, ITEM)
#define ranged(ITEM) run_with(ranged, ITEM)
#define head(ITEM) run_with(head, ITEM)
#define neck(ITEM) run_with(neck, ITEM)
#define shoulders(ITEM) run_with(shoulders, ITEM)
#define back(ITEM) run_with(back, ITEM)
#define chest(ITEM) run_with(chest, ITEM)
#define wrist(ITEM) run_with(wrist, ITEM)
#define gloves(ITEM) run_with(gloves, ITEM)
#define belt(ITEM) run_with(belt, ITEM)
#define legs(ITEM) run_with(legs, ITEM)
#define boots(ITEM) run_with(boots, ITEM)
#define ring1(ITEM) run_with(ring1, ITEM)
#define ring2(ITEM) run_with(ring2, ITEM)
#define trinket1(ITEM) run_with(trinket1, ITEM)
#define trinket2(ITEM) run_with(trinket2, ITEM)

  void run_with_setup(SimRunQueue *srq, Setup setup, std::string log) {
    srq->push([=]() {
      current_char->get_talents()->set_current_index(setup.talent_index);
      setBuffSetup(setup.buff_index);
      auto e = current_char->get_equipment();
      e->set_mainhand(setup.mainhand);
      e->set_offhand(setup.offhand);
      e->set_ranged(setup.ranged);
      e->set_head(setup.head);
      e->set_neck(setup.neck);
      e->set_shoulders(setup.shoulders);
      e->set_back(setup.back);
      e->set_chest(setup.chest);
      e->set_wrist(setup.wrist);
      e->set_gloves(setup.gloves);
      e->set_belt(setup.belt);
      e->set_legs(setup.legs);
      e->set_boots(setup.boots);
      e->set_ring1(setup.ring1);
      e->set_ring2(setup.ring2);
      e->set_trinket1(setup.trinket1);
      e->set_trinket2(setup.trinket2);
      auto lg = log;
      fprintf(stderr, "%40s -- ", lg.c_str());
      run_a_sim();
    });
  }

  auto get_dual_weild_setup() -> Setup {
    Setup s;
    s.mainhand = Items::DragonfangBlade;
    s.offhand = Items::CoreHoundTooth;
    s.ranged = Items::SatyrsBow;
    s.head = Items::LionheartHelm;
    s.neck = Items::OnyxiaToothPendant;
    s.shoulders = Items::BloodsoakedPauldrons;
    s.back = Items::DragonsBloodCape;
    s.chest = Items::ZandalarVindicatorsBreastplate;
    s.wrist = Items::WristguardsofTrueFlight;
    s.gloves = Items::AgedCoreLeatherGloves;
    s.belt = Items::OnslaughtGirdle;
    s.legs = Items::BloodsoakedLegplates;
    s.boots = Items::SabatonsofWrath;
    s.ring1 = Items::BandofAccuria;
    s.ring2 = Items::CircleofAppliedForce;
    s.trinket1 = Items::BlackhandsBreadth;
    s.trinket2 = Items::DiamondFlask;

    return s;
  }

  auto get_tank_setup() -> Setup {
    Setup s;
    s.mainhand = Items::QuelSerrar;
    s.offhand = Items::ElementiumReinforcedBulwark;
    s.ranged = Items::MandokirsSting;
    s.head = Items::HelmofWrath;
    s.neck = Items::MedallionofSteadfastMight;
    s.shoulders = Items::PauldronsofWrath;
    s.back = Items::DragonsBloodCape;
    s.chest = Items::BreastplateofWrath;
    s.wrist = Items::BraceletsofWrath;
    s.gloves = Items::GauntletsofMight;
    s.belt = Items::WaistbandofWrath;
    s.legs = Items::LegplatesofWrath;
    s.boots = Items::SabatonsofWrath;
    s.ring1 = Items::BandofAccuria;
    s.ring2 = Items::ArchimtirosRingofReckoning;
    s.trinket1 = Items::BlackhandsBreadth;
    s.trinket2 = Items::DiamondFlask;

    return s;
  }

  auto get_shield_setup() -> Setup {
    Setup s;
    s.mainhand = Items::DragonfangBlade;
    s.offhand = Items::ElementiumReinforcedBulwark;
    s.ranged = Items::BlastershotLauncher;
    s.head = Items::LionheartHelm;
    s.neck = Items::OnyxiaToothPendant;
    s.shoulders = Items::BloodsoakedPauldrons;
    s.back = Items::CloakoftheShroudedMists;
    s.chest = Items::ZandalarVindicatorsBreastplate;
    s.wrist = Items::WristguardsofStability;
    s.gloves = Items::AgedCoreLeatherGloves;
    s.belt = Items::OnslaughtGirdle;
    s.legs = Items::BloodsoakedLegplates;
    s.boots = Items::SabatonsofWrath;
    s.ring1 = Items::BandofAccuria;
    s.ring2 = Items::CircleofAppliedForce;
    s.trinket1 = Items::BlackhandsBreadth;
    s.trinket2 = Items::DiamondFlask;

    return s;
  }

  void run() {
    Setup setup;

    set_protection_rotation();
    set_prot();

    setup = get_dual_weild_setup();
    setup.set_fury_prot();
    setup.set_full_worldbuffs();
    iterate_through_items(setup, "Dual Weild Fury");

    setup = get_dual_weild_setup();
    setup.set_impale_prot();
    setup.set_full_worldbuffs();
    iterate_through_items(setup, "Dual Weild Prot");

    setup = get_shield_setup();
    setup.set_fury_prot();
    setup.set_full_worldbuffs();
    iterate_through_items(setup, "Shield Fury");

    setup = get_shield_setup();
    setup.set_impale_prot();
    setup.set_full_worldbuffs();
    iterate_through_items(setup, "Shield Prot");

    /* do_org_splits(setup); */

    run_next_sim();
  }

  enum Spec { fury, impale };

  enum Gear { dual_weild, shield, tank };

  void add_org_split(SimRunQueue *srq, Spec spec, Gear gear, bool world_buffs,
                     std::string spec_string, std::string gear_string,
                     std::string world_buffs_string) {
    Setup setup;

    switch (gear) {
    case Gear::dual_weild:
      setup = get_dual_weild_setup();
      break;
    case Gear::shield:
      setup = get_shield_setup();
      break;
    case Gear::tank:
      setup = get_tank_setup();
      break;
    }

    switch (spec) {
    case Spec::fury:
      setup.set_fury_prot();
      break;
    case Spec::impale:
      setup.set_impale_prot();
      break;
    }

    if (world_buffs) {
      setup.set_full_worldbuffs();
    } else {
      setup.set_no_worldbuffs();
    }

    char buff[100];
    snprintf(buff, 100, "%10s, %10s, %6s", spec_string.c_str(),
             gear_string.c_str(), world_buffs_string.c_str());

    run_with_setup(srq, setup, buff);
  }

#define add_split(S, G, W)                                                     \
  add_org_split(srq.get(), S, G, W, #S, #G, #W);

  void do_org_splits(Setup setup) {
    std::shared_ptr<SimRunQueue> srq;

    srq = std::make_shared<SimRunQueue>("");

    add_split(fury, shield, true);
    add_split(impale, shield, true);
    add_split(fury, tank, true);
    add_split(impale, tank, true);

    add_split(fury, shield, false);
    add_split(impale, shield, false);
    add_split(fury, tank, false);
    add_split(impale, tank, false);

    add_split(fury, dual_weild, true);
    add_split(impale, dual_weild, true);

    add_split(fury, dual_weild, false);
    add_split(impale, dual_weild, false);

    queues.push(srq);
  }

  void iterate_through_items(Setup setup, std::string name) {
    auto srq = std::make_shared<SimRunQueue>(std::move(name));
    run_with_setup(srq.get(), setup, "current setup");

    if (setup.offhand != ElementiumReinforcedBulwark) {
      auto s2 = setup;
      s2.mainhand = WarbladeoftheHakkariMh;
      s2.offhand = WarbladeoftheHakkariOh;
      s2.gloves = GauntletsofMight;
      run_with_setup(srq.get(), s2, "Warblades of the Hakkari + Might");

      s2 = setup;
      s2.mainhand = ThunderfuryBlessedBladeoftheWindseeker;
      s2.gloves = GauntletsofMight;
      run_with_setup(srq.get(), s2, "Thunderfury + CHT + Might");

      s2 = setup;
      s2.mainhand = ThunderfuryBlessedBladeoftheWindseeker;
      s2.gloves = EdgemastersHandguards;
      run_with_setup(srq.get(), s2, "Thunderfury + CHT + Edge");
    }

    if (setup.offhand == ElementiumReinforcedBulwark) {
      auto s2 = setup;
      s2.mainhand = ThunderfuryBlessedBladeoftheWindseeker;
      s2.gloves = GauntletsofMight;
      run_with_setup(srq.get(), s2, "Thunderfury + Might");
    }

    if (setup.offhand == ElementiumReinforcedBulwark) {
      auto s2 = setup;
      s2.mainhand = ThunderfuryBlessedBladeoftheWindseeker;
      s2.gloves = EdgemastersHandguards;
      run_with_setup(srq.get(), s2, "Thunderfury + Edgemasters");
    }

    shoulders(DrakeTalonPauldrons);

    back(CloakofDraconicMight);
    back(PuissantCape);
    back(CloakofFiremaw);
    back(MightoftheTribe);
    /* if (setup.offhand == ElementiumReinforcedBulwark) { */
    /*   auto s2 = setup; */
    /*   s2.back = ZulianTigerhideCloak; */
    /*   s2.ranged = SatyrsBow; */
    /*   s2.ring1 = CircleofAppliedForce; */
    /*   run_with_setup(srq.get(), s2, "ZulianTigerhideCloak\n"); */
    /* } else { */
      back(ZulianTigerhideCloak);
    /* } */

    chest(MalfurionsBlessedBulwark);
    chest(RunedBloodstainedHauberk);
    chest(SavageGladiatorChain);

    legs(BloodsoakedLegplates);
    legs(KnightCaptainsPlateLeggings);
    legs(LegguardsoftheFallenCrusader);

    /* if (setup.offhand == ElementiumReinforcedBulwark) { */
    /*   auto s2 = setup; */
    /*   s2.boots = ChromaticBoots; */

    /*   s2.ranged = SatyrsBow; */
    /*   s2.ring1 = CircleofAppliedForce; */
    /*   run_with_setup(srq.get(), s2, "Chromatic Boots"); */
    /* } else { */
      boots(ChromaticBoots);
    /* } */

    ring2(DonJuliosBand);

    trinket1(DrakeFangTalisman);
    trinket1(HandofJustice);

    ranged(StrikersMark);
    ranged(Bloodseeker);

    queues.push(srq);
  }

  void run_a_sim() {
    if (sim_in_progress)
      return;
    if (current_char->get_spells()->get_attack_mode() ==
            AttackMode::MeleeAttack &&
        current_char->get_equipment()->get_mainhand() == nullptr)
      return;
    if (current_char->get_spells()->get_attack_mode() ==
            AttackMode::RangedAttack &&
        current_char->get_equipment()->get_ranged() == nullptr)
      return;

    sim_in_progress = true;

    QVector<QString> setup_strings;
    raid_setup[0][0]["setup_string"] =
        CharacterEncoder(current_char).get_current_setup_string();

    for (const auto &party : raid_setup) {
      for (const auto &party_member : party) {
        if (party_member.contains("setup_string"))
          setup_strings.append(party_member["setup_string"].toString());
      }
    }

    const int num_stat_weights = 1 + sim_settings->get_active_options().size();
    thread_pool->run_sim(setup_strings, true,
                         sim_settings->get_combat_iterations_full_sim(),
                         num_stat_weights);

    emit simProgressChanged();
  }

signals:
  void finished();
};
