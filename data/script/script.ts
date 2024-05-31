interface Message {
	type: "startMessage" | "updateMessage";
}
interface startMessage extends Message {
	assignedID: number;
	patternList: string[];
	paletteList: string[];
}
interface updateMessage extends Message {
	senderID: number;
	patternIndex: number;
	paletteIndex: number;
	hsv: {
		h: number;
		s: number;
		v: number;
	};
	rgba: {
		r: number;
		g: number;
		b: number;
		a: number;
	};
}

let colorPicker_Element = document.getElementsByClassName("IroWheel")[0];

let patternDropdown_Element = document.getElementById("pattern_selector")! as HTMLSelectElement;
let paletteDropdown_Element = document.getElementById("palette_selector")! as HTMLSelectElement;

let patternOptions = [];
let paletteOptions = [];

let lastUpdateMessage: updateMessage;

let own_ID;
let lastMessageOwner_ID = 0; //ID

const socket_status_symbol = document.getElementById("socket_status_symbol");
const webSocket = new WebSocket("ws:/" + "/" + "192.168.1.215" + ":81");
webSocket.onopen = function (e) {
	socket_status_symbol!.style.backgroundColor = "#00cc00"; // #88534D
	socket_status_symbol!.innerHTML = "✓";
	console.log("%c[Socket Attatched]", "font-weight: bold;font-size:1rem");

	colorPicker.on("color:change", (newColor) => {
		// 		//console.table(newColorVar)

		if (webSocket.readyState === webSocket.OPEN) {
			if (JSON.stringify(newColor.rgba) != JSON.stringify(lastUpdateMessage.rgba)) {
				lastUpdateMessage.rgba = newColor.rgba;
				webSocket.send(JSON.stringify(lastUpdateMessage));
				console.log("%c[Socket Sent]: ", "font-weight: bold;font-size:1rem", lastUpdateMessage);
			}
		} else {
			socket_status_symbol!.style.backgroundColor = "#800000"; // #88534D
			socket_status_symbol!.innerHTML = "✖";
			document.querySelector(".desktop-error")!.classList.add("shown");
			document.querySelector(".mobile-error")!.classList.add("shown");
		}
	});
};
webSocket.onerror = (e) => {
	console.log("%c[Socket Error]", "font-weight: bold;font-size:1rem");
};
webSocket.onmessage = (e) => {
	let ignoreFlag = false;

	let json_message: startMessage | updateMessage;
	json_message = JSON.parse(e.data);

	if (json_message.type == "startMessage") {
		json_message = json_message as startMessage;

		console.log("%c[Socket Recived]: Start Message", "font-weight: bold;font-size:1rem", json_message);

		// Assign number to client
		own_ID = json_message.assignedID;
		document.getElementById("clientID")!.innerHTML = `You're ${own_ID + 1}st user`;

		// Clear Options
		patternDropdown_Element.innerHTML = "";
		paletteDropdown_Element.innerHTML = "";

		// Fill up Pattern Options
		json_message.patternList.forEach((pattern, i) => {
			const element = document.createElement("option");
			element.setAttribute("value", String(i));
			element.innerText = pattern;
			patternDropdown_Element.appendChild(element);
		});

		// Fill up Palette Options
		json_message.paletteList.forEach((palette, i) => {
			const element = document.createElement("option");
			element.setAttribute("value", String(i));
			element.innerText = palette;

			paletteDropdown_Element.appendChild(element);
		});
	} else if (json_message.type == "updateMessage") {
		json_message = json_message as updateMessage;

		console.log("%c[Socket Recived]: Update Message", "font-weight: bold;font-size:1rem", json_message);

		lastUpdateMessage = json_message;
		//Color

		// Setting ignore to pervent echoing
		// 			/////////////// DÜZEEEELLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLT
		ignoreFlag = true;
		setTimeout(() => {
			console.log("IgnoreFlag:", ignoreFlag);
			ignoreFlag = false;
		}, 100);

		if (ignoreFlag) {
			let rgba = json_message.rgba;
			colorPicker.color.rgba = rgba;
		}

		patternDropdown_Element.value = String(json_message.patternIndex);
		paletteDropdown_Element.value = String(json_message.paletteIndex);
	} else {
		console.log("%c[Socket Recived]: ", "font-weight: bold;font-size:1rem", e.data);
	}

	//document.getElementById("change_this").innerHTML = e.data;
};

patternDropdown_Element.addEventListener("change", (e) => {
	lastUpdateMessage.patternIndex = Number(patternDropdown_Element.value);

	webSocket.send(JSON.stringify(lastUpdateMessage));
	console.log("%c[Socket Sent]: ", "font-weight: bold;font-size:1rem", lastUpdateMessage);
});

paletteDropdown_Element.addEventListener("change", (e) => {
	lastUpdateMessage.paletteIndex = Number(paletteDropdown_Element.value);

	webSocket.send(JSON.stringify(lastUpdateMessage));
	console.log("%c[Socket Sent]: ", "font-weight: bold;font-size:1rem", lastUpdateMessage);
});

let sliderDirection;
// eslint-disable-next-line @typescript-eslint/ban-ts-comment
// @ts-ignore

var colorPicker = new iro.ColorPicker("#color_picker", {
	wheelLightness: false,
	margin: 20,
	layout: [
		{
			// eslint-disable-next-line @typescript-eslint/ban-ts-comment
			// @ts-ignore
			component: iro.ui.Wheel,
			options: {
				borderWidth: 5,
			},
		},
		{
			// eslint-disable-next-line @typescript-eslint/ban-ts-comment
			// @ts-ignore
			component: iro.ui.Slider,
			options: {
				sliderType: "alpha",
				borderWidth: 2,
			},
		},
	],
	layoutDirection: sliderDirection,
});

if (screen.width >= screen.height) {
	sliderDirection = "horizontal";
} else {
	sliderDirection = "vertical";
}

/*
 let power = { element: document.getElementById("power"), status: true };
 power.element.addEventListener("click", () => {
 if (power.status) {
 power.status = !power.status;
 } else {
 power.status = !power.status;
 }
 });
 */
