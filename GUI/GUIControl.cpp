#include "GUIControl.h"

#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "BuffModel.h"
#include "Character.h"
#include "CharacterTalents.h"
#include "DebuffModel.h"
#include "Equipment.h"
#include "EquipmentDb.h"
#include "EnchantModel.h"
#include "Faction.h"
#include "ItemModel.h"
#include "Race.h"
#include "SimSettings.h"
#include "Target.h"
#include "Weapon.h"
#include "WeaponModel.h"

GUIControl::GUIControl(QObject* parent) : ClassicSimControl(parent) {

    load_gui_settings();
}
GUIControl::~GUIControl() {}

QString GUIControl::getIcon(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_icon(tree_position, talent_position);
}

bool GUIControl::showPosition(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->show_position(tree_position, talent_position);
}

bool GUIControl::showBottomArrow(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->show_bottom_arrow(tree_position, talent_position);
}
bool GUIControl::showRightArrow(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->show_right_arrow(tree_position, talent_position);
}

QString GUIControl::getBottomArrow(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_bottom_arrow(tree_position, talent_position);
}

QString GUIControl::getRightArrow(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_right_arrow(tree_position, talent_position);
}

bool GUIControl::isMaxed(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->is_maxed(tree_position, talent_position);
}

bool GUIControl::bottomChildAvailable(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->bottom_child_available(tree_position, talent_position);
}

bool GUIControl::bottomChildActive(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->bottom_child_active(tree_position, talent_position);
}

bool GUIControl::rightChildAvailable(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->right_child_available(tree_position, talent_position);
}

bool GUIControl::rightChildActive(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->right_child_active(tree_position, talent_position);
}

bool GUIControl::isActive(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->is_active(tree_position, talent_position);
}

bool GUIControl::isAvailable(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->is_available(tree_position, talent_position);
}

bool GUIControl::hasTalentPointsRemaining() const {
    return current_char->get_talents()->has_talent_points_remaining();
}

QString GUIControl::getRank(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_rank(tree_position, talent_position);
}

QString GUIControl::getMaxRank(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_max_rank(tree_position, talent_position);
}

void GUIControl::incrementRank(const QString& tree_position, const QString& talent_position) {
    current_char->get_talents()->increment_rank(tree_position, talent_position);
    emit talentsUpdated();
    emit statsChanged();
}

void GUIControl::decrementRank(const QString& tree_position, const QString& talent_position) {
    current_char->get_talents()->decrement_rank(tree_position, talent_position);
    emit talentsUpdated();
    emit statsChanged();
}

QString GUIControl::getRequirements(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_requirements(tree_position, talent_position);
}

QString GUIControl::getCurrentRankDescription(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_current_rank_description(tree_position, talent_position);
}

QString GUIControl::getNextRankDescription(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_next_rank_description(tree_position, talent_position);
}

int GUIControl::get_tree_points(const QString& tree_position) const {
    return current_char->get_talents()->get_tree_points(tree_position);
}

QString GUIControl::get_formatted_talent_allocation() const {
    return QString("%1 / %2 / %3").arg(get_left_talent_tree_points()).arg(get_mid_talent_tree_points()).arg(get_right_talent_tree_points());
}

QString GUIControl::getTreeName(const QString& tree_position) const {
    return current_char->get_talents()->get_tree_name(tree_position);
}

QString GUIControl::getTalentName(const QString& tree_position, const QString& talent_position) const {
    return current_char->get_talents()->get_talent_name(tree_position, talent_position);
}

void GUIControl::maxRank(const QString& tree_position, const QString& talent_position) {
    current_char->get_talents()->increase_to_max_rank(tree_position, talent_position);
    emit talentsUpdated();
    emit statsChanged();
}

void GUIControl::minRank(const QString& tree_position, const QString& talent_position) {
    current_char->get_talents()->decrease_to_min_rank(tree_position, talent_position);
    emit talentsUpdated();
    emit statsChanged();
}

void GUIControl::clearTree(const QString& tree_position) {
    current_char->get_talents()->clear_tree(tree_position);
    emit talentsUpdated();
    emit statsChanged();
}

void GUIControl::setTalentSetup(const int talent_index) {
    current_char->get_talents()->set_current_index(talent_index);
    emit talentsUpdated();
    emit statsChanged();
}

int GUIControl::get_left_talent_tree_points() const {
    return get_tree_points("LEFT");
}

int GUIControl::get_mid_talent_tree_points() const {
    return get_tree_points("MID");
}

int GUIControl::get_right_talent_tree_points() const {
    return get_tree_points("RIGHT");
}

void GUIControl::selectClass(const QString& class_name) {
    if (!chars.contains(class_name)) {
        qDebug() << QString("Class %1 not found in char list!").arg(class_name);
        return;
    }

    if (!supported_classes.contains(class_name)) {
        qDebug() << QString("Class %1 not implemented").arg(class_name);
        return;
    }

    set_character(chars[class_name]);
}

void GUIControl::selectRace(const QString& race_name) {
    if (!races.contains(race_name)) {
        qDebug() << QString("Race %1 not found").arg(race_name);
        return;
    }

    if (!current_char->race_available(races[race_name])) {
        qDebug() << QString("Race %1 not available for %2").arg(race_name, current_char->class_name);
        return;
    }

    current_char->set_race(races[race_name]);
    emit raceChanged();
    emit statsChanged();
}

void GUIControl::selectFaction(const int faction) {
    if (this->current_char->get_faction()->get_faction() == faction)
        return;

    if (current_char->class_name == "Shaman" || current_char->class_name == "Paladin") {
        const AvailableFactions::Name target_faction = current_char->get_faction()->get_faction_as_enum();
        set_character(chars["Warrior"]);

        if (current_char->get_faction()->get_faction_as_enum() != target_faction)
            current_char->switch_faction();

        reset_race(current_char);
    } else {
        current_char->switch_faction();
        reset_race(current_char);
    }

    if (current_char->get_faction()->is_alliance()) {
        Weapon* wpn = current_char->get_equipment()->get_mainhand();
        if (wpn != nullptr)
            wpn->clear_windfury();
    }

    current_char->get_equipment()->clear_items_not_available_for_faction();

    buff_model->update_buffs();
    debuff_model->update_debuffs();
    mh_temporary_enchants->set_character(current_char);
    item_model->update_items();
    weapon_model->update_items();

    emit raceChanged();
    emit statsChanged();
    emit factionChanged();
    emit enchantChanged();
    emit equipmentChanged();
    emit talentsUpdated();
}

bool GUIControl::raceAvailable(const QString& race_name) {
    if (!races.contains(race_name)) {
        qDebug() << QString("Race %1 not found").arg(race_name);
        return false;
    }

    return current_char->race_available(races[race_name]);
}

void GUIControl::load_gui_settings() {
    QFile file("Saves/GUI-setup.xml");

    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader reader(&file);
        reader.readNextStartElement();

        while (reader.readNextStartElement())
            activate_gui_setting(reader.name(), reader.readElementText().trimmed());
    }

    equipment_db->set_content_phase(sim_settings->get_phase());

    if (current_char == nullptr)
        set_character(chars["Warrior"]);

    if (!current_char->get_faction()->get_faction_races().contains(current_char->get_race()->get_name())) {
        QString name = current_char->get_race()->get_name();
        selectFaction(current_char->get_faction()->is_alliance() ? AvailableFactions::Name::Horde : AvailableFactions::Name::Alliance);
        selectRace(name);
    }
}

void GUIControl::save_gui_settings() {
    QFile file("Saves/GUI-setup.xml");
    file.remove();

    if (file.open(QIODevice::ReadWrite)) {
        QXmlStreamWriter stream(&file);
        stream.setAutoFormatting(true);
        stream.writeStartDocument();
        stream.writeStartElement("settings");

        stream.writeTextElement("class", current_char->class_name);
        stream.writeTextElement("race", current_char->get_race()->get_name());
        stream.writeTextElement("window", active_window);
        stream.writeTextElement("num_iterations_quick_sim", QString("%1").arg(sim_settings->get_combat_iterations_quick_sim()));
        stream.writeTextElement("num_iterations_full_sim", QString("%1").arg(sim_settings->get_combat_iterations_full_sim()));
        stream.writeTextElement("combat_length", QString("%1").arg(sim_settings->get_combat_length()));
        stream.writeTextElement("phase", QString::number(static_cast<int>(sim_settings->get_phase())));
        stream.writeTextElement("ruleset", QString("%1").arg(sim_settings->get_ruleset()));
        stream.writeTextElement("target_creature_type", current_char->get_target()->get_creature_type_string());
        stream.writeTextElement("threads", QString("%1").arg(sim_settings->get_num_threads_current()));

        QSet<SimOption::Name> options = sim_settings->get_active_options();
        for (const auto& option : options)
            stream.writeTextElement("sim_option", QString("%1").arg(option));

        stream.writeEndElement();
        stream.writeEndDocument();
        file.close();
    }
}

void GUIControl::activate_gui_setting(const QStringRef& name, const QString& value) {
    if (name == "class")
        set_character(chars[value]);
    else if (name == "race")
        selectRace(value);
    else if (name == "window")
        active_window = value;
    else if (name == "num_iterations_quick_sim")
        sim_settings->set_combat_iterations_quick_sim(value.toInt());
    else if (name == "num_iterations_full_sim")
        sim_settings->set_combat_iterations_full_sim(value.toInt());
    else if (name == "combat_length")
        sim_settings->set_combat_length(value.toInt());
    else if (name == "phase")
        sim_settings->set_phase(Content::get_phase(value.toInt()));
    else if (name == "ruleset")
        sim_settings->use_ruleset(static_cast<Ruleset>(value.toInt()), current_char);
    else if (name == "target_creature_type" && current_char != nullptr)
        current_char->get_target()->set_creature_type(value);
    else if (name == "threads")
        sim_settings->set_num_threads(value.toInt());
    else if (name == "sim_option")
        sim_settings->add_sim_option(static_cast<SimOption::Name>(value.toInt()));
}

void GUIControl::save_settings() {
  save_gui_settings();
  ClassicSimControl::save_settings();
}
