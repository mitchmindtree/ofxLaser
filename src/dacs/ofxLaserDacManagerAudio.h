#pragma once

#include "ofxLaserDacManagerBase.h"
#include "ofxLaserDacAudio.h"

namespace ofxLaser {
class DacManagerAudio : public DacManagerBase {
    public:
    DacManagerAudio() {}
    ~DacManagerAudio() {}
    virtual vector<DacData> updateDacList() override;
    virtual DacBase* getAndConnectToDac(const string& id) override;
    virtual bool disconnectAndDeleteDac(const string& id) override;
    virtual string getType() override { return "Audio"; }
    virtual void exit() override {}
};
}
