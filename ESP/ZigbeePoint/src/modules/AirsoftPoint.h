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
    }

    void callBack(uint8_t arr[], size_t size) override {
        ZigbeeController::confirmed();

        if (size == 0) {
            for (size_t i = 1; i < _clusterSize; i++) {
                _clusters[i].reportAttribs();
            }
        } else {
            for (size_t i = 1; i <= size; i++) {
                if (arr[i] == '1') _modules[i].setup();
            }
        }
    }

private:
    std::map<String, ModuleBase*>* _modules;
    CustomCluster* _clusters;
    size_t _clusterSize;
};