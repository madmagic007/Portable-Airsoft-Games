#pragma once

#include "ModuleBase.h"
#include "../zigbee/ZigbeeController.h"
#include "map"

class AirsoftPoint : public ModuleBase {
public:
    using ModuleBase::ModuleBase;

    void set(std::map<String, ModuleBase*>& modules, CustomCluster* clusters, size_t clusterSize) {
        _modules = &modules;
        _clusters = clusters;
        _clusterSize = clusterSize;

        _setup =  true;
    }

    void receiveData(uint8_t arr[], size_t size) override {
        ZigbeeController::confirmed();

        if (!_reportedAttribs) {
            Serial.println("reporting attribs");
            _reportedAttribs = true;
            for (size_t i = 1; i < _clusterSize; i++) {
                _clusters[i].reportAttribs();
            }
        }

        size_t n = min(min(size, _modules->size() - 1), _clusterSize - 1);
        if (n != 0) {
            for (size_t i = 0; i < n; i++) {
                if (arr[i] != '1') continue;
                Serial.printf("%d is 1\n", i);

                if (_clusters[i + 1]._linkedModule) {
                    _clusters[i + 1]._linkedModule->doSetup();
                }
            }
        }
    }

private:
    std::map<String, ModuleBase*>* _modules;
    CustomCluster* _clusters;
    size_t _clusterSize;
    bool _reportedAttribs;
};