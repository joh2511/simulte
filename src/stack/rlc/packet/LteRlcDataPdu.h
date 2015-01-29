//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#ifndef LTERLCDATAPDU_H_
#define LTERLCDATAPDU_H_

#include "LteRlcDataPdu_m.h"
#include "LteRlcDefs.h"

/**
 * @class LteRlcDataPdu
 * @brief Base class for Lte RLC UM/AM Data Pdu
 *
 * Class derived from base class contained
 * in msg declaration: define common fields for UM/AM PDU
 * A Data PDU contains a list of SDUs: a SDU can be
 * a whole SDU or a fragment
 */
class LteRlcDataPdu : public LteRlcDataPdu_Base {

protected:

    /// List Of MAC SDUs
    RlcSduList sduList_;

    // number of SDU stored in the message
    unsigned int numSdu_;

    // define the segmentation info for the PDU
    FramingInfo fi_;

    // Sequence number of the PDU
    unsigned int pduSequenceNumber_;

    // Length of the PDU
    int64 rlcPduLength_;

public:

    /**
     * Constructor
     */
    LteRlcDataPdu(const char* name = NULL, int kind = 0) :
        LteRlcDataPdu_Base(name, kind)
    {
        numSdu_ = 0;
    }

    virtual ~LteRlcDataPdu()
    {

        // Needs to delete all contained packets
        RlcSduList::iterator sit;
        for (sit = sduList_.begin(); sit != sduList_.end(); sit++)
            dropAndDelete(*sit);
    }

    void setPduSequenceNumber(unsigned int sno);
    unsigned int getPduSequenceNumber();

    void setFramingInfo(FramingInfo fi);
    FramingInfo getFramingInfo();

    unsigned int getNumSdu() { return numSdu_; }

    /**
     * pushSdu() gets ownership of the packet
     * and stores it inside the rlc sdu list
     * in back position
     *
     * @param pkt packet to store
     */
    virtual void pushSdu(cPacket* pkt)
    {
        take(pkt);
        rlcPduLength_ += pkt->getByteLength();
        sduList_.push_back(pkt);
        numSdu_++;
    }

    /**
     * popSdu() pops a packet from front of
     * the sdu list and drops ownership before
     * returning it
     *
     * @return popped packet
     */
    virtual cPacket* popSdu()
    {
        cPacket* pkt = sduList_.front();
        sduList_.pop_front();
        rlcPduLength_ -= pkt->getByteLength();
        numSdu_--;
        drop(pkt);
        return pkt;
    }
};

/**
 * @class LteRlcUmDataPdu
 * @brief Lte RLC UM Data Pdu
 *
 * Define additional fields for UM PDU
 */
class LteRlcUmDataPdu : public LteRlcDataPdu {

public:

    LteRlcUmDataPdu(const char* name = NULL, int kind = 0) :
        LteRlcDataPdu(name, kind)
    {
        rlcPduLength_ = RLC_HEADER_UM;
    }

    virtual ~LteRlcUmDataPdu() {}
};

/**
 * @class LteRlcAmDataPdu
 * @brief Lte RLC AM Data Pdu
 *
 * Define additional fields for AM PDU
 */
class LteRlcAmDataPdu : public LteRlcDataPdu {

    // if true, a status PDU is required
    bool pollStatus_;

public:

    LteRlcAmDataPdu(const char* name = NULL, int kind = 0) :
        LteRlcDataPdu(name, kind)
    {
        rlcPduLength_ = RLC_HEADER_UM;
    }

    virtual ~LteRlcAmDataPdu() {}

    void setPollStatus(bool p) { pollStatus_ = p; }
    bool getPollStatus() { return pollStatus_; }
};


Register_Class(LteRlcDataPdu);

#endif /* LTERLCDATAPDU_H_ */
