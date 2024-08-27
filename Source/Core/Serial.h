#pragma once

#include "Containers/String.h"

class CSerialInterface;

enum eSerial {
    SERIAL_DUMMY
};

class CSerial {
protected:
    static CSerialInterface *m_pInterface;

public:
    explicit CSerial(eSerial nMethod);

    void GetSerial(CString &sSerial);

    bool ValidateSerial(CString const &sSerial);
};
