/* eslint-disable */
// Lightweight gallery canvas for OEM UI kits — original implementation.
// Sections + artboards + pan/zoom. No third-party host bridge required.
const { useState, useRef, useEffect, useCallback, createContext, useContext } = React;

const DC = {
  bg: "#e8eaed",
  grid: "rgba(20,30,40,0.07)",
  title: "#1a2330",
  subtitle: "#5a6675",
  font: '"DM Sans", system-ui, sans-serif',
};

if (typeof document !== "undefined" && !document.getElementById("dc-styles")) {
  const s = document.createElement("style");
  s.id = "dc-styles";
  s.textContent = [
    ".dc-root{min-height:100vh;background:var(--dc-bg,#e8eaed);font-family:var(--dc-font);overflow:hidden;position:relative}",
    ".dc-world{transform-origin:0 0;will-change:transform}",
    ".dc-section{margin-bottom:72px}",
    ".dc-section h2{margin:0 0 4px;font-size:22px;font-weight:700;color:#1a2330}",
    ".dc-section p{margin:0 0 20px;font-size:13px;color:#5a6675;max-width:720px;line-height:1.45}",
    ".dc-row{display:flex;flex-wrap:wrap;gap:36px 28px;align-items:flex-start}",
    ".dc-slot{position:relative}",
    ".dc-card{background:#fff;border-radius:10px;box-shadow:0 8px 28px rgba(0,0,0,.12);overflow:hidden;isolation:isolate}",
    ".dc-label{position:absolute;bottom:100%;left:0;margin-bottom:8px;font-size:12px;font-weight:600;color:#3a4552;white-space:nowrap}",
    ".dc-hint{position:fixed;left:16px;bottom:16px;z-index:50;background:rgba(26,35,48,.88);color:#fff;padding:8px 12px;border-radius:8px;font-size:12px}",
  ].join("\n");
  document.head.appendChild(s);
}

const DCCtx = createContext({ zoom: 1 });

function DesignCanvas({ children }) {
  const [pan, setPan] = useState({ x: 48, y: 48 });
  const [zoom, setZoom] = useState(0.85);
  const drag = useRef(null);

  const onWheel = useCallback((e) => {
    e.preventDefault();
    if (e.ctrlKey || e.metaKey) {
      const f = e.deltaY > 0 ? 0.92 : 1.08;
      setZoom((z) => Math.min(1.6, Math.max(0.35, z * f)));
    } else {
      setPan((p) => ({ x: p.x - e.deltaX, y: p.y - e.deltaY }));
    }
  }, []);

  useEffect(() => {
    const el = document.getElementById("dc-root");
    if (!el) return;
    el.addEventListener("wheel", onWheel, { passive: false });
    return () => el.removeEventListener("wheel", onWheel);
  }, [onWheel]);

  const onPointerDown = (e) => {
    if (e.button !== 0 && e.button !== 1) return;
    if (e.target.closest(".dc-card,button,input,a")) return;
    drag.current = { x: e.clientX, y: e.clientY, pan };
    e.currentTarget.setPointerCapture(e.pointerId);
  };
  const onPointerMove = (e) => {
    if (!drag.current) return;
    const d = drag.current;
    setPan({ x: d.pan.x + (e.clientX - d.x), y: d.pan.y + (e.clientY - d.y) });
  };
  const onPointerUp = () => { drag.current = null; };

  return (
    <DCCtx.Provider value={{ zoom }}>
      <div
        id="dc-root"
        className="dc-root"
        style={{ "--dc-bg": DC.bg, "--dc-font": DC.font, cursor: drag.current ? "grabbing" : "grab" }}
        onPointerDown={onPointerDown}
        onPointerMove={onPointerMove}
        onPointerUp={onPointerUp}
      >
        <div
          className="dc-world"
          style={{
            transform: `translate(${pan.x}px, ${pan.y}px) scale(${zoom})`,
            backgroundImage: `linear-gradient(${DC.grid} 1px, transparent 1px), linear-gradient(90deg, ${DC.grid} 1px, transparent 1px)`,
            backgroundSize: "24px 24px",
            minWidth: "100%",
            minHeight: "100%",
            padding: "40px 48px 120px",
          }}
        >
          {children}
        </div>
        <div className="dc-hint">拖拽空白处平移 · Ctrl+滚轮缩放</div>
      </div>
    </DCCtx.Provider>
  );
}

function DCSection({ id, title, subtitle, children }) {
  return (
    <section className="dc-section" data-id={id}>
      <h2 style={{ color: DC.title }}>{title}</h2>
      {subtitle ? <p style={{ color: DC.subtitle }}>{subtitle}</p> : null}
      <div className="dc-row">{children}</div>
    </section>
  );
}

function DCArtboard({ id, label, width = 480, height = 480, style, children }) {
  return (
    <div className="dc-slot" data-id={id} style={{ width }}>
      <div className="dc-label">{label}</div>
      <div className="dc-card" style={{ width, height, ...style }}>{children}</div>
    </div>
  );
}
