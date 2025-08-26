//import * as Sys from "sys";
import {logger} from "zigbee-herdsman-converters/lib/logger";
import {determineEndpoint} from "zigbee-herdsman-converters/lib/modernExtend";

const tag = {
	cluster: "genAnalogInput",
	type: ["attributeReport", "readResponse"],
	convert: (model, msg, publish, options, meta) => {
		const readValue = msg.data.presentValue;
		return {"tag": readValue};
	}
};

function createFloat(v1 = 0, v2 = 0, v3 = 0, v4 = 0) {
	let int32 = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;

	const buffer = new ArrayBuffer(4);
	const view = new DataView(buffer);

	view.setUint32(0, int32);
	return view.getFloat32(0);
}

const light = {
	key: ["setColor"],
	convertSet: async (entity, key, value, meta) => {
		let val = createFloat(value.r, value.g, value.b, value.duration);
		await entity.write("genAnalogOutput", {"presentValue": val});
	}
};

const buzzer = {
	key: ["buzz"],
	convertSet: async (entity, key, value, meta) => {
		let val = createFloat(value.duration);
		await entity.write("genAnalogOutput", {"presentValue": val});
	}
};

export default {
    zigbeeModel: ["AirsoftPoint"],
    model: "AirsoftPoint",
    vendor: "MadMagic",
    description: "Airsoft Game Point",
    fromZigbee: [tag],
    toZigbee: [light, buzzer],
    extend: [],
    meta: {},
};

