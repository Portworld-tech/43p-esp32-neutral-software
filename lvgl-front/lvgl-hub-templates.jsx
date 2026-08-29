/* eslint-disable */
// Neutral smart-home hub templates — protocol-agnostic OEM screens.
const { useState: useStateH, createContext, useContext } = React;

/** Shared hub context — Provider lives in lvgl-hub-app.jsx */
const HubCtx = createContext(null);
function useHub() {
  return useContext(HubCtx);
}

function HubShell({ title, clock = "15:30", right, onBack, dots = 0, active = 0, children }) {
  return (
    <div className="lv">
      <div className="lv-head">
        {onBack
          ? <button type="button" className="sf-back" onClick={onBack}><LIco.back width="18" height="18" /></button>
          : <div className="clock">{clock}</div>}
        <div className="room">{title}</div>
        <div className="wx" style={{ minWidth: 56, justifyContent: "flex-end" }}>{right || null}</div>
      </div>
      <div className="lv-body" style={{ display: "flex", flexDirection: "column", minHeight: 0 }}>{children}</div>
      {dots > 0 && (
        <div className="lv-dots">
          {Array.from({ length: dots }, (_, i) => (
            <div key={i} className={"lv-dot" + (i === active ? " on" : "")} />
          ))}
        </div>
      )}
    </div>
  );
}

/**
 * Flex-safe scroll region — no scrollbar gutter (won't shift layout).
 * Use for long lists inside HubShell / 480×480 screens.
 */
function ScrollPane({ children, className = "", style, asGrid = false }) {
  return (
    <div
      className={"scroll-pane no-sb" + (asGrid ? " scroll-pane--grid" : "") + (className ? " " + className : "")}
      data-no-swipe
      style={style}
    >
      {children}
    </div>
  );
}

function ProtoPill({ label, ok = true }) {
  return (
    <span style={{
      display: "inline-flex", alignItems: "center", gap: 5, padding: "3px 8px",
      borderRadius: 4, fontSize: 11, fontWeight: 700,
      background: ok ? "rgba(52,211,153,0.12)" : "rgba(244,63,94,0.12)",
      color: ok ? "var(--on)" : "var(--alert)",
      border: `1px solid ${ok ? "rgba(52,211,153,0.35)" : "rgba(244,63,94,0.35)"}`,
    }}>
      <span style={{ width: 5, height: 5, borderRadius: 1, background: "currentColor" }} />
      {label}
    </span>
  );
}

function StatChip({ icon, label, value, hue = "var(--accent)" }) {
  return (
    <div style={{
      flex: 1, minWidth: 0, borderRadius: 12, padding: "10px 12px",
      background: "var(--bg-card)", border: "1px solid var(--line)",
    }}>
      <div style={{ display: "flex", alignItems: "center", gap: 6, color: hue, marginBottom: 6 }}>
        {icon}
        <span style={{ fontSize: 10, color: "var(--t3)", fontWeight: 700 }}>{label}</span>
      </div>
      <div style={{ fontSize: 20, fontWeight: 600 }}>{value}</div>
    </div>
  );
}

function RoomTile({ name, devices, on, hue = "#2dd4bf", onClick }) {
  return (
    <button type="button" onClick={onClick} style={{
      border: "1px solid var(--line)",
      borderLeft: `3px solid ${on ? hue : "transparent"}`,
      background: "var(--bg-card)", borderRadius: 12, padding: 12, textAlign: "left",
      color: "var(--t1)", cursor: "pointer", display: "flex", flexDirection: "column", gap: 8, minHeight: 100,
    }}>
      <div className="badge" style={{ width: 36, height: 36, background: window.A(hue, 0.14), color: hue }}>
        <LIco.home width="18" height="18" />
      </div>
      <div>
        <div style={{ fontSize: 14, fontWeight: 700 }}>{name}</div>
        <div style={{ fontSize: 11, color: "var(--t3)", marginTop: 2 }}>{devices} pts · {on ? "Active" : "Idle"}</div>
      </div>
    </button>
  );
}

function SceneBtn({ title, sub, hue, icon, onClick }) {
  return (
    <button type="button" onClick={onClick} style={{
      border: "1px solid var(--line)", background: "var(--bg-card)", borderRadius: 12,
      padding: "12px", color: "var(--t1)", cursor: "pointer", textAlign: "left",
      display: "flex", flexDirection: "column", gap: 8, minHeight: 88,
    }}>
      <div className="badge" style={{ width: 36, height: 36, background: window.A(hue, 0.14), color: hue }}>{icon}</div>
      <div>
        <div style={{ fontSize: 13, fontWeight: 700 }}>{title}</div>
        <div style={{ fontSize: 11, color: "var(--t3)" }}>{sub}</div>
      </div>
    </button>
  );
}

const barBtn = {
  flex: 1, height: 44, borderRadius: 8, border: "1px solid var(--line)",
  background: "var(--bg-card)", color: "var(--t1)", fontWeight: 700, cursor: "pointer",
};

function TplHubHome({ onRooms, onGateway, onScenes, onEnergy }) {
  const hub = useHub();
  const openRoom = (id) => {
    if (hub) hub.openRoom(id);
    else if (onRooms) onRooms(id);
  };
  const go = (r) => {
    if (hub) hub.go(r);
    else if (r === "gateway" && onGateway) onGateway();
    else if (r === "scenes" && onScenes) onScenes();
    else if (r === "energy" && onEnergy) onEnergy();
  };
  const protos = hub ? hub.protocols : [
    { id: "mqtt", name: "MQTT", ok: true },
    { id: "modbus", name: "Modbus TCP", ok: true },
    { id: "rs485", name: "RS485", ok: false },
  ];
  const power = hub ? `${hub.powerKw.toFixed(1)} kW` : "2.4 kW";
  return (
    <HubShell title="Home Hub" right={<LIco.wifi width="18" height="18" />} dots={4} active={0}>
      <div style={{ display: "flex", gap: 6, flexWrap: "wrap", marginBottom: 10 }}>
        {protos.slice(0, 3).map((p) => (
          <button key={p.id || p.name} type="button" onClick={() => go("gateway")} style={{ background: "none", border: "none", padding: 0, cursor: "pointer" }}>
            <ProtoPill label={p.name.replace(" Broker", "").replace(" Bus A", "")} ok={p.ok} />
          </button>
        ))}
      </div>
      <div style={{ display: "flex", gap: 8, marginBottom: 12 }}>
        <div style={{ flex: 1, cursor: "pointer" }} onClick={() => go("energy")}><StatChip icon={<LIco.gauge width="14" height="14" />} label="Power" value={power} hue="var(--warm)" /></div>
        <StatChip icon={<LIco.snow width="14" height="14" />} label="Indoor" value="24.5°" hue="var(--cool)" />
        <StatChip icon={<LIco.drop width="14" height="14" />} label="RH" value="48%" hue="var(--violet)" />
      </div>
      <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 8, flex: 1, alignContent: "start" }}>
        {(hub ? hub.rooms : [
          { id: "living", name: "Living", devices: 8 },
          { id: "bed", name: "Bedroom", devices: 5 },
          { id: "kitchen", name: "Kitchen", devices: 4 },
        ]).slice(0, 3).map((r, i) => (
          <RoomTile
            key={r.id || r.name}
            name={r.name}
            devices={r.devices}
            on={hub ? hub.roomActive(r.id) : i !== 1}
            hue={["#2dd4bf", "#818cf8", "#eab308"][i]}
            onClick={() => openRoom(r.id)}
          />
        ))}
        <button type="button" onClick={() => go("gateway")} style={{
          border: "1px dashed var(--line-2)", borderRadius: 12, background: "transparent",
          color: "var(--t2)", cursor: "pointer", fontWeight: 600, fontSize: 13,
        }}>
          <div style={{ display: "flex", flexDirection: "column", alignItems: "center", gap: 6 }}>
            <LIco.bus width="22" height="22" />Protocols
          </div>
        </button>
      </div>
      <div style={{ display: "flex", gap: 8, marginTop: 10 }}>
        <button type="button" onClick={() => go("scenes")} style={barBtn}>Scenes</button>
        <button type="button" onClick={() => go("energy")} style={barBtn}>Energy</button>
      </div>
    </HubShell>
  );
}

function TplRoomControl({ onBack }) {
  const hub = useHub();
  if (hub) return <HubRoomPage />;
  return (
    <PageShell title="Living" dots={4} active={1} onBack={onBack}>
      <Six>
        <OnOffWidget name="Ceiling" icon={<LIco.bulb />} on={true} compact />
        <DimmerWidget name="Strip" level={55} compact />
        <CurtainWidget name="Curtain" position={70} compact />
        <ShutterWidget name="Shutter" position={30} compact />
        <ClimWidget name="AC" ambient={25} setpoint={24} mode="cool" compact />
        <ThermostatArc name="Floor" ambient={22} setpoint={23} compact />
      </Six>
    </PageShell>
  );
}

function TplGateway({ onBack }) {
  const hub = useHub();
  if (hub) return <HubGatewayPage />;
  const rows = [
    { name: "MQTT Broker", detail: "mqtt://192.168.1.8:1883", ok: true, icon: <LIco.link width="16" height="16" /> },
    { name: "Modbus TCP", detail: "192.168.1.20:502 · 12 slaves", ok: true, icon: <LIco.layers width="16" height="16" /> },
    { name: "RS485 Bus A", detail: "9600 8N1 · timeout", ok: false, icon: <LIco.bus width="16" height="16" /> },
    { name: "Wi-Fi STA", detail: "Office-AP · -58 dBm", ok: true, icon: <LIco.wifi width="16" height="16" /> },
  ];
  return (
    <HubShell title="Gateway" onBack={onBack} right={<span style={{ fontSize: 11, color: "var(--t3)" }}>3/4</span>}>
      <div style={{ display: "grid", gap: 8, flex: 1, alignContent: "start" }}>
        {rows.map((r) => (
          <div key={r.name} style={{
            display: "flex", alignItems: "center", gap: 10, padding: "10px 12px",
            borderRadius: 12, background: "var(--bg-card)", border: "1px solid var(--line)",
          }}>
            <div className="badge" style={{
              width: 36, height: 36,
              background: r.ok ? "rgba(52,211,153,0.12)" : "rgba(244,63,94,0.12)",
              color: r.ok ? "var(--on)" : "var(--alert)",
            }}>{r.icon}</div>
            <div style={{ flex: 1, minWidth: 0 }}>
              <div style={{ fontSize: 14, fontWeight: 700 }}>{r.name}</div>
              <div style={{ fontSize: 11, color: "var(--t3)", overflow: "hidden", textOverflow: "ellipsis", whiteSpace: "nowrap" }}>{r.detail}</div>
            </div>
            <ProtoPill label={r.ok ? "OK" : "Fault"} ok={r.ok} />
          </div>
        ))}
      </div>
      <div style={{ fontSize: 11, color: "var(--t4)", marginTop: 8 }}>Bind drivers in firmware / point table — UI stays protocol-agnostic.</div>
    </HubShell>
  );
}

function TplScenes({ onBack }) {
  const hub = useHub();
  if (hub) return <HubScenesPage />;
  const scenes = [
    { id: "home", title: "Home", sub: "Lights + climate", hue: "#34d399", icon: <LIco.home width="18" height="18" /> },
    { id: "away", title: "Away", sub: "Arm + all off", hue: "#f59e0b", icon: <LIco.away width="18" height="18" /> },
    { id: "movie", title: "Movie", sub: "Dim + curtain", hue: "#818cf8", icon: <LIco.moon width="18" height="18" /> },
    { id: "sleep", title: "Sleep", sub: "Night mode", hue: "#2dd4bf", icon: <LIco.moonSleep width="18" height="18" /> },
    { id: "guest", title: "Guest", sub: "Living + bath", hue: "#eab308", icon: <LIco.bulb width="18" height="18" /> },
    { id: "eco", title: "Eco", sub: "Load shed", hue: "#22d3ee", icon: <LIco.gauge width="18" height="18" /> },
  ];
  return (
    <HubShell title="Scenes" onBack={onBack} dots={4} active={2}>
      <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 8, flex: 1, alignContent: "start" }}>
        {scenes.map((s) => <SceneBtn key={s.id} {...s} />)}
      </div>
    </HubShell>
  );
}

function TplEnergy({ onBack }) {
  const hub = useHub();
  const back = () => (hub ? hub.go("home") : onBack && onBack());
  const peak = hub ? hub.powerKw.toFixed(1) : "2.4";
  const today = hub ? (12 + hub.powerKw * 2.1).toFixed(1) : "18.6";
  const indoor = hub ? hub.metrics.indoor : 24.5;
  const rh = hub ? hub.metrics.rh : 48;
  const bars = [
    { label: "Lighting", pct: 28, hue: "#eab308" },
    { label: "HVAC", pct: 46, hue: "#22d3ee" },
    { label: "Sockets", pct: 18, hue: "#818cf8" },
    { label: "Other", pct: 8, hue: "#7d8792" },
  ];
  return (
    <HubShell title="Energy" onBack={back} dots={4} active={3}>
      <div style={{ borderRadius: 12, padding: 14, marginBottom: 12, background: "var(--bg-card)", border: "1px solid var(--line)" }}>
        <div className="eyebrow">Today</div>
        <div style={{ display: "flex", alignItems: "baseline", gap: 8, marginTop: 4 }}>
          <span style={{ fontSize: 40, fontWeight: 300 }}>{today}</span>
          <span style={{ color: "var(--t3)", fontWeight: 600 }}>kWh</span>
        </div>
        <div style={{ fontSize: 11, color: "var(--t3)" }}>
          Peak {peak} kW · 室内 {indoor}° · RH {rh}% · Modbus #1
        </div>
      </div>
      <div style={{ display: "grid", gap: 10, flex: 1 }}>
        {bars.map((b) => (
          <div key={b.label}>
            <div style={{ display: "flex", justifyContent: "space-between", fontSize: 12, marginBottom: 4 }}>
              <span style={{ fontWeight: 600 }}>{b.label}</span>
              <span style={{ color: "var(--t3)" }}>{b.pct}%</span>
            </div>
            <div style={{ height: 8, borderRadius: 2, background: "rgba(255,255,255,0.08)" }}>
              <div style={{ width: `${b.pct}%`, height: "100%", borderRadius: 2, background: b.hue }} />
            </div>
          </div>
        ))}
      </div>
      {hub && (
        <button type="button" onClick={() => hub.go("gateway")} style={{ ...barBtn, marginTop: 10, width: "100%" }}>
          查看计量总线 →
        </button>
      )}
    </HubShell>
  );
}

function TplPointTable({ onBack }) {
  const hub = useHub();
  const back = () => (hub ? hub.go("gateway") : onBack && onBack());
  const points = [
    { name: "LR Ceiling", proto: "Modbus", addr: "1 / 0x0001", type: "Coil" },
    { name: "LR Dimmer", proto: "Modbus", addr: "1 / 0x000A", type: "Hold" },
    { name: "AC Set", proto: "MQTT", addr: "home/lr/ac/set", type: "Num" },
    { name: "Curtain", proto: "RS485", addr: "BusA · ID3", type: "Hold" },
    { name: "Door", proto: "MQTT", addr: "home/entry/door", type: "Bool" },
    { name: "Floor heat", proto: "Modbus", addr: "2 / 0x0100", type: "Hold" },
  ];
  return (
    <HubShell title="Points" onBack={back} right={<span style={{ fontSize: 11, color: "var(--t3)" }}>{points.length}</span>}>
      <ScrollPane asGrid style={{ gap: 6, gridTemplateColumns: "1fr" }}>
        {points.map((p) => (
          <div key={p.name} style={{
            display: "grid", gridTemplateColumns: "1.1fr 0.7fr 1.3fr 0.5fr", gap: 4,
            alignItems: "center", padding: "8px 10px", borderRadius: 8,
            background: "var(--bg-card)", border: "1px solid var(--line)", fontSize: 10,
          }}>
            <div style={{ fontWeight: 700, fontSize: 12, overflow: "hidden", textOverflow: "ellipsis", whiteSpace: "nowrap" }}>{p.name}</div>
            <div style={{ color: "var(--accent)", fontWeight: 700 }}>{p.proto}</div>
            <div style={{ color: "var(--t3)", fontFamily: "var(--font-m)", overflow: "hidden", textOverflow: "ellipsis", whiteSpace: "nowrap" }}>{p.addr}</div>
            <div style={{ color: "var(--t2)", textAlign: "right" }}>{p.type}</div>
          </div>
        ))}
      </ScrollPane>
    </HubShell>
  );
}

function TplSecurity({ onBack, onKeypad }) {
  const hub = useHub();
  if (hub) return <HubSecurityPage />;
  return (
    <HubShell title="Security" onBack={onBack}>
      <div style={{ flex: 1, minHeight: 0 }}><AlarmWidget state="armed" mode="Away" onOpen={onKeypad} page /></div>
      <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 8, marginTop: 10 }}>
        <div style={secCard}><LIco.cam width="18" height="18" /><span>Cams OK</span></div>
        <div style={secCard}><LIco.lock width="18" height="18" /><span>Doors lock</span></div>
      </div>
    </HubShell>
  );
}
const secCard = {
  display: "flex", alignItems: "center", gap: 8, padding: "10px 12px",
  borderRadius: 10, background: "var(--bg-card)", border: "1px solid var(--line)",
  color: "var(--t2)", fontSize: 12, fontWeight: 600,
};

function TplHvacFocus() {
  const hub = useHub();
  const d = hub ? hub.devices.living : { acSp: 24, acMode: "cool", acOn: true, heatSp: 23, heatOn: true };
  const set = (patch) => hub && hub.patchRoom("living", patch);
  return (
    <PageShell title="Climate" dots={3} active={0} onBack={hub ? () => hub.go("home") : undefined}>
      <TwoV>
        <ClimWidget
          name="AC" room="Living" ambient={25}
          setpoint={d.acSp} mode={d.acOn ? d.acMode : "off"} compact wide
          onChange={hub ? ({ mode, setpoint }) => set({
            acMode: mode, acOn: mode !== "off", acSp: setpoint ?? d.acSp,
          }) : undefined}
        />
        <ThermostatArc
          name="Floor heat" room="Living" ambient={22.5}
          setpoint={d.heatSp} on={d.heatOn} compact
          onChange={hub ? ({ on, setpoint }) => set({
            heatOn: on ?? d.heatOn, heatSp: setpoint ?? d.heatSp,
          }) : undefined}
        />
      </TwoV>
    </PageShell>
  );
}

function TplMinimalHome({ onScene }) {
  const hub = useHub();
  const fire = (id) => {
    if (hub) {
      if (id === "bus" || id === "protocols") hub.go("gateway");
      else if (id === "alloff") hub.applyScene("alloff");
      else hub.applyScene(id);
      return;
    }
    onScene && onScene(id);
  };
  const wake = () => hub && hub.go("home");
  return (
    <div className="lv lv-solid" onClick={wake} style={{ cursor: "pointer" }}>
      <div style={{ padding: "36px 24px 0" }} onClick={wake}>
        <div style={{ fontFamily: "var(--font-clock)", fontWeight: 300, fontSize: 56, letterSpacing: "-0.03em", whiteSpace: "nowrap" }}>
          {hub ? hub.clock : "15:30"}
        </div>
        <div style={{ fontSize: 14, color: "var(--t3)", marginTop: 8 }}>
          {hub ? `低功耗待机 · ${hub.sceneLabel(hub.activeScene)}` : "Low-power standby"}
        </div>
      </div>
      <div style={{ flex: 1 }} />
      <div
        style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 8, padding: "0 18px 22px" }}
        onClick={(e) => e.stopPropagation()}
      >
        <SceneBtn title="全部关闭" sub="广播" hue="#7d8792" icon={<LIco.power width="18" height="18" />} onClick={() => fire("alloff")} />
        <SceneBtn title="舒适" sub="空调+灯" hue="#22d3ee" icon={<LIco.home width="18" height="18" />} onClick={() => fire("home")} />
        <SceneBtn title="离家" sub="安防" hue="#f59e0b" icon={<LIco.shieldFill width="18" height="18" />} onClick={() => fire("away")} />
        <SceneBtn title="总线" sub="协议" hue="#2dd4bf" icon={<LIco.bus width="18" height="18" />} onClick={() => fire("bus")} />
      </div>
    </div>
  );
}

function TplApplianceGrid() {
  const hub = useHub();
  const init = [
    { name: "Socket A", on: true, hue: "#eab308" },
    { name: "Socket B", on: false, hue: "#eab308" },
    { name: "Pump", on: true, hue: "#22d3ee" },
    { name: "Boiler", on: false, hue: "#f59e0b" },
    { name: "Valve 1", on: true, hue: "#818cf8" },
    { name: "Valve 2", on: true, hue: "#818cf8" },
    { name: "Fan coil", on: false, hue: "#34d399" },
    { name: "Garden", on: false, hue: "#34d399" },
  ];
  const [items, setItems] = useStateH(init);
  const toggle = (i) => {
    setItems((arr) => arr.map((x, n) => (n === i ? { ...x, on: !x.on } : x)));
    hub && hub.flash("已切换负载");
  };
  return (
    <HubShell
      title="Appliances"
      onBack={hub ? () => hub.go("home") : undefined}
      right={<span style={{ fontSize: 11, color: "var(--t3)" }}>RS485</span>}
      dots={3}
      active={1}
    >
      <ScrollPane asGrid style={{ gridTemplateColumns: "1fr 1fr" }}>
        {items.map((it, i) => (
          <button key={it.name} type="button" onClick={() => toggle(i)} style={{
            minHeight: 72, borderRadius: 10, padding: "10px 12px", cursor: "pointer", textAlign: "left",
            border: "1px solid var(--line)", borderLeft: `3px solid ${it.on ? it.hue : "transparent"}`,
            background: "var(--bg-card)", color: "var(--t1)",
            display: "flex", alignItems: "center", gap: 10,
          }}>
            <div className="badge" style={{ width: 32, height: 32, background: window.A(it.hue, 0.14), color: it.hue }}>
              <LIco.plug2 width="16" height="16" />
            </div>
            <div>
              <div style={{ fontSize: 13, fontWeight: 700 }}>{it.name}</div>
              <div style={{ fontSize: 11, color: it.on ? it.hue : "var(--t3)", fontWeight: 600 }}>{it.on ? "ON" : "OFF"}</div>
            </div>
          </button>
        ))}
      </ScrollPane>
    </HubShell>
  );
}

Object.assign(window, {
  HubCtx, useHub,
  HubShell, ScrollPane, ProtoPill, StatChip, RoomTile, SceneBtn,
  TplHubHome, TplRoomControl, TplGateway, TplScenes, TplEnergy,
  TplPointTable, TplSecurity, TplHvacFocus, TplMinimalHome, TplApplianceGrid,
});
