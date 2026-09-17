// OPTIONAL — this file is not built unless you uncomment the test block in
// CMakeLists.txt. Tests are not required for this assignment; see "Adding
// tests later" in assignment.md. It's here as a starting point if you want one.

#include "beacon.h"

#include <QtTest>

// Qt Test: each private slot in this class is one test.
class TestBeacon : public QObject
{
    Q_OBJECT

private slots:
    void typeName_forNdb_isNdb()
    {
        // Arrange
        const BeaconType type = BeaconType::NDB;
        const QString expected = QStringLiteral("NDB");

        // Act
        const QString actual = beaconTypeName(type);

        // Assert
        QCOMPARE(actual, expected);
    }

    void validate_validBeacon_hasNoMessages()
    {
        // Arrange
        const Beacon beacon = validVor();

        // Act
        const QStringList messages = validateBeacon(beacon);

        // Assert
        QVERIFY(messages.isEmpty());
    }

    // A test for one validation rule looks like this:
    //
    // void validate_identWithOneLetter_isRejected()
    // {
    //     Beacon beacon = validVor();
    //     beacon.ident = QStringLiteral("N");
    //
    //     const QStringList messages = validateBeacon(beacon);
    //
    //     QVERIFY(messages.contains("Ident must be 2 to 3 capital letters (A-Z)."));
    // }

private:
    // A beacon that breaks no rules. Each test copies it and changes one thing.
    static Beacon validVor()
    {
        Beacon beacon;
        beacon.ident = QStringLiteral("NVK");
        beacon.name = QStringLiteral("Nordvik");
        beacon.type = BeaconType::VOR;
        beacon.frequency = 113.25;
        beacon.hasDme = true;
        beacon.latitude = 63.4521;
        beacon.longitude = 10.9210;
        beacon.rangeNm = 130;
        beacon.inService = true;
        return beacon;
    }
};

QTEST_APPLESS_MAIN(TestBeacon)
#include "tst_beacon.moc"
