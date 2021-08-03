#include "ofxLaserDacManagerAudio.h"

using namespace ofxLaser;

vector<DacData> DacManagerAudio::updateDacList() {
    vector<ofSoundDevice> devices = ofSoundStreamListDevices();
    vector<DacData> dacs = {};
    for (auto device: devices) {
        DacData data(getType(), device.name);
        dacs.push_back(data);
    }
    return dacs;
}

DacBase* DacManagerAudio::getAndConnectToDac(const string& id) {
    DacAudio* dac = (DacAudio*) getDacById(id);
    // If we've already setup a stream for this ID, return a pointer to the existing dac.
    if (dac) {
        return dac;
    }
    // Loop through the DACs until we find a matching ID.
    vector<ofSoundDevice> devices = ofSoundStreamListDevices();
    for (auto device: devices) {
        if (id.find(device.name) != std::string::npos) {
            dac = new DacAudio();
            auto settings = dac->defaultSettings(device);
            if (!dac->setup(settings)) {
                ofLogError("Failed to setup audio DAC " + id + ". Continuing search...");
                dac = nullptr;
                continue;
            }
            dacsById[id] = dac;
            break;
        }
    }
    return dac;
}

bool DacManagerAudio::disconnectAndDeleteDac(const string& id) {
    if (dacsById.count(id) == 0) {
        ofLogError("DacManagerAudio::disconnectAndDeleteDac("+id+") - dac not found");
        return false;
    }
    DacAudio* dac = (DacAudio*)dacsById.at(id);
    dac->close();
    dacsById.erase(id);
    delete dac;
    return true;
}
