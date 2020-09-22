//
// Created by akgun on 4.09.2020.
//

#include "TestApp.h"

void TestApp::testCRC(TypeDistributor& distributor, TypeCollector& collector, TypeNode& node) {


    const std::string data = "55AA00A10000003200680003000400010000000000000000EE4A7F0000010000000000000000000000000000000000000000000000000000000055AA00A30000000F6D6174726F702D6D756C7469706C79"
                       "55AA00A3000000112E2F6D6174726F702D6D756C7469706C7955AA00A400000034"
                       "31300A0A302E312C20302E322C20302E342C20302E362C20302E382C20312E302C20302E382C20302E342C20302E322C20302E31"
                       "55AA00A500000004";

    const size_t size = data.size() / 2;

    uint8_t *out = new uint8_t[size];

    Util::str2hex(out, data.c_str(), data.size());

    uint32_t crc = 0;

    crc = CRC::Calculate(out, size, Util::crcTable, crc);

    LOGS_I(distributor->getHost(), "CRC : 0x%X", crc);

    delete[] out;
}
