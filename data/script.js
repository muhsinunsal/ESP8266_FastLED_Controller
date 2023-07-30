class Package {
    /*
        Package.body: {
        type: string;
        package: object;
    }
    */
    constructor(type, pack = null) {
        this.type = type;
        this.package = pack;
    }
}

class BrightnessPackage extends Package {
    /*
        ColorPackage.body: {
        type: "Color";
        package: {
            brightness: double;
            };
        }
    */
    constructor(alpha = 1) {
        super("Brightness");
        this.package = { "alpha": alpha };
    }
}

class ColorPackage extends Package {
    /*
        ColorPackage.body: {
        type: "Color";
        package: {
            rgb: {
                red: number;
                green: number;
                blue: number;
            };
            alpha: number,
            hsv: {
                hue: number;
                saturation: number;
                value: number;
            };
        }
    */

    constructor(red = 225, green = 225, blue = 225, hue = 0, saturation = 0, value = 225) {
        super("Color");
        let rgb = { red, green, blue };
        let hsv = { hue, saturation, value };
        this.package = { "rgb": rgb, "hsv": hsv };
    }
}

// DOESN'T WORK
/*
function isDifferentEnough(initial, final, limit) {
    let red_Ratio = ((final.package.rgb.red - initial.package.rgb.red) * 100) / initial.package.rgb.red;
    let green_Ratio = ((final.package.rgb.green - initial.package.rgb.green) * 100) / initial.package.rgb.blue;
    let blue_Ratio = ((final.package.rgb.blue - initial.package.rgb.blue) * 100) / initial.package.rgb.blue;
    return Math.abs(red_Ratio + green_Ratio + blue_Ratio) < limit;
}
*/

let socket_status_symbol = document.getElementById(
    "socket_status_symbol"
);
socket = new WebSocket("ws:/" + "/" + "192.168.1.215" + ":81");
socket.onopen = function (e) {
    socket_status_symbol.style.backgroundColor = "#00cc00"; // #88534D
    socket_status_symbol.innerHTML = "✓";
    console.log("[socket] WebSocket attatched. ");



    let oldColor_package = new ColorPackage();
    let oldBrightness_package = new BrightnessPackage();

    colorPicker.on("color:change", (color) => {
        let newColor_package = new ColorPackage(color.red, color.green, color.blue, color.hue, color.saturation, color.value);
        let newBrightness_package = new BrightnessPackage(color.alpha)
        if (socket.readyState === socket.OPEN) {
            //Check if Color is changed if not don't send
            if (JSON.stringify(oldColor_package) != JSON.stringify(newColor_package)) {
                socket.send(JSON.stringify(newColor_package));
                console.table(newColor_package.package);
                oldColor_package = newColor_package;
            }
            //Check if Brightness is changed if not don't send
            if (JSON.stringify(oldBrightness_package) != JSON.stringify(newBrightness_package)) {
                socket.send(JSON.stringify(newBrightness_package));
                console.table(newBrightness_package.package);
                oldBrightness_package = newBrightness_package;
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
    
    if (e.data.includes("Response_Color")) {
        const starterColor = JSON.parse(e.data);
        console.log(starterColor)
    }
    //document.getElementById("change_this").innerHTML = e.data;
};
let sliderDirection;
if (screen.width >= screen.height) {
    sliderDirection = "horizontal";
} else {
    sliderDirection = "vertical";
}

var colorPicker = new iro.ColorPicker("main", {
    wheelLightness: false,
    margin: 20,
    color: "rgb(255,225,225)",
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

let colorPicker_Element = document.getElementsByClassName("IroWheel")[0];

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