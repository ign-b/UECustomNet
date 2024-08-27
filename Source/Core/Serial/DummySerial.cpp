#include "DummySerial.h"

void CDummySerial::GetSerial(CString &sSerial) {
    return sSerial.Assign("DummySerialChangeMe");
}

bool CDummySerial::ValidateSerial(CString const &sSerial) {
    return true;
}
