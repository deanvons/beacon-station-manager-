#pragma once

#include <QString>
#include <QStringList>
#include <QtNumeric>

// The two kinds of navigation beacon this tool manages.
//   NDB — Non-Directional Beacon. Transmits the same signal in every direction;
//         an aircraft's instrument simply points toward it. Frequency in kHz.
//   VOR — VHF Omnidirectional Range. Tells an aircraft which compass bearing it
//         is on, as seen from the station. Frequency in MHz.
enum class BeaconType { NDB, VOR };

// One beacon, as stored in the configuration file. Plain data, no behaviour.
struct Beacon
{
    QString ident;               // 2–3 capital letters, unique, e.g. "NVK"
    QString name;                // human-readable name, e.g. "Nordvik"
    BeaconType type = BeaconType::VOR;
    double frequency = 108.00;   // kHz for an NDB, MHz for a VOR
    bool hasDme = false;         // distance-measuring equipment fitted (VOR only)
    double latitude = 0.0;       // decimal degrees, -90 to 90
    double longitude = 0.0;      // decimal degrees, -180 to 180
    int rangeNm = 50;            // usable range in nautical miles, 10 to 200
    bool inService = true;       // false while the beacon is switched off for maintenance
};

// Two beacons are equal when every field matches. Floating-point fields are
// compared with qFuzzyCompare, because values that went through a spin box
// may differ in the last few bits.
inline bool operator==(const Beacon& a, const Beacon& b)
{
    return a.ident == b.ident
        && a.name == b.name
        && a.type == b.type
        && qFuzzyCompare(a.frequency, b.frequency)
        && a.hasDme == b.hasDme
        && qFuzzyCompare(a.latitude, b.latitude)
        && qFuzzyCompare(a.longitude, b.longitude)
        && a.rangeNm == b.rangeNm
        && a.inService == b.inService;
}

inline bool operator!=(const Beacon& a, const Beacon& b)
{
    return !(a == b);
}

// "NDB" or "VOR"
QString beaconTypeName(BeaconType type);

// Checks a single beacon against the rules in the assignment brief.
// Returns one message per broken rule; an empty list means the beacon is valid.
// (Whether an ident is unique is checked by the registry, not here.)
QStringList validateBeacon(const Beacon& beacon);
