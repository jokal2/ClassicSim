#pragma once

#include "ClassicSimControl.h"

class GUIControl : public ClassicSimControl {
    Q_OBJECT
public:
    /* Character */
    Q_PROPERTY(QString classColor READ get_class_color NOTIFY classChanged)
    Q_PROPERTY(QString className READ get_class_name NOTIFY classChanged)
    Q_PROPERTY(QString raceName READ get_race_name NOTIFY raceChanged)
    Q_PROPERTY(bool isAlliance READ get_is_alliance NOTIFY factionChanged)
    Q_PROPERTY(bool isHorde READ get_is_horde NOTIFY factionChanged)

    Q_INVOKABLE void selectClass(const QString& class_name);
    Q_INVOKABLE void selectRace(const QString& race_name);
    Q_INVOKABLE void selectFaction(const int faction);
    Q_INVOKABLE bool raceAvailable(const QString& race_name);
    /* End of Character */

    /* Talents */
    Q_PROPERTY(int talentPointsRemaining READ get_talent_points_remaining NOTIFY talentsUpdated)
    Q_PROPERTY(QString leftTalentTreeBackground READ getLeftBackgroundImage NOTIFY classChanged)
    Q_PROPERTY(QString midTalentTreeBackground READ getMidBackgroundImage NOTIFY classChanged)
    Q_PROPERTY(QString rightTalentTreeBackground READ getRightBackgroundImage NOTIFY classChanged)
    Q_PROPERTY(int leftTreePoints READ get_left_talent_tree_points NOTIFY talentsUpdated)
    Q_PROPERTY(int midTreePoints READ get_mid_talent_tree_points NOTIFY talentsUpdated)
    Q_PROPERTY(int rightTreePoints READ get_right_talent_tree_points NOTIFY talentsUpdated)
    Q_PROPERTY(QString talentAllocation READ get_formatted_talent_allocation NOTIFY talentsUpdated)

    Q_INVOKABLE QString getIcon(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool showPosition(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool showBottomArrow(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool showRightArrow(const QString& tree_position, const QString& talent_position) const;

    Q_INVOKABLE QString getBottomArrow(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE QString getRightArrow(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool bottomChildAvailable(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool bottomChildActive(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool rightChildAvailable(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool rightChildActive(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool isActive(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool isAvailable(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool isMaxed(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE bool hasTalentPointsRemaining() const;
    Q_INVOKABLE QString getRank(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE QString getMaxRank(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE void incrementRank(const QString& tree_position, const QString& talent_position);
    Q_INVOKABLE void decrementRank(const QString& tree_position, const QString& talent_position);
    Q_INVOKABLE QString getRequirements(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE QString getCurrentRankDescription(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE QString getNextRankDescription(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE QString getTreeName(const QString& tree_position) const;
    Q_INVOKABLE QString getTalentName(const QString& tree_position, const QString& talent_position) const;
    Q_INVOKABLE void maxRank(const QString& tree_position, const QString& talent_position);
    Q_INVOKABLE void minRank(const QString& tree_position, const QString& talent_position);

    Q_INVOKABLE void clearTree(const QString& tree_position);

    Q_INVOKABLE void setTalentSetup(const int talent_index);

    int get_left_talent_tree_points() const;
    int get_mid_talent_tree_points() const;
    int get_right_talent_tree_points() const;
    int get_tree_points(const QString& tree_position) const;
    QString get_formatted_talent_allocation() const;
    /* End of Talents */

    virtual void save_settings();
    void save_gui_settings();
    void load_gui_settings();

    void activate_gui_setting(const QStringRef& name, const QString& value);

    GUIControl(QObject* parent = nullptr);
    ~GUIControl();
};
