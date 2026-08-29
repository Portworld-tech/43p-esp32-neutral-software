/* eslint-disable */
// Page chrome + layout grids + PIN keypad (OEM kit).
const { useState: useStateP } = React;

function PageShell({ title = "Room", clock = "15:30", wx = "", wxIcon, dots = 3, active = 0, onBack, children }) {
  return (
    <div className="lv">
      <div className="lv-head">
        {onBack
          ? <button type="button" onClick={onBack} className="sf-back"><LIco.back width="18" height="18" /></button>
          : <div className="clock">{clock}</div>}
        <div className="room">{title}</div>
        <div className="wx">{wxIcon}{wx ? <span>{wx}</span> : null}</div>
      </div>
      <div className="lv-body">{children}</div>
      <div className="lv-dots">
        {Array.from({ length: dots }, (_, i) => (
          <div key={i} className={"lv-dot" + (i === active ? " on" : "")} />
        ))}
      </div>
    </div>
  );
}

const gridBase = {
  display: "grid",
  gap: 8,
  flex: 1,
  minHeight: 0,
  width: "100%",
  alignSelf: "stretch",
};
function TwoV({ children }) { return <div style={{ ...gridBase, gridTemplateRows: "1fr 1fr" }}>{wrapCells(children)}</div>; }
function TwoH({ children }) { return <div style={{ ...gridBase, gridTemplateColumns: "1fr 1fr" }}>{wrapCells(children)}</div>; }
function ThreeV({ children }) { return <div style={{ ...gridBase, gridTemplateRows: "1fr 1fr 1fr" }}>{wrapCells(children)}</div>; }
function ThreeH({ children }) { return <div style={{ ...gridBase, gridTemplateColumns: "1fr 1fr 1fr" }}>{wrapCells(children)}</div>; }
function Four({ children }) {
  return <div style={{ ...gridBase, gridTemplateColumns: "1fr 1fr", gridTemplateRows: "1fr 1fr" }}>{wrapCells(children)}</div>;
}
function Six({ children }) { return <div style={{ ...gridBase, gridTemplateColumns: "1fr 1fr", gridTemplateRows: "1fr 1fr 1fr" }}>{wrapCells(children)}</div>; }
function Five({ children }) {
  const kids = React.Children.toArray(children);
  return (
    <div style={{ ...gridBase, gridTemplateColumns: "1fr 1fr", gridTemplateRows: "1fr 1fr 1fr" }}>
      <div style={{ gridColumn: "1 / 3", minHeight: 0, overflow: "hidden" }}>{cellInner(kids[0])}</div>
      {kids.slice(1, 5).map((c, i) => <div key={i} style={{ minHeight: 0, overflow: "hidden" }}>{cellInner(c)}</div>)}
    </div>
  );
}
function Single({ children }) {
  const kid = React.Children.toArray(children).filter(Boolean)[0];
  return (
    <div style={{ flex: 1, minHeight: 0, width: "100%", display: "flex", flexDirection: "column" }}>
      {kid ? cellInner(kid) : null}
    </div>
  );
}

function cellInner(child) {
  return (
    <div style={{ height: "100%", minHeight: 0, overflow: "hidden", display: "flex", flexDirection: "column" }}>
      {React.isValidElement(child)
        ? React.cloneElement(child, {
          style: { ...(child.props && child.props.style), flex: 1, minHeight: 0, width: "100%", height: "100%" },
        })
        : child}
    </div>
  );
}
function wrapCells(children) {
  return React.Children.map(children, (c, i) => (
    c == null ? null : <div key={i} style={{ minHeight: 0, height: "100%", overflow: "hidden" }}>{cellInner(c)}</div>
  ));
}

/**
 * Adaptive room layout — mirrors Smart-LVGL templates by widget count:
 * 1 Single · 2 TwoV · 3 ThreeV · 4 Four · 5 Five(wide top) · 6 Six · 7–8 paginated Six
 */
function AdaptiveGrid({ children, pageIndex = 0 }) {
  const kids = React.Children.toArray(children).filter(Boolean);
  const pageSize = 6;
  const page = Math.max(0, Math.min(pageIndex, Math.max(0, Math.ceil(kids.length / pageSize) - 1)));
  const slice = kids.slice(page * pageSize, page * pageSize + pageSize);
  const n = slice.length;
  if (n === 0) return null;
  if (n === 1) return <Single>{slice[0]}</Single>;
  if (n === 2) return <TwoV>{slice}</TwoV>;
  if (n === 3) return <ThreeV>{slice}</ThreeV>;
  if (n === 4) return <Four>{slice}</Four>;
  if (n === 5) return <Five>{slice}</Five>;
  return <Six>{slice}</Six>;
}

function layoutHintsForCount(n, indexInPage, widgetType) {
  if (n <= 1) {
    return {
      compact: false,
      page: widgetType !== "clim" && widgetType !== "thermo",
      wide: false,
      full: widgetType === "clim" || widgetType === "thermo",
    };
  }
  if (n === 5 && indexInPage === 0 && (widgetType === "clim" || widgetType === "thermo")) {
    return { compact: true, page: false, wide: true, full: false };
  }
  if (n <= 3) {
    return { compact: widgetType === "clim" || widgetType === "thermo", page: false, wide: widgetType === "clim", full: false };
  }
  return { compact: true, page: false, wide: false, full: false };
}

/** Horizontal swipe on empty chrome — skips .track / interactive controls */
function SwipeSurface({ onSwipeLeft, onSwipeRight, enabled = true, children, style, className = "" }) {
  const { useRef } = React;
  const start = useRef(null);
  const onDown = (e) => {
    if (!enabled) return;
    const t = e.target;
    if (t && t.closest && t.closest(".track, .track-rail, .track-knob, .scroll-pane, [data-no-swipe], input, textarea, button.pwr")) return;
    if (t && t.closest && t.closest("[data-swipe-block]")) return;
    start.current = { x: e.clientX, y: e.clientY };
  };
  const finish = (e) => {
    if (!start.current || !enabled) return;
    const dx = e.clientX - start.current.x;
    const dy = e.clientY - start.current.y;
    start.current = null;
    if (Math.abs(dx) < 64) return;
    if (Math.abs(dx) < Math.abs(dy) * 1.15) return;
    if (dx < 0) onSwipeLeft && onSwipeLeft();
    else onSwipeRight && onSwipeRight();
  };
  return (
    <div
      className={className}
      style={{ touchAction: "pan-y", height: "100%", display: "flex", flexDirection: "column", minHeight: 0, ...style }}
      onPointerDown={onDown}
      onPointerUp={finish}
      onPointerCancel={() => { start.current = null; }}
    >
      {children}
    </div>
  );
}

function HomeScreen({ onAlarm, dots = 3, active = 0 }) {
  return (
    <div className="lv">
      <div style={{ padding: "28px 24px 0" }}>
        <div style={{ fontFamily: "var(--font-clock)", fontWeight: 300, fontSize: 84, lineHeight: 0.95, letterSpacing: "-0.03em" }}>15:30</div>
        <div style={{ fontSize: 15, color: "var(--t3)", marginTop: 8, fontWeight: 500 }}>Saturday · Hub</div>
      </div>
      <div style={{ padding: "16px 20px 0" }}>
        <div style={{
          border: "1px solid var(--line)", borderRadius: 12, padding: "14px 16px",
          background: "var(--bg-card)", display: "flex", gap: 14, alignItems: "center",
        }}>
          <div style={{ color: "var(--cool)" }}><LIco.gauge width="36" height="36" /></div>
          <div style={{ flex: 1 }}>
            <div style={{ fontSize: 28, fontWeight: 300 }}>24.5°</div>
            <div style={{ fontSize: 12, color: "var(--t3)" }}>Indoor · 48% RH</div>
          </div>
          <div style={{ textAlign: "right", fontSize: 12, color: "var(--t3)", lineHeight: 1.5 }}>
            <div>Power 2.1 kW</div>
            <div style={{ color: "var(--on)", fontWeight: 700 }}>MQTT OK</div>
          </div>
        </div>
      </div>
      <div style={{ flex: 1 }} />
      <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr", gap: 10, padding: "0 20px 20px" }}>
        <button type="button" style={tileBtn}>
          <LIco.home width="22" height="22" style={{ color: "var(--on)" }} />
          <div><b>Home</b><div style={{ fontSize: 11, color: "var(--t3)" }}>Active</div></div>
        </button>
        <button type="button" onClick={onAlarm} style={tileBtn}>
          <LIco.shieldFill width="22" height="22" style={{ color: "var(--heat)" }} />
          <div><b>Security</b><div style={{ fontSize: 11, color: "var(--t3)" }}>Armed</div></div>
        </button>
      </div>
      <div className="lv-dots" style={{ paddingBottom: 6 }}>
        {Array.from({ length: dots }, (_, i) => (
          <div key={i} className={"lv-dot" + (i === active ? " on" : "")} />
        ))}
      </div>
    </div>
  );
}

const tileBtn = {
  display: "flex", alignItems: "center", gap: 12, minHeight: 72, padding: "12px 14px",
  borderRadius: 12, border: "1px solid var(--line)", background: "var(--bg-card)",
  color: "var(--t1)", cursor: "pointer", textAlign: "left",
};

function AlarmKeypad({ onClose }) {
  const [code, setCode] = useStateP("");
  const max = 4;
  const press = (k) => {
    if (k === "del") setCode((c) => c.slice(0, -1));
    else if (code.length < max) setCode((c) => c + k);
  };
  const ok = code.length === max;
  return (
    <div className="lv lv-panel" style={{ padding: "14px 22px" }}>
      <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
        <button type="button" className="sf-back" onClick={onClose}>×</button>
        <div className="eyebrow">Disarm</div>
        <div style={{ width: 36 }} />
      </div>
      <div style={{ textAlign: "center", marginTop: 10 }}>
        <div className="badge" style={{ width: 48, height: 48, margin: "0 auto 8px", background: window.A("#34d399", 0.14), color: "var(--on)" }}>
          <LIco.lock width="24" height="24" />
        </div>
        <div style={{ fontWeight: 700 }}>Enter PIN</div>
      </div>
      <div style={{ display: "flex", gap: 12, justifyContent: "center", margin: "14px 0" }}>
        {Array.from({ length: max }, (_, i) => (
          <div key={i} style={{
            width: 12, height: 12, borderRadius: 2,
            background: i < code.length ? "var(--accent)" : "transparent",
            border: i < code.length ? "none" : "2px solid var(--t4)",
          }} />
        ))}
      </div>
      <div style={{ display: "grid", gridTemplateColumns: "1fr 1fr 1fr", gap: 8, padding: "0 24px" }}>
        {"123456789".split("").map((k) => (
          <button key={k} type="button" onClick={() => press(k)} style={keyStyle}>{k}</button>
        ))}
        <div />
        <button type="button" onClick={() => press("0")} style={keyStyle}>0</button>
        <button type="button" onClick={() => press("del")} style={keyStyle}>⌫</button>
      </div>
      <div style={{ flex: 1 }} />
      <button type="button" disabled={!ok} onClick={onClose} style={{
        height: 44, borderRadius: 8, border: "none", fontWeight: 700,
        background: ok ? "var(--accent)" : "var(--bg-card-2)",
        color: ok ? "var(--ink-on-accent)" : "var(--t4)", cursor: ok ? "pointer" : "default",
      }}>OK</button>
    </div>
  );
}

const keyStyle = {
  height: 44, borderRadius: 8, border: "1px solid var(--line)", background: "var(--bg-card)",
  color: "var(--t1)", fontSize: 20, fontWeight: 500, cursor: "pointer",
};

Object.assign(window, {
  PageShell, TwoV, TwoH, ThreeV, ThreeH, Four, Six, Five, Single,
  AdaptiveGrid, layoutHintsForCount, SwipeSurface, HomeScreen, AlarmKeypad,
});
