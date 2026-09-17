#include "beacon.h"

QString beaconTypeName(BeaconType type)
{
    switch (type) {
    case BeaconType::NDB: return QStringLiteral("NDB");
    case BeaconType::VOR: return QStringLiteral("VOR");
    }
    return QStringLiteral("?");
}

QStringList validateBeacon(const Beacon& beacon)
{
    // TODO (Milestone 2): implement the validation rules from the brief, test-first.
    Q_UNUSED(beacon);
    return {};
}
