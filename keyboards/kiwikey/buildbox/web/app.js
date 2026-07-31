// QMK raw HID identity for kiwikey/buildbox (keyboard.json + usb_descriptor_common.h)
const HID_FILTERS = [
	{ vendorId: 0x4B57, productId: 0xBB00, usagePage: 0xFF60, usage: 0x61 },
];

// webhid_stream handshake wire format - keep in sync with features/webhid_stream.h
const VIA_ID_CUSTOM_SET_VALUE = 0x07;
const WEBHID_CONFIG_CHANNEL_ID = 0x20; // must stay clear of QMK's reserved via_channel_id range (0-5) - see webhid_stream.h
const WEBHID_VALUE_PING = 0x01;
const WEBHID_VALUE_STOP = 0x02;
const WEBHID_PING_INTERVAL_MS = 500; // firmware auto-disarms after 2000ms of silence

const connectBtn = document.getElementById('connect-btn');
const disconnectBtn = document.getElementById('disconnect-btn');
const statusDot = document.getElementById('status-dot');
const statusText = document.getElementById('status-text');
const deviceNameEl = document.getElementById('device-name');
const unsupportedEl = document.getElementById('unsupported');
const statReports = document.getElementById('stat-reports');
const statBytes = document.getElementById('stat-bytes');
const overlay = document.getElementById('screen-overlay');
const canvas = document.getElementById('screen');
const ctx = canvas.getContext('2d');

let device = null;
let reportCount = 0;
let byteCount = 0;
let pingIntervalId = null;

async function sendConfig(valueId) {
	if (!device || !device.opened) return;
	const report = new Uint8Array(32);
	report[0] = VIA_ID_CUSTOM_SET_VALUE;
	report[1] = WEBHID_CONFIG_CHANNEL_ID;
	report[2] = valueId;
	try {
		await device.sendReport(0, report);
	} catch (err) {
		console.error('Failed to send HID report:', err);
	}
}

function startPinging() {
	stopPinging();
	sendConfig(WEBHID_VALUE_PING);
	pingIntervalId = setInterval(() => sendConfig(WEBHID_VALUE_PING), WEBHID_PING_INTERVAL_MS);
}

function stopPinging() {
	if (pingIntervalId !== null) {
		clearInterval(pingIntervalId);
		pingIntervalId = null;
	}
}

function setStatus(state, label) {
	statusDot.className = 'status-dot' + (state ? ` ${state}` : '');
	statusText.textContent = label;
}

function setConnectedUI(connected, name) {
	connectBtn.disabled = connected;
	disconnectBtn.disabled = !connected;
	deviceNameEl.textContent = connected ? name : '';
	// Overlay stays up through the HID-open moment - it only clears once a
	// real mirror message arrives (see dispatch()), which is proof the
	// stream is actually flowing, not just that the device is open.
	if (!connected) {
		overlay.style.display = 'flex';
		overlay.textContent = 'No Signal';
		ctx.fillStyle = '#000';
		ctx.fillRect(0, 0, canvas.width, canvas.height);
	}
}

async function connect(hidDevice) {
	try {
		if (!hidDevice.opened) await hidDevice.open();
	} catch (err) {
		setStatus('', 'Connection failed');
		console.error('Failed to open HID device:', err);
		return;
	}

	device = hidDevice;
	device.oninputreport = handleInputReport;

	setStatus('connected', 'Connected');
	setConnectedUI(true, device.productName || 'BuildBox');
	startPinging();
}

async function disconnect() {
	stopPinging();
	if (device) {
		await sendConfig(WEBHID_VALUE_STOP); // best-effort courtesy; the firmware's ping watchdog is the real safety net
		device.oninputreport = null;
		if (device.opened) device.close();
	}
	device = null;
	setStatus('', 'Disconnected');
	setConnectedUI(false, '');
}

// webhid_stream opcodes - keep in sync with features/webhid_stream.h
const OP_PAD = 0x00;
const OP_HELLO = 0x01;
const OP_CLEAR = 0x02;
const OP_RECT = 0x10;
const OP_TEXT = 0x11;
const OP_ICON = 0x12;
const OP_CIRCLE = 0x13;
const OP_FLUSH = 0x20;
const OP_RESET = 0xFF;
const SYNC_BYTE = 0xA5;

// Rough canvas-font approximations for the device's bitmap fonts (font_id
// order matches features/webhid_shadow.c's font_id_for()). Not a pixel
// match - the real fonts aren't shipped to the browser - just readable.
const FONT_CSS = {
	0: '16px monospace', // thintel16
	1: '32px monospace', // thintel32
	2: '8px monospace',  // font_oled
	3: '16px monospace', // nanoplex16
	4: '32px monospace', // nanoplex32
};

let rxQueue = []; // raw bytes awaited parsing into framed messages
let msgSeq = 0; // TEMP DEBUG - remove once the knob ring is diagnosed

function handleInputReport(event) {
	const data = new Uint8Array(event.data.buffer, event.data.byteOffset, event.data.byteLength);
	reportCount++;
	byteCount += data.byteLength;
	for (let i = 0; i < data.length; i++) rxQueue.push(data[i]);
	parseQueue();
}

function parseQueue() {
	let consumed = 0;
	while (true) {
		// TEMP DEBUG - remove once the knob ring is diagnosed
		const skipStart = consumed;
		while (consumed < rxQueue.length && rxQueue[consumed] !== SYNC_BYTE) consumed++;
		if (consumed > skipStart) {
			console.log(`PARSE: skipped ${consumed - skipStart} non-sync byte(s): [${rxQueue.slice(skipStart, consumed).join(',')}]`);
		}
		if (rxQueue.length - consumed < 3) break; // need sync+opcode+len at least
		const opcode = rxQueue[consumed + 1];
		const len = rxQueue[consumed + 2];
		if (rxQueue.length - consumed < 3 + len) break; // message not fully arrived yet
		const payload = rxQueue.slice(consumed + 3, consumed + 3 + len);
		console.log(`MSG #${++msgSeq}: opcode=0x${opcode.toString(16)} len=${len}`);
		dispatch(opcode, payload);
		consumed += 3 + len;
	}
	rxQueue.splice(0, consumed);
}

function readU16(payload, i) {
	return payload[i] | (payload[i + 1] << 8);
}

function rgb565ToCss(v) {
	const r5 = (v >> 11) & 0x1f, g6 = (v >> 5) & 0x3f, b5 = v & 0x1f;
	const r = (r5 << 3) | (r5 >> 2);
	const g = (g6 << 2) | (g6 >> 4);
	const b = (b5 << 3) | (b5 >> 2);
	return `rgb(${r},${g},${b})`;
}

function dispatch(opcode, payload) {
	if (overlay.style.display !== 'none') {
		overlay.style.display = 'none'; // first real message = proof the stream is live
	}
	switch (opcode) {
		case OP_PAD: break;
		case OP_HELLO: handleHello(payload); break;
		case OP_CLEAR: handleClear(payload); break;
		case OP_RECT: handleRect(payload); break;
		case OP_TEXT: handleText(payload); break;
		case OP_ICON: handleIcon(payload); break;
		case OP_CIRCLE: handleCircle(payload); break;
		case OP_FLUSH: break; // immediate-mode drawing, nothing to present
		case OP_RESET: handleReset(); break;
		default: break; // unknown opcode - ignore, forward-compatible
	}
}

function handleHello(p) {
	const protoVer = p[0];
	const width = readU16(p, 1);
	const height = readU16(p, 3);
	console.log(`webhid_stream: HELLO v${protoVer}, ${width}x${height}`);
}

function handleClear(p) {
	ctx.fillStyle = rgb565ToCss(readU16(p, 0));
	ctx.fillRect(0, 0, canvas.width, canvas.height);
}

function handleRect(p) {
	const x0 = readU16(p, 0), y0 = readU16(p, 2), x1 = readU16(p, 4), y1 = readU16(p, 6);
	const color = rgb565ToCss(readU16(p, 8));
	const filled = p[10] !== 0;
	const x = Math.min(x0, x1), y = Math.min(y0, y1);
	const w = Math.abs(x1 - x0) + 1, h = Math.abs(y1 - y0) + 1;
	if (filled) {
		ctx.fillStyle = color;
		ctx.fillRect(x, y, w, h);
	} else {
		ctx.strokeStyle = color;
		ctx.strokeRect(x + 0.5, y + 0.5, w - 1, h - 1);
	}
}

function handleText(p) {
	const x = readU16(p, 0), y = readU16(p, 2);
	const fontId = p[4];
	const fg = rgb565ToCss(readU16(p, 5));
	const bg = rgb565ToCss(readU16(p, 7));
	const len = p[9];
	const str = new TextDecoder().decode(new Uint8Array(p.slice(10, 10 + len)));
	const font = FONT_CSS[fontId] || '16px monospace';
	const lineHeight = parseInt(font, 10) || 16;

	ctx.font = font;
	ctx.textBaseline = 'top';
	const width = ctx.measureText(str).width;
	ctx.fillStyle = bg;
	ctx.fillRect(x, y, width, lineHeight);
	ctx.fillStyle = fg;
	ctx.fillText(str, x, y);
}

function handleIcon(p) {
	const x = readU16(p, 0), y = readU16(p, 2);
	const iconId = p[4];
	// Icon/GIF assets aren't exported to the browser yet - placeholder box
	// with the id so mirrored layout is still visible and debuggable.
	ctx.fillStyle = '#333';
	ctx.fillRect(x, y, 16, 16);
	ctx.strokeStyle = '#888';
	ctx.strokeRect(x + 0.5, y + 0.5, 15, 15);
	ctx.fillStyle = '#fff';
	ctx.font = '8px monospace';
	ctx.textBaseline = 'top';
	ctx.fillText(String(iconId), x + 2, y + 4);
}

function handleCircle(p) {
	const x = readU16(p, 0), y = readU16(p, 2), radius = readU16(p, 4);
	const color = rgb565ToCss(readU16(p, 6));
	const filled = p[8] !== 0;

	// TEMP DEBUG - remove once the knob ring is diagnosed
	console.log(`CIRCLE: x=${x} y=${y} r=${radius} color=${color} filled=${filled}`);

	ctx.beginPath();
	ctx.arc(x, y, radius, 0, Math.PI * 2);
	if (filled) {
		ctx.fillStyle = color;
		ctx.fill();
	} else {
		ctx.strokeStyle = color;
		ctx.stroke();
	}
}

function handleReset() {
	ctx.fillStyle = '#000';
	ctx.fillRect(0, 0, canvas.width, canvas.height);
	rxQueue = [];
}

connectBtn.addEventListener('click', async () => {
	setStatus('connecting', 'Connecting…');
	try {
		const devices = await navigator.hid.requestDevice({ filters: HID_FILTERS });
		if (devices.length === 0) {
			setStatus('', 'Disconnected');
			return;
		}
		await connect(devices[0]);
	} catch (err) {
		setStatus('', 'Disconnected');
		console.error('HID request failed:', err);
	}
});

disconnectBtn.addEventListener('click', disconnect);

navigator.hid?.addEventListener('disconnect', (event) => {
	if (device && event.device === device) disconnect();
});

setInterval(() => {
	statReports.textContent = reportCount;
	statBytes.textContent = byteCount;
	reportCount = 0;
	byteCount = 0;
}, 1000);

(async function init() {
	if (!('hid' in navigator)) {
		unsupportedEl.hidden = false;
		connectBtn.disabled = true;
		return;
	}

	setConnectedUI(false, '');

	// Re-attach to a device the user already granted permission for, without a click.
	// Must also check for the raw HID collection specifically - this device exposes
	// several HID interfaces (keyboard, shared/extrakeys, raw HID, ...) under the same
	// vendorId/productId, and granting permission to more than one during testing means
	// vendorId/productId alone isn't enough to find the right one.
	const granted = await navigator.hid.getDevices();
	const known = granted.find(
		(d) => d.vendorId === 0x4B57 && d.productId === 0xBB00 && d.collections.some((c) => c.usagePage === 0xFF60 && c.usage === 0x61)
	);
	if (known) await connect(known);
})();
