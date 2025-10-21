import { deviceAddCustomCluster } from "zigbee-herdsman-converters/lib/modernExtend";
import { Zcl } from "zigbee-herdsman";
import {logger} from "zigbee-herdsman-converters/lib/logger";

//TODO
//ADD `globalThis.controller = controller;` to index.js:148

const fromZB = {
	cluster: "toMQTT",
	type: ["attributeReport", "topic"],
	convert: (model, msg, publish, options, meta) => {
		const rawValue = msg.data.value;
		const split = rawValue.split("|");
		const topic = split[0];
		const value = split.slice(1).join("|");
	
		const payload = { 
			"device": options.friendly_name,
			"value": value
		};
		
		globalThis.controller.mqtt.publish(topic, JSON.stringify(payload), {});
	}
};

const toZB = {
	key: ["write"],
	convertSet: async (entity, key, value, meta) => {
		const split = value.split("|");
		const id = Number(split[0]);
		const vValue = split.slice(1).join("|");

		for (const endpoint of meta.device.endpoints) {
			if (endpoint.ID != id) continue;
			await endpoint.write("fromMQTT", {"value": vValue});
		}
	}
};

export default {
    zigbeeModel: ["AirsoftPoint"],
    model: "AirsoftPoint",
    vendor: "MadMagic",
    description: "Airsoft Game Point",
    fromZigbee: [fromZB],
    toZigbee: [toZB],
    extend: [
    	deviceAddCustomCluster("toMQTT", {
		ID: 0xff00,
		attributes: {
			value: {ID: 0x0000, type: Zcl.DataType.CHAR_STR}
		},
		commands: {},
		commandsResponse: {},
	}),
    	deviceAddCustomCluster("fromMQTT", {
		ID: 0xff01,
		attributes: {
			value: {ID: 0x0000, type: Zcl.DataType.CHAR_STR}
		},
		commands: {},
		commandsResponse: {},
	})
    ],
    meta: {},
};
