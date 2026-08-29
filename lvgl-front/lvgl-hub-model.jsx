/* eslint-disable */
// Room widget catalog + scene/power helpers (demo model for OEM hub kit).

const WIDGET_CATALOG = [
  { type: "onoff", label: "开关灯", labelEn: "Light", defaultName: "主灯", defaultNameEn: "Ceiling" },
  { type: "dimmer", label: "调光", labelEn: "Dimmer", defaultName: "灯带", defaultNameEn: "Strip" },
  { type: "curtain", label: "窗帘", labelEn: "Curtain", defaultName: "窗帘", defaultNameEn: "Curtain" },
  { type: "shutter", label: "卷帘", labelEn: "Shutter", defaultName: "卷帘", defaultNameEn: "Shutter" },
  { type: "clim", label: "空调", labelEn: "AC", defaultName: "空调", defaultNameEn: "AC" },
  { type: "thermo", label: "地暖", labelEn: "Floor heat", defaultName: "地暖", defaultNameEn: "Floor" },
  { type: "fan", label: "风扇", labelEn: "Fan", defaultName: "风扇", defaultNameEn: "Fan" },
  { type: "plug", label: "插座", labelEn: "Socket", defaultName: "插座", defaultNameEn: "Socket" },
];

let _wid = 1;
function nextWid(prefix) {
  _wid += 1;
  return `${prefix}_${_wid}`;
}

function createWidget(type, overrides = {}) {
  const meta = WIDGET_CATALOG.find((c) => c.type === type) || WIDGET_CATALOG[0];
  const base = {
    id: overrides.id || nextWid(type),
    type,
    name: overrides.name || meta.defaultName,
    nameEn: overrides.nameEn || meta.defaultNameEn,
    enabled: overrides.enabled !== false,
  };
  if (type === "onoff" || type === "plug" || type === "fan") {
    return { ...base, on: overrides.on ?? false };
  }
  if (type === "dimmer") {
    return { ...base, on: overrides.on ?? true, level: overrides.level ?? 50 };
  }
  if (type === "curtain" || type === "shutter") {
    return { ...base, position: overrides.position ?? 50 };
  }
  if (type === "clim") {
    return {
      ...base,
      on: overrides.on ?? true,
      mode: overrides.mode || "cool",
      setpoint: overrides.setpoint ?? 24,
    };
  }
  if (type === "thermo") {
    return { ...base, on: overrides.on ?? true, setpoint: overrides.setpoint ?? 22 };
  }
  return base;
}

function makeInitialRoomWidgets() {
  return {
    living: [
      createWidget("onoff", { id: "liv_ceil", name: "主灯", nameEn: "Ceiling", on: true }),
      createWidget("dimmer", { id: "liv_strip", name: "灯带", nameEn: "Strip", level: 55, on: true }),
      createWidget("curtain", { id: "liv_cur", name: "窗帘", nameEn: "Curtain", position: 70 }),
      createWidget("shutter", { id: "liv_shut", name: "卷帘", nameEn: "Shutter", position: 30 }),
      createWidget("clim", { id: "liv_ac", name: "空调", nameEn: "AC", on: true, mode: "cool", setpoint: 24 }),
      createWidget("thermo", { id: "liv_heat", name: "地暖", nameEn: "Floor", on: true, setpoint: 23 }),
      createWidget("fan", { id: "liv_fan", name: "风扇", nameEn: "Fan", on: false }),
      createWidget("plug", { id: "liv_plug", name: "插座", nameEn: "Socket", on: true }),
    ],
    bed: [
      createWidget("onoff", { id: "bed_ceil", name: "床头灯", nameEn: "Bedside", on: false }),
      createWidget("dimmer", { id: "bed_strip", name: "夜灯", nameEn: "Night", level: 20, on: true }),
      createWidget("curtain", { id: "bed_cur", name: "窗帘", nameEn: "Curtain", position: 100 }),
      createWidget("thermo", { id: "bed_heat", name: "地暖", nameEn: "Floor", on: true, setpoint: 22 }),
      createWidget("clim", { id: "bed_ac", name: "空调", nameEn: "AC", on: false, mode: "off", setpoint: 26 }),
    ],
    kitchen: [
      createWidget("onoff", { id: "kit_ceil", name: "厨灯", nameEn: "Kitchen", on: true }),
      createWidget("dimmer", { id: "kit_strip", name: "台面灯", nameEn: "Counter", level: 80, on: true }),
      createWidget("plug", { id: "kit_plug", name: "插座", nameEn: "Socket", on: true }),
      createWidget("fan", { id: "kit_fan", name: "排风扇", nameEn: "Exhaust", on: false }),
      createWidget("curtain", { id: "kit_cur", name: "窗帘", nameEn: "Curtain", position: 40 }),
    ],
    study: [
      createWidget("onoff", { id: "stu_ceil", name: "顶灯", nameEn: "Ceiling", on: true }),
      createWidget("dimmer", { id: "stu_strip", name: "灯带", nameEn: "Strip", level: 40, on: true }),
      createWidget("clim", { id: "stu_ac", name: "空调", nameEn: "AC", on: true, mode: "cool", setpoint: 25 }),
      createWidget("curtain", { id: "stu_cur", name: "窗帘", nameEn: "Curtain", position: 60 }),
      createWidget("plug", { id: "stu_plug", name: "桌插", nameEn: "Desk", on: true }),
    ],
  };
}

function findType(widgets, type) {
  return (widgets || []).find((w) => w.type === type && w.enabled !== false);
}

/** Flatten widgets → legacy device fields (for themes / Ink matrix) */
function toLegacyDevice(widgets) {
  const onoff = findType(widgets, "onoff") || findType(widgets, "plug");
  const dimmer = findType(widgets, "dimmer");
  const curtain = findType(widgets, "curtain");
  const shutter = findType(widgets, "shutter");
  const clim = findType(widgets, "clim");
  const thermo = findType(widgets, "thermo");
  const fan = findType(widgets, "fan");
  return {
    ceiling: !!(onoff && onoff.on),
    strip: dimmer ? (dimmer.on ? dimmer.level : 0) : 0,
    curtain: curtain ? curtain.position : 0,
    shutter: shutter ? shutter.position : 0,
    acOn: !!(clim && clim.on && clim.mode !== "off"),
    acMode: clim ? (clim.on ? clim.mode : "off") : "off",
    acSp: clim ? clim.setpoint : 24,
    heatOn: !!(thermo && thermo.on),
    heatSp: thermo ? thermo.setpoint : 22,
    fanOn: !!(fan && fan.on),
  };
}

function devicesFromWidgets(roomWidgets) {
  const out = {};
  Object.keys(roomWidgets).forEach((id) => {
    out[id] = toLegacyDevice(roomWidgets[id]);
  });
  return out;
}

function mapLegacyPatch(widgets, patch) {
  let next = widgets.map((w) => ({ ...w }));
  const touch = (type, fn) => {
    const i = next.findIndex((w) => w.type === type && w.enabled !== false);
    if (i < 0) return;
    next[i] = fn({ ...next[i] });
  };
  if (Object.prototype.hasOwnProperty.call(patch, "ceiling")) {
    touch("onoff", (w) => ({ ...w, on: !!patch.ceiling }));
  }
  if (Object.prototype.hasOwnProperty.call(patch, "strip")) {
    touch("dimmer", (w) => ({
      ...w,
      level: patch.strip,
      on: patch.strip > 0,
    }));
  }
  if (Object.prototype.hasOwnProperty.call(patch, "curtain")) {
    touch("curtain", (w) => ({ ...w, position: patch.curtain }));
  }
  if (Object.prototype.hasOwnProperty.call(patch, "shutter")) {
    touch("shutter", (w) => ({ ...w, position: patch.shutter }));
  }
  if (Object.prototype.hasOwnProperty.call(patch, "acOn") || Object.prototype.hasOwnProperty.call(patch, "acMode") || Object.prototype.hasOwnProperty.call(patch, "acSp")) {
    touch("clim", (w) => ({
      ...w,
      on: patch.acOn !== undefined ? patch.acOn : (patch.acMode !== undefined ? patch.acMode !== "off" : w.on),
      mode: patch.acMode !== undefined ? patch.acMode : w.mode,
      setpoint: patch.acSp !== undefined ? patch.acSp : w.setpoint,
    }));
  }
  if (Object.prototype.hasOwnProperty.call(patch, "heatOn") || Object.prototype.hasOwnProperty.call(patch, "heatSp")) {
    touch("thermo", (w) => ({
      ...w,
      on: patch.heatOn !== undefined ? patch.heatOn : w.on,
      setpoint: patch.heatSp !== undefined ? patch.heatSp : w.setpoint,
    }));
  }
  if (Object.prototype.hasOwnProperty.call(patch, "fanOn")) {
    const fi = next.findIndex((w) => w.type === "fan");
    if (fi < 0) next.push(createWidget("fan", { on: !!patch.fanOn, enabled: true }));
    else next[fi] = { ...next[fi], on: !!patch.fanOn, enabled: true };
  }
  return next;
}

function estimatePowerWidgets(roomWidgets) {
  let w = 0.25;
  Object.values(roomWidgets).forEach((list) => {
    (list || []).forEach((d) => {
      if (!d.enabled) return;
      if ((d.type === "onoff" || d.type === "plug" || d.type === "fan") && d.on) w += 0.16;
      if (d.type === "dimmer" && d.on) w += (d.level || 0) * 0.004;
      if (d.type === "clim" && d.on && d.mode !== "off") w += 0.85;
      if (d.type === "thermo" && d.on) w += 0.55;
      if (d.type === "curtain" || d.type === "shutter") w += (d.position || 0) * 0.0004;
    });
  });
  return Math.round(Math.min(6.5, w) * 10) / 10;
}

function computeMetricsWidgets(roomWidgets) {
  let tempSum = 0;
  let n = 0;
  let active = 0;
  Object.values(roomWidgets).forEach((list) => {
    const d = toLegacyDevice(list);
    if (d.acOn) { tempSum += d.acSp; n += 1; }
    else if (d.heatOn) { tempSum += d.heatSp; n += 1; }
    if (d.ceiling || d.strip > 0 || d.acOn || d.heatOn || d.fanOn) active += 1;
  });
  const indoor = n ? tempSum / n : 24.0;
  const rh = Math.min(72, Math.max(35, 46 + active * 2 - (n > 2 ? 3 : 0)));
  return {
    indoor: Math.round(indoor * 10) / 10,
    rh: Math.round(rh),
    activeRooms: active,
    onlinePts: 36 + active * 3,
  };
}

function applySceneToWidgets(prev, sceneId) {
  const next = {};
  Object.keys(prev).forEach((id) => {
    next[id] = prev[id].map((w) => ({ ...w }));
  });
  const mapRoom = (id, fn) => { next[id] = fn(next[id].map((w) => ({ ...w }))); };
  const allOff = (list) => list.map((w) => {
    if (w.type === "onoff" || w.type === "plug" || w.type === "fan") return { ...w, on: false };
    if (w.type === "dimmer") return { ...w, on: false, level: 0 };
    if (w.type === "curtain" || w.type === "shutter") return { ...w, position: 0 };
    if (w.type === "clim") return { ...w, on: false, mode: "off" };
    if (w.type === "thermo") return { ...w, on: false };
    return w;
  });

  if (sceneId === "away" || sceneId === "alloff") {
    Object.keys(next).forEach((id) => { next[id] = allOff(next[id]); });
  } else if (sceneId === "home" || sceneId === "comfort") {
    mapRoom("living", (list) => mapLegacyPatch(list, {
      ceiling: true, strip: 60, curtain: 80, shutter: 40, acOn: true, acMode: "cool", acSp: 24, heatOn: false,
    }));
    mapRoom("bed", (list) => mapLegacyPatch(list, {
      ceiling: false, strip: 0, heatOn: true, heatSp: 22, acOn: false, acMode: "off",
    }));
    mapRoom("kitchen", (list) => mapLegacyPatch(list, { ceiling: true, strip: 40 }));
    mapRoom("study", (list) => mapLegacyPatch(list, { ceiling: true, strip: 35, acOn: false, acMode: "off" }));
  } else if (sceneId === "movie") {
    mapRoom("living", (list) => mapLegacyPatch(list, {
      ceiling: false, strip: 15, curtain: 0, shutter: 0, acOn: true, acMode: "cool", acSp: 25,
    }));
  } else if (sceneId === "sleep") {
    Object.keys(next).forEach((id) => {
      next[id] = mapLegacyPatch(allOff(next[id]), id === "bed"
        ? { strip: 8, heatOn: true, heatSp: 22 }
        : {});
      if (id === "bed") {
        next[id] = mapLegacyPatch(next[id], { strip: 8, heatOn: true, heatSp: 22, ceiling: false });
        // re-enable dimmer night light
        next[id] = next[id].map((w) => (w.type === "dimmer" ? { ...w, on: true, level: 8 } : w));
      }
    });
  } else if (sceneId === "eco") {
    Object.keys(next).forEach((id) => {
      next[id] = next[id].map((w) => {
        if (w.type === "dimmer") return { ...w, level: Math.min(w.level, 30) };
        if (w.type === "clim") return { ...w, setpoint: Math.min(w.setpoint + 1, 28) };
        if (w.type === "thermo") return { ...w, setpoint: Math.max(w.setpoint - 1, 18) };
        return w;
      });
    });
  } else if (sceneId === "guest") {
    mapRoom("living", (list) => mapLegacyPatch(list, {
      ceiling: true, strip: 70, curtain: 90, shutter: 60, acOn: true, acMode: "cool", acSp: 24,
    }));
    mapRoom("kitchen", (list) => mapLegacyPatch(list, { ceiling: true, strip: 50 }));
  }
  return next;
}

function roomWidgetsActive(widgets) {
  const d = toLegacyDevice(widgets || []);
  return d.ceiling || d.strip > 0 || d.acOn || d.heatOn || d.fanOn;
}

Object.assign(window, {
  WIDGET_CATALOG, createWidget, makeInitialRoomWidgets,
  toLegacyDevice, devicesFromWidgets, mapLegacyPatch,
  estimatePowerWidgets, computeMetricsWidgets, applySceneToWidgets, roomWidgetsActive,
});
