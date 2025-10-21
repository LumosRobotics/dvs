#ifndef MAIN_APPLICATION_COMMUNICATION_RECEIVED_DATA_H_
#define MAIN_APPLICATION_COMMUNICATION_RECEIVED_DATA_H_

#include "duoplot/internal.h"
#include "duoplot/math/math.h"
#include "duoplot/fillable_uint8_array.h"

class ReceivedData
{
private:
    duoplot::internal::CommunicationHeader hdr_;
    duoplot::internal::Function function_;
    uint8_t* payload_data_;
    uint8_t* raw_data_;
    uint64_t num_data_bytes_;
    uint64_t total_num_bytes_;

public:
    ReceivedData();
    ReceivedData(const ReceivedData& other) = delete;
    ReceivedData(ReceivedData&& other);
    ReceivedData& operator=(const ReceivedData& other) = delete;
    ReceivedData& operator=(ReceivedData&& other);

    ReceivedData(const size_t size_to_allocate);
    ~ReceivedData();

    void parseHeader();

    uint8_t* payloadData() const;
    uint8_t* rawData() const;
    uint64_t size() const;
    duoplot::internal::Function getFunction() const;
    const duoplot::internal::CommunicationHeader& getCommunicationHeader() const;
};

#endif  // MAIN_APPLICATION_COMMUNICATION_RECEIVED_DATA_H_
