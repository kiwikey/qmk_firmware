// QMK raw HID identity for kiwikey/buildbox (keyboard.json + usb_descriptor_common.h)
const HID_FILTERS = [
	{ vendorId: 0x4B57, productId: 0xBB00, usagePage: 0xFF60, usage: 0x61 },
];

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

function setStatus(state, label) {
	statusDot.className = 'status-dot' + (state ? ` ${state}` : '');
	statusText.textContent = label;
}

function setConnectedUI(connected, name) {
	connectBtn.disabled = connected;
	disconnectBtn.disabled = !connected;
	deviceNameEl.textContent = connected ? name : '';
	overlay.style.display = connected ? 'none' : 'flex';
	overlay.textContent = connected ? '' : 'No Signal';
	if (!connected) {
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
}

function disconnect() {
	if (device) {
		device.oninputreport = null;
		if (device.opened) device.close();
	}
	device = null;
	setStatus('', 'Disconnected');
	setConnectedUI(false, '');
}

function handleInputReport(event) {
	reportCount++;
	byteCount += event.data.byteLength;
	// Frame/pixel decoding hooks in here once the device-side wire format is defined.
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
	const granted = await navigator.hid.getDevices();
	const known = granted.find((d) => d.vendorId === 0x4B57 && d.productId === 0xBB00);
	if (known) await connect(known);
})();
