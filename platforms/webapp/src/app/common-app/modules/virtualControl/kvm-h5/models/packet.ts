export const packet = {
    H_CMD_IMAGE: 0x02,
    H_CMD_KEYBOARD_STATE: 0x04,
    H_CMD_CONNECT_STATE: 0x08,
    H_CMD_MOUSE_MODE: 0x25,

    C_CMD_DEFINITION: 0x27,
    H_CMD_DEFINITION: 0x28,

    C_CMD_VMM_PORT: 0x35,
    H_CMD_VMM_PORT: 0x36,
    C_CMD_BOOTDEVICE: 0x37,
    H_CMD_BOOTDEVICE: 0x38,
    H_CMD_REPORT_KVMKEY: 0x40,
    H_CMD_NO_PRIVILEGE: 0x51,
    C_CMD_VIDEO_START: 0x40,
    C_CMD_VIDEO_STOP: 0x41,
    C_CMD_GET_SUITE: 0x42,
    C_CMD_RSP_SUITE: 0x43,
    C_CMD_SET_SUITE: 0x44,

    C_CMD_CONNECT: 0x06,
    C_CMD_HEARTBEAT: 0x09,
    C_CMD_MOUSE_MODE: 0x24,
    C_CMD_MOUSE: 0x05,
    C_CMD_KEYBOARD: 0x03,

    CMD_USERREST: 0x30,
    C_CMD_VMM_CODEKEY: 0x31,
    H_CMD_VMM_CODEKEY: 0x32,
    CMD_POWER_SECURITY: 0x33,
    CMD_POWER_ON: 0x21,
    CMD_POWER_FORCE_OFF: 0x20,
    CMD_POWER_OFF: 0x25,
    CMD_POWER_FORCE_REBOOT: 0x22,
    CMD_POWER_FORCE_OFF_AND_ON: 0x23,

    CMD_BOOT_NORMAL: 0x00,
    CMD_BOOT_PXE: 0x01,
    CMD_BOOT_HARD_DISK: 0x02,
    CMD_BOOT_CDROM: 0x05,
    CMD_BOOT_BIOS: 0x06,
    CMD_BOOT_FLOPPY: 0x0f,

    C_CMD_UMS_NOTIFY_STATE: 0x45,
    H_CMD_UMS_NOTIFY_STATE: 0x46,

    C_CMD_VMM_STATE: 0x47,
    H_CMD_VMM_STATE: 0x48,

    C_IV_PATTERN: 0x57,
    H_IV_PATTERN: 0x58,

    C_FLOPPY_STATE: 0x59,
    H_FLOPPY_STATE: 0x60,

    getSuitePacket(bladeId, security) {
        const data = new Uint8Array(2);
        data[0] = packet.C_CMD_GET_SUITE;
        data[1] = bladeId;

        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    getFloppyState(bladeId, security) {
        const data = new Uint8Array(2);
        data[0] = packet.C_FLOPPY_STATE;
        data[1] = bladeId;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    getIVPatternPacket(bladeId, security) {
        const data = new Uint8Array(2);
        data[0] = packet.C_IV_PATTERN;
        data[1] = bladeId;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    setSuitePacket(bladeId, security, iterations, hmac) {
        const data = new Uint8Array(7);
        data[0] = packet.C_CMD_SET_SUITE;
        data[1] = bladeId;
        data[2] = hmac;
        data[3] = (iterations & 0xff000000) >> 24;
        data[4] = (iterations & 0x00ff0000) >> 16;
        data[5] = (iterations & 0x0000ff00) >> 8;
        data[6] = iterations & 0x000000ff;

        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    connectPacket(bladeId, security) {
        const data = new Uint8Array(133);
        const connKey = security.getKvmReconnectCode();
        let length = 5;
        data[0] = packet.C_CMD_CONNECT;
        data[1] = bladeId;
        data[2] = 0;
        data[3] = 1;
        data[4] = 1;
        if (connKey != null) {
            length += 128;
            for (let i = 0; i < 128; i++) {
                data[5 + i] = connKey[i];
            }
        }
        if (!security.isKvmEncryption()) {
            return packet._MakePacket(security.getKvmTagId(), data, length);
        } else {
            length = (0x80 << 8) | length;
            return packet._MakeEncryptPacket(security.getKvmAuthCode(), data, length);
        }
    },

    heartBeatPacket(bladeId, security) {
        const data = new Uint8Array(2);
        data[0] = packet.C_CMD_HEARTBEAT;
        data[1] = bladeId;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    definitionPacket(bladeId, security, value) {
        const data = new Uint8Array(5);
        data[0] = packet.C_CMD_DEFINITION;
        data[1] = bladeId;
        data[2] = value;
        data[3] = 1;
        data[4] = 0;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    powerControlPacket(bladeId, security, cmd) {
        if (!security.isKvmEncryption()) {
            const data = new Uint8Array(2);
            data[0] = cmd;
            data[1] = bladeId;
            return packet._MakePacket(security.getKvmTagId(), data, data.length);
        } else {
            const data2 = new Uint8Array(18);
            const cmdData = new Uint8Array(16);
            cmdData[15] = cmd;
            const encrypData = security.kvmEncrypt(cmdData);

            data2[0] = packet.CMD_POWER_SECURITY;
            data2[1] = bladeId;
            for (let i = 0; i < 16; i++) {
                data2[i + 2] = encrypData[i];
            }

            return packet._MakePacket(security.getKvmTagId(), data2, data2.length);
        }
    },

    mouseModePacket(bladeId, security, mode) {
        const data = new Uint8Array(5);
        data[0] = packet.C_CMD_MOUSE_MODE;
        data[1] = bladeId;
        data[2] = mode;
        data[3] = 0;
        data[4] = 0;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    bootOptionPacket(bladeId, security, cmd) {
        const data = new Uint8Array(5);
        data[0] = packet.H_CMD_BOOTDEVICE;
        data[1] = bladeId;
        data[2] = cmd;
        data[3] = 0;
        data[4] = 0;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    mouseAbsPacket(bladeId, security, x, y, button) {
        if (!security.isKvmEncryption()) {
            const data = new Uint8Array(8);
            data[0] = packet.C_CMD_MOUSE;
            data[1] = bladeId;
            data[2] = button;
            data[3] = (x & 0xff00) >> 8;
            data[4] = x & 0xff;
            data[5] = (y & 0xff00) >> 8;
            data[6] = y & 0xff;
            data[7] = 0;
            return packet._MakePacket(security.getKvmTagId(), data, data.length);
        } else {
            const data2 = new Uint8Array(18);
            const mouseData = new Uint8Array(6);
            mouseData[0] = button;
            mouseData[1] = (x & 0xff00) >> 8;
            mouseData[2] = x & 0xff;
            mouseData[3] = (y & 0xff00) >> 8;
            mouseData[4] = y & 0xff;
            mouseData[5] = 0;
            const encrypData = security.kvmEncryptExt(mouseData);
            data2[0] = packet.C_CMD_MOUSE;
            data2[1] = bladeId;
            for (let i = 0; i < 16; i++) {
                data2[2 + i] = encrypData[i];
            }
            return packet._MakePacket(security.getKvmTagId(), data2, data2.length);
        }
    },

    mouseRelPacket(bladeId, security, x, y, button) {
        if (!security.isKvmEncryption()) {
            const data = new Uint8Array(6);
            data[0] = packet.C_CMD_MOUSE;
            data[1] = bladeId;
            data[2] = button;
            data[3] = x;
            data[4] = y;
            data[5] = 0;
            return packet._MakePacket(security.getKvmTagId(), data, data.length);
        } else {
            const data2 = new Uint8Array(18);
            const mouseData = new Uint8Array(4);
            mouseData[0] = button;
            mouseData[1] = x;
            mouseData[2] = y;
            mouseData[3] = 0;
            const encrypData = security.kvmEncryptExt(mouseData);
            data2[0] = packet.C_CMD_MOUSE;
            data2[1] = bladeId;
            for (let i = 0; i < 16; i++) {
                data2[2 + i] = encrypData[i];
            }
            return packet._MakePacket(security.getKvmTagId(), data2, data2.length);
        }
    },

    keyPressedPacket(bladeId, security, keyData) {
        const data = new Uint8Array(18);
        data[0] = packet.C_CMD_KEYBOARD;
        data[1] = 0;
        const encrypData = security.kvmEncryptExt(keyData);
        for (let i = 0; i < 16; i++) {
            data[2 + i] = encrypData[i];
        }
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    vmmPortPacket(bladeId, security) {
        const data = new Uint8Array(2);
        data[0] = packet.C_CMD_VMM_PORT;
        data[1] = bladeId;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    vmmCodekeyPacket(bladeId, security) {
        const data = new Uint8Array(2);
        data[0] = packet.C_CMD_VMM_CODEKEY;
        data[1] = bladeId;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    videoStart(bladeId, security) {
        const data = new Uint8Array(2);

        data[0] = packet.C_CMD_VIDEO_START;
        data[1] = 0x00;

        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    videoStop(bladeId, security) {
        const data = new Uint8Array(2);

        data[0] = packet.C_CMD_VIDEO_STOP;
        data[1] = 0x00;

        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    _MakePacket(id, data, length) {
        const packetDate = new Uint8Array(length + 10);
        const low = (length + 2) & 0xff;
        const high = ((length + 2) & 0xff00) >> 8;

        packetDate[0] = -2;
        packetDate[1] = -10;
        packetDate[2] = high;
        packetDate[3] = low;
        packetDate[4] = (id >> 24) & 0xff;
        packetDate[5] = (id >> 16) & 0xff;
        packetDate[6] = (id >> 8) & 0xff;
        packetDate[7] = id & 0xff;
        packetDate[8] = 0;
        packetDate[9] = 0;
        for (let i = 0; i < length; i++) {
            packetDate[10 + i] = data[i];
        }

        return packetDate;
    },

    _MakeEncryptPacket(id, data, length) {
        const len = length & 0x7fff;
        const packetDate = new Uint8Array(len + 30);
        const low = (length + 2) & 0xff;
        const high = ((length + 2) & 0xff00) >> 8;

        packetDate[0] = -2;
        packetDate[1] = -10;
        packetDate[2] = high;
        packetDate[3] = low;

        for (let i = 0; i < 24; i++) {
            packetDate[4 + i] = id[i];
        }

        for (let j = 0; j < len; j++) {
            packetDate[30 + j] = data[j];
        }

        return packetDate;
    },

    getUmsNotifyState(bladeId, security) {
        const data = new Uint8Array(2);
        data[0] = packet.C_CMD_UMS_NOTIFY_STATE;
        data[1] = bladeId;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    },

    getVmmState(bladeId, security) {
        const data = new Uint8Array(2);
        data[0] = packet.C_CMD_VMM_STATE;
        data[1] = bladeId;
        return packet._MakePacket(security.getKvmTagId(), data, data.length);
    }
};
