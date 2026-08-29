/* eslint-disable */
// Thermostat / HVAC widgets — OEM slate/teal kit (supports onChange for hub binding).
function roundBtn(on, hue, size = 48) {
  return {
    width: size, height: size, borderRadius: 8,
    border: "1px solid var(--line)",
    background: on ? window.A(hue, 0.14) : "var(--bg-card-2)",
    color: on ? hue : "var(--t4)",
    cursor: on ? "pointer" : "default",
    display: "flex", alignItems: "center", justifyContent: "center", flexShrink: 0,
  };
}

function ThermostatArc({
  name = "Heating", room = "", ambient = 20.5, setpoint: spProp = 22,
  min = 16, max = 28, on: onProp = true, dia = 200, compact = false, onChange,
}) {
  const { useState, useRef, useEffect } = React;
  const H = "#f59e0b";
  const [on, setOn] = useState(onProp);
  const [sp, setSp] = useState(spProp);
  useEffect(() => { setOn(onProp); }, [onProp]);
  useEffect(() => { setSp(spProp); }, [spProp]);
  const emit = (nextOn, nextSp) => onChange && onChange({ on: nextOn, setpoint: nextSp });
  const frac = (sp - min) / (max - min);
  const cx = 120, cy = 120, r = 96, sw = 12;
  const C = 2 * Math.PI * r;
  const arcLen = C * 0.75;
  const startDeg = 135;
  const ang = (startDeg + 270 * frac) * (Math.PI / 180);
  const knobX = cx + r * Math.cos(ang);
  const knobY = cy + r * Math.sin(ang);
  const step = (d) => {
    const n = Math.max(min, Math.min(max, sp + d));
    setSp(n);
    emit(on, n);
  };
  const toggle = () => {
    const n = !on;
    setOn(n);
    emit(n, sp);
  };

  if (compact) {
    return (
      <div className="w w-compact" data-on={on ? "1" : "0"} style={{ height: "100%", justifyContent: "space-between", gap: 4 }}>
        <div style={{ display: "flex", alignItems: "center", gap: 8, flexShrink: 0 }}>
          <div className="badge" style={{ width: 30, height: 30, background: window.A(H, 0.14), color: on ? H : "var(--t3)" }}>
            <LIco.heat width="16" height="16" />
          </div>
          <div style={{ flex: 1, minWidth: 0 }}>
            <div className="w-title" style={{ fontSize: 13, fontWeight: 700 }}>{name}</div>
            <div className="w-sub" style={{ fontSize: 10, color: "var(--t3)" }}>Indoor {ambient}°</div>
          </div>
          <button type="button" className="pwr" onClick={toggle} style={{
            width: 28, height: 28, background: on ? H : "var(--bg-card-2)",
            color: on ? "var(--ink-on-accent)" : "var(--t3)", border: on ? "none" : "1px solid var(--line)",
          }}><LIco.power width="14" height="14" /></button>
        </div>
        <div style={{ display: "flex", alignItems: "center", justifyContent: "space-between", flexShrink: 0 }}>
          <button type="button" onClick={() => step(-0.5)} disabled={!on} style={roundBtn(on, H, 32)}><LIco.minus width="16" height="16" /></button>
          <div style={{ fontSize: 28, fontWeight: 300, color: on ? H : "var(--t4)", fontFamily: "var(--font-d)" }}>{sp}°</div>
          <button type="button" onClick={() => step(0.5)} disabled={!on} style={roundBtn(on, H, 32)}><LIco.plus width="16" height="16" /></button>
        </div>
      </div>
    );
  }

  const svgRef = useRef(null);
  const setFromPoint = (clientX, clientY) => {
    const rect = svgRef.current.getBoundingClientRect();
    const px = ((clientX - rect.left) / rect.width) * 240 - cx;
    const py = ((clientY - rect.top) / rect.height) * 240 - cy;
    let deg = Math.atan2(py, px) * (180 / Math.PI);
    let rel = deg - startDeg;
    while (rel < 0) rel += 360;
    if (rel > 270) rel = rel > 315 ? 0 : 270;
    const n = Math.round(min + (rel / 270) * (max - min));
    setSp(n);
    emit(on, n);
  };
  const onDown = (e) => {
    if (!on) return;
    e.stopPropagation();
    setFromPoint(e.clientX, e.clientY);
    const mv = (ev) => setFromPoint(ev.clientX, ev.clientY);
    const up = () => { document.removeEventListener("pointermove", mv); document.removeEventListener("pointerup", up); };
    document.addEventListener("pointermove", mv);
    document.addEventListener("pointerup", up);
  };

  return (
    <div className="w" data-on={on ? "1" : "0"} style={{ height: "100%", alignItems: "center" }}>
      <div style={{ width: "100%", display: "flex", justifyContent: "space-between", alignItems: "center" }}>
        <div>
          <div style={{ fontSize: 16, fontWeight: 700 }}>{name}</div>
          <div style={{ fontSize: 12, color: "var(--t3)" }}>{room || "Zone"}</div>
        </div>
        <button type="button" className="pwr" onClick={toggle} style={{
          width: 38, height: 38, background: on ? H : "var(--bg-card-2)",
          color: on ? "var(--ink-on-accent)" : "var(--t3)", border: on ? "none" : "1px solid var(--line)",
        }}><LIco.power width="17" height="17" /></button>
      </div>
      <div style={{ flex: 1, display: "flex", alignItems: "center", justifyContent: "center" }}>
        <div style={{ position: "relative", width: dia, height: dia }}>
          <svg ref={svgRef} viewBox="0 0 240 240" width={dia} height={dia} onPointerDown={onDown} style={{ touchAction: "none", cursor: on ? "pointer" : "default" }}>
            <circle cx={cx} cy={cy} r={r} fill="none" stroke="rgba(255,255,255,0.08)" strokeWidth={sw}
              strokeLinecap="butt" strokeDasharray={`${arcLen} ${C}`} transform={`rotate(${startDeg} ${cx} ${cy})`} />
            <circle cx={cx} cy={cy} r={r} fill="none" stroke={on ? H : "var(--t4)"} strokeWidth={sw}
              strokeLinecap="butt" strokeDasharray={`${arcLen * frac} ${C}`} transform={`rotate(${startDeg} ${cx} ${cy})`} />
            {on && <rect x={knobX - 7} y={knobY - 7} width="14" height="14" rx="2" fill="#fff" stroke={H} strokeWidth="2" />}
          </svg>
          <div style={{ position: "absolute", inset: 0, display: "flex", flexDirection: "column", alignItems: "center", justifyContent: "center" }}>
            <div className="eyebrow">Setpoint</div>
            <div style={{ fontSize: 48, fontWeight: 300, color: on ? H : "var(--t4)", fontFamily: "var(--font-d)", lineHeight: 1 }}>{sp}°</div>
            <div style={{ fontSize: 12, color: "var(--t3)", marginTop: 6 }}>Indoor {ambient}°</div>
          </div>
        </div>
      </div>
      <div style={{ display: "flex", gap: 16, alignItems: "center" }}>
        <button type="button" onClick={() => step(-0.5)} disabled={!on} style={roundBtn(on, H)}><LIco.minus width="20" height="20" /></button>
        <div style={{ fontSize: 12, color: "var(--t3)" }}>{min}° – {max}°</div>
        <button type="button" onClick={() => step(0.5)} disabled={!on} style={roundBtn(on, H)}><LIco.plus width="20" height="20" /></button>
      </div>
    </div>
  );
}

function ClimWidget({
  name = "AC", room = "", ambient = 25, setpoint: spProp = 22,
  min = 16, max = 30, mode: modeProp = "cool", compact = false, wide = false, onChange,
}) {
  const { useState, useEffect } = React;
  const MODES = [
    { id: "off", l: "Off", hue: "#7d8792", icon: LIco.power },
    { id: "cool", l: "Cool", hue: "#22d3ee", icon: LIco.snow },
    { id: "heat", l: "Heat", hue: "#f59e0b", icon: LIco.flame },
    { id: "auto", l: "Auto", hue: "#818cf8", icon: LIco.auto },
  ];
  const [mode, setMode] = useState(modeProp);
  const [sp, setSp] = useState(spProp);
  useEffect(() => { setMode(modeProp); }, [modeProp]);
  useEffect(() => { setSp(spProp); }, [spProp]);
  const cur = MODES.find((m) => m.id === mode) || MODES[0];
  const on = mode !== "off";
  const H = cur.hue;
  const emit = (m, s) => onChange && onChange({ mode: m, setpoint: s });
  const step = (d) => {
    const n = Math.max(min, Math.min(max, sp + d));
    setSp(n);
    emit(mode, n);
  };
  const pickMode = (m) => {
    setMode(m);
    emit(m, sp);
  };

  const ModeBar = ({ small, iconsOnly }) => (
    <div style={{ display: "grid", gridTemplateColumns: "repeat(4,1fr)", gap: 3, background: "var(--bg-card-2)", borderRadius: 8, padding: 2, flexShrink: 0 }}>
      {MODES.map((m) => {
        const active = m.id === mode;
        const Ic = m.icon;
        return (
          <button key={m.id} type="button" onClick={(e) => { e.stopPropagation(); pickMode(m.id); }} style={{
            border: "none", cursor: "pointer", borderRadius: 6, padding: iconsOnly ? "6px 0" : small ? "5px 0" : "8px 0",
            background: active ? window.A(m.hue, 0.18) : "transparent",
            color: active ? m.hue : "var(--t3)",
            display: "flex", flexDirection: "column", alignItems: "center", gap: 1,
            fontSize: small ? 9 : 10, fontWeight: 700,
          }}>
            <Ic width={small || iconsOnly ? 14 : 16} height={small || iconsOnly ? 14 : 16} />
            {!iconsOnly && <span>{m.l}</span>}
          </button>
        );
      })}
    </div>
  );

  if (compact) {
    return (
      <div className={"w w-compact"} data-on={on ? "1" : "0"} style={{ height: "100%", justifyContent: wide ? "center" : "space-between", gap: wide ? 8 : 4 }}>
        <div style={{ display: "flex", alignItems: "center", gap: 8, flexShrink: 0 }}>
          <div className="badge" style={{ width: 30, height: 30, background: window.A(H, 0.14), color: on ? H : "var(--t3)" }}>
            <LIco.snow width="16" height="16" />
          </div>
          <div style={{ flex: 1, minWidth: 0 }}>
            <div className="w-title" style={{ fontSize: wide ? 15 : 13, fontWeight: 700 }}>{name}</div>
            <div className="w-sub" style={{ fontSize: 10, color: "var(--t3)" }}>{on ? `${cur.l} · ${ambient}°` : "Off"}</div>
          </div>
          {wide && (
            <div style={{ display: "flex", alignItems: "center", gap: 6 }}>
              <button type="button" onClick={() => step(-0.5)} disabled={!on} style={roundBtn(on, H, 32)}><LIco.minus width="14" height="14" /></button>
              <div style={{ fontSize: 26, fontWeight: 300, color: on ? H : "var(--t4)", minWidth: 44, textAlign: "center" }}>{on ? sp : "--"}</div>
              <button type="button" onClick={() => step(0.5)} disabled={!on} style={roundBtn(on, H, 32)}><LIco.plus width="14" height="14" /></button>
            </div>
          )}
          {!wide && <div style={{ fontSize: 22, fontWeight: 300, color: on ? H : "var(--t4)", flexShrink: 0 }}>{on ? sp : "--"}°</div>}
        </div>
        <ModeBar small={!wide} iconsOnly={!wide} />
      </div>
    );
  }

  return (
    <div className="w" data-on={on ? "1" : "0"} style={{ height: "100%", gap: 12 }}>
      <div>
        <div style={{ fontSize: 17, fontWeight: 700 }}>{name}</div>
        <div style={{ fontSize: 12, color: "var(--t3)" }}>{room || "Zone"} · indoor {ambient}°</div>
      </div>
      <div style={{ flex: 1, display: "flex", alignItems: "center", justifyContent: "center", gap: 18 }}>
        <button type="button" onClick={() => step(-0.5)} disabled={!on} style={roundBtn(on, H, 52)}><LIco.minus width="22" height="22" /></button>
        <div style={{ fontSize: 64, fontWeight: 300, color: on ? H : "var(--t4)", fontFamily: "var(--font-d)", lineHeight: 1 }}>{on ? sp : "--"}°</div>
        <button type="button" onClick={() => step(0.5)} disabled={!on} style={roundBtn(on, H, 52)}><LIco.plus width="22" height="22" /></button>
      </div>
      <ModeBar />
    </div>
  );
}

window.ThermostatArc = ThermostatArc;
window.ClimWidget = ClimWidget;
