#ifndef KEYUTILS_H
#define KEYUTILS_H

#include <QString>

#include "proto/keys.pb.h"
#include "track/keys.h"

class KeyUtils {
  public:
    enum KeyNotation {
        // The default notation (set with setNotation).
        DEFAULT = 0,
        OPEN_KEY = 1,
        LANCELOT = 2
    };

    static const char* keyDebugName(mixxx::track::io::key::ChromaticKey key);

    static inline bool keyIsMajor(mixxx::track::io::key::ChromaticKey key) {
        return key > mixxx::track::io::key::INVALID &&
                key < mixxx::track::io::key::C_MINOR;
    }

    static QString keyToString(mixxx::track::io::key::ChromaticKey key,
                               KeyNotation notation=DEFAULT);

    static mixxx::track::io::key::ChromaticKey keyFromNumericValue(double value);

    static double keyToNumericValue(mixxx::track::io::key::ChromaticKey key);

    static mixxx::track::io::key::ChromaticKey scaleKeyOctaves(
        mixxx::track::io::key::ChromaticKey key, double scale);

    static mixxx::track::io::key::ChromaticKey scaleKeySteps(
        mixxx::track::io::key::ChromaticKey key, int steps);

    static mixxx::track::io::key::ChromaticKey guessKeyFromText(const QString& text);

    static mixxx::track::io::key::ChromaticKey calculateGlobalKey(
        const KeyChangeList& key_changes, int iTotalSamples);

    static void setNotation(
        const QMap<mixxx::track::io::key::ChromaticKey, QString>& notation);

  private:
    static QMutex s_notationMutex;
    static QMap<mixxx::track::io::key::ChromaticKey, QString> s_notation;
    static QMap<QString, mixxx::track::io::key::ChromaticKey> s_reverseNotation;
};

#endif /* KEYUTILS_H */