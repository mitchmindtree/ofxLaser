//
//  ofxLaserDacAssigner.cpp
//  ofxLaser
//
//  Created by Seb Lee-Delisle on 06/11/2017.
//
//

#include "ofxLaserDacAssigner.h"

using namespace ofxLaser;

DacAssigner * DacAssigner :: dacAssigner = NULL;

DacAssigner * DacAssigner::instance() {
	if(dacAssigner == NULL) {
		dacAssigner = new DacAssigner();
	}
	return dacAssigner;
}


DacAssigner :: DacAssigner() {

    if(dacAssigner == NULL) {
		dacAssigner = this;
	} else {
		ofLog(OF_LOG_ERROR, "Multiple ofxLaser::DacManager instances created");
        throw;
	}
    
    dacManagers.push_back(new DacManagerLaserdock());
    dacManagers.push_back(new DacManagerHelios());
    dacManagers.push_back(new DacManagerEtherdream());
    updateDacList();
	
}

DacAssigner :: ~DacAssigner() {
    //dacAssigner = NULL;
}

const vector<DacData>& DacAssigner ::getDacList(){
    return dacDataList; 
}

const vector<DacData>& DacAssigner ::updateDacList(){
    
    // get a new list of dacdata
    vector<DacData> newdaclist;
    
    for(DacManagerBase* dacManager : dacManagers) {
        // ask every dac manager for an updated list of DacData objects
        // and insert them into our new vector.
        vector<DacData> newdacs = dacManager->updateDacList();
        newdaclist.insert( newdaclist.end(), newdacs.begin(), newdacs.end() );
        
    }
    
    // go through the existing list, check against the new
    // list and if it can't find it any more, mark it as
    // unavailable.
    
    for(DacData& dacdata : dacDataList) {
        bool nowavailable = false;
        for(DacData& newdacdata : newdaclist) {
            // compare the new dac to the existing one
            if(newdacdata.id == dacdata.id) {
                
                // Store the new dac's availability
                // (the new dac should always be
                // available but just in case...)
                nowavailable = newdacdata.available;
                
                // We have a dacdata that is not available
                // but has an assigned projector which means that
                // the projector is waiting for that dac to
                // become available.
                // So let's get the dac and assign it to the projector!
                if(!dacdata.available && (dacdata.assignedProjector!=nullptr)) {
                    DacBase* dacToAssign = getManagerForType(dacdata.type)->getAndConnectToDac(dacdata.id);
                    dacdata.assignedProjector->setDac(dacToAssign);
                    dacdata.available = true;
                }
                break;
            }
        }
        
        dacdata.available = nowavailable;
        
    }
    
    // now go through the new dac list again, and find
    // dacs that are not already in the existing list
    for(DacData& newdacdata : newdaclist) {
        bool isnew = true;
        for(DacData& dacdata : dacDataList) {
            if(dacdata.id == newdacdata.id) {
                isnew = false;
                break;
            }
        }
        
        // if it's new, add it to the list
        if(isnew) {
            dacDataList.push_back(newdacdata);
        }
    }
    
    // sort the list (the DacData class has overloaded operators
    // that make the list sortable alphanumerically by their IDs
	std::sort(dacDataList.begin(), dacDataList.end());
    
    return dacDataList; 
    
}


bool DacAssigner ::assignToProjector(const string& daclabel, Projector& projector){
    
    DacData* dacdataptr = &getDacDataForLabel(daclabel);
    
    if(&emptyDacData==dacdataptr) {
        
        // no dacdata found! This usually means that we're loading
        // and the new dac hasn't been found yet. So we need to reserve
        // one.
        
        // extract the type and id out of the daclabel (perhaps make this
        // a separate function?)
        string dactype = daclabel.substr(0, daclabel.find(" "));
        string dacid = daclabel.substr(daclabel.find(" ")+1, string::npos);
        
        dacDataList.emplace_back(dactype, dacid, "", &projector);
        dacdataptr = &dacDataList.back();
        dacdataptr->available = false;
       
        return false;
        
    }
    DacData& dacdata = *dacdataptr;
    
    ofLogNotice("DacAssigner::assignToProjector - " + dacdata.label, ofToString(projector.projectorIndex));
    
  
    // get manager for type
    DacManagerBase* manager = getManagerForType(dacdata.type);
    if(manager==nullptr) {
        ofLogError("DacAssigner ::assignToProjector - invalid type " + dacdata.type);
        return false;
    }
    
    
    // if projector already has a dac then delete it!
    disconnectDacFromProjector(projector);
    
    DacBase* dacToAssign = nullptr;
    
    if(dacdata.assignedProjector!=nullptr) {
        // remove from current projector
        
        // Is this bad? Maybe better to get the dac
        // from its manager?
        dacToAssign = dacdata.assignedProjector->getDac();
        dacdata.assignedProjector->removeDac();
        dacdata.assignedProjector = nullptr;
        
    } else {
    
        // get dac from manager
        dacToAssign = manager->getAndConnectToDac(dacdata.id);
        
    }
    // if success
    if(dacToAssign!=nullptr) {
        // give the dac to the projector
        projector.setDac(dacToAssign);
        // store a reference to the projector in the
        // dacdata
        dacdata.assignedProjector = &projector;
        
        
        // clear the reference to this projector from the other dac data
        for(DacData& dacdataToCheck : dacDataList) {
            if(&dacdata == &dacdataToCheck) continue;
            else if(dacdataToCheck.assignedProjector == &projector) {
                dacdataToCheck.assignedProjector = nullptr;
            }
        }
        
    } else {
        // if we can't get a dac object for the label
        // the dac must have disconnected since we updated
        // the list!
        // Maybe we should store the projector in the
        // DacData anyway it can be connected if / when
        // it's found?
        dacdata.available = false;
        return false;
    }
    
    return true; 
}

bool DacAssigner :: disconnectDacFromProjector(Projector& projector) {
    DacData& dacData = getDacDataForProjector(projector);
    if(dacData.assignedProjector!=nullptr) {
        dacData.assignedProjector = nullptr;
        projector.removeDac();
        getManagerForType(dacData.type)->disconnectAndDeleteDac(dacData.id);
        return true;
    } else {
        return false;
    }
}
DacManagerBase* DacAssigner :: getManagerForType(string type){
    for(DacManagerBase* manager : dacManagers) {
        if(manager->getType() == type) {
            return manager;
            
            break;
        }
    }
    return nullptr;
    
}

DacData& DacAssigner ::getDacDataForLabel(const string& label){
    for(DacData& dacData : dacDataList) {
        if(dacData.label == label) {
            return dacData;
        }
    }
    
    
    return emptyDacData ;
}


DacData& DacAssigner ::getDacDataForProjector(Projector& projector){
    return getDacDataForLabel(projector.getDacLabel());
}

