var colorPicker_Element = document.getElementsByClassName("IroWheel")[0];
var patternDropdown_Element = document.getElementById("pattern_selector");
var paletteDropdown_Element = document.getElementById("palette_selector");
var patternOptions = [];
var paletteOptions = [];
var lastUpdateMessage;
var own_ID;
var lastMessageOwner_ID = 0; //ID
var socket_status_symbol = document.getElementById("socket_status_symbol");
var webSocket = new WebSocket("ws:/" + "/" + "192.168.1.215" + ":81");
webSocket.onopen = function (e) {
    socket_status_symbol.style.backgroundColor = "#00cc00"; // #88534D
    socket_status_symbol.innerHTML = "✓";
    console.log("%c[Socket Attatched]", "font-weight: bold;font-size:1rem");
    colorPicker.on("color:change", function (newColor) {
        // 		//console.table(newColorVar)
        if (webSocket.readyState === webSocket.OPEN) {
            if (JSON.stringify(newColor.rgba) != JSON.stringify(lastUpdateMessage.rgba)) {
                lastUpdateMessage.rgba = newColor.rgba;
                webSocket.send(JSON.stringify(lastUpdateMessage));
                console.log("%c[Socket Sent]: ", "font-weight: bold;font-size:1rem", lastUpdateMessage);
            }
        }
        else {
            socket_status_symbol.style.backgroundColor = "#800000"; // #88534D
            socket_status_symbol.innerHTML = "✖";
            document.querySelector(".desktop-error").classList.add("shown");
            document.querySelector(".mobile-error").classList.add("shown");
        }
    });
};
webSocket.onerror = function (e) {
    console.log("%c[Socket Error]", "font-weight: bold;font-size:1rem");
};
webSocket.onmessage = function (e) {
    var ignoreFlag = false;
    var json_message;
    json_message = JSON.parse(e.data);
    if (json_message.type == "startMessage") {
        json_message = json_message;
        console.log("%c[Socket Recived]: Start Message", "font-weight: bold;font-size:1rem", json_message);
        // Assign number to client
        own_ID = json_message.assignedID;
        document.getElementById("clientID").innerHTML = "You're ".concat(own_ID + 1, "st user");
        // Clear Options
        patternDropdown_Element.innerHTML = "";
        paletteDropdown_Element.innerHTML = "";
        // Fill up Pattern Options
        json_message.patternList.forEach(function (pattern, i) {
            var element = document.createElement("option");
            element.setAttribute("value", String(i));
            element.innerText = pattern;
            patternDropdown_Element.appendChild(element);
        });
        // Fill up Palette Options
        json_message.paletteList.forEach(function (palette, i) {
            var element = document.createElement("option");
            element.setAttribute("value", String(i));
            element.innerText = palette;
            paletteDropdown_Element.appendChild(element);
        });
    }
    else if (json_message.type == "updateMessage") {
        json_message = json_message;
        console.log("%c[Socket Recived]: Update Message", "font-weight: bold;font-size:1rem", json_message);
        lastUpdateMessage = json_message;
        //Color
        // Setting ignore to pervent echoing
        // 			/////////////// DÜZEEEELLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLT
        ignoreFlag = true;
        setTimeout(function () {
            console.log("IgnoreFlag:", ignoreFlag);
            ignoreFlag = false;
        }, 100);
        if (ignoreFlag) {
            var rgba = json_message.rgba;
            colorPicker.color.rgba = rgba;
        }
        patternDropdown_Element.value = String(json_message.patternIndex);
        paletteDropdown_Element.value = String(json_message.paletteIndex);
    }
    else {
        console.log("%c[Socket Recived]: ", "font-weight: bold;font-size:1rem", e.data);
    }
    //document.getElementById("change_this").innerHTML = e.data;
};
patternDropdown_Element.addEventListener("change", function (e) {
    lastUpdateMessage.patternIndex = Number(patternDropdown_Element.value);
    webSocket.send(JSON.stringify(lastUpdateMessage));
    console.log("%c[Socket Sent]: ", "font-weight: bold;font-size:1rem", lastUpdateMessage);
});
paletteDropdown_Element.addEventListener("change", function (e) {
    lastUpdateMessage.paletteIndex = Number(paletteDropdown_Element.value);
    webSocket.send(JSON.stringify(lastUpdateMessage));
    console.log("%c[Socket Sent]: ", "font-weight: bold;font-size:1rem", lastUpdateMessage);
});
var sliderDirection;
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
}
else {
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
