class Package {
    constructor(type, pack = null) {
        this.type = type;
        this.package = pack;
    }
}

class ColorPackage extends Package {
    constructor(red = 189, green = 189, blue = 189, alpha = 189, hue = 189, saturation = 189, value = 189) {
        super("color");
        let rgba = { r: red, g: green, b: blue, a: alpha };
        let hsv = { h: hue, s: saturation, v: value };
        this.package = { color: { "rgba": rgba, "hsv": hsv } };
    }
}

class PatternIndexPackage extends Package {
    constructor(patternIndex) {
        super("patternIndex");
        this.package = { "patternIndex": patternIndex }
    }
}

class PaletteIndexPackage extends Package {
    constructor(paletteIndex) {
        super("paletteIndex");
        this.package = { "paletteIndex": paletteIndex }
    }
}


let colorPicker_Element = document.getElementsByClassName("IroWheel")[0];

let patternDropdown_Element = document.getElementById("pattern_selector");
let paletteDropdown_Element = document.getElementById("palette_selector");

let patternOptions = [];
let paletteOptions = [];

let oldColorPackage = new ColorPackage();

let own_ID;
let lastMessageOwner_ID = 0; // ID

let socket_status_symbol = document.getElementById(
    "socket_status_symbol"
);
socket = new WebSocket("ws:/" + "/" + "192.168.1.215" + ":81");
socket.onopen = function (e) {
    socket_status_symbol.style.backgroundColor = "#00cc00"; // #88534D
    socket_status_symbol.innerHTML = "✓";
    console.log("[socket] WebSocket attatched. ");


    colorPicker.on("color:change", (color) => {
        const newColorVar = {
            rgba: color.rgba,
            hsv: color.hsv
        };
        //console.table(newColorVar)

        if (socket.readyState === socket.OPEN) {
            if (JSON.stringify(newColorVar) != JSON.stringify(oldColorPackage.package.color)) {

                oldColorPackage.package.color = newColorVar
                socket.send(JSON.stringify(oldColorPackage));
                //console.table(oldColorPackage.package.color)
            }
        } else {
            socket_status_symbol.style.backgroundColor = "#800000"; // #88534D
            socket_status_symbol.innerHTML = "✖";
        }
    });
};
socket.onerror = function (e) {
    console.log("[socket] WebSocket had an Error. ");
};
socket.onmessage = function (e) {
    console.log("[SOCKET RESPONSE] " + e.data);
    let ignoreFlag = false;

    const json_message = JSON.parse(e.data)

    switch (json_message.type) {

        //One time at start message
        case "startMessage":

            // Assign number to client
            own_ID = json_message.userID;
            const idElement = document.getElementById("clientID");
            idElement.innerHTML = own_ID;

            // Fill up Pattern Options
            for (let i = 0; i < json_message.package.pattern_list.length; i++) {
                const element = document.createElement("option");
                element.setAttribute("value", i);
                element.innerText = json_message.package.pattern_list[i];

                patternDropdown_Element.appendChild(element)
            }

            // Fill up Palette Options
            for (let i = 0; i < json_message.package.palette_list.length; i++) {
                const element = document.createElement("option");
                element.setAttribute("value", i);
                element.innerText = json_message.package.palette_list[i];

                paletteDropdown_Element.appendChild(element)
            }

            break;

        case "color":
            // Setting ignore to pervent echoing
            /////////////// DÜZEEEELLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLT
            ignoreFlag = true;
            setTimeout(() => {
                console.log("IgnoreFlag", ignoreFlag)
                ignoreFlag = false;
            }, 100);

            if(ignoreFlag){
                let rgba = json_message.package.rgba
                colorPicker.color.rgba = rgba
            }
            break;
        case "patternIndex":
            patternDropdown_Element.value = json_message.package.patternIndex;
            break;

        case "paletteIndex":
            paletteDropdown_Element.value = json_message.package.paletteIndex;
            break;
    }

    //document.getElementById("change_this").innerHTML = e.data;
};

patternDropdown_Element.addEventListener("change", (e) => {
    const package = new PatternIndexPackage(patternDropdown_Element.value);
    socket.send(JSON.stringify(package));
});

paletteDropdown_Element.addEventListener("change", (e) => {
    const package = new PaletteIndexPackage(paletteDropdown_Element.value);
    console.log(package)
    socket.send(JSON.stringify(package));
});

let sliderDirection;
var colorPicker = new iro.ColorPicker("main", {
    wheelLightness: false,
    margin: 20,
    layout: [
        {
            component: iro.ui.Wheel,
        },
        {
            component: iro.ui.Slider,
            options: {
                sliderType: 'alpha'
            }
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