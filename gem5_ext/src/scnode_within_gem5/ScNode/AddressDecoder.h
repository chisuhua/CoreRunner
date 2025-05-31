
#ifndef ADDRESSDECODER_H
#define ADDRESSDECODER_H

#include <utility>
#include <vector>

namespace DRAMSys
{

struct DecodedAddress
{
    DecodedAddress(unsigned channel,
                   unsigned rank,
                   unsigned stack,
                   unsigned bankgroup,
                   unsigned bank,
                   unsigned row,
                   unsigned column,
                   unsigned bytes) :
        channel(channel),
        rank(rank),
        stack(stack),
        bankgroup(bankgroup),
        bank(bank),
        row(row),
        column(column),
        byte(bytes)
    {
    }

    DecodedAddress() = default;

    unsigned channel = 0;
    unsigned rank = 0;
    unsigned stack = 0;
    unsigned bankgroup = 0;
    unsigned bank = 0;
    unsigned row = 0;
    unsigned column = 0;
    unsigned byte = 0;
};

class AddressDecoder
{
public:
    AddressDecoder(const Config::AddressMapping& addressMapping);

    [[nodiscard]] DecodedAddress decodeAddress(uint64_t encAddr) const;
    [[nodiscard]] unsigned decodeChannel(uint64_t encAddr) const;
    [[nodiscard]] uint64_t encodeAddress(DecodedAddress decodedAddress) const;
    [[nodiscard]] uint64_t maxAddress() const { return maximumAddress; }

    void print() const;
    void plausibilityCheck(const MemSpec &memSpec);

private:
    unsigned banksPerGroup;
    unsigned bankgroupsPerRank;

    uint64_t maximumAddress;

    // This container stores for each used xor gate a pair of address bits, the first bit is
    // overwritten with the result
    std::vector<std::vector<unsigned>> vXor;
    std::vector<unsigned> vChannelBits;
    std::vector<unsigned> vRankBits;
    std::vector<unsigned> vStackBits;
    std::vector<unsigned> vBankGroupBits;
    std::vector<unsigned> vBankBits;
    std::vector<unsigned> vRowBits;
    std::vector<unsigned> vColumnBits;
    std::vector<unsigned> vByteBits;
};

} // namespace DRAMSys

#endif // ADDRESSDECODER_H
