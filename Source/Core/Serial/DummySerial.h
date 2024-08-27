#pragma once

#include "Serial/SerialInterface.h"

class CDummySerial : public CSerialInterface {
public:
    void GetSerial(CString &sSerial) override;

    bool ValidateSerial(CString const &sSerial) override;
};
